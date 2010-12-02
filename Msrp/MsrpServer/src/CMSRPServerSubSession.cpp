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


#include "MSRPCommon.h"
#include "MSRPServerCommon.h"
#include "CMSRPServerSubSession.h"
#include "MMSRPConnectionManager.h"
#include "CMSRPServerSession.h"
#include "CMSRPMessageBase.h"
#include "MMSRPConnection.h"
#include "CMSRPMessageHandler.h"
#include "CMSRPResponse.h"
#include "CMSRPReport.h"
#include "s32mem.h"
#include "CMSRPMessage.h"
#include "TStateFactory.h"
#include "TStates.h"
#include "CMsrpToPathHeader.h"
#include "CMSRPFromPathHeader.h"


CRMessageContainer::CRMessageContainer()
    {
    iStatus = FALSE;
    }

CRMessageContainer::~CRMessageContainer()
    {
    
    }

TInt CRMessageContainer::Write(TInt aParam,const TDesC8& aDes,TInt aOffset) const
    {
    // Write the data, change status and complete the RMessage.
    return iMsg.Write(aParam, aDes, aOffset);        
    }

void CRMessageContainer:: ReadL(TInt aParam,TDes8& aDes,TInt aOffset) const
    {
    // Write the data, change status and complete the RMessage.
    return iMsg.ReadL(aParam, aDes, aOffset);        
    }

void CRMessageContainer::Complete(TInt aReason)
    {
    iStatus = FALSE;
    if ( !iMsg.IsNull() )
        {
        MSRPLOG("CRMessageContainer::Complete message completed");
        iMsg.Complete(aReason);    
        }
    }

TBool CRMessageContainer::Check()
    {
    return iStatus;
    }

TBool CRMessageContainer::set(const RMessage2& aMessage)
    {    
    if(FALSE == iStatus)
        {
        iMsg = aMessage; // Exclicit copy.
        iStatus = TRUE;
        return iStatus;        
        }
    else
        {
        MSRPLOG("CRMessageContainer::set Error!! Container already full");
        return FALSE;
        }    
    }

CMSRPServerSubSession* CMSRPServerSubSession::NewL( 
    CMSRPServerSession& aServerSession, 
    CStateFactory& aStateFactory,
    const TDesC8& aSessionId )
    {
    CMSRPServerSubSession* self = 
        CMSRPServerSubSession::NewLC( aServerSession, aStateFactory, aSessionId );

    CleanupStack::Pop(self);
    return self;    
    }    


CMSRPServerSubSession* CMSRPServerSubSession::NewLC( 
    CMSRPServerSession& aServerSession,
    CStateFactory& aStateFactory,
    const TDesC8& aSessionId )
    {
    
    CMSRPServerSubSession* self =
                            new (ELeave) CMSRPServerSubSession( aServerSession, aStateFactory );
    CleanupStack::PushL(self);
    self->ConstructL( aSessionId );
    return self;
    }


CMSRPServerSubSession::CMSRPServerSubSession( CMSRPServerSession& aServerSession,CStateFactory& aStateFactory )
                         : iServerSession(aServerSession), iStateFactory(aStateFactory)
                                              
    {
    }


CMSRPServerSubSession::~CMSRPServerSubSession()
    {
    MSRPLOG("CMSRPServerSubSession::~CMSRPServerSubSession Entered");
    
    iState = NULL;         

    QueueLog();

    iOutMsgQ.Destroy();
    iInCommingMsgQ.Destroy();
    iPendingSendMsgQ.Destroy();
    iPendingForDeletionQ.Destroy();
    iPendingDataSendCompleteQ.Destroy();
    iPendingDataIncCompleteQ.Destroy();
    
    QueueLog();

    delete iCurrentMsgHandler;
    if(iConnection)
        {
        iConnection->ReleaseConnection(*this);
        iConnection = NULL;
        }
    delete iLocalSessionID;
    delete iRemoteSessionID;
    delete iReceivedResp;
    delete iReceiveFileMsgHdler;

    MSRPLOG("CMSRPServerSubSession::~CMSRPServerSubSession Exit");
    }


void CMSRPServerSubSession::ConstructL( const TDesC8& aSessionId )
    {
    // Not the place where this should be done!!
    MSRPLOG("CMSRPServerSubSession::ConstructL");  
    iState = iStateFactory.getStateL( EIdle );
    iLocalSessionID = HBufC8::NewL( aSessionId.Length() );
    *iLocalSessionID = aSessionId;    
    }


