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
    else
        {
        User::Leave( KErrArgument );
        }
    
    iBuffer.CreateL( KMaxBufferSize );
    iState = EIdle;
    MSRPStrings::OpenL();
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
        iMessage = new ( ELeave ) CMSRPMessage();            
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
    delete iMessage;
    delete iResponse;
    delete iFileBuffer;
    iBuffer.Close();
    MSRPStrings::Close();
    iChunkList.ResetAndDestroy();
    iChunkList.Close();
    //iFile.Flush();
    iFile.Close();
    }


void CMSRPMessageHandler::AddHeaderL( TMsrpHeaderType aHeaderType, TPtrC8& aHeaderValue, TPtrC8& /*aHeaderTypeAndValue*/ )
    {
    MSRPLOG( "CMSRPMessageHandler::AddHeaderL enter" )
    CMSRPMessageBase* messageBase = NULL;
    if( iResponse )
        {
        messageBase = iResponse;
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

    
void CMSRPMessageHandler::AddContentL( TPtrC8& aContent )
    {
    MSRPLOG( "CMSRPMessageHandler::AddContentL enter" )
    if( iMessage )
        {
        if( iBuffer.Length() + aContent.Length() > iBuffer.MaxLength() )
            {
            iBuffer.ReAllocL(iBuffer.Length() + aContent.Length());
            iBuffer.Append(aContent);
            }
        else
            {
            iBuffer.Append(aContent);
            }
        }    
    MSRPLOG( "CMSRPMessageHandler::AddContentL exit" )
    }


void CMSRPMessageHandler::SetTransactionId( TPtrC8& aTransactionId )
    {
    MSRPLOG( "CMSRPMessageHandler::SetTransactionId enter" )    
    iTransactionId = aTransactionId;    
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
    MSRPLOG( "CMSRPMessageHandler::EndOfMessageL enter" )  
    if( iMessage && iBuffer.Length() )
        {
        HBufC8* contentOfMessage = HBufC8::NewL( iBuffer.Length() );
        *contentOfMessage = iBuffer;
        iMessage->SetContent( contentOfMessage );
        iBuffer.Zero();
        }
    iMsgEndStatus = aStatus;    
    MSRPLOG( "CMSRPMessageHandler::EndOfMessageL exit" )
    }


MMSRPIncomingMessage::TMSRPMessageType CMSRPMessageHandler::MessageType( )
    {
    MSRPLOG( "CMSRPMessageHandler::MessageType enter" )
    MSRPLOG2("Message Type = %d", iActiveMsgType ); 
    return iActiveMsgType;
    }


CMSRPMessage* CMSRPMessageHandler::GetIncomingMessage( )
    {
    MSRPLOG( "CMSRPMessageHandler::GetIncomingMessage enter" )  
    if( iMessage )
        {
        return iMessage;
        }
    return NULL;
    }


void CMSRPMessageHandler::UpdateResponseStateL(CMSRPMessageHandler *incomingMsgHandler)
    {
    MSRPLOG( "CMSRPMessageHandler::UpdateResponseState enter" )
    TBuf8<100> iTransactionId = incomingMsgHandler->TransactionId();
    for(TInt i=0;i<iChunkList.Count();i++)
       {
        if(iTransactionId == iChunkList[i]->GetTransactionId())
            {
             iChunkList[i]->SetResponseSent(ETrue);
             ReceiveFileStateL(iChunkList[i]->GetEndPos()- iChunkList[i]->GetStartPos() + 1);                    
            }
       }
    MSRPLOG( "CMSRPMessageHandler::UpdateResponseState exit" )
    }


void CMSRPMessageHandler::ReceiveFileStateL( TInt aBytesTransferred )
    {
    MSRPLOG( "CMSRPMessageHandler::ReceiveFileState enter" )
    MSRPLOG2( "CMSRPMessageHandler::ReceiveFileState Notify %d", iProgress )
    MSRPLOG2( "CMSRPMessageHandler::ReceiveFileState iFileNotified %d", iNotifiedBytes )    
    MSRPLOG2( "CMSRPMessageHandler::ReceiveFileState iunnotified %d", iPendingBytes )
    MSRPLOG2( "CMSRPMessageHandler::ReceiveFileState aBytesTransferred %d", aBytesTransferred )
    MSRPLOG2( "CMSRPMessageHandler::ReceiveFileState iFileSize %d", iFileSize )
    
    iPendingBytes += aBytesTransferred;
    
    if(iNotifiedBytes + iPendingBytes == iFileSize)
        {
        iFileTransferComplete = ETrue;
        }
    
    if(iPendingBytes >= KMaxChunkReadSize)
        {
        MSRPLOG( "CMSRPMessageHandler::ReceiveFileState unnotified exceeds threshold" )
        MSRPLOG2( "CMSRPMessageHandler::ReceiveFileState iFileReceiveComplete: %d", iFileTransferComplete )                
            
        iNotifiedBytes += iPendingBytes;
        iPendingBytes = 0;
        //notify client of progress
        if(iProgress && !iFileTransferComplete)
            iMSRPMessageObserver->MessageReceiveProgressL(iNotifiedBytes, iFileSize);
        }
 
     MSRPLOG( "CMSRPMessageHandler::ReceiveFileState exit" )   
     }
    

TBool CMSRPMessageHandler::SendResponseL( MMSRPMessageObserver* aMessageObserver, 
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
                && (aResponseCode == CMSRPResponse::EAllOk) )
            {
            sendResponse = EFalse;
            }
        CleanupStack::PopAndDestroy(frHeaderValue);
        }
    
    if( !iMessage->ToPathHeader() || !iMessage->FromPathHeader() )
        {
        aResponseCode = CMSRPResponse::EUnknownCode;
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
        aConnection.SendL( *this );
        }
  
    MSRPLOG( "CMSRPMessageHandler::SendResponseL exit" )
	
	if( aResponseCode == CMSRPResponse::EAllOk )
        return ETrue;
    else
        return EFalse;
    }


