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

//  Include Files
#include "CMSRPSessionImplementation.h"
#include "msrpstrings.h"
#include "MSRPStrConsts.h"
#include "RMSRPSession.h"
#include "MMSRPSessionObserver.h"
#include "CMSRPConnectionListener.h"
#include "CMSRPIncomingListener.h"
#include "CMSRPSendResultListener.h"
#include "CMSRPMessage.h"
#include "CMSRPToPathHeader.h"
#include "CMSRPFromPathHeader.h"
#include "CMSRPMessageIdHeader.h"
#include "CMSRPSuccessReportHeader.h"
#include "CMsrpFailureReportHeader.h"
#include "CMsrpContentTypeHeader.h"
#include "CMSRPSessionParams.h"


// SYSTEM INCLUDES
#include <e32math.h>


// Constants
const TInt KMsrpFixedLength=19; // fixed length of a uri


//  Member Functions

CMSRPSessionImplementation* CMSRPSessionImplementation::NewL( RMSRP& aRMSRP,
                                                              MMSRPSessionObserver& aObserver,
                                                              const TUint aIapId )
	{
	MSRPLOG("CMSRPSessionImplementation::NewL");
	CMSRPSessionImplementation *self = CMSRPSessionImplementation::NewLC( aRMSRP, aObserver, aIapId );

	CleanupStack::Pop(self);
	return self;
	}


CMSRPSessionImplementation* CMSRPSessionImplementation::NewLC( RMSRP& aRMSRP,
                                                               MMSRPSessionObserver& aObserver,
                                                               const TUint aIapId )
    {
    MSRPLOG("CMSRPSessionImplementation::NewLC");
    CMSRPSessionImplementation *self = new (ELeave) CMSRPSessionImplementation( aRMSRP, aObserver );                                                            

    CleanupStack::PushL(self);                                                       
    self->ConstructL( aIapId );
    return self;
    }

CMSRPSessionImplementation::CMSRPSessionImplementation( RMSRP& aRMSRP,
                                                        MMSRPSessionObserver& aObserver )
                                                        :iRMSRP(aRMSRP), 
                                                        iSessionObserver(aObserver),
                                                        iSuccessReport(ENo),
                                                        iFailureReport(EYes)
    {
    MSRPLOG("CMSRPSessionImplementation::Ctor");
	}


void CMSRPSessionImplementation::ConstructL( const TUint aIapId )
	{
	MSRPLOG("CMSRPSessionImplementation::ConstructL enter");
	
	MSRPStrings::OpenL();
	iMSRPSession = new ( ELeave ) RMSRPSession();
	    
	User::LeaveIfError(iMSRPSession->CreateServerSubSession( iRMSRP, aIapId ) );
	MSRPLOG("Sub session opened successfully!");
		
	RBuf8 sessionID;
	sessionID.CreateL( KMaxLengthOfSessionId );
	CleanupClosePushL( sessionID );
	
	RBuf8 localHost;
	localHost.CreateL( KMaxLengthOfHost );
	CleanupClosePushL( localHost );
	
	iMSRPSession->GetLocalPathL( localHost, sessionID );
	
	iLocalMsrpPath.CreateL( KMsrpUriScheme, KMsrpFixedLength + localHost.Length() + sessionID.Length() );
		
	iLocalMsrpPath.Append( localHost );
	iLocalMsrpPath.Append( KColon );
	iLocalMsrpPath.AppendNum( KMsrpPort );
	iLocalMsrpPath.Append( KForwardSlash );   
	iLocalMsrpPath.Append( sessionID );
	iLocalMsrpPath.Append( KSemicolon );
	iLocalMsrpPath.Append( KTransport );
	
	
	iConnectionListener = CMSRPConnectionListener::NewL(
	        *this, *iMSRPSession );
	iIncomingListener = CMSRPIncomingListener::NewL(
	        *this, *iMSRPSession );
	iSendResultListener = CMSRPSendResultListener::NewL(
	        *this, *iMSRPSession, sessionID );
	
	CleanupStack::PopAndDestroy(2); //sessionID and localHost
	MSRPLOG("CMSRPSessionImplementation::ConstructL exit");
	}


CMSRPSessionImplementation::~CMSRPSessionImplementation()
    {
    MSRPLOG("CMSRPSessionImplementation::Dtor Entered");
        
    MSRPStrings::Close();
    iLocalMsrpPath.Close();
    iRemoteMsrpPath.Close();
    
    delete iConnectionListener;
    delete iIncomingListener;
    delete iSendResultListener;
    
    iSentMessages.ResetAndDestroy();
    iSentMessages.Close();
    
    if( iMSRPSession )
        {
        iMSRPSession->CloseServerSubSession();
        }    
    delete iMSRPSession;

    MSRPLOG("CMSRPSessionImplementation::Dtor Exit");    
    }