#if 0
HBufC8* CMSRPServerSubSession::CreateSubSessionIDL( )
    {
    MSRPLOG("CMSRPServerSubSession::CreateSubSessionIDL");
    HBufC8 *sessID = HBufC8::NewLC(KMSRPSessIdLength);
    TInt64 randSeed(Math::Random());
    TInt rand;
          
    /* Append Random bits using two Random strings */
    rand = Math::Rand(randSeed);
    (sessID->Des()).AppendNumFixedWidth(rand,EHex,KMSRPSessIdLength/2);
    rand = Math::Rand(randSeed);
    (sessID->Des()).AppendNumFixedWidth(rand,EHex,KMSRPSessIdLength/2);
        
    CleanupStack::Pop(1);// sessID
    return sessID;
    }
#endif


void CMSRPServerSubSession::ProcessEventL( TMSRPFSMEvent aEvent)
    {
    // Call the relevant state. Setup Traps for graceful error propagation to the client.
    MSRPLOG("CMSRPServerSubSession::ProcessEventL() Entered");

    QueueLog();    
            
    iState = iState->EventL(aEvent, this); 
    
    MSRPLOG("CMSRPServerSubSession::ProcessEventL() Exit ");
    }


void CMSRPServerSubSession::ServiceL( const RMessage2& aMessage )
    {
    MSRPLOG("CMSRPServerSubSession::ServiceL()");

    // Process Client Server functions to FSM Events.

    // Getting rid of the switch for translation.
    TMSRPFSMEvent event = (TMSRPFSMEvent) (aMessage.Function() - 2); 

    // Store the incoming aMessage to form the context of the state machine.
    iClientMessage = &aMessage;        

    // let's handle first the special case of EMSRPProgressReportsEvent
    // that event belongs to subsession
    if ( event == EMSRPProgressReportsEvent )
        {
        iSendProgressReports = aMessage.Int0();
        CompleteClient( KErrNone );
        return;
        }
    else
        {
        ProcessEventL(event);    
        }
    }


MMSRPConnectionManager& CMSRPServerSubSession::ConnectionManager( )
    {
    return iServerSession.ConnectionManager();
    }


// Implementation of interfaces from MMSRPConnectionObserver.
void CMSRPServerSubSession::ConnectionStateL( TInt /*aNewState*/, TInt /*aStatus*/ )
    {    
    ProcessEventL(EConnectionStateChangedEvent);
    }


TBool CMSRPServerSubSession::MessageReceivedL( CMSRPMessageHandler* aMsg )
    {
    MSRPLOG2("CMSRPServerSubSession::MessageReceivedL - New message received = %d", aMsg);
    if ( CheckMessageSessionIdL( aMsg ) )
        {
        if ( aMsg->IsMessageComplete() )
            {
            if ( iCurrentlyReceivingMsgQ.FindElement( aMsg ) )
                {
                iCurrentlyReceivingMsgQ.explicitRemove( aMsg );
                }
            }
        iReceivedMsg = aMsg;
        iReceivedMsg->SetMessageObserver( this );
        ProcessEventL( EMSRPIncomingMessageReceivedEvent );
        return ETrue;
        }
        
    return EFalse;    
    }


void CMSRPServerSubSession::UnclaimedMessageL( CMSRPMessageHandler* aMsg )
    {
    MSRPLOG("CMSRPServerSubSession::UnclaimedMessage - Unclaimed message received!!");
    
    if( (aMsg->MessageType() == MMSRPIncomingMessage::EMSRPMessage ||
            aMsg->MessageType() == MMSRPIncomingMessage::EMSRPNotDefined )
            && EFalse == matchSessionIDL(aMsg->GetIncomingMessage()->ToPathHeader()))
        {        
        aMsg->SendResponseL(this, *iConnection, ESessionDoesNotExist );
        if ( aMsg->IsMessageComplete() )
            {
            // no response was needed, must check report also 
            aMsg->SendReportL( this, *iConnection, ESessionDoesNotExist );        
            if ( aMsg->IsMessageComplete() )
                {
                delete aMsg;
                }
            else
                {
                iPendingForDeletionQ.Queue(*aMsg);
                }
            }
        else
            {
            iPendingForDeletionQ.Queue(*aMsg);
            }
        }
    else
        {
        delete aMsg; 
        }    
    }

