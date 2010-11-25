/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// CLASS HEADER
#include "CMSRPMessageHandler.h"


// INCLUDES
#include <e32math.h>
#include <s32mem.h>

#include "MMSRPWriterObserver.h"
#include "CMSRPMessage.h"
#include "CMSRPResponse.h"
#include "CMSRPReport.h"
#include "CMSRPByteRangeHeader.h"
#include "CMSRPToPathHeader.h"
#include "CMSRPFromPathHeader.h"
#include "CMSRPMessageIdHeader.h"
#include "CMSRPSuccessReportHeader.h"
#include "CMSRPFailureReportHeader.h"
#include "CMSRPContentTypeHeader.h"
#include "MSRPStrings.h"
#include "MsrpStrConsts.h"
#include "TMSRPUtil.h"
#include "MMSRPConnection.h"
#include "MMSRPMessageObserver.h"
#include "CMSRPStatusHeader.h"

// Constants
const TInt KEndlineConstLength = 10;


TInt CMSRPMessageHandler::LinkOffset()
    {
    return _FOFF(CMSRPMessageHandler,iLink);
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::NewL
// -----------------------------------------------------------------------------
//
CMSRPMessageHandler* CMSRPMessageHandler::NewL( MMSRPMessageObserver* aMessageObserver, const TDesC8& aMessage )
    {    
    CMSRPMessageHandler* self = CMSRPMessageHandler::NewLC( aMessageObserver, aMessage );
    CleanupStack::Pop( self );
    return self;
    }

CMSRPMessageHandler* CMSRPMessageHandler::NewL( TMSRPMessageType aMessageType )
    {
    CMSRPMessageHandler* self = CMSRPMessageHandler::NewLC( aMessageType );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::NewLC
// -----------------------------------------------------------------------------
//
CMSRPMessageHandler* CMSRPMessageHandler::NewLC( MMSRPMessageObserver* aMessageObserver, const TDesC8& aMessage )
    {    
    CMSRPMessageHandler* self = new (ELeave) CMSRPMessageHandler( aMessageObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aMessage );
    return self;
    }

CMSRPMessageHandler* CMSRPMessageHandler::NewLC( TMSRPMessageType aMessageType )
    {
    CMSRPMessageHandler* self = new (ELeave) CMSRPMessageHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aMessageType );
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::ConstructL( const TDesC8& aMessage )
    {
    MSRPLOG( "CMSRPMessageHandler::ConstructL enter" )
    RDesReadStream readStream( aMessage );

    if ( CMSRPMessage::IsMessage( aMessage ) )
        {
        iMessage = CMSRPMessage::InternalizeL( readStream );
        if( iMessage->IsContent() )
            {
            iContentPtr.Set(iMessage->Content());
            }
        }    
    else if ( CMSRPReport::IsReport( aMessage ) )
        {
        iReport = CMSRPReport::InternalizeL( readStream );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    iBuffer.CreateL( KMaxBufferSize );
    iState = EIdle;
    MSRPStrings::OpenL();
    
    iFs.Connect();
    MSRPLOG( "CMSRPMessageHandler::ConstructL exit" )
    }


void CMSRPMessageHandler::ConstructL( TMSRPMessageType aMessageType )
    {
    MSRPLOG2( "CMSRPMessageHandler::ConstructL enter - MessageType %d", aMessageType )
    if( aMessageType == EMSRPMessage )
        {
        iMessage = new ( ELeave ) CMSRPMessage();
        }
    else if( aMessageType == EMSRPResponse )
        {
        iResponse = new ( ELeave ) CMSRPResponse();
        }
    else if( aMessageType == EMSRPReport )
        {
        // Since Reports are not supported now.. jus create a message class
        iReport = new ( ELeave ) CMSRPReport();            
        }
    else
        {
        // Maybe AUTH or some unrecognized request. For now just create a message class
        iMessage = new ( ELeave ) CMSRPMessage();
        }
    
    iActiveMsgType = aMessageType;
    iBuffer.CreateL( KSmallBuffer );
    iState = EIdle;    
    MSRPStrings::OpenL();
    iFs.Connect();
    MSRPLOG( "CMSRPMessageHandler::ConstructL exit" )
    }


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPMessageHandler::CMSRPMessageHandler( MMSRPMessageObserver* aMessageObserver )
    {
    iMSRPMessageObserver = aMessageObserver;
    }

CMSRPMessageHandler::CMSRPMessageHandler( )
    {
    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPMessageHandler::~CMSRPMessageHandler( )
    {
    MSRPLOG2( "-> CMSRPMessageHandler::~CMSRPMessageHandler = %d", this )
    delete iMessage;
    delete iResponse;
    delete iFileBuffer;
    iSentChunks.ResetAndDestroy();
    iSentChunks.Close();
    iBuffer.Close();
    MSRPStrings::Close();
    iFile.Close();
    MSRPLOG( "<- CMSRPMessageHandler::~CMSRPMessageHandler" )
    }


void CMSRPMessageHandler::AddHeaderL( TMsrpHeaderType aHeaderType, TPtrC8& aHeaderValue, TPtrC8& /*aHeaderTypeAndValue*/ )
    {
    MSRPLOG( "CMSRPMessageHandler::AddHeaderL enter" )
    CMSRPMessageBase* messageBase = NULL;
    if( iResponse )
        {
        messageBase = iResponse;
        }
    else if ( iReport )
        {
        messageBase = iReport;
        }
    else
        {
        messageBase = iMessage;
        }
        
    TInt err = KErrNone;
    switch( aHeaderType )
        {
        case EToPath:        
            {
            messageBase->SetToPathHeader( CMSRPToPathHeader::DecodeL( aHeaderValue ) );
            break;
            }
        case EFromPath:      
            {
            messageBase->SetFromPathHeader( CMSRPFromPathHeader::DecodeL( aHeaderValue ) );
            break;
            }
        case EMessageId:     
            {
            messageBase->SetMessageIdHeader( CMSRPMessageIdHeader::NewL( aHeaderValue ) );
            break;
            }
        case EByteRange:     
            {
            TInt startPos(0);
            TInt endPos(0);
            TInt total(0); 
            err =  ExtractByteRangeParams( aHeaderValue, startPos, endPos, total );
            messageBase->SetByteRangeHeader( CMSRPByteRangeHeader::NewL( startPos, endPos, total ));
            break;
            }
        case ESuccessReport: 
            {
            if( aHeaderValue == MSRPStrings::StringF( MSRPStrConsts::EYes ).DesC())
                {
                messageBase->SetSuccessReportHeader( 
                        CMSRPSuccessReportHeader::NewL( MSRPStrings::StringF( MSRPStrConsts::EYes ) ) );
                }
            else if( aHeaderValue == MSRPStrings::StringF( MSRPStrConsts::ENo ).DesC())
                {
                messageBase->SetSuccessReportHeader( 
                        CMSRPSuccessReportHeader::NewL( MSRPStrings::StringF( MSRPStrConsts::ENo ) ) );
                }  
            else
                {
                err = KErrArgument;
                }
            break;
            }                
        case EFailureReport: 
            {
            if( aHeaderValue == MSRPStrings::StringF( MSRPStrConsts::EYes ).DesC())
                {
                messageBase->SetFailureReportHeader( 
                        CMSRPFailureReportHeader::NewL( MSRPStrings::StringF( MSRPStrConsts::EYes ) ) );
                }
            else if( aHeaderValue == MSRPStrings::StringF( MSRPStrConsts::ENo ).DesC())
                {
                messageBase->SetFailureReportHeader( 
                        CMSRPFailureReportHeader::NewL( MSRPStrings::StringF( MSRPStrConsts::ENo ) ) );
                }
            else if( aHeaderValue == MSRPStrings::StringF( MSRPStrConsts::EPartial ).DesC())
                {
                messageBase->SetFailureReportHeader( 
                        CMSRPFailureReportHeader::NewL( MSRPStrings::StringF( MSRPStrConsts::EPartial ) ) );
                }
            else
                {
                err = KErrArgument;
                }
            break;
            }                
        case EContentType:   
            {
            messageBase->SetContentTypeHeader( CMSRPContentTypeHeader::NewL( aHeaderValue ) );
            break;
            }                
        case EStatus:        
            {
            TInt statusValue = TMSRPUtil::ConvertToNumber( aHeaderValue );
            messageBase->SetStatusHeader( CMSRPStatusHeader::NewL( statusValue ) );
            break; // required for reports only. Not handled now
            }              
            
        default:             
            {
            break;
            }                   
        }
    User::LeaveIfError(err);
    
    MSRPLOG( "CMSRPMessageHandler::AddHeaderL exit" )
    }

    
void CMSRPMessageHandler::AddContentL( TPtrC8& aContent, TBool aByteRangeFound )
    {
    MSRPLOG2( "CMSRPMessageHandler::AddContentL enter, instance = %d", this )
    if( iMessage )
        {
        if ( aByteRangeFound )
            {
            if( iBuffer.Length() + aContent.Length() > iBuffer.MaxLength() )
                {
                HBufC8* combined = HBufC8::NewLC( iBuffer.Length() + aContent.Length() );
                TPtr8 ptr = combined->Des();
                ptr = iBuffer;
                ptr.Append( aContent );
                // must write to file
                WriteMessageToFileL( ptr );
                CleanupStack::PopAndDestroy( ); // combined
                iBuffer.Zero();
                }
            else
                {
                iBuffer.Append( aContent );
                iCurrentNumberOfBytes = iBuffer.Length();
                }
            }
        else
            {
            AppendMessageToFileL( aContent );
            }
        }    
    MSRPLOG( "CMSRPMessageHandler::AddContentL exit" )
    }


void CMSRPMessageHandler::SetTransactionId( TDesC8& aTransactionId )
    {
    MSRPLOG( "CMSRPMessageHandler::SetTransactionId enter" )
    HBufC8* transactionId = HBufC8::NewL( aTransactionId.Length() );
    *transactionId = aTransactionId;
    iSentChunks.Append( transactionId );
    MSRPLOG( "CMSRPMessageHandler::SetTransactionId exit" )
    }


void CMSRPMessageHandler::SetStatusOfResponseL( TPtrC8& aStatusCode, TPtrC8& /*aStatusMessage*/ )
    {
    MSRPLOG( "CMSRPMessageHandler::SetStatusOfResponseL enter" )
    if( iResponse )
        {
        TUint statusCodeVal = TMSRPUtil::ConvertToNumber( aStatusCode );
        RStringF statusString = GetStatusStringL( statusCodeVal );                        
        iResponse->SetStatusCodeL( statusCodeVal );
        iResponse->SetReasonPhrase( statusString );
        }    
    MSRPLOG( "CMSRPMessageHandler::SetStatusOfResponseL exit" )
    }


void CMSRPMessageHandler::EndOfMessageL( TMsrpMsgEndStatus aStatus )
    {
    MSRPLOG2( "CMSRPMessageHandler::EndOfMessageL enter = %d", this )
    MSRPLOG2( "CMSRPMessageHandler::EndOfMessageL enter = %d", aStatus )
    iMessageEnding = aStatus;
    if( iMessage && iBuffer.Length() && ( aStatus == EMessageEnd ) )
        {
        if ( !iTempFileName.Length() )
            {
            HBufC8* contentOfMessage = HBufC8::NewL( iBuffer.Length() );
            *contentOfMessage = iBuffer;
            iMessage->SetContent( contentOfMessage );
            iBuffer.Zero();
            }
        else
            {
            WriteMessageToFileL( iBuffer );
            iBuffer.Zero();
            }
        }
    if ( aStatus == EMessageEnd )
        {
        iState = EMessageDone;
        }
    
    if( aStatus == EMessageTerminated )
        {
        iTerminateReceiving = ETrue;
        iState = ETerminated;
        }
        
    MSRPLOG( "CMSRPMessageHandler::EndOfMessageL exit" )
    }


MMSRPIncomingMessage::TMSRPMessageType CMSRPMessageHandler::MessageType( )
    {
    MSRPLOG( "CMSRPMessageHandler::MessageType enter" )
    MSRPLOG2("Message Type = %d", iActiveMsgType ); 
    return iActiveMsgType;
    }


CMSRPMessage* CMSRPMessageHandler::GetIncomingMessage( ) const
    {
    MSRPLOG( "CMSRPMessageHandler::GetIncomingMessage enter" )  
    return iMessage;
    }

void CMSRPMessageHandler::SendResponseL( MMSRPMessageObserver* aMessageObserver, 
                                            MMSRPConnection& aConnection, TUint aResponseCode )
    {
    MSRPLOG( "CMSRPMessageHandler::SendResponseL enter" )
        
    iMSRPMessageObserver = aMessageObserver;
    
    if( !aResponseCode )
        {
        aResponseCode = CheckValidityOfMessage( EMSRPMessage, iMessage );
        }
    
    TBool sendResponse = ETrue;
    if( iMessage->FailureReportHeader() )
        {
        HBufC8* frHeaderValue = iMessage->FailureReportHeader()->ToTextValueLC();
        if( frHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::ENo ).DesC() )
            {
            sendResponse = EFalse;
            }
        else if( (frHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::EPartial ).DesC()) 
                && (aResponseCode == EAllOk) )
            {
            sendResponse = EFalse;
            }
        CleanupStack::PopAndDestroy(frHeaderValue);
        }
    
    if( !iMessage->ToPathHeader() || !iMessage->FromPathHeader() )
        {
        aResponseCode = EUnknownCode;
        sendResponse = EFalse;
        }
    
    if(sendResponse)
        {
        RStringF statusString = GetStatusStringL( aResponseCode );
        iResponse = new ( ELeave ) CMSRPResponse( aResponseCode, statusString ) ;
        
        HBufC8* fromPathValue = iMessage->FromPathHeader()->ToTextValueLC();
        iResponse->SetToPathHeader( CMSRPToPathHeader::DecodeL( *fromPathValue ) );
        CleanupStack::PopAndDestroy(fromPathValue);
        
        HBufC8* toPathValue = iMessage->ToPathHeader()->ToTextValueLC();
        iResponse->SetFromPathHeader( CMSRPFromPathHeader::DecodeL( *toPathValue ) );
        CleanupStack::PopAndDestroy(toPathValue);   
        
        iActiveMsgType = EMSRPResponse;
        iState = EIdle;
        aConnection.SendL( *this );
        }
  
    MSRPLOG( "CMSRPMessageHandler::SendResponseL exit" )
    }

TBool CMSRPMessageHandler::SendReportL( 
    MMSRPMessageObserver* aMessageObserver, 
    MMSRPConnection& aConnection, TUint aStatusCode )
    {
    MSRPLOG( "CMSRPMessageHandler::SendReportL enter" )
    
    TBool sendReport = EFalse;
    if ( iActiveMsgType == EMSRPResponse )
        {
        // currently sending a response
        MSRPLOG( "CMSRPMessageHandler::SendReportL sendin a response..." )
        return sendReport;
        }
    iMSRPMessageObserver = aMessageObserver;
    if( iMessage->SuccessReportHeader() )
        {
        HBufC8* successHeaderValue = iMessage->SuccessReportHeader()->ToTextValueLC();
        if( successHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::EYes ).DesC() )
            {
            MSRPLOG( "CMSRPMessageHandler::SendReportL report needed!" )
            sendReport = ETrue;
            }
        CleanupStack::PopAndDestroy( successHeaderValue );
        }
        
    if ( sendReport )
        {
        iReport = new ( ELeave ) CMSRPReport( ) ;
        iReport->SetStatusHeader( CMSRPStatusHeader::NewL( aStatusCode ) );

        HBufC8* fromPathValue = iMessage->FromPathHeader()->ToTextValueLC();
        iReport->SetToPathHeader( CMSRPToPathHeader::DecodeL( *fromPathValue ) );
        CleanupStack::PopAndDestroy(fromPathValue);
        
        HBufC8* toPathValue = iMessage->ToPathHeader()->ToTextValueLC();
        iReport->SetFromPathHeader( CMSRPFromPathHeader::DecodeL( *toPathValue ) );
        CleanupStack::PopAndDestroy(toPathValue);   

        HBufC8* messageId = iMessage->MessageIdHeader()->ToTextValueLC();
        iReport->SetMessageIdHeader( CMSRPMessageIdHeader::NewL( *messageId ) );
        CleanupStack::PopAndDestroy( messageId );   
        
        TInt size( 0 );
        if ( iTempFileName.Length() )
            {
            OpenTemporaryFileL( iTempFileName );
            iTempFile->Size( size );
            }
        else
            {
            size = iBuffer.Length();
            }
        CMSRPByteRangeHeader* byteRange = 
                CMSRPByteRangeHeader::NewL( 1, size, size );
        iReport->SetByteRangeHeader( byteRange );
        
        iActiveMsgType = EMSRPReport;
        iState = EIdle;
        aConnection.SendL( *this );
        }
    MSRPLOG( "CMSRPMessageHandler::SendReportL exit" )
    return sendReport;
    }

TDesC8& CMSRPMessageHandler::TransactionId( )
    {
    MSRPLOG( "CMSRPMessageHandler::TransactionId enter" )
    return *iSentChunks[ iSentChunks.Count() - 1 ];
    }


CMSRPResponse* CMSRPMessageHandler::GetIncomingResponse( ) const
    {
    MSRPLOG( "CMSRPMessageHandler::GetIncomingResponse enter" )
    return iResponse;
    }

CMSRPReport* CMSRPMessageHandler::GetIncomingReport( ) const
    {
    MSRPLOG( "-> CMSRPMessageHandler::GetIncomingReport" )
    return iReport;
    }

void CMSRPMessageHandler::SendMessageL( MMSRPConnection& aConnection )
    {
    MSRPLOG( "CMSRPMessageHandler::SendMessageL enter" )    
    iActiveMsgType = EMSRPMessage;
    isSending = ETrue;
    if ( iMessage->IsFile() )
        {
        User::LeaveIfError(iFile.Open(iFs, iMessage->GetFileName(), EFileShareReadersOrWriters)); 
        iFile.Size(iFileSize);
        iFileBuffer = HBufC8::NewL(KMaxChunkReadSize);            
        FillFileBufferL();
        }
    else
        {
        iContentPtr.Set( iMessage->Content() );
        }
        
    aConnection.SendL( *this );
    MSRPLOG( "CMSRPMessageHandler::SendMessageL exit" )
    }

TBool CMSRPMessageHandler::IsOwnerOfResponse( MMSRPIncomingMessage& aIncomingMessage )
    {
    MSRPLOG2( "CMSRPMessageHandler::IsOwnerOfResponse enter =%d", this )
    for ( TInt i = 0; i < iSentChunks.Count(); i++ )
        {
        if( aIncomingMessage.TransactionId() == *iSentChunks[ i ] )
            {
            MSRPLOG( "CMSRPMessageHandler::IsOwnerOfResponse enter, yes" )
            iResponseNeeded = EFalse;
            return ETrue;
            }  
        }
              
    MSRPLOG( "CMSRPMessageHandler::IsOwnerOfResponse exit" )
    return EFalse;
    }

TInt CMSRPMessageHandler::FillFileBufferL()
    {    
    MSRPLOG( "CMSRPMessageHandler::FillFileBufferL enter" )
    iStartPosInBuffer = 0;
    iEndPosInBuffer = 0;
    iBufPosInFile+=iFileBuffer->Length();
    iFileBuffer->Des().Zero();
    TPtr8 ptr(iFileBuffer->Des());
    User::LeaveIfError(iFile.Read(iBufPosInFile, ptr, KMaxChunkReadSize));
    MSRPLOG2( "CMSRPMessageHandler::FillFileBufferL File Buffer Length %d", iFileBuffer->Length());
    MSRPLOG( "CMSRPMessageHandler::FillFileBufferL exit" )     
	return iFileBuffer->Length();
    }

void CMSRPMessageHandler::TerminateReceiving( 
    MMSRPMessageObserver* aMessageObserver, 
    MMSRPConnection& aConnection )
    {
    MSRPLOG( "CMSRPMessageHandler::SetFSTerminate" )
    iTerminateReceiving = ETrue;

    SendResponseL( aMessageObserver, aConnection, EStopSending );
    }

void CMSRPMessageHandler::TerminateSending()
    {
    MSRPLOG( "CMSRPMessageHandler::SetFSTerminate" )
    iTerminateSending = ETrue;
    }

TBool CMSRPMessageHandler::IsTransmissionTerminated( )
    {
    MSRPLOG3("-> CMSRPMessageHandler::IsTransmissionTerminated, %d and %d", iTerminateReceiving, iTerminateSending )
    if ( iTerminateReceiving || iTerminateSending )
        {
        return ETrue;
        }
        
    return EFalse;
    }

TBool CMSRPMessageHandler::IsReportNeeded( )
    {
    if( iMessage->SuccessReportHeader() )
        {
        HBufC8* successHeaderValue = iMessage->SuccessReportHeader()->ToTextValueLC();
        if( successHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::EYes ).DesC() )
            {
            CleanupStack::PopAndDestroy( successHeaderValue );
            return ETrue;
            }
        CleanupStack::PopAndDestroy( successHeaderValue );
        }
        
    return EFalse;
    }