TBool CMSRPMessageHandler::IfResponseReqL()
    {
    TBool responseReq = ETrue;
    if( iMessage->FailureReportHeader() )
        {
        HBufC8* frHeaderValue = iMessage->FailureReportHeader()->ToTextValueLC();
        if( frHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::ENo ).DesC() )
            {
             responseReq = EFalse;
            }
        else if( (frHeaderValue->Des() == MSRPStrings::StringF( MSRPStrConsts::EPartial ).DesC()))
            {
            responseReq = EFalse;
            }
        CleanupStack::PopAndDestroy(frHeaderValue);     
        }
    return responseReq;
    }


TDesC8& CMSRPMessageHandler::TransactionId( )
    {
    MSRPLOG( "CMSRPMessageHandler::TransactionId enter" )
    return iTransactionId;
    }


CMSRPResponse* CMSRPMessageHandler::GetIncomingResponse( )
    {
    MSRPLOG( "CMSRPMessageHandler::GetIncomingResponse enter" )
    if( iResponse )
        {
        return iResponse;
        }
    return NULL;
    }


void CMSRPMessageHandler::SendMessageL( MMSRPConnection& aConnection )
    {
    MSRPLOG( "CMSRPMessageHandler::SendMessageL enter" )    
    iActiveMsgType = EMSRPMessage;
    aConnection.SendL( *this );
    MSRPLOG( "CMSRPMessageHandler::SendMessageL exit" )
    }


void CMSRPMessageHandler::SendFileL(MMSRPConnection& aConnection)
    {
    MSRPLOG( "CMSRPMessageHandler::SendFileL enter" ) 
    /* Flags  */
    isFile = ETrue;    
    iProgress = iMessage->GetNotifyProgress();
    
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFile.Open(iFs, iMessage->GetFileName(), EFileShareReadersOrWriters)); 
    iFile.Size(iFileSize);
    iFileBuffer = HBufC8::NewL(KMaxChunkReadSize);            
    FillFileBufferL();
    iActiveMsgType = EMSRPMessage;
    aConnection.SendL( *this );
    
    MSRPLOG( "CMSRPMessageHandler::SendFileL exit" )     
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


void CMSRPMessageHandler::ReceiveFileL( )
    {
    MSRPLOG( "CMSRPMessageHandler::ReceiveFileL enter" )
    isFile = ETrue;
    iProgress = iMessage->GetNotifyProgress();
    
    User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iFile.Replace(iFs,iMessage->GetFileName(),EFileWrite));
    iFileSize = iMessage->GetFileSize();
    iFileBuffer = HBufC8::NewL(KMaxChunkReadSize);
    
    MSRPLOG( "CMSRPMessageHandler::ReceiveFileL exit" )
    }