// Implementation of interface from MMSRPMsgObserver.

void CMSRPServerSubSession::MessageSendCompleteL( CMSRPMessageHandler* aMessageHandler )
    {
    // Called when a message is fully sent out.
    iCurrentMsgHandler = aMessageHandler;
    ProcessEventL( EMSRPDataSendCompleteEvent );
    }

void CMSRPServerSubSession::MessageResponseSendCompleteL(CMSRPMessageHandler& aMsg)
    {
    // Called when a message is fully sent out.
    // Common event handling.
    if ( iPendingForDeletionQ.FindElement( &aMsg ) )
        {
        // no need to handle, this message did not belong to this session
        iPendingForDeletionQ.explicitRemove( &aMsg );
        delete &aMsg;
        return;
        }
    iReceivedResp = &aMsg;
    ProcessEventL( EMSRPResponseSendCompleteEvent );
    delete iReceivedResp;
    iReceivedResp = NULL;
    }

void CMSRPServerSubSession::MessageReportSendCompleteL( CMSRPMessageHandler& aMsg )
    {
    if ( iPendingForDeletionQ.FindElement( &aMsg ) )
        {
        // no need to handle, this message did not belong to this session
        iPendingForDeletionQ.explicitRemove( &aMsg );
        delete &aMsg;
        return;
        }
    iReceivedReport = &aMsg;
    ProcessEventL( EMSRPReportSendCompleteEvent );
    delete iReceivedReport;
    iReceivedReport = NULL;
    }

void CMSRPServerSubSession::MessageSendProgressL( CMSRPMessageHandler* aMessageHandler )
    {
    MSRPLOG2( "CMSRPServerSubSession::MessageSendProgressL, instance = %d", aMessageHandler )
    if ( iSendProgressReports )
        {
        iSendProgressMsg = aMessageHandler;
        ProcessEventL( EMSRPSendProgressEvent );
        }
    }

void CMSRPServerSubSession::MessageReceiveProgressL( CMSRPMessageHandler* aMessageHandler )
    {
    MSRPLOG2( "CMSRPServerSubSession::MessageReceiveProgressL, instance = %d", aMessageHandler )
    if ( iSendProgressReports )
        {
        if ( CheckMessageSessionIdL( aMessageHandler ) )
            {
            if ( !iCurrentlyReceivingMsgQ.FindElement( aMessageHandler ) )
                {
                iCurrentlyReceivingMsgQ.Queue( *aMessageHandler );
                }
            iReceiveProgressMsg = aMessageHandler;
            ProcessEventL( EMSRPReceiveProgressEvent );
            }
        }
    }

void CMSRPServerSubSession::MessageCancelledL( )
    {
    MSRPLOG( "CMSRPServerSubSession::MessageCancelledL enter" )
    ProcessEventL( EMSRPDataCancelledEvent );
    MSRPLOG( "CMSRPServerSubSession::MessageCancelledL exit" )
    }

void CMSRPServerSubSession::WriterError()
    {
    MSRPLOG( "CMSRPServerSubSession::WriterError!!" )
    }


// Utility Stuff - Common functions used often by States.
void CMSRPServerSubSession::CompleteClient(TInt aReason)
    {
    iClientMessage->Complete(aReason);
    iClientMessage = NULL;
    }

TInt CMSRPServerSubSession::Write(TInt aParam,const TDesC8& aDes)
    {
    TInt retVal = iClientMessage->Write(aParam, aDes);  
    return retVal; 
    }


TBool CMSRPServerSubSession::QueueClientSendRequestsL()
    {
    // Generic function to Queue the client send requests message. Would be used by 
    // many states to add to the pending client requests queue.
    MSRPLOG("CMSRPServerSubSession::QueueClientSendRequestsL");
        
    const RMessage2* aMessage = iClientMessage;
    iClientMessage = NULL;

    aMessage->ReadL( 0, iSendMSRPdataPckg);

    CMSRPMessageHandler *aMessageHandler 
        = CMSRPMessageHandler::NewL(this,
            iSendMSRPdataPckg().iExtMessageBuffer); 
    
    iPendingSendMsgQ.Queue(*aMessageHandler);    
    aMessage->Complete( KErrNone );        
        return TRUE;            
    }