TDesC8& CMSRPSessionImplementation::LocalMSRPPath()
    {
    MSRPLOG("CMSRPSessionImplementation::LocalMSRPPath");
    return iLocalMsrpPath;
    }


void CMSRPSessionImplementation::SetSessionParams( CMSRPSessionParams& aSessionParams )
    {
    MSRPLOG("CMSRPSessionImplementation::SetSessionParams");
    iSuccessReport = aSessionParams.SuccessReportHeader();
    iFailureReport = aSessionParams.FailureReportHeader();
    }


void CMSRPSessionImplementation::GetRemotePathComponentsL( TPtrC8& aRemoteHost, TUint& aRemotePort, TPtrC8& aRemoteSessionID )
    {
    MSRPLOG("CMSRPSessionImplementation::GetRemotePathComponentsL enter");
    TUriParser8 parser;
    User::LeaveIfError( parser.Parse( iRemoteMsrpPath ) ); 
 
    aRemoteHost.Set( parser.Extract( EUriHost ) );
    
    const TDesC8& remPort = parser.Extract( EUriPort );

    TLex8 portLex(remPort);
    User::LeaveIfError( portLex.Val( aRemotePort ) );
        
    const TDesC8& remotePath = parser.Extract( EUriPath );
    
    TInt loc = remotePath.Find( KSemicolon );
    aRemoteSessionID.Set( remotePath.Mid( 1, loc-1) );    
    MSRPLOG("CMSRPSessionImplementation::GetRemotePathComponentsL exit");
    }


void CMSRPSessionImplementation::ConnectL( const TDesC8& aRemoteMsrpPath )
    {
    MSRPLOG("CMSRPSessionImplementation::ConnectL enter");
    
    iRemoteMsrpPath.CreateL( aRemoteMsrpPath );
    
    TPtrC8 remoteHost;
    TPtrC8 remoteSessionID;
    TUint remotePort;
    
    GetRemotePathComponentsL( remoteHost, remotePort, remoteSessionID );
    
    iConnectionListener->ConnectL( remoteHost, remotePort, remoteSessionID );
    MSRPLOG("CMSRPSessionImplementation::ConnectL exit");
    }


void CMSRPSessionImplementation::ListenL( const TDesC8& aRemoteMsrpPath )
    {    
    MSRPLOG("CMSRPSessionImplementation::ListenL enter");
    
    iRemoteMsrpPath.CreateL( aRemoteMsrpPath );
    
    TPtrC8 remoteHost;
    TPtrC8 remoteSessionID;
    TUint remotePort;
    
    GetRemotePathComponentsL( remoteHost, remotePort, remoteSessionID );
    
    iIncomingListener->ListenConnections( remoteHost, remotePort, remoteSessionID );     
    MSRPLOG("CMSRPSessionImplementation::ListenL exit");
    }


void CMSRPSessionImplementation::ConnectionEstablishedL( TInt aStatus )
    {
    
    MSRPLOG("CMSRPSessionImplementation::ConnectionEstablished enter");
    
    iSessionObserver.ConnectStatus(aStatus);
    if ( aStatus == KErrNone )
        {
        // start listening to incoming messages directed to this session
        iIncomingListener->ListenMessages( );

        // start listening when message has been sent,  to responses
        iSendResultListener->ListenSendResultL( );                
        }
    
    MSRPLOG("CMSRPSessionImplementation::ConnectionEstablished exit");
    }
    