void CMSRPMessageHandler::WritetoFileL(CMSRPMessageHandler *incomingMsgHandler )
    {
    MSRPLOG( "CMSRPMessageHandler::WriteToFile enter" )
    CMSRPMessage* inFileChunk = incomingMsgHandler->GetIncomingMessage();
    HBufC8* messageContent = NULL;
    if(inFileChunk->IsContent())
        {
        messageContent = HBufC8::NewL(inFileChunk->Content().Length());
        *messageContent = inFileChunk->Content();        
        WriteChunkToFileL(*messageContent,incomingMsgHandler->TransactionId());
       
        if (!incomingMsgHandler->IfResponseReqL())
            ReceiveFileStateL(messageContent->Length());
        }
    delete messageContent;
    MSRPLOG( "CMSRPMessageHandler::WriteToFile exit" )          
    }               
    
    
void CMSRPMessageHandler::WriteChunkToFileL(const TDesC8& aFileChunk ,TDesC8& aTransactionId)  
    {    
    MSRPLOG( "CMSRPMessageHandler::WriteChunktoFile enter" )      
     if(iBufPosInFile<(iFileSize-1))
       {
       iFile.Write(iBufPosInFile,aFileChunk);
       
       /* Create and update the structure */
       CMSRPMessageChunkState* iChunk = CMSRPMessageChunkState::NewL( ); 
       iChunk->SetStartPos(iBufPosInFile);
       iBufPosInFile += aFileChunk.Length();
       iChunk->SetEndPos(iBufPosInFile-1);
       iChunk->SetTransactionId(aTransactionId);
       iChunkList.Append(iChunk);
        }
       else
           {
            //Receive Bytes greater than file Size
            User::Leave( KErrArgument );
           }
  
       MSRPLOG( "CMSRPMessageHandler::WriteChunktoFile exit" )
     
    }


TBool CMSRPMessageHandler::IsOwnerOfResponse( MMSRPIncomingMessage& aIncomingMessage )
    {
    MSRPLOG( "CMSRPMessageHandler::IsOwnerOfResponse enter" )
    if(isFile)
        {
         /*  compare for transaction id  */
        for(TInt i=0;i<iChunkList.Count();i++)
           {
            if(aIncomingMessage.TransactionId() == iChunkList[i]->GetTransactionId())
             {
             if( iState == EWaitingForResponse || ( iState == EMessageSent && iResponseNeeded ))
                 {
                   if (i == iChunkList.Count()-1)
                     {
                      iResponseNeeded = EFalse;
                      iState = EMessageDone;
                     }
                 }
               return ETrue;
             }
           }
        }
    if( aIncomingMessage.TransactionId() == iTransactionId )
        {
        if( iState == EWaitingForResponse || ( iState == EMessageSent && iResponseNeeded ))
            {
            iResponseNeeded = EFalse;
            return ETrue;
            }
        }  
              
    MSRPLOG( "CMSRPMessageHandler::IsOwnerOfResponse exit" )
    return EFalse;
    }


void CMSRPMessageHandler::ConsumeFileResponseL(MMSRPIncomingMessage& aIncomingMessage )
    {
    MSRPLOG( "CMSRPMessageHandler::ConsumeFileResponseL enter" )
    CMSRPResponse* response = aIncomingMessage.GetIncomingResponse();
    TUint statusCode = response->StatusCode();
    
    for(TInt i=0;i<iChunkList.Count();i++)
       {
        if(aIncomingMessage.TransactionId()== iChunkList[i]->GetTransactionId())
            {
            iChunkList[i]->SetResponseReceived(ETrue);
            SendFileStateL(iChunkList[i]->GetEndPos()- iChunkList[i]->GetStartPos() + 1 ) ;
            }                     
       }          
      
    MSRPLOG( "CMSRPMessageHandler::ConsumeFileResponseL exit" )     
    } 