void CMSRPServerSubSession::SendProgressToClientL( CMSRPMessageHandler* aMessageHandler )
    {
    MSRPLOG2("CMSRPServerSubSession::SendProgressToClientL enter, instance = %d", aMessageHandler );
    TInt bytesTransferred;
    TInt totalBytes;
    aMessageHandler->CurrentSendProgress( bytesTransferred, totalBytes );
    MSRPLOG2( "CMSRPServerSubSession::SendProgressToClientL, bytesr = %d", bytesTransferred )
    MSRPLOG2( "CMSRPServerSubSession::SendProgressToClientL, total = %d", totalBytes )
    HBufC8* messageId = aMessageHandler->MessageIdLC();
    iSendResultListenMSRPDataPckg().iStatus = EAllOk;
    iSendResultListenMSRPDataPckg().iIsProgress = TRUE;
    iSendResultListenMSRPDataPckg().iBytesSent = bytesTransferred;
    iSendResultListenMSRPDataPckg().iTotalBytes = totalBytes;
    if ( messageId->Length() < KMaxLengthOfSessionId )
        {
        iSendResultListenMSRPDataPckg().iMessageId = *messageId;
        }
    CleanupStack::PopAndDestroy(); // messageId
    iResponseListner.Write(0,iSendResultListenMSRPDataPckg);
    iResponseListner.Complete( KErrNone );

    MSRPLOG("CMSRPServerSubSession::SendProgressToClientL exit");
    }


void CMSRPServerSubSession::ReceiveProgressToClientL( CMSRPMessageHandler* aMessageHandler )
    {
    MSRPLOG2("CMSRPServerSubSession::ReceiveProgressToClientL enter, instance = %d", aMessageHandler );
    TInt bytesReceived;
    TInt totalBytes;
    aMessageHandler->CurrentReceiveProgress( bytesReceived, totalBytes );
    MSRPLOG2( "CMSRPServerSubSession::ReceiveProgressToClientL, bytesr = %d", bytesReceived )
    MSRPLOG2( "CMSRPServerSubSession::ReceiveProgressToClientL, total = %d", totalBytes )
    HBufC8* messageId = aMessageHandler->MessageIdLC();
    iListenMSRPdataPckg().iStatus    = KErrNone;
    iListenMSRPdataPckg().iIsProgress = TRUE;
    iListenMSRPdataPckg().iBytesRecvd = bytesReceived;
    iListenMSRPdataPckg().iTotalBytes = totalBytes;
    if ( messageId->Length() < KMaxLengthOfSessionId )
        {
        iListenMSRPdataPckg().iMessageId = *messageId;
        }
    CleanupStack::PopAndDestroy(); // messageId
    iIncommingMessageListner.Write(0,iListenMSRPdataPckg);
    iIncommingMessageListner.Complete( KErrNone );
    MSRPLOG("CMSRPServerSubSession::ReceiveProgressToClientL exit");
    }


void CMSRPServerSubSession::sendResultToClientL(CMSRPMessageHandler *msgHandler)
    {
    MSRPLOG("CMSRPServerSubSession::sendResultToClientL");
    HBufC8* messageId = NULL;

    // Allocates memory.
    TUint errorCode = msgHandler->GetSendResultL( messageId );            

    iSendResultListenMSRPDataPckg().iIsProgress = FALSE;
    iSendResultListenMSRPDataPckg().iMessageId = *messageId;
    iSendResultListenMSRPDataPckg().iStatus = errorCode;
    iResponseListner.Write(0,iSendResultListenMSRPDataPckg);

    delete messageId;
    iResponseListner.Complete( KErrNone );
    }


