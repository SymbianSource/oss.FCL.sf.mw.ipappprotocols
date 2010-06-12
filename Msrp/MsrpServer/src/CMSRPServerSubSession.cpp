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
    iMsg.Complete(aReason);    
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

CMSRPServerSubSession* CMSRPServerSubSession::NewL( CMSRPServerSession& aServerSession, CStateFactory& aStateFactory )
    {
    CMSRPServerSubSession* self = 
        CMSRPServerSubSession::NewLC( aServerSession, aStateFactory );

    CleanupStack::Pop(self);
    return self;    
    }    


CMSRPServerSubSession* CMSRPServerSubSession::NewLC( CMSRPServerSession& aServerSession,CStateFactory& aStateFactory )
    {
    
    CMSRPServerSubSession* self =
                            new (ELeave) CMSRPServerSubSession( aServerSession, aStateFactory );
    CleanupStack::PushL(self);
    self->ConstructL();
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

    iOutMsgQ.Destroy();
    iInCommingMsgQ.Destroy();
    iPendingSendMsgQ.Destroy();
    iPendingForDeletionQ.Destroy();

    QueueLog();

    if(iCurrentMsgHandler)
        {
        delete iCurrentMsgHandler;
        iCurrentMsgHandler = NULL;
        }
    
    if(iConnection)
        {
        iConnection->ReleaseConnection(*this);
        iConnection = NULL;
        }

    if(iLocalSessionID)
        {
        delete iLocalSessionID;
        iLocalSessionID = NULL;
        }

    if( iRemoteSessionID )
        {
        delete iRemoteSessionID;
        iRemoteSessionID = NULL;
        }

    if( iReceivedResp )
        {
        delete iReceivedResp;
        iReceivedResp = NULL;
        }
    if( iReceiveFileMsgHdler )
        {
        delete iReceiveFileMsgHdler;
        iReceiveFileMsgHdler = NULL;
        }

    MSRPLOG("CMSRPServerSubSession::~CMSRPServerSubSession Exit");
    }


void CMSRPServerSubSession::ConstructL( )
    {
    // Not the place where this should be done!!
    MSRPLOG("CMSRPServerSubSession::ConstructL");  
    iState = iStateFactory.getStateL( EIdle );
    iLocalSessionID = CreateSubSessionIDL();    
    }


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


TBool CMSRPServerSubSession::ProcessEventL( TMSRPFSMEvent aEvent)
    {
    // Call the relevant state. Setup Traps for graceful error propagation to the client.
    MSRPLOG("CMSRPServerSubSession::ProcessEventL() Entered");

    QueueLog();    
            
    iState = iState->EventL(aEvent, this); 
    
    MSRPLOG("CMSRPServerSubSession::ProcessEventL() Exit ");
    return TRUE;
    }


TBool CMSRPServerSubSession::ServiceL( const RMessage2& aMessage )
    {
    MSRPLOG("CMSRPServerSubSession::ServiceL()");

    // Process Client Server functions to FSM Events.

    // Getting rid of the switch for translation.
    TMSRPFSMEvent event = (TMSRPFSMEvent) (aMessage.Function() - 2); 

    // Store the incomming aMessage to form the context of the state machine.
    iClientMessage = &aMessage;        
        
    return ProcessEventL(event);    
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
    MSRPLOG("CMSRPServerSubSession::MessageReceivedL - New message received");
    TBool retVal = FALSE;
    if(checkMessageForSelfL(aMsg))
        {           
        iReceivedMsg = aMsg;
        ProcessEventL(EMSRPIncomingMessageReceivedEvent);
        retVal = TRUE;
        }

    return retVal;    
    }


void CMSRPServerSubSession::UnclaimedMessageL( CMSRPMessageHandler* aMsg )
    {
    MSRPLOG("CMSRPServerSubSession::UnclaimedMessage - Unclaimed message received!!");
    
    if( (aMsg->MessageType() == MMSRPIncomingMessage::EMSRPMessage ||
            aMsg->MessageType() == MMSRPIncomingMessage::EMSRPNotDefined )
            && EFalse == matchSessionIDL(aMsg->GetIncomingMessage()->ToPathHeader()))
        {        
        TBool sendToClient = aMsg->SendResponseL(this, *iConnection, 
                CMSRPResponse::ESessionDoesNotExist);          
        iPendingForDeletionQ.Queue(*aMsg);
        }
    else
        {
        delete aMsg; 
        }    
    }