void CMSRPMessageHandler::SendFileStateL(TInt aBytesTransferred )
    {
    MSRPLOG( "CMSRPMessageHandler::SendFileStateL enter" )
    MSRPLOG2( "CMSRPMessageHandler::SendFileStateL granularity %d", iProgress )
    MSRPLOG2( "CMSRPMessageHandler::SendFileStateL iFileNotified %d", iNotifiedBytes )    
    MSRPLOG2( "CMSRPMessageHandler::SendFileStateL iunnotified %d", iPendingBytes )
    MSRPLOG2( "CMSRPMessageHandler::SendFileStateL aBytesTransferred %d", aBytesTransferred )
    MSRPLOG2( "CMSRPMessageHandler::SendFileStateL iFileSize %d", iFileSize )

    iPendingBytes += aBytesTransferred;
    
    if(iNotifiedBytes + iPendingBytes == iFileSize)
        {
        iFileTransferComplete = ETrue;      
        } 
    
    if(iPendingBytes  >= KMaxChunkReadSize)
        {
        MSRPLOG( "CMSRPMessageHandler::SendFileStateL unnotified exceeds threshold" )
        MSRPLOG2( "CMSRPMessageHandler::SendFileStateL iFileSendComplete: %d", iFileTransferComplete )                
        iNotifiedBytes += iPendingBytes;
        iPendingBytes = 0;

        //notify client of progress
        if( iProgress && !iFileTransferComplete )//send progress requested and avoid double notifn. as send complete not handled in waitfor clientstate
            iMSRPMessageObserver->MessageSendProgressL(iNotifiedBytes, iFileSize);//iByteinPos

        }
    MSRPLOG( "CMSRPMessageHandler::SendFileStateL exit" )
    }


TBool CMSRPMessageHandler::FileTransferComplete( )
    {
    if(iFileTransferComplete)
        return ETrue;
    else
        return EFalse;
    }


TBool CMSRPMessageHandler::IsInFile()
    {
    if(isFile)
      {
       return ETrue;
      }
    return EFalse;
    }


TBool CMSRPMessageHandler::ConsumeResponseL( MMSRPIncomingMessage& aIncomingMessage )
    {
    MSRPLOG( "CMSRPMessageHandler::ConsumeResponseL enter" )
    
    TBool ret;
    CMSRPResponse* response = aIncomingMessage.GetIncomingResponse();
    TUint statusCode = response->StatusCode();
       
    if( iMessage->FailureReportHeader() &&  
            (iMessage->FailureReportHeader()->ToTextValueLC()->Des() == 
            MSRPStrings::StringF( MSRPStrConsts::EPartial ).DesC()) &&
                statusCode == CMSRPResponse::EAllOk )                
        {
        iState = EMessageDone;
        ret = EFalse;
        }
    else
        {        
        RStringF statusString = response->ReasonPhrase();
        iResponse = new (ELeave) CMSRPResponse( statusCode, statusString );
        HBufC8* toPathValue = response->ToPathHeader()->ToTextValueLC();
        HBufC8* fromPathValue = response->FromPathHeader()->ToTextValueLC();
        iResponse->SetToPathHeader( CMSRPToPathHeader::DecodeL( toPathValue->Des() ) );
        iResponse->SetFromPathHeader( CMSRPFromPathHeader::DecodeL( fromPathValue->Des() ) );
        CleanupStack::PopAndDestroy(fromPathValue);
        CleanupStack::PopAndDestroy(toPathValue);
        iState = EMessageDone;
        ret = ETrue;
        }
    
    if( iMessage->FailureReportHeader() )
        {
        CleanupStack::PopAndDestroy(); // FR header value from above
        }
         
    MSRPLOG( "CMSRPMessageHandler::ConsumeResponseL exit" )
    return ret;
    }


TBool CMSRPMessageHandler::IsMessageComplete()
    {
    MSRPLOG( "CMSRPMessageHandler::IsMessageComplete enter" )
    if( iState == EMessageDone )
        return ETrue;
    else
        return EFalse;
    }


TBool CMSRPMessageHandler::GetSendResultL( TUint &aErrorCode, HBufC8* &aMessageId )
    {
    MSRPLOG( "CMSRPMessageHandler::GetSendResult enter" )
    if( iState == EMessageDone )
        {
        aMessageId = iMessage->MessageIdHeader()->ToTextValueLC();
        CleanupStack::Pop(aMessageId);
        if(iResponse)
            {
            TUint code = iResponse->StatusCode();
            if( code == CMSRPResponse::EAllOk )
                {
                aErrorCode = ENoError;
                }
            else if ( code == CMSRPResponse::ETimeout )
                {
                aErrorCode = ENetworkTimeout;
                }
            else
                {
                aErrorCode = EUnrecoverableError;
                }            
            }
        else
            {
            aErrorCode = ENoError;
            }
        }    
    MSRPLOG( "CMSRPMessageHandler::GetSendResult exit" )
    if( aErrorCode == EUnrecoverableError )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


MMSRPWriterObserver::TWriteStatus CMSRPMessageHandler::GetSendBufferL( TPtrC8& aData, TBool aInterrupt )
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
    
    aData.Set(iBuffer);        
    MSRPLOG( "CMSRPMessageHandler::GetSendBufferL exit" )
    return EMsrpSocketWrite;
    }