TUint CMSRPMessageHandler::ConsumeResponseL( MMSRPIncomingMessage& aIncomingMessage )
    {
    MSRPLOG2( "CMSRPMessageHandler::ConsumeResponseL enter, this = %d", this )
   
    TUint responseCode( EUnknownCode );
    CMSRPResponse* response = aIncomingMessage.GetIncomingResponse();
    if ( response )
        {
        RStringF statusString = response->ReasonPhrase();
        responseCode = response->StatusCode();
        delete iResponse;
        iResponse = NULL;
        iResponse = new (ELeave) CMSRPResponse( responseCode, statusString );
        HBufC8* toPathValue = response->ToPathHeader()->ToTextValueLC();
        HBufC8* fromPathValue = response->FromPathHeader()->ToTextValueLC();
        iResponse->SetToPathHeader( CMSRPToPathHeader::DecodeL( toPathValue->Des() ) );
        iResponse->SetFromPathHeader( CMSRPFromPathHeader::DecodeL( fromPathValue->Des() ) );
        CleanupStack::PopAndDestroy(fromPathValue);
        CleanupStack::PopAndDestroy(toPathValue);
        }

    // removing the transaction id
    for ( TInt i = 0; i < iSentChunks.Count(); i++ )
        {
        if( aIncomingMessage.TransactionId() == *iSentChunks[ i ] )
            {
            delete iSentChunks[ i ];
            iSentChunks.Remove( i );
            }  
        }

    if ( iState == EWaitingForResponse )
        {
        iState = EMessageDone;
        }
       
    MSRPLOG( "CMSRPMessageHandler::ConsumeResponseL exit" )
    return responseCode;
    }