TBool CMSRPServerSubSession::sendMsgToClientL(CMSRPMessageHandler *incommingMsgHandler)
    {
    // Use the client send result listner to respond.
    // Extract the data and complete the iIncommingMessageListner.
    MSRPLOG("CMSRPServerSubSession::sendMsgToClientL");
    CMSRPMessage* inMsg = incommingMsgHandler->GetIncomingMessage();
    
    if ( iCurrentlyReceivingMsgQ.FindElement( incommingMsgHandler ) )
        {
        iCurrentlyReceivingMsgQ.explicitRemove( incommingMsgHandler );
        }

    if ( inMsg )
        {
        CBufSeg* buf1 = CBufSeg::NewL( 256 ); // expandsize 256
        CleanupStack::PushL( buf1 );
        RBufWriteStream writeStream( *buf1 );
        CleanupClosePushL( writeStream );
    
        inMsg->ExternalizeL( writeStream );
        writeStream.CommitL();
    
        iListenMSRPdataPckg().iIsProgress = FALSE;
        if ( buf1->Size() > KMaxLengthOfIncomingMessageExt )
            {
            // invalid message size
            iListenMSRPdataPckg().iStatus = KErrArgument;
            }
        else
            {
            buf1->Read( 0, iListenMSRPdataPckg().iExtMessageBuffer, buf1->Size() );
                iListenMSRPdataPckg().iStatus = KErrNone;            
            }
    
        CleanupStack::PopAndDestroy(2); // buf1, writestream    
       
        iIncommingMessageListner.Write(0,iListenMSRPdataPckg);
        iIncommingMessageListner.Complete(KErrNone);    
    
        return ETrue;
        }
        
    return EFalse;
    }

TBool CMSRPServerSubSession::sendReportToClientL( CMSRPMessageHandler *incommingMsgHandler )
    {
    // Use the client send result listner to respond.
    // Extract the data and complete the iIncommingMessageListner.
    MSRPLOG("CMSRPServerSubSession::sendReportToClientL");
    CMSRPReport* inMsg = incommingMsgHandler->GetIncomingReport();

    if ( inMsg )
        {
        CBufSeg* buf1 = CBufSeg::NewL( 256 ); // expandsize 256
        CleanupStack::PushL( buf1 );
        RBufWriteStream writeStream( *buf1 );
        CleanupClosePushL( writeStream );
    
        inMsg->ExternalizeL( writeStream );
        writeStream.CommitL();
    
        iListenMSRPdataPckg().iIsProgress = FALSE;
        if ( buf1->Size() > KMaxLengthOfIncomingMessageExt )
            {
            // invalid message size
            iListenMSRPdataPckg().iStatus = KErrArgument;
            }
        else
            {
            buf1->Read( 0, iListenMSRPdataPckg().iExtMessageBuffer, buf1->Size() );
                iListenMSRPdataPckg().iStatus = KErrNone;            
            }
    
        CleanupStack::PopAndDestroy(2); // buf1, writestream    
       
        iIncommingMessageListner.Write(0,iListenMSRPdataPckg);
        iIncommingMessageListner.Complete(KErrNone);    
    
        return ETrue;
        }
        
    return EFalse;
    }

void CMSRPServerSubSession::ReadSendDataPckgL()
    {
    iClientMessage->ReadL( 0, iSendMSRPdataPckg);
    return;
    }

TBool CMSRPServerSubSession::listnerSetupComplete()
    {
    if( iIncommingMessageListner.Check() && iResponseListner.Check() )
        {
        return ETrue;
        }
    
    return EFalse;        
    }

CMSRPServerSubSession::TQueueType CMSRPServerSubSession::getQToProcess()
    {
    // Priority 0, progress reports
    if ( iPendingSendProgressQ.Length( ) )
        {
        return TSendProgressQueue;
        }
    if ( iPendingReceiveProgressQ.Length( ) )
        {
        return TReceiveProgressQueue;
        }
    
    // priority 1, completed send messages
    if ( iPendingDataSendCompleteQ.Length() )
        {
        return TCompletedSendQueue;
        }

    // priority 2, completed receive message
    if ( iPendingDataIncCompleteQ.Length() )
        {
        return TCompletedIncQueue;
        }

    // priority 3, incoming messages and client requests
    if( iPendingSendMsgQ.Length() > iInCommingMsgQ.Length() )
        {
        return TClientQueue;
        }
    else
        {
        return TInCommingMsgQueue;
        }
    }