MMSRPWriterObserver::TMsgStatus CMSRPMessageHandler::WriteDoneL( TInt aStatus )
    {
    MSRPLOG( "CMSRPMessageHandler::WriteDoneL enter" )
    MMSRPWriterObserver::TMsgStatus retStatus = EUndefined;

    if( !aStatus )
        {
        /*if file send and message and response not needed, update progress*/
        if (isFile && iActiveMsgType == EMSRPMessage && iResponseNeeded == FALSE)
            {
            if (iFileBytesSent > 0)
                SendFileStateL(iFileBytesSent);
            }
        
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
                    iMSRPMessageObserver->MessageSendCompleteL();
                    break;                    
                    }
                case EMSRPResponse:
                    {
                    iState = EMessageDone;
                    iMSRPMessageObserver->MessageResponseSendCompleteL(*this);
                    break;
                    }
                }
            //iActiveMsgType = EMSRPNotDefined;
            retStatus = MMSRPWriterObserver::EComplete;
            }        
        else if( iState ==  EInProgress )
            {
            retStatus = MMSRPWriterObserver::EPending;
            }            
        else if(iState == EChunkSent)
            {
            retStatus = MMSRPWriterObserver::EInterrupted;
            iState = EIdle;
            }        
        else
           {
           iMSRPMessageObserver->WriterError();
           }
        }
    MSRPLOG( "CMSRPMessageHandler::WriteDone exit" )
    return retStatus;
    }


void CMSRPMessageHandler::CreateByteRangeHeaderL()
    {
    MSRPLOG( "CMSRPMessageHandler::CreateByteRangeHeaderL enter" )
    CMSRPByteRangeHeader* byteRange = NULL;
       
    if ( iMessage->IsFile())
        {                
        byteRange = CMSRPByteRangeHeader::NewL( iBufPosInFile+iStartPosInBuffer+1,KUnknownRange,iFileSize);
      
        CleanupStack::PushL(byteRange);
        iMessage->SetByteRangeHeader( byteRange );
        CleanupStack::Pop(byteRange);
        }
    else
        {
        if( !iMessage->ByteRangeHeader() )
            {
             if ( iMessage->IsContent() )
                 {
                 byteRange = CMSRPByteRangeHeader::NewL( 1,
                 iMessage->Content().Length(), iMessage->Content().Length() );
                 CleanupStack::PushL(byteRange);
                 }
             else
                 {
                 byteRange = CMSRPByteRangeHeader::NewL( 1, 0, 0 );
                 CleanupStack::PushL(byteRange);
                 }
            iMessage->SetByteRangeHeader( byteRange );
            CleanupStack::Pop(byteRange);
            }
        }
    MSRPLOG( "CMSRPMessageHandler::CreateByteRangeHeaderL exit" )
    }


void CMSRPMessageHandler::CreateTransactionId()
    {
    MSRPLOG( "CMSRPMessageHandler::CreateTransactionId enter" )
    
    TTime now;
    now.HomeTime();
    TInt64 seed = now.Int64();
    TInt random = Math::Rand( seed );
    iTransactionId.NumUC( random );
    
    MSRPLOG( "CMSRPMessageHandler::CreateTransactionId exit" )
    }