TBool CMSRPMessageHandler::IsMessageComplete()
    {
    MSRPLOG2( "CMSRPMessageHandler::IsMessageComplete enter= %d", this )
    MSRPLOG2( "CMSRPMessageHandler::IsMessageComplete enter= %d", iMessageEnding )
    MSRPLOG2( "CMSRPMessageHandler::IsMessageComplete enter= %d", iState )
    if ( iMessageEnding != EMessageEnd && iMessageEnding != EMessageTerminated )
        {
        return EFalse;
        }
    else if( iState == EMessageDone || iState == ETerminated )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


TUint CMSRPMessageHandler::GetSendResultL( HBufC8* &aMessageId )
    {
    MSRPLOG( "CMSRPMessageHandler::GetSendResult enter" )
    TUint aErrorCode( EUnknownCode );
    aMessageId = iMessage->MessageIdHeader()->ToTextValueLC();
    CleanupStack::Pop(aMessageId);
    if(iResponse)
        {
        aErrorCode = iResponse->StatusCode();
        }
        
    return aErrorCode;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::IsFailureHeaderPartial
// -----------------------------------------------------------------------------
//
TBool CMSRPMessageHandler::IsFailureHeaderPartial()
    {
    if( iMessage->FailureReportHeader() )
        {
        HBufC8* frHeaderValue = iMessage->FailureReportHeader()->ToTextValueLC();
        if( frHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::EPartial ).DesC() ) 
            {
            CleanupStack::PopAndDestroy(frHeaderValue);
            return ETrue;
            }
        CleanupStack::PopAndDestroy(frHeaderValue);
        }
        
    return EFalse;
    }

const TDesC8& CMSRPMessageHandler::GetSendBufferL( 
    MMSRPWriterObserver::TWriteStatus& aStatus, 
    TBool aInterrupt )
    {
    MSRPLOG( "CMSRPMessageHandler::GetSendBufferL enter" )
    iInterrupt = aInterrupt;
    iWriteDone = EFalse;
    iBuffer.Zero();
    
    while( !iWriteDone )
        {
        if( iState == EIdle )
            {
            if( iActiveMsgType == EMSRPMessage )
                {
                CreateByteRangeHeaderL();
                }
            WriteInitialLineToBufferL();
            WriteHeadersToBufferL();
            }
            
        else if( iState == EInProgress )
            {
            if( iMessage->IsFile())
                {
                if (iActiveMsgType == EMSRPMessage)
                    WriteFileContentToBufferL();
                else
                    WriteEndLineToBuffer();
                }            
            else
                {
                if( iContentPtr.Length() )
                    WriteContentToBuffer();
                else
                    WriteEndLineToBuffer();
                }
            }
        else
            {
             User::LeaveIfError(KErrArgument);
            }
        }
    
    aStatus = EMsrpSocketWrite;
    MSRPLOG( "CMSRPMessageHandler::GetSendBufferL exit" )
    return iBuffer;
    }


MMSRPWriterObserver::TMsgStatus CMSRPMessageHandler::WriteDoneL( TInt aStatus )
    {
    MSRPLOG( "CMSRPMessageHandler::WriteDoneL enter" )
    MSRPLOG2( "CMSRPMessageHandler::WriteDoneL state = %d", iState )
    MSRPLOG2( "CMSRPMessageHandler::WriteDoneL activestate = %d", iActiveMsgType )
    MMSRPWriterObserver::TMsgStatus retStatus = EUndefined;

    if( !aStatus )
        {
        if( iState == EMessageSent )
            {
            switch( iActiveMsgType )
                {
                case EMSRPMessage:
                    {
                    if( iResponseNeeded )
                        {
                        iState = EWaitingForResponse;
                        }
                    else
                        {
                        iState = EMessageDone;
                        }  
                    iMSRPMessageObserver->MessageSendProgressL( this );   
                    iMSRPMessageObserver->MessageSendCompleteL( this );
                    retStatus = MMSRPWriterObserver::EComplete;
                    break;                    
                    }
                case EMSRPResponse:
                    {
                    if( iTerminateReceiving )
                        {
                        iState = ETerminated;
                        iMessageEnding = EMessageTerminated;
                        }
                    else
                        {
                        iState = EMessageDone;
                        }
                    iActiveMsgType = EMSRPNotDefined;
                    iMSRPMessageObserver->MessageResponseSendCompleteL(*this);
                    if ( IsReportNeeded() )
                        {
                        retStatus = MMSRPWriterObserver::ESendingReport;
                        }
                    else
                        {
                        retStatus = MMSRPWriterObserver::EComplete;
                        }
                    break;
                    }
                case EMSRPReport:
                    {
                    iMessageEnding = EMessageEnd;
                    iState = EMessageDone;
                    iMSRPMessageObserver->MessageReportSendCompleteL( *this );
                    retStatus = MMSRPWriterObserver::EComplete;
                    break;
                    }
                }
            }        
        else if( iState ==  EInProgress )
            {
            iMSRPMessageObserver->MessageSendProgressL( this ); 
            if ( iMessageEnding == EMessageNotDefined )
                {
                retStatus = MMSRPWriterObserver::EPending;
                }
            else
                {
                iState = EIdle;
                retStatus = MMSRPWriterObserver::EWaitingForResponse;
                }
            }
        else if( iState == ETerminated )
            {
            if( iResponseNeeded )
                {
                iState = EWaitingForResponse;
                }
                                        
            iMSRPMessageObserver->MessageCancelledL();
            retStatus = MMSRPWriterObserver::EComplete;
            }
        else
           {
           iMSRPMessageObserver->WriterError();
           }
        }

    return retStatus;
    }


void CMSRPMessageHandler::CreateByteRangeHeaderL()
    {
    MSRPLOG( "CMSRPMessageHandler::CreateByteRangeHeaderL enter" )
    CMSRPByteRangeHeader* byteRange = NULL;
       
    if ( iMessage->IsFile())
        {                
        byteRange = CMSRPByteRangeHeader::NewL( iBufPosInFile+iStartPosInBuffer+1,KUnknownRange,iFileSize);
        }
    else
        {
        if ( iMessage->IsContent() )
             {
             byteRange = CMSRPByteRangeHeader::NewL( 1,
             iMessage->Content().Length(), iMessage->Content().Length() );
             }
        else
             {
             byteRange = CMSRPByteRangeHeader::NewL( 1, 0, 0 );
             }
        }
    iMessage->SetByteRangeHeader( byteRange );
    MSRPLOG( "CMSRPMessageHandler::CreateByteRangeHeaderL exit" )
    }


void CMSRPMessageHandler::CreateTransactionIdL()
    {
    MSRPLOG( "CMSRPMessageHandler::CreateTransactionId enter" )
    
    TTime now;
    now.HomeTime();
    TInt64 seed = now.Int64();
    TInt random = Math::Rand( seed );
    HBufC8* transactionId = HBufC8::NewL( KMaxLengthOfTransactionIdString );
    TPtr8 temp( transactionId->Des() );
    temp.NumUC( random );
    
    if ( iActiveMsgType != EMSRPReport )
        {
        // let's make sure the message content does not include the end line
        // otherwise we need to regenerate the random number
        TBuf8< KMaxLengthOfMessageEndString > endLine;
        endLine.Append( KAsterisk );
        endLine.Append( KDashLine );
        endLine.Append( temp );
        endLine.Append( KAsterisk );
        
        if ( iMessage->IsFile() )
            {
            // must first load the right chunk to memory temporarily
            RFile file;
            CleanupClosePushL( file );
            User::LeaveIfError(
                file.Open( iFs, iMessage->GetFileName(), EFileShareReadersOrWriters ) );
        
            // Check that the file does not contain the generated ID
            TInt readPosition( 0 );
            HBufC8* tempFileBuffer = HBufC8::NewLC( KMaxChunkReadSize );
            TPtr8 fileBuffer( tempFileBuffer->Des() );
            User::LeaveIfError(
                file.Read( readPosition, fileBuffer, KMaxChunkReadSize ) );
            while ( fileBuffer.Length() )
                {
                if ( fileBuffer.Match( endLine ) != KErrNotFound )
                    {
                    // found
                    random = Math::Rand( seed );
                    temp.NumUC( random );
                    endLine.Zero();
                    endLine.Append( KAsterisk );
                    endLine.Append( KDashLine );
                    endLine.Append( temp );
                    endLine.Append( KAsterisk );
                    readPosition = 0;
                    }
                else
                    {
                    readPosition += ( KMaxChunkReadSize - KMaxLengthOfMessageEndString );
                    }
                User::LeaveIfError(
                    file.Read( readPosition, fileBuffer, KMaxChunkReadSize ) );
                }
            CleanupStack::PopAndDestroy( tempFileBuffer );
            CleanupStack::PopAndDestroy( ); // file
            }
        else
            {
            // content
            while ( iMessage->Content().Match( temp ) != KErrNotFound )
                {
                // found
                random = Math::Rand( seed );
                temp.NumUC( random );
                endLine.Zero();
                endLine.Append( KAsterisk );
                endLine.Append( KDashLine );
                endLine.Append( temp );
                endLine.Append( KAsterisk );
                }
            }
        }
    iSentChunks.Append( transactionId );
    
    MSRPLOG( "CMSRPMessageHandler::CreateTransactionId exit" )
    }


void CMSRPMessageHandler::WriteInitialLineToBufferL()
    {
    MSRPLOG( "CMSRPMessageHandler::WriteInitialLineToBufferL enter" )
    
    if( iActiveMsgType == EMSRPMessage )
        {
        iMessageEnding = EMessageNotDefined;
        }
    iBuffer.Append( MSRPStrings::StringF( MSRPStrConsts::EMSRP ).DesC() );

    iBuffer.Append( KSpaceChar );
    
    if (! (iActiveMsgType == EMSRPResponse) )
         {
         CreateTransactionIdL();
         }
   
    iBuffer.Append( *iSentChunks[ iSentChunks.Count() -1 ] );

    iBuffer.Append( KSpaceChar );
    
    if ( iActiveMsgType == EMSRPMessage )
        {
        iBuffer.Append( MSRPStrings::StringF( MSRPStrConsts::ESend ).DesC() );
        }
    else if ( iActiveMsgType == EMSRPResponse )
        {
        TBuf8< KMaxLengthOfStatusCode > statusCode;
        statusCode.NumUC( iResponse->StatusCode() );
        iBuffer.Append( statusCode );
        iBuffer.Append( KSpaceChar );
        iBuffer.Append( iResponse->ReasonPhrase().DesC() );
        }
    else if ( iActiveMsgType == EMSRPReport )
        {
        iBuffer.Append( MSRPStrings::StringF( MSRPStrConsts::EReport ).DesC() );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    iBuffer.Append( KCRAndLF );
    
    iState = EInProgress;
    MSRPLOG( "CMSRPMessageHandler::WriteInitialLineToBufferL exit" )    
    }


void CMSRPMessageHandler::WriteHeadersToBufferL()
    {
    MSRPLOG( "CMSRPMessageHandler::WriteHeadersToBufferL enter" )
    CMSRPMessageBase* messageBase = NULL;
    if( iActiveMsgType == EMSRPMessage )
        {
        messageBase = iMessage;
        }
    else if ( iActiveMsgType == EMSRPResponse )
        {
        messageBase = iResponse;
        }
    else if ( iActiveMsgType == EMSRPReport )
        {
        messageBase = iReport;
        }
    else
        {
        User::Leave( KErrArgument );
        }
            
    // Check if appending headers will exceed max size
    
    __ASSERT_ALWAYS( messageBase->ToPathHeader(), User::Leave( KErrNotFound ) );
    iBuffer.Append( messageBase->ToPathHeader()->ToTextLC()->Des() );
    CleanupStack::PopAndDestroy(); // above
    iBuffer.Append( KCRAndLF );

    __ASSERT_ALWAYS( messageBase->FromPathHeader(), User::Leave( KErrNotFound ) );
    iBuffer.Append( messageBase->FromPathHeader()->ToTextLC()->Des() );
    CleanupStack::PopAndDestroy(); // above
    iBuffer.Append( KCRAndLF );
    
    if( iActiveMsgType != EMSRPResponse )
        {
        __ASSERT_ALWAYS( messageBase->MessageIdHeader(), User::Leave( KErrNotFound ) );
        iBuffer.Append( messageBase->MessageIdHeader()->ToTextLC()->Des() );
        CleanupStack::PopAndDestroy(); // above
        iBuffer.Append( KCRAndLF );
        }
    
    if( iActiveMsgType == EMSRPMessage )
        {
        if ( messageBase->SuccessReportHeader() )
            {
            iBuffer.Append( messageBase->SuccessReportHeader()->ToTextLC()->Des() );
            CleanupStack::PopAndDestroy(); // above 
            iBuffer.Append( KCRAndLF );
            }

        if ( messageBase->FailureReportHeader() )
            {
            HBufC8* valueString = messageBase->FailureReportHeader()->ToTextValueLC();
            if ( valueString->Des() == MSRPStrings::StringF( MSRPStrConsts::EYes ).DesC() )
                {
                iResponseNeeded = ETrue;
                }
            CleanupStack::PopAndDestroy(valueString);

            iBuffer.Append( messageBase->FailureReportHeader()->ToTextLC()->Des() );
            CleanupStack::PopAndDestroy(); // above
            iBuffer.Append( KCRAndLF );
            }
        else
            {
            iResponseNeeded = ETrue;
            }
        }
    
    if( iActiveMsgType != EMSRPResponse )
        {
        iBuffer.Append( messageBase->ByteRangeHeader()->ToTextLC()->Des() );
        CleanupStack::PopAndDestroy(); // above 
        iBuffer.Append( KCRAndLF );
        }

    if( iActiveMsgType == EMSRPReport )
        {
        iBuffer.Append( messageBase->StatusHeader()->ToTextLC()->Des() );
        CleanupStack::PopAndDestroy(); // above 
        iBuffer.Append( KCRAndLF );
        }
        
    if( iActiveMsgType == EMSRPMessage )
        {
        if ( messageBase->ContentTypeHeader() )
            {
            iBuffer.Append( messageBase->ContentTypeHeader()->ToTextLC()->Des() );
            CleanupStack::PopAndDestroy(); // above 
            iBuffer.Append( KCRAndLF );
            iBuffer.Append( KCRAndLF );
            
            if(!iMessage->IsFile() && !iMessage->IsContent()) 
                {
                iBuffer.Append( KCRAndLF );
                }            
            }
        }
        
    MSRPLOG( "CMSRPMessageHandler::WriteHeadersToBufferL exit" )
    }


void CMSRPMessageHandler::WriteFileContentToBufferL()
    {
    MSRPLOG( "CMSRPMessageHandler::WriteFileContentToBuffer enter" )
    TInt endlineSize = iSentChunks[ iSentChunks.Count() -1 ]->Length() + KEndlineConstLength;
    TInt remBufferSize = iBuffer.MaxLength() - iBuffer.Length() - endlineSize - KCRAndLF().Length();
    TInt chunkLength = iFileBuffer->Length() - iEndPosInBuffer;
    
    if(chunkLength > remBufferSize)
        {
        iFileBytesSent = remBufferSize;
        iBuffer.Append(iFileBuffer->Mid(iEndPosInBuffer, iFileBytesSent));
        iEndPosInBuffer += iFileBytesSent;
        
        if( iTerminateSending )
            {
            iBuffer.Append(KCRAndLF());
            WriteEndLineToBuffer( EMessageTerminated );
            iStartPosInBuffer = iEndPosInBuffer;
            iState = ETerminated;
            }
        else if(iInterrupt)
            {
            iBuffer.Append(KCRAndLF());
            WriteEndLineToBuffer(EMessageContinues);
            iStartPosInBuffer = iEndPosInBuffer;
            }        
       /* else
            {
            //msg state remains InProgress (in write done return pending)
            }*/
        }
    else //bytes completed in current file buffer
        {
        iFileBytesSent = chunkLength;
        iBuffer.Append(iFileBuffer->Mid(iEndPosInBuffer, iFileBytesSent));
        iEndPosInBuffer += iFileBytesSent;
        
        if ( FillFileBufferL() == 0 )
            {
            iBuffer.Append( KCRAndLF( ) );
            WriteEndLineToBuffer( EMessageEnd );
            iState = EMessageSent;
            }
        
        if( iTerminateSending && iState != EMessageSent )
            {
            iBuffer.Append( KCRAndLF( ) );
            WriteEndLineToBuffer( EMessageTerminated );
            iState = ETerminated;
            }
        }
#if 0
/* This is an alternative piece of code which writes the message in chunks. 
 * The size of chunk is determined by the size of the iBuffer
 */
    else //bytes completed in current file buffer
        {
        iFileBytesSent = chunkLength;
        iBuffer.Append(iFileBuffer->Mid(iEndPosInBuffer, iFileBytesSent));
        iEndPosInBuffer += iFileBytesSent;
        iBuffer.Append(KCRAndLF());
        
        if(iTerminateFS)
            {
            WriteEndLineToBuffer(EMessageTerminated);
            iState = ETerminated;
            }
        else if(FillFileBufferL() > 0)
            {
            //next file buffer has data
            WriteEndLineToBuffer(EMessageContinues);
            }
        else
            {
            WriteEndLineToBuffer(EMessageEnd);
            iState = EMessageSent;
            }
        }
#endif
    
    iWriteDone = ETrue;
    MSRPLOG( "CMSRPMessageHandler::WriteFileContentToBuffer exit" )
    }


void CMSRPMessageHandler::WriteContentToBuffer()
    {
    MSRPLOG( "CMSRPMessageHandler::WriteContentToBuffer enter" )
    
    TInt remBufferSize = iBuffer.MaxLength() - iBuffer.Length();        
    TInt remContentLength = iContentPtr.Length() + KCRAndLF().Length();
    
    if( remContentLength > remBufferSize )
        {
        iBuffer.Append( iContentPtr.Mid(0, remBufferSize) );
        iContentPtr.Set(iContentPtr.Mid(remBufferSize));
        iWriteDone = ETrue;
        }
    else
        {
        iBuffer.Append( iContentPtr );
        iContentPtr.Set(KNullDesC8);
        iBuffer.Append( KCRAndLF );
        }
            
    MSRPLOG( "CMSRPMessageHandler::WriteContentToBuffer exit" )
    }


void CMSRPMessageHandler::WriteEndLineToBuffer(MMSRPMessageHandler::TMsrpMsgEndStatus aEndFlag)
    {
    MSRPLOG( "CMSRPMessageHandler::WriteEndLineToBuffer enter" )
        
    TInt remBufferSize = iBuffer.MaxLength() - iBuffer.Length();        
    TInt endlineSize = iSentChunks[ iSentChunks.Count() -1 ]->Length() + KEndlineConstLength;
    
    if( endlineSize <= remBufferSize )
        {
        iBuffer.Append( KDashLine );
        iBuffer.Append( *iSentChunks[ iSentChunks.Count() -1 ] );
        
        if(iActiveMsgType == EMSRPResponse)
            {
            iBuffer.Append( KMessageEndSign );
            iState = EMessageSent;
            }
        else
            {
            if( aEndFlag == EMessageTerminated )
                {
                MSRPLOG( "CMSRPMessageHandler::WriteEndLineToBuffer - Appending #" )    
                iBuffer.Append( KMessageTerminatedSign );
                iMessageEnding = EMessageTerminated;
                }
            else if( aEndFlag == EMessageContinues )
                {
                MSRPLOG( "CMSRPMessageHandler::WriteEndLineToBuffer - Appending +" )
                iBuffer.Append( KMessageContinuesSign );
                iMessageEnding = EMessageContinues;
                }
            else
                {
                MSRPLOG( "CMSRPMessageHandler::WriteEndLineToBuffer - Appending $" )
                iBuffer.Append( KMessageEndSign );
                iMessageEnding = EMessageEnd;
                iState = EMessageSent;
                }
            }
        iBuffer.Append( KCRAndLF );        
        }
        
    iWriteDone = ETrue;
    MSRPLOG( "CMSRPMessageHandler::WriteEndLineToBuffer exit" )
    }


TInt CMSRPMessageHandler::ExtractByteRangeParams( TDesC8& aHeaderValue, TInt& aStartPos, TInt& aEndPos, TInt& aTotal )
    {        
    MSRPLOG( "CMSRPMessageHandler::ExtractByteRangeParams exit" )
    
    TChar dashCharacter( KDashCharacter );
    TInt position1 = aHeaderValue.Locate( dashCharacter );
    if ( position1 == KErrNotFound )
        {
        // no data
        return KErrArgument;
        }
    TPtrC8 startPosDes = aHeaderValue.Left( position1 );
    TChar divideCharacter( KDividedCharacter );
    TInt position2 = aHeaderValue.Locate( divideCharacter );
    if ( position2 == KErrNotFound )
        {
        // no data
        return KErrArgument;
        }
    TPtrC8 endPosDes = aHeaderValue.Mid( position1 + 1, position2 - ( position1 + 1 ) );
    TPtrC8 totalDes = aHeaderValue.Mid( position2 + 1 );

    aStartPos = TMSRPUtil::ConvertToNumber( startPosDes );
    aEndPos = TMSRPUtil::ConvertToNumber( endPosDes );
    aTotal = TMSRPUtil::ConvertToNumber( totalDes );
    
    MSRPLOG( "CMSRPMessageHandler::ExtractByteRangeParams exit" )
    return KErrNone;
    }


RStringF CMSRPMessageHandler::GetStatusStringL( TUint aStatusCode )
    {
    MSRPLOG( "CMSRPMessageHandler::GetStatusString enter" )
    RStringF statusString;
    CleanupClosePushL( statusString );
    
    switch( aStatusCode )
        {
        case EAllOk:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EAllOk );
            break;
            }
        case EUnintelligibleRequest:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EUnintelligibleRequest );
            break;
            }
        case EActionNotAllowed:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EActionNotAllowed );
            break;
            }
        case ETimeout:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::ETimeout );
            break;
            }
        case EStopSending:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EStopSending );
            break;
            }
        case EMimeNotUnderstood:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EMimeNotUnderstood );
            break;
            }
        case EParameterOutOfBounds:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EParameterOutOfBounds );
            break;
            }
        case ESessionDoesNotExist:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::ESessionDoesNotExist );
            break;
            }
        case EUnknownRequestMethod:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EUnknownRequestMethod );
            break;
            }
        case ESessionAlreadyBound:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::ESessionAlreadyBound );
            break;
            }
        default:
            {
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( );
    MSRPLOG( "CMSRPMessageHandler::GetStatusString exit" )
    return statusString;
    }