TBool CMSRPServerSubSession::QueuesEmpty()
    {
    // Returns TRUE if there are no messages to process.
    if( iPendingSendProgressQ.isEmpty() &&
        iPendingReceiveProgressQ.isEmpty() &&
        iPendingDataSendCompleteQ.isEmpty() &&
        iPendingDataIncCompleteQ.isEmpty() &&
        iPendingSendMsgQ.isEmpty() && 
        iInCommingMsgQ.isEmpty() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CMSRPServerSubSession::informConnectionReadyToClient()
    {
    // The function informs the Client side that the pending connection request(Connect or Listen)
    // is now complete. 

    if(EDownstream == iConnDirection)
        {
        // Case of Connect request. 
        iConnectionListner.Complete(KErrNone);
        }
    else
        {
        // Case of Listen.
        iIncommingMessageListner.Complete(KErrNone);        
        }
    return TRUE;
    }

void CMSRPServerSubSession::QueueLog()
    {
    if(iOutMsgQ.Length() || iInCommingMsgQ.Length() || iPendingSendMsgQ.Length()||
        iPendingForDeletionQ.Length() || iPendingDataSendCompleteQ.Length() ||
        iPendingDataIncCompleteQ.Length() || iPendingSendProgressQ.Length() || 
        iPendingReceiveProgressQ.Length() )
        {
        // If any of the Queue is not empty. Log a event.
        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iOutMsgQ       %d",
                    iOutMsgQ.Length());
        
        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iInCommingMsgQ %d",
                    iInCommingMsgQ.Length());
        
        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iPendingSendMsgQ %d",
                    iPendingSendMsgQ.Length());

        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iPendingForDeletionQ %d",
                    iPendingForDeletionQ.Length());

        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iPendingDataSendCompleteQ %d",
                iPendingDataSendCompleteQ.Length());

        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iPendingDataIncCompleteQ %d",
                iPendingDataIncCompleteQ.Length());

        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iPendingSendProgressQ %d",
                iPendingSendProgressQ.Length() );

        MSRPLOG2("CMSRPServerSubSession::ProcessEventL() Queue iPendingReceiveProgressQ %d",
                iPendingReceiveProgressQ.Length() );
        }
    else
        {
        MSRPLOG("CMSRPServerSubSession::ProcessEventL() Queue All Empty");
        }        
    }


// -----------------------------------------------------------------------------
// CMSRPServerSubSession::CheckMessageSessionIdL
// -----------------------------------------------------------------------------
//
TBool CMSRPServerSubSession::CheckMessageSessionIdL( CMSRPMessageHandler *aMsgHandler )
    {
    MSRPLOG( "-> CMSRPServerSubSession::CheckMessageSessionIdL" );
    CMSRPMessageBase *message = aMsgHandler->GetIncomingResponse();
    if(!message)
        { 
        message = aMsgHandler->GetIncomingMessage();
        }
    
    if( !message )
        {
        message = aMsgHandler->GetIncomingReport();
        }
  
    if( !message )
        {
        return EFalse; 
        }

    // Check if the sessionID in the 'To' path matches the LocalSessionID.
    if( !matchSessionIDL(message->ToPathHeader()) ||
            !matchSessionIDL(message->FromPathHeader(), EFalse ) )
        {
        return EFalse;
        }
        
    return ETrue;
    }
    
TBool CMSRPServerSubSession::matchSessionIDL(const CMSRPHeaderBase *aPathHeader, TBool local)
    {
    TBool retVal( EFalse );
    
    HBufC8* textValue = aPathHeader->ToTextValueLC();        
    TPtrC8 receivedSessionID = extractSessionID(*textValue);    
    
    if(local && receivedSessionID  == *iLocalSessionID)
        {
        retVal =  ETrue;
        }

    
    if(!local && receivedSessionID == *iRemoteSessionID)
        {
        retVal =  ETrue;
        }

    CleanupStack::PopAndDestroy(textValue);

    return retVal;
    }


TPtrC8 CMSRPServerSubSession::extractSessionID(const TDesC8& aPathBuffer)
    {
    // Extracts the sessionId from aPathBuffer and matches it again
    TInt posEndOfSessionString   = 
                aPathBuffer.LocateReverseF(KSemiColonCharacter);
    TInt posStartOfSessionString = 
                aPathBuffer.LocateReverseF(KDividedCharacter) + 1;

    TPtrC8 receivedSessionId = aPathBuffer.Mid(posStartOfSessionString, \
        (posEndOfSessionString - posStartOfSessionString));

    return receivedSessionId;
    }


CStateFactory& CMSRPServerSubSession::StateFactory()
    {
    return iStateFactory;
    }