void CMSRPMessageHandler::WriteInitialLineToBufferL()
    {
    MSRPLOG( "CMSRPMessageHandler::WriteInitialLineToBufferL enter" )
    
    iBuffer.Append( MSRPStrings::StringF( MSRPStrConsts::EMSRP ).DesC() );

    iBuffer.Append( KSpaceChar );
    
    if (! (iActiveMsgType == EMSRPResponse) )
         {
         CreateTransactionId();
         }
   
    iBuffer.Append( iTransactionId );

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
            if ( valueString->Des() == MSRPStrings::StringF( MSRPStrConsts::EYes ).DesC() ||
                valueString->Des() == MSRPStrings::StringF( MSRPStrConsts::EPartial ).DesC() )
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
    TInt endlineSize = iTransactionId.Length() + KEndlineConstLength;
    TInt remBufferSize = iBuffer.MaxLength() - iBuffer.Length() - endlineSize - KCRAndLF().Length();
    TInt chunkLength = iFileBuffer->Length() - iEndPosInBuffer;
    
    /*if(remBufferSize<0) TODO*/
    if(chunkLength > remBufferSize)
        {
        iFileBytesSent = remBufferSize;
        iBuffer.Append(iFileBuffer->Mid(iEndPosInBuffer, iFileBytesSent));
        iEndPosInBuffer += iFileBytesSent;
        if(iInterrupt)
            {
            WriteEndLineToBuffer(EMessageContinues);
            //add chunk entry
            CMSRPMessageChunkState* iChunk = CMSRPMessageChunkState::NewL( ); 
            iChunk->SetStartPos(iBufPosInFile+iStartPosInBuffer);           
            iChunk->SetEndPos(iBufPosInFile+iEndPosInBuffer-1);//since endpos is pointing to next start pos 
            iChunk->SetTransactionId(iTransactionId);
            iChunkList.Append(iChunk);

            iStartPosInBuffer = iEndPosInBuffer;
            iState = EChunkSent;
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
        iBuffer.Append(KCRAndLF());
        
        //add chunk entry
        CMSRPMessageChunkState* iChunk = CMSRPMessageChunkState::NewL( ); 
        iChunk->SetStartPos(iBufPosInFile+iStartPosInBuffer);           
        iChunk->SetEndPos(iBufPosInFile+iEndPosInBuffer-1); 
        iChunk->SetTransactionId(iTransactionId);
        iChunkList.Append(iChunk);
        
        if(FillFileBufferL() > 0)
            {
            //next file buffer has data
            WriteEndLineToBuffer(EMessageContinues);
            iState = EChunkSent;        
            }
        else
            {
            WriteEndLineToBuffer(EMessageEnd);
            iState = EMessageSent;
            }

        }
    
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
    TInt endlineSize = iTransactionId.Length() + KEndlineConstLength;
    
    if( endlineSize <= remBufferSize )
        {
        iBuffer.Append( KDashLine );
        iBuffer.Append( iTransactionId );
        
        if(iActiveMsgType == EMSRPResponse)
            {
            iBuffer.Append( KMessageEndSign );
            iState = EMessageSent;
            }
        else
            {
            if( aEndFlag )
               {
               iBuffer.Append( KMessageContinuesSign );
               }
            else
               {
               iBuffer.Append( KMessageEndSign );
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
        case CMSRPResponse::EAllOk:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EAllOk );
            break;
            }
        case CMSRPResponse::EUnintelligibleRequest:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EUnintelligibleRequest );
            break;
            }
        case CMSRPResponse::EActionNotAllowed:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EActionNotAllowed );
            break;
            }
        case CMSRPResponse::ETimeout:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::ETimeout );
            break;
            }
        case CMSRPResponse::EStopSending:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EStopSending );
            break;
            }
        case CMSRPResponse::EMimeNotUnderstood:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EMimeNotUnderstood );
            break;
            }
        case CMSRPResponse::EParameterOutOfBounds:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EParameterOutOfBounds );
            break;
            }
        case CMSRPResponse::ESessionDoesNotExist:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::ESessionDoesNotExist );
            break;
            }
        case CMSRPResponse::EUnknownRequestMethod:
            {
            statusString = MSRPStrings::StringF( MSRPStrConsts::EUnknownRequestMethod );
            break;
            }
        case CMSRPResponse::ESessionAlreadyBound:
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
            return CMSRPResponse::EUnintelligibleRequest;
            }
        CMSRPMessage* message = static_cast<CMSRPMessage*>(aMessage);
        if(message->IsContent() && !message->ContentTypeHeader())
            {
            return CMSRPResponse::EUnintelligibleRequest;
            }
        }
    
    MSRPLOG( "CMSRPMessageHandler::CheckValidityOfMessage exit" )
    return CMSRPResponse::EAllOk;
    }