// Implementation of interface from MMSRPMsgObserver.

void CMSRPServerSubSession::MessageSendCompleteL()
    {
    // Called when a message is fully sent out.
    ProcessEventL(EMSRPDataSendCompleteEvent);
    }

void CMSRPServerSubSession::MessageResponseSendCompleteL(CMSRPMessageHandler& aMsg)
    {
    // Called when a message is fully sent out.
    // Common event handling.
    iReceivedResp = &aMsg;
    if(iFileShare)
        {        
        ProcessEventL(EMSRPResponseSendCompleteEvent);
        }
        if(iPendingForDeletionQ.explicitRemove(iReceivedResp))
            {
            delete iReceivedResp;
            iReceivedResp = NULL;
            }
        
    }


void CMSRPServerSubSession::MessageSendProgressL(TInt aBytesSent, TInt aTotalBytes)
    {
    iBytesTransferred = aBytesSent;
    iTotalBytes = aTotalBytes;
    ProcessEventL(EMSRPSendProgressEvent);    
    }


void CMSRPServerSubSession::MessageReceiveProgressL(TInt aBytesRecvd, TInt aTotalBytes)
    {
    iBytesTransferred = aBytesRecvd;
    iTotalBytes = aTotalBytes;
    ProcessEventL(EMSRPReceiveProgressEvent);    
    }