TUint CMSRPMessageHandler::CheckValidityOfMessage( TMSRPMessageType aMessageType, CMSRPMessageBase* aMessage )
    {
    MSRPLOG( "CMSRPMessageHandler::CheckValidityOfMessage enter" )
        
    if( aMessageType == EMSRPMessage )
        {
        if(!aMessage->MessageIdHeader())
            {
            return EUnintelligibleRequest;
            }
        CMSRPMessage* message = static_cast<CMSRPMessage*>(aMessage);
        if(message->IsContent() && !message->ContentTypeHeader())
            {
            return EUnintelligibleRequest;
            }
        }
    
    MSRPLOG( "CMSRPMessageHandler::CheckValidityOfMessage exit" )
    return EAllOk;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::OpenTemporaryFileL
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::OpenTemporaryFileL( const TDesC& aFilename )
    {
    MSRPLOG( "CMSRPMessageHandler::OpenTemporaryFileL enter" )

    iTempFile = new ( ELeave ) RFile();
    // create temporary filename
    iTempFile->Open( iFs, aFilename, EFileShareExclusive | EFileWrite );

    MSRPLOG2( "CMSRPMessageHandler::OpenTemporaryFileL exit, filename = %S", &iTempFileName )
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::CreateTemporaryFileL
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::CreateTemporaryFileL( )
    {
    MSRPLOG( "CMSRPMessageHandler::CreateTemporaryFileL enter" )

    iTempFile = new ( ELeave ) RFile();
    // create temporary filename
    User::LeaveIfError( iTempFile->Temp(
        iFs, KDefaultTempFilePath, iTempFileName, EFileShareExclusive | EFileWrite ) );
    iMessage->SetFileName( iTempFileName );

    MSRPLOG2( "CMSRPMessageHandler::CreateTemporaryFileL exit, filename = %S", &iTempFileName )
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::WriteMessageToFileL
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::WriteMessageToFileL( TDesC8& aBuffer )
    {
    MSRPLOG( "CMSRPSocketReader::WriteMessageToFileL enter" )

    if ( !iTempFileName.Length() )
        {
        CreateTemporaryFileL();
        }
    else
        {
        OpenTemporaryFileL( iTempFileName );
        }

    MSRPLOG2( "CMSRPSocketReader::WriteMessageToFileL writing to file = %S", &iMessage->GetFileName() );

    TInt fileSize;
    iTempFile->Size( fileSize );
    if ( iMessage->ByteRangeHeader()->StartPosition() > fileSize )
        {
        iTempFile->SetSize(
            iMessage->ByteRangeHeader()->StartPosition() );
        }
    iTempFile->Size( fileSize );

    iTempFile->Write(
        iMessage->ByteRangeHeader()->StartPosition() - 1, aBuffer );
    MSRPLOG2( "CMSRPSocketReader::WriteMessageToFileL writing to pos = %d", iMessage->ByteRangeHeader()->StartPosition() - 1 );
    iTempFile->Size( iCurrentNumberOfBytes );
    iTempFile->Close();
    delete iTempFile;
    iTempFile = NULL;
                        
    MSRPLOG( "CMSRPSocketReader::WriteMessageToFileL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::AppendMessageToFileL
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::AppendMessageToFileL( TDesC8& aBuffer )
    {
    MSRPLOG( "CMSRPSocketReader::AppendMessageToFileL enter" )
    OpenTemporaryFileL( iMessage->GetFileName() );

    TInt filePos( 0 );
    iTempFile->Seek( ESeekEnd, filePos );

TInt fileSize;
iTempFile->Size( fileSize );
MSRPLOG2( "CMSRPSocketReader::AppendMessageToFileL writing to pos = %d", fileSize );

    iTempFile->Write( aBuffer );
    iTempFile->Size( iCurrentNumberOfBytes );
    iTempFile->Close();
    delete iTempFile;
    iTempFile = NULL;

    MSRPLOG( "CMSRPSocketReader::AppendMessageToFileL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::MessageId
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPMessageHandler::MessageIdLC( )
    {
    return iMessage->MessageIdHeader()->ToTextValueLC();
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::CurrentReceiveProgress
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::CurrentReceiveProgress( TInt& aBytesTransferred, TInt& aTotalBytes )
    {
    aBytesTransferred = iCurrentNumberOfBytes;
    aTotalBytes = iMessage->ByteRangeHeader()->TotalLength(); 
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::CurrentSendProgress
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::CurrentSendProgress( TInt& aBytesTransferred, TInt& aTotalBytes )
    {
    if ( iBufPosInFile )
        {
        aBytesTransferred = iBufPosInFile;
        aTotalBytes = iFileSize; 
        }
    else
        {
        aBytesTransferred = iMessage->Content().Length() - iContentPtr.Length();
        aTotalBytes = iMessage->Content().Length(); 
        }
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::CheckMessageChunk
// -----------------------------------------------------------------------------
//
TBool CMSRPMessageHandler::CheckMessageChunkL( CMSRPMessageHandler& aOtherMessage )
    {
    MSRPLOG( "-> CMSRPMessageHandler::CheckMessageChunk" )
    HBufC8* ownToPath = iMessage->ToPathHeader()->ToTextValueLC(); 
    HBufC8* ownMessageId = iMessage->MessageIdHeader()->ToTextValueLC();
    HBufC8* otherToPath = aOtherMessage.GetIncomingMessage()->ToPathHeader()->ToTextValueLC();
    HBufC8* otherMessageId = aOtherMessage.GetIncomingMessage()->MessageIdHeader()->ToTextValueLC();
    
    if ( *ownToPath == *otherToPath && *ownMessageId == *otherMessageId )
        {
        CleanupStack::PopAndDestroy( 4 ); // above texts
        MSRPLOG( "CMSRPMessageHandler::CheckMessageChunk MATCH!" )
        // there is already a message, this chunk belongs to a previous
        // message 
        if ( iMessage->ByteRangeHeader() )
            {
            CMSRPByteRangeHeader* byteR = CMSRPByteRangeHeader::NewL(
                    iMessage->ByteRangeHeader()->StartPosition( ),
                    iMessage->ByteRangeHeader()->EndPosition( ),
                    iMessage->ByteRangeHeader()->TotalLength( ) );
            aOtherMessage.GetIncomingMessage()->SetByteRangeHeader( byteR );
            }
        aOtherMessage.SetTransactionId( *iSentChunks[ iSentChunks.Count() -1 ] );
        MSRPLOG( "CMSRPMessageHandler::CheckMessageChunk MATCH2!" )
        return ETrue;
        }
        
    CleanupStack::PopAndDestroy( 4 ); // above texts
    MSRPLOG( "<- CMSRPMessageHandler::CheckMessageChunk" )
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CMSRPMessageHandler::CheckMessageChunk
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::SetMessageObserver( MMSRPMessageObserver* aMessageObserver )
    {
    iMSRPMessageObserver = aMessageObserver;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::IsSending
// -----------------------------------------------------------------------------
//
TBool CMSRPMessageHandler::IsSending( )
    {
    return isSending;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::TemporaryFileName
// -----------------------------------------------------------------------------
//
TFileName& CMSRPMessageHandler::TemporaryFileName( )
    {
    return iTempFileName;
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::SetTemporaryFileName
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::SetTemporaryFileName( TFileName aFileName ) 
    {
    iTempFileName = aFileName; 
    iMessage->SetFileName( iTempFileName );
    }

// -----------------------------------------------------------------------------
// CMSRPMessageHandler::ResponseHandled
// -----------------------------------------------------------------------------
//
void CMSRPMessageHandler::ResponseHandled( )
    {
    delete iResponse;
    iResponse = NULL;
    iActiveMsgType = EMSRPMessage;
    iBuffer.Zero();
    }

// End of file