void CMSRPSessionImplementation::HandleIncomingMessageL(
    const TDesC8& aIncomingMessage, TInt aStatus )
    {
    MSRPLOG( "CMSRPSessionImplementation::HandleIncomingMessageL enter" )

    // the incoming buffer must be internalized
    RDesReadStream readStream( aIncomingMessage );

    if ( CMSRPMessage::IsMessage( aIncomingMessage ) )
        {
        CMSRPMessage* message = NULL;
        message = CMSRPMessage::InternalizeL( readStream );
        CleanupStack::PushL(message);
        
        HBufC8* messageContent = NULL;
        if(message->IsContent())
            {
            messageContent = HBufC8::NewL(message->Content().Length());
            CleanupStack::PushL( messageContent );
            *messageContent = message->Content();
            }
        else
            {
            messageContent = HBufC8::NewL(KNullDesC8().Length());
            CleanupStack::PushL( messageContent );
            *messageContent = KNullDesC8();
            }

        iSessionObserver.IncomingMessage( messageContent, aStatus );
        CleanupStack::Pop( messageContent );
        CleanupStack::PopAndDestroy(message);
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    MSRPLOG( "CMSRPSessionImplementation::HandleIncomingMessageL exit" )
    }

void CMSRPSessionImplementation::ReceiveProgress(TInt aBytesReceived, TInt aTotalBytes)
    {
    iSessionObserver.FileReceiveProgress(aBytesReceived, aTotalBytes);
    }

void CMSRPSessionImplementation::SendProgress(TInt aBytesSent, TInt aTotalBytes)
    {
    iSessionObserver.FileSendProgress(aBytesSent, aTotalBytes);    
    }

CMSRPMessage* CMSRPSessionImplementation::CreateMsrpMessageL(
    const TDesC8& aMessage, const TDesC8& aToPath,
    const TDesC8& aFromPath, const TDesC8& aMimeType )
    {
    
    MSRPLOG( "CMSRPSessionImplementation::CreateMsrpMessageL enter" )
    
    // To path
    CMSRPMessage* msrpMessage = new ( ELeave ) CMSRPMessage();
    CleanupStack::PushL( msrpMessage );
    CMSRPToPathHeader* toPath = CMSRPToPathHeader::DecodeL( aToPath );
    msrpMessage->SetToPathHeader( toPath );

    // from path
    CMSRPFromPathHeader* fromPath = CMSRPFromPathHeader::DecodeL( aFromPath );
    msrpMessage->SetFromPathHeader( fromPath );

    // message id header
    TTime now;
    now.HomeTime();
    TInt64 seed = now.Int64();
    // Create a random number as the session ID
    TInt random = Math::Rand( seed );
    TBuf8< 100 > idString;
    idString.AppendNum( random );
    CMSRPMessageIdHeader* messageIdHeader = CMSRPMessageIdHeader::NewL( idString );
    msrpMessage->SetMessageIdHeader( messageIdHeader );

    // success report header
    if( iSuccessReport != ENo )
        {
        RStringF string = MSRPStrings::StringF( MSRPStrConsts::EYes );
        CMSRPSuccessReportHeader* successReportHeader = CMSRPSuccessReportHeader::NewL( string );
        msrpMessage->SetSuccessReportHeader( successReportHeader );
        }
    
    // failure report header
    if( iFailureReport != EYes )
        {
        RStringF string;
        CleanupClosePushL(string);
        if( iFailureReport == ENo )
            {
            string = MSRPStrings::StringF( MSRPStrConsts::ENo );
            }
        else
            {
            string = MSRPStrings::StringF( MSRPStrConsts::EPartial );
            }
        CMSRPFailureReportHeader* failureReportHeader = CMSRPFailureReportHeader::NewL( string );
        msrpMessage->SetFailureReportHeader( failureReportHeader );
        CleanupStack::PopAndDestroy(); // string
        }
     
    // content type
    if(aMimeType.Length())
        {
        CMSRPContentTypeHeader* contentType = CMSRPContentTypeHeader::NewL( aMimeType );
        msrpMessage->SetContentTypeHeader( contentType );
        
        if(aMessage.Length())
            {
            // content of the message
            HBufC8* contentOfMessage = HBufC8::NewL( aMessage.Length() );
            *contentOfMessage = aMessage;

            msrpMessage->SetContent( contentOfMessage );
            }        
        }
    
    CleanupStack::Pop(msrpMessage);
    MSRPLOG( "CMSRPSessionImplementation::CreateMsrpMessageL exit" )
    return msrpMessage;
    }


void CMSRPSessionImplementation::SendMessageL( CMSRPMessage* aMessage )
    {
    MSRPLOG( "CMSRPSessionImplementation::SendMessageL enter" )

    iSentMessages.AppendL( aMessage );

    // making sure the required headers are included
    if ( !aMessage->FromPathHeader() || !aMessage->ToPathHeader() || !aMessage->MessageIdHeader() )
        {
        User::Leave( KErrArgument );
        }

    CBufSeg* buf1 = CBufSeg::NewL( 256 ); // expandsize 256
    CleanupStack::PushL( buf1 );
    RBufWriteStream writeStream( *buf1 );
    CleanupClosePushL( writeStream );

    aMessage->ExternalizeL( writeStream );
    writeStream.CommitL();

    // MSRP message externalized to buffer, now let's move it to flat buffer
    if ( buf1->Size() > KMaxLengthOfIncomingMessageExt )
        {
        // invalid message size
        User::Leave( KErrArgument );
        }

    buf1->Read( 0, iExtMessageBuffer, buf1->Size() );
    CleanupStack::PopAndDestroy( 2 ); // buf1, writestream

    // message creation complete, send the message
    // synchronously sent the message
    User::LeaveIfError( iMSRPSession->SendMessage( iExtMessageBuffer ) );
    
    MSRPLOG( "CMSRPSessionImplementation::SendMessageL exit" )
    }


HBufC8* CMSRPSessionImplementation::SendBufferL(
    const TDesC8& aMessage,
    const TDesC8& aMimeType )
    {
    MSRPLOG( "CMSRPSessionImplementation::SendBufferL enter" )
    
    if(aMessage.Length() && !aMimeType.Length())
        {
        User::Leave( KErrArgument );
        }
    
    CMSRPMessage* msrpMessage = CreateMsrpMessageL(
                                aMessage, iRemoteMsrpPath, iLocalMsrpPath, aMimeType );
    CleanupStack::PushL( msrpMessage );
    SendMessageL( msrpMessage );
    HBufC8* messageId = msrpMessage->MessageIdHeader()->ToTextValueLC();
    CleanupStack::Pop(messageId);
    CleanupStack::Pop( msrpMessage );
    MSRPLOG( "CMSRPSessionImplementation::SendBufferL exit" )
    return messageId;
    }


void CMSRPSessionImplementation::CancelSendingL( TDesC8& aMessageId )
    {
    MSRPLOG( "CMSRPSessionImplementation::CancelSendingL " )
    // then let's send a message to server to cancel sending
    User::LeaveIfError( iMSRPSession->CancelSending( aMessageId ) );
    }


void CMSRPSessionImplementation::SendStatusL( TInt aStatus, const TDesC8& aMessageid )
    {
    MSRPLOG2( "CMSRPSessionImplementation::SendStatus = %d", aStatus )
    if(isReceiveFile || isSendFile )
        {
        if(isReceiveFile)
            {
            iSessionObserver.ReceiveFileNotification( aStatus );
           
            }
        if(isSendFile)
            {
            iSessionObserver.SendFileNotification( aStatus );
            }
        }
     else
        {
        for ( TInt i = 0; i < iSentMessages.Count(); i++ )
            {
            HBufC8* messageid = iSentMessages[ i ]->MessageIdHeader()->ToTextValueLC();
            if ( *messageid == aMessageid )
                {
                // match
                delete iSentMessages[ i ];
                iSentMessages.Remove( i );
                iSessionObserver.SendResult( aStatus, aMessageid );
                CleanupStack::PopAndDestroy(messageid); // messageid from above
                break;
                }
            CleanupStack::PopAndDestroy(messageid); // messageid from above
            }
        }

    MSRPLOG( "CMSRPSessionImplementation::SendStatus exit" )
    }

void CMSRPSessionImplementation::HandleConnectionErrors( TInt aErrorStatus )
    {
    MSRPLOG2( "CMSRPSessionImplementation::HandleConnectionErrors Error = %d", aErrorStatus )
    iSessionObserver.ListenPortFailure(aErrorStatus);  
    
    }

void CMSRPSessionImplementation::SendFileL(const TFileName& aFileName, const TDesC8& aMimeType)
    {
    MSRPLOG( "CMSRPSessionImplementation::SendFileL enter" );
   
    if(aFileName.Length() && !aMimeType.Length())
        {
        User::Leave( KErrArgument );
        }
     isSendFile = ETrue;
    CMSRPMessage* iFile = SetFileParamsL(aFileName,iRemoteMsrpPath, iLocalMsrpPath,aMimeType );
    
     //Set progress indication
    iFile->SetNotifyProgress(iProgress);
     
    CBufSeg* buf = CBufSeg::NewL( 100 ); // expandsize to 100 
    
    //todo need to check what's optimal value here
    CleanupStack::PushL( buf );
    RBufWriteStream writeStr( *buf );
    CleanupClosePushL( writeStr );
    iFile->ExternalizeL( writeStr );
    writeStr.CommitL();
    
    // MSRP message externalized to buffer, now let's move it to flat buffer
    if ( buf->Size() > KMaxLengthOfIncomingMessageExt )
        {
        // invalid message size
        User::Leave( KErrArgument );
        }

    buf->Read( 0, iExtMessageBuffer, buf->Size() );
    CleanupStack::PopAndDestroy( 2 ); // buf, writestream
   
    delete iFile;
	iFile = NULL;
    // send the filename
   User::LeaveIfError( iMSRPSession->SendFileL( iExtMessageBuffer ) );
   MSRPLOG( "CMSRPSessionImplementation::SendFileL exit" ); 
    }  
    
void CMSRPSessionImplementation::ReceiveFileL(const TFileName& aFileName,const TInt aFileSize, const TDesC8& aMimeType)
    {
    MSRPLOG( "CMSRPSessionImplementation::ReceiveFileL enter" );
    isReceiveFile = ETrue;
    if(aFileName.Length() && !aMimeType.Length())
            {
            User::Leave( KErrArgument );
            }
    CMSRPMessage* iFile = SetFileParamsL(aFileName,iRemoteMsrpPath, iLocalMsrpPath,aMimeType );
       
       //Set FileSize
       iFile->SetFileSize(aFileSize);
       
       //Set progress indication
       iFile->SetNotifyProgress(iProgress);
        
       CBufSeg* buf = CBufSeg::NewL( 100 ); // expandsize to 100 
       
       //todo need to check what's optimal value here
       CleanupStack::PushL( buf );
       RBufWriteStream writeStr( *buf );
       CleanupClosePushL( writeStr );
       iFile->ExternalizeL( writeStr );
       writeStr.CommitL();
       
       // MSRP message externalized to buffer, now let's move it to flat buffer
       if ( buf->Size() > KMaxLengthOfIncomingMessageExt )
           {
           // invalid message size
           User::Leave( KErrArgument );
           }

       buf->Read( 0, iExtMessageBuffer, buf->Size() );
       CleanupStack::PopAndDestroy( 2 ); // buf, writestream
       
       delete iFile;
	   iFile = NULL;
          // send the filename
          User::LeaveIfError( iMSRPSession->ReceiveFileL( iExtMessageBuffer ) );
       
       
    }

CMSRPMessage* CMSRPSessionImplementation::SetFileParamsL(const TFileName& aFileName,const TDesC8& aToPath,
                                                        const TDesC8& aFromPath, 
                                                        const TDesC8& aMimeType )
    {
      MSRPLOG( "CMSRPSessionImplementation::SetFileParamsL enter" );
    
      CMSRPMessage* msrpMessage = new ( ELeave ) CMSRPMessage();
      CleanupStack::PushL( msrpMessage );
    
      //set filename
      msrpMessage->SetFileName( aFileName );
    
      //set to path
      CMSRPToPathHeader* toPath = CMSRPToPathHeader::DecodeL( aToPath );
      msrpMessage->SetToPathHeader( toPath );

      //set from path
      CMSRPFromPathHeader* fromPath = CMSRPFromPathHeader::DecodeL( aFromPath );
      msrpMessage->SetFromPathHeader( fromPath );

      //set content type
      if(aMimeType.Length())
          {
          CMSRPContentTypeHeader* contentType = CMSRPContentTypeHeader::NewL( aMimeType );
          msrpMessage->SetContentTypeHeader( contentType );
          }
      
      //set  message id header
      TTime now;
      now.HomeTime();
      TInt64 seed = now.Int64();
      // Create a random number as the session ID
      TInt random = Math::Rand( seed );
      TBuf8< 100 > idString;
      idString.AppendNum( random );
      CMSRPMessageIdHeader* messageIdHeader = CMSRPMessageIdHeader::NewL( idString );
      msrpMessage->SetMessageIdHeader( messageIdHeader );
      
      // success report header
        if( iSuccessReport != ENo )
            {
            RStringF string = MSRPStrings::StringF( MSRPStrConsts::EYes );
            CMSRPSuccessReportHeader* successReportHeader = CMSRPSuccessReportHeader::NewL( string );
            msrpMessage->SetSuccessReportHeader( successReportHeader );
            }
        
        // failure report header
        if( iFailureReport != EYes )
            {
            RStringF string;
            CleanupClosePushL(string);
            if( iFailureReport == ENo )
                {
                string = MSRPStrings::StringF( MSRPStrConsts::ENo );
                }
            else
                {
                string = MSRPStrings::StringF( MSRPStrConsts::EPartial );
                }
            CMSRPFailureReportHeader* failureReportHeader = CMSRPFailureReportHeader::NewL( string );
            msrpMessage->SetFailureReportHeader( failureReportHeader );
            CleanupStack::PopAndDestroy(); // string
            }
      
          CleanupStack::Pop(msrpMessage); // msrpMessage
         
     MSRPLOG( "CMSRPSessionImplementation::SetFileParamsL enter" );
	 return msrpMessage;
    }


void CMSRPSessionImplementation::NotifyProgress(TBool aFlag)
    {
     iProgress = aFlag;
    }