void CMSRPServerSubSession::WriterError()
    {
    
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


void CMSRPServerSubSession::NotifyFileReceiveResultToClientL(CMSRPMessageHandler */*msgHandler*/)
    {
    //TODO
    MSRPLOG("CMSRPServerSubSession::NotifyFileReceiveResultToClientL enter");
    iSendResultListenMSRPDataPckg().iStatus = 200;
    iSendResultListenMSRPDataPckg().iIsProgress = FALSE;
    iResponseListner.Write(0,iSendResultListenMSRPDataPckg);
    iResponseListner.Complete( KErrNone );
    MSRPLOG("CMSRPServerSubSession::NotifyFileReceiveResultToClientL exit");
    }


void CMSRPServerSubSession::NotifyFileSendResultToClientL(CMSRPMessageHandler */*msgHandler*/)
    {
    //TODO
    MSRPLOG("CMSRPServerSubSession::NotifyFileSendResultToClientL enter");
    iSendResultListenMSRPDataPckg().iStatus = 200;
    iSendResultListenMSRPDataPckg().iIsProgress = FALSE;
    iResponseListner.Write(0,iSendResultListenMSRPDataPckg);
    iResponseListner.Complete( KErrNone );
    MSRPLOG("CMSRPServerSubSession::NotifyFileSendResultToClientL exit");
    }


TBool CMSRPServerSubSession::SendProgressToClientL(CMSRPMessageHandler */*msgHandler*/)
    {
    MSRPLOG("CMSRPServerSubSession::SendProgressToClientL enter");
    iSendResultListenMSRPDataPckg().iStatus    = KErrNone;
    iSendResultListenMSRPDataPckg().iIsProgress = TRUE;
    iSendResultListenMSRPDataPckg().iBytesSent = iBytesTransferred;
    iSendResultListenMSRPDataPckg().iTotalBytes = iTotalBytes;
    
    iResponseListner.Write(0,iSendResultListenMSRPDataPckg);
    iResponseListner.Complete( KErrNone );

    MSRPLOG("CMSRPServerSubSession::SendProgressToClientL exit");
    return TRUE;
    }


TBool CMSRPServerSubSession::ReceiveProgressToClientL(CMSRPMessageHandler */*msgHandler*/)
    {
    MSRPLOG("CMSRPServerSubSession::ReceiveProgressToClientL enter");
    iListenMSRPdataPckg().iStatus    = KErrNone;
    iListenMSRPdataPckg().iIsProgress = TRUE;
    iListenMSRPdataPckg().iBytesRecvd = iBytesTransferred;
    iListenMSRPdataPckg().iTotalBytes = iTotalBytes;
    
    iIncommingMessageListner.Write(0,iListenMSRPdataPckg);
    iIncommingMessageListner.Complete( KErrNone );
    MSRPLOG("CMSRPServerSubSession::ReceiveProgressToClientL exit");
    return TRUE;
    }


TBool CMSRPServerSubSession::sendResultToClientL(CMSRPMessageHandler *msgHandler)
    {
    MSRPLOG("CMSRPServerSubSession::sendResultToClientL");
    HBufC8* messageId = NULL;
    TUint i=0;

    // Allocates memory.
    TBool error = msgHandler->GetSendResultL( i, messageId );            

    iSendResultListenMSRPDataPckg().iIsProgress = FALSE;
    iSendResultListenMSRPDataPckg().iMessageId = *messageId;
    iSendResultListenMSRPDataPckg().iStatus    = i;
    iResponseListner.Write(0,iSendResultListenMSRPDataPckg);

    delete messageId;
    iResponseListner.Complete( KErrNone );
    return error;
    }


TBool CMSRPServerSubSession::sendMsgToClientL(CMSRPMessageHandler *incommingMsgHandler)
    {
    // Use the client send result listner to respond.
    // Extract the data and complete the iIncommingMessageListner.
    MSRPLOG("CMSRPServerSubSession::sendMsgToClientL");
    CMSRPMessage* inMsg = incommingMsgHandler->GetIncomingMessage();

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

    return TRUE;
    }

void CMSRPServerSubSession::ReadSendDataPckgL()
    {
    iClientMessage->ReadL( 0, iSendMSRPdataPckg);
    return;
    }

TBool CMSRPServerSubSession::listnerSetupComplete()
    {
    if(iIncommingMessageListner.Check() && iResponseListner.Check())
        return TRUE;
    
    return FALSE;        
    }

CMSRPServerSubSession::TQueueType CMSRPServerSubSession::getQToProcess()
    {
    // Check the pending incomming message queue and client request queue and
    // decides which to process.
    // The longer queue is given a priority. If queues are equal incomming message queue 
    // is given priority.

    if(iPendingSendMsgQ.Length() > iInCommingMsgQ.Length())
        return TClientQueue;
    else
        return TInCommingMsgQueue;        
    }


TBool CMSRPServerSubSession::QueuesEmpty()
    {
    // Returns TRUE if there are no messages to process.
    if(iPendingSendMsgQ.isEmpty() && iInCommingMsgQ.isEmpty())
        return TRUE;
    else
        return FALSE;
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
        iPendingForDeletionQ.Length())
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
        }
    else
        {
        MSRPLOG("CMSRPServerSubSession::ProcessEventL() Queue All Empty");
        }        
    }


TBool CMSRPServerSubSession::checkMessageForSelfL(CMSRPMessageHandler *aMsgHandler)
    {
    MSRPLOG("CMSRPServerSubSession::checkMessageForSelfL");
    CMSRPMessageBase *message = aMsgHandler->GetIncomingResponse();
    if(!message)
        { 
        message = aMsgHandler->GetIncomingMessage();
        }
    
    if(!message)
        return FALSE;

    // Check if the sessionID in the 'To' path matches the LocalSessionID.
    if(FALSE == matchSessionIDL(message->ToPathHeader()))
        return FALSE;

    // Check if the sessionID in the 'From' path matches the known RemoteSessionID.
    return matchSessionIDL(message->FromPathHeader(), FALSE);            
    }


TBool CMSRPServerSubSession::matchSessionIDL(const CMSRPHeaderBase *aPathHeader, TBool local)
    {
    TBool retVal = FALSE;
    
    HBufC8* textValue = aPathHeader->ToTextValueLC();        
    TPtrC8 receivedSessionID = extractSessionID(*textValue);    
    
    if(local && receivedSessionID  == *iLocalSessionID)
        retVal =  TRUE;

    
    if(!local && receivedSessionID == *iRemoteSessionID)
        retVal =  TRUE;

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

