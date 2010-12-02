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

#include "TStates.h"
#include "TStateFactory.h"

#include "CMSRPServerSubSession.h"
#include "CMSRPMessageHandler.h"
#include "CMSRPResponse.h"
#include "MMSRPConnection.h"


TStateBase* TStateBase::HandleStateErrorL(CMSRPServerSubSession *aContext)
    {
    // Error handling for invalid events received in a given state.
    if(NULL != aContext->iClientMessage)
        {
        // Complete the client with a error.
        // Set this to iClientReceivedEventData to NULL.
        aContext->CompleteClient(KErrNone);                 
        }

    if(NULL != aContext->iReceivedMsg)
        {
        // !! Handle
        }
    return aContext->StateFactory().getStateL(EIdle);
    }  

TStateBase* TStateBase::processIncommingMessageL(CMSRPServerSubSession *aContext, 
                 CMSRPMessageHandler* incommingMsg)
    {
    MSRPLOG("TStateBase::processIncommingMessage Entered!");
    CMSRPMessageHandler *incommingMsgHandler;
    if(NULL == incommingMsg)
        {
        incommingMsgHandler = aContext->iInCommingMsgQ.DeQueue();
        }
    else
        {
        incommingMsgHandler = incommingMsg;
        }
 
    if(incommingMsgHandler)
        {        
        if(MMSRPIncomingMessage::EMSRPResponse == incommingMsgHandler->MessageType())
            {
            return handlesResponseL(aContext,incommingMsgHandler);                    
            }
        if( MMSRPIncomingMessage::EMSRPReport == incommingMsgHandler->MessageType( ) )
            {
            return handleRequestsL( aContext,incommingMsgHandler );
            }
        else
            {
            return handleRequestsL(aContext,incommingMsgHandler);
            }        
        }
    else
        return NULL; 
    } 

TStateBase* TStateBase::processPendingMessageQL(CMSRPServerSubSession *aContext)
    {
    MSRPLOG("TStateBase::processPendingMessagesL Entered!");
    CMSRPMessageHandler *msgHandler;
     
    msgHandler = aContext->iPendingSendMsgQ.DeQueue();
    msgHandler->SendMessageL(*aContext->iConnection);
    aContext->iOutMsgQ.Queue( *msgHandler );
    return aContext->StateFactory().getStateL( EActive );
    }
    
TStateBase* TStateBase::processCompletedMessageL( CMSRPServerSubSession *aContext )
    {
    MSRPLOG("TStateBase::processCompletedMessageL Entered!");
    CMSRPMessageHandler* msgHandler = aContext->iPendingDataSendCompleteQ.DeQueue();
    
    if ( msgHandler->IsTransmissionTerminated() )
        {
        MSRPLOG("TStateBase::processCompletedMessageL transmission terminated");
        if( aContext->iResponseListner.Check() )
            {
            MSRPLOG("TStateBase::processCompletedMessageL complete with KErrCancel");    
            aContext->iResponseListner.Complete( KErrCancel );
            }
        }
    else
        {
        aContext->sendResultToClientL( msgHandler );      
        }

    delete msgHandler;
    return aContext->StateFactory().getStateL( EWaitForClient );
    }
    
TStateBase* TStateBase::processCompletedIncomingMessageL( CMSRPServerSubSession *aContext )
    {
    MSRPLOG("TStateBase::processCompletedIncomingMessageL Entered!");
    CMSRPMessageHandler* msgHandler = aContext->iPendingDataIncCompleteQ.DeQueue();
    
    if ( msgHandler->IsTransmissionTerminated() )
        {
        MSRPLOG("TStateBase::processCompletedIncomingMessageL transmission terminated");
        if( aContext->iIncommingMessageListner.Check() )
            {
            MSRPLOG("TStateBase::processCompletedIncomingMessageL complete with KErrCancel");    
            aContext->iIncommingMessageListner.Complete( KErrCancel );
            }
        }
    else
        {
        aContext->sendMsgToClientL( msgHandler );      
        delete msgHandler;
        }
        
    return aContext->StateFactory().getStateL( EWaitForClient );
    }
 
TStateBase* TStateBase::processReceiveReportL( CMSRPServerSubSession *aContext )
    {
    MSRPLOG("TStateBase::processReceiveReportL Entered!");
    CMSRPMessageHandler* msgHandler = aContext->iPendingReceiveProgressQ.DeQueue();

    aContext->ReceiveProgressToClientL( msgHandler );
    return aContext->StateFactory().getStateL( EWaitForClient );
    }
    
TStateBase* TStateBase::processSendReportL( CMSRPServerSubSession *aContext )
    {
    MSRPLOG("TStateBase::processSendReportL Entered!");
    CMSRPMessageHandler* msgHandler = aContext->iPendingSendProgressQ.DeQueue();

    aContext->SendProgressToClientL( msgHandler );
    return aContext->StateFactory().getStateL( EWaitForClient );
    }

TStateBase* TStateBase::handlesResponseL(CMSRPServerSubSession *aContext,
                 CMSRPMessageHandler *incommingMsgHandler)
    {
    TStateBase *nextState; 
    MSRPLOG("TStateBase::handlesResponseL Entered!");
    
    // Search the outgoing Queue to find the owner of the owner of this response.
    CMSRPMessageHandler *outgoingMsgHandler = 
                    aContext->iOutMsgQ.getMatch(incommingMsgHandler);
    
    if(NULL == outgoingMsgHandler)
        {
        // No outgoingMsgHandler to match the received response. Stray response.
        MSRPLOG("TStateBase::handlesResponseL() Received Stray Response!!");
        MSRPLOG("TStateBase::handlesResponseL() No Outgoing message handler found");
        nextState = this;
        }
    else
        {
        CleanupStack::PushL( incommingMsgHandler );
        TUint responseCode = outgoingMsgHandler->ConsumeResponseL( *incommingMsgHandler );
         
        if( outgoingMsgHandler->IsMessageComplete() ||
            responseCode != EAllOk ||
            ( outgoingMsgHandler->IsFailureHeaderPartial() &&
              responseCode != EAllOk ) )
            {
            aContext->iConnection->CancelSendingL( outgoingMsgHandler );
            aContext->sendResultToClientL( outgoingMsgHandler );
            aContext->iOutMsgQ.explicitRemove( outgoingMsgHandler );    
            delete outgoingMsgHandler;
            }
        else
            {
            aContext->iConnection->ContinueSendingL( *outgoingMsgHandler );
            }
                 
        CleanupStack::Pop(incommingMsgHandler); 
        nextState = aContext->StateFactory().getStateL(EWaitForClient);     
        }
    
    delete incommingMsgHandler;    
    
    MSRPLOG("TStateBase::handlesResponseL Exit!");
    return nextState;    
    }
 
TStateBase* TStateBase::handleRequestsL(CMSRPServerSubSession *aContext,
                 CMSRPMessageHandler *incommingMsgHandler)
    {
    MSRPLOG("-> TStateBase::handleRequestsL");
    MMSRPIncomingMessage::TMSRPMessageType msgType = incommingMsgHandler->MessageType();
    
    if(MMSRPIncomingMessage::EMSRPMessage == msgType) // SEND request
        {
        MSRPLOG("SEND request received");
        incommingMsgHandler->SendResponseL(aContext, *aContext->iConnection, EAllOk );        
        incommingMsgHandler->SendReportL(aContext, *aContext->iConnection, EAllOk );        
        if( incommingMsgHandler->IsMessageComplete() )
            {
            MSRPLOG("-> TStateBase::handleRequestsL complete");
            aContext->sendMsgToClientL(incommingMsgHandler);
            delete incommingMsgHandler;
            //aContext->iPendingForDeletionQ.Queue(*incommingMsgHandler);
            return aContext->StateFactory().getStateL(EWaitForClient);
            }
        return aContext->StateFactory().getStateL( EActive );
        }
    else if(MMSRPIncomingMessage::EMSRPReport == msgType) // Drop Reports
        {
        aContext->sendMsgToClientL( incommingMsgHandler );
        delete incommingMsgHandler;
        //aContext->iPendingForDeletionQ.Queue(*incommingMsgHandler);
        return aContext->StateFactory().getStateL( EWaitForClient );
        }
    else // It is an unrecognized request eg. AUTH
        {
        MSRPLOG("Unrecognized request received");
        incommingMsgHandler->SendResponseL(aContext, *aContext->iConnection, EUnknownRequestMethod );        
        aContext->iPendingForDeletionQ.Queue(*incommingMsgHandler);
        return aContext->StateFactory().getStateL(EWaitForClient);
        }
    } 
    
TStateBase* TStateBase::handleClientListnerCancelL( CMSRPServerSubSession* aContext, 
        TMSRPFSMEvent aEvent)
    {
    MSRPLOG("-> TStateBase::handleClientListnerCancelL");
    if(aEvent == EMSRPCancelReceivingEvent)
        {
        // let's find the correct listener first
        TBuf8< KMaxLengthOfTransactionIdString > messageId;
        aContext->iClientMessage->ReadL( 0, messageId );
        // going through all incoming instances
        CMSRPMessageHandler* owner = aContext->iCurrentlyReceivingMsgQ.FindElement( messageId );
        if ( owner )
            {
            MSRPLOG2("TStateBase::handleClientListnerCancelL terminating %d", owner );
            owner->TerminateReceiving( aContext, *aContext->iConnection );
            }
        else
            {
            MSRPLOG("TStateBase::handleClientListnerCancelL completing incoming listener"  );
            if( aContext->iIncommingMessageListner.Check() )
                {
                aContext->iIncommingMessageListner.Complete( KErrNone );
                }
            }
        
        // Confirm completion of the Cancel Request.
        aContext->CompleteClient( KErrNone );
        }    
    
    if(aEvent == EMSRPCancelSendRespListeningEvent)
        {
        // Confirm completion of the Cancel Request.
        aContext->CompleteClient( KErrNone );
        
        // Complete the Listner if that is active.
        if(aContext->iResponseListner.Check())
            aContext->iResponseListner.Complete( KErrNone );        
        }
    
    return aContext->StateFactory().getStateL(EWaitForClient);    
    }
    
TStateBase* TStateBase::HandleClientCancelSendingL( CMSRPServerSubSession* aContext )
    {
    MSRPLOG("-> TStateBase::HandleClientCancelSendingL");
    // let's find the correct listener first
    TBuf8< KMaxLengthOfTransactionIdString > messageId;
    aContext->iClientMessage->ReadL( 0, messageId );
    
    // going through all incoming instances
    CMSRPMessageHandler* owner = aContext->iOutMsgQ.FindElement( messageId );
    if ( owner )
        {
        MSRPLOG2("TStateBase::HandleClientCancelSendingL terminating %d", owner );
        owner->TerminateSending( );
        }
    
    // Confirm completion of the Cancel Request.
    aContext->CompleteClient( KErrNone );
    return this;
    }
  
TStateBase* TStateBase::handleConnectionStateChangedL(CMSRPServerSubSession *aContext)
    {
    TStateBase* state = NULL;    
    TInt iConnectionEvent = aContext->iConnection->getConnectionState();
    
    MSRPLOG2("handleConnectionStateChanged::Connection Event %d \n",iConnectionEvent ); 
    
    switch(iConnectionEvent)
        {
        case -1: // Error Scenario    
            {
            // empty queue
            MSRPLOG("TStateBase::handleConnectionStateChangedL, emptying queue " ); 
            while ( aContext->iCurrentlyReceivingMsgQ.DeQueue() );
            state = handleConnectionErrorsL(aContext);
            break;
            }
        
        case 1:
        case 2:
            MSRPLOG("handleConnectionStateChanged::Connection in Progress \n");
            state = this;
            break;
         
        case 3: 
            // Connected.
            MSRPLOG("handleConnectionStateChanged: Connected now." );    
            aContext->informConnectionReadyToClient();
            state = aContext->StateFactory().getStateL(EWaitForClient);            
            break;
        
        case 4:
        case 5:
            MSRPLOG("handleConnectionStateChanged: TimeOut now." ); 
            if(EDownstream == aContext->iConnDirection)
                aContext->iConnectionListner.Complete(-33); // TODO : Change to query the exact erorr code from the connection.
            else
                aContext->iIncommingMessageListner.Complete(-33); // TODO : Change to query the exact erorr code from the connection.    
             
            state = aContext->StateFactory().getStateL(EWaitForClient);            
            break;
        
        case 0:      
            // Not Connected.
            MSRPLOG("handleConnectionStateChanged: TimeOut now." );
            state = this;  
            break;
        }
    
    return state;    
    }

TStateBase* TStateBase::handleConnectionErrorsL(CMSRPServerSubSession *aContext)
    {
    TStateBase* state;
    if(TRUE == aContext->iConnectionListner.Check())
        {
        // Have a connection listner to inform connection errors.
        // This scenario happens where there is a connection error during "Connect"
        
        aContext->iConnectionListner.Complete(KErrCouldNotConnect);  // TODO : Change to query the exact erorr code from the connection.
        aContext->iConnection->ReleaseConnection(*aContext);
        aContext->iConnection = NULL;
        state = aContext->StateFactory().getStateL(EWaitForClient);        
        }
     else
        {
        // If do not have a connection listner.
        // This is a case where a established connection(connect/listen) 
        // is now reporting a Error.
        
        // ConnectionErrors in the current scheme of things will be reported via
        // iIncommingMessageListner.
        
        if(!aContext->iIncommingMessageListner.Check())
            {
            // Case where the subsession is waiting for the Client subsessions 
            // to provide it with listner to reports issues on.
            // Just move to the EIdle state. EIdle is the not connected state.

            // The connection error here will be reported from the TIdle state.
            
            state = aContext->StateFactory().getStateL(EIdle);
            }
        else
            {
            aContext->iIncommingMessageListner.Complete(KErrDisconnected);
            aContext->iConnection->ReleaseConnection(*aContext);
            aContext->iConnection = NULL;
            state = aContext->StateFactory().getStateL(EIdle);
            }
        }
    return state;
    }

TStateBase* TStateBase::handleQueuesL(CMSRPServerSubSession *aContext)
    {
    TStateBase * state;

    if( aContext->QueuesEmpty() )
        {
        state = aContext->StateFactory().getStateL( EActive );
        }
    else
        {   
        CMSRPServerSubSession::TQueueType type = aContext->getQToProcess();
        if ( type == CMSRPServerSubSession::TReceiveProgressQueue )
            {
            state  =  processReceiveReportL( aContext );       
            }
        else if ( type == CMSRPServerSubSession::TSendProgressQueue )
            {
            state  =  processSendReportL( aContext );       
            }
        else if ( type == CMSRPServerSubSession::TCompletedSendQueue )
            {
            state  =  processCompletedMessageL( aContext );       
            }
        else if ( type == CMSRPServerSubSession::TCompletedIncQueue )
            {
            state  =  processCompletedIncomingMessageL( aContext );       
            }
        else if( type == CMSRPServerSubSession::TInCommingMsgQueue )
            {
            state  =  processIncommingMessageL( aContext );
            }
        else
            {
            state  =  processPendingMessageQL( aContext );
            }
        }   

    return state;
    }

TStateBase* TStateBase::handleInCommingMessagesL(CMSRPServerSubSession *aContext)
     {
     CMSRPMessageHandler* incommingMsg = aContext->iReceivedMsg;
     aContext->iReceivedMsg = NULL;
     
     return processIncommingMessageL(aContext, incommingMsg);             
     }

TStateBase* TStateBase::MessageSendCompleteL(CMSRPServerSubSession *aContext)
    {
    // Handle send message complete event.
    if( NULL == aContext->iCurrentMsgHandler)
        {
        MSRPLOG( "TStateBase::MessageSendCompleteL :: iCurrentMsgHandler is NULL \n" );                                     
        return this;
        }
        
    if ( aContext->iCurrentMsgHandler->IsTransmissionTerminated() )
        {
        MSRPLOG("TStateBase::processCompletedMessageL transmission terminated");
        if( aContext->iResponseListner.Check() )
            {
            MSRPLOG("TStateBase::processCompletedMessageL complete with KErrCancel");    
            aContext->iResponseListner.Complete( KErrCancel );
            }
        return aContext->StateFactory().getStateL( EWaitForClient );
        }
        
    if( aContext->iCurrentMsgHandler->IsMessageComplete() )
        {
        // Messages with Failure Report set to "No" will never get a response.        
        aContext->sendResultToClientL( aContext->iCurrentMsgHandler );      
        CMSRPMessageHandler* temp = aContext->iOutMsgQ.getMatch( aContext->iCurrentMsgHandler );
        if ( temp )
            {
            aContext->iOutMsgQ.explicitRemove( aContext->iCurrentMsgHandler );
            }
        delete aContext->iCurrentMsgHandler;
        aContext->iCurrentMsgHandler = NULL;
        return aContext->StateFactory().getStateL( EWaitForClient );
        }
    else
        {
        aContext->iCurrentMsgHandler = NULL;
        // Handle any pending events in Queue.
        return handleQueuesL(aContext);     
        }
    }

TStates TStateIdle::identity()
    {
    return EIdle;
    }
 
TStateBase* TStateIdle::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
    {
    // TStateIdle is a state where the subsession is waiting to get connected.     
    // It is the first state which is entered post sub-session creation.
    // Also it is the state that is entered in case there is of a connection error.
    
    // It may be possible that the client could not be informed about the 
    // connection error. Thus any client listner setup events will inform the client about 
    // the current disconnected state.
    
    // Any existing queues of received messages, received client send requests would be 
    // retained as it is. Any events for send messages received from the client 
    // would be queued up.
    
    // An event from the connection layer is considered invalid as the connection is 
    // assumed to be dead. Such events will result in a error report being published.

     MSRPLOG2("Entered TStateIdle Event %d",aEvent);
     TStateBase* state;
     
     switch(aEvent)
         {
         case ELocalMSRPPathEvent:
             state =  handleLocalMSRPPathRequestL(aContext);
             break;     
                 
         case EMSRPConnectEvent:  
             aContext->iConnDirection = EDownstream; 
             state = handleConnectRequestL(aContext);
             break;
             
         case EMSRPListenEvent:        
             aContext->iConnDirection = EUpstream;
             state = handleListenRequestL( aContext );
             break;         

        case EMSRPListenMessagesEvent:
        case EMSRPListenSendResultEvent:
            // A MSRP Connection error code would be more appropriate here.
            aContext->CompleteClient(KErrCouldNotConnect); 
            state = this;
            break;
            
        case EMSRPCancelReceivingEvent:          
        case EMSRPCancelSendRespListeningEvent:
             state = handleClientListnerCancelL(aContext, aEvent);
                 break;

        case EConnectionStateChangedEvent:
             state = handleConnectionStateChangedL(aContext);
             break;
                 
        default:
             // Any such error usually a client/server protocol voilation.
             // A bug to fix.
             MSRPLOG2("TStateIdle::EventL :: Err!! Invalid state to have received %d",aEvent);                     
             state =  HandleStateErrorL(aContext);
             break;
        }
    return state;     
    }
 
TStateBase* TStateIdle::handleLocalMSRPPathRequestL( CMSRPServerSubSession *aContext)
    {
    // The function is reponsible for contructing a  Local MSRP path and returning it to the 
    // client.
    // path:msrp://atlanta.example.com:7654/jshA7weztas;tcp
    
    // !! The current implementation should change to fully construct a MSRP path in the 
    // subsessoin and send back the result as path buffer. Right now this done in the client. !!
    
    MSRPLOG("TStateIdle::HandleLocalPathRequestL()");
     
    TInetAddr localAddr;
    aContext->ConnectionManager().ResolveLocalIPAddressL( localAddr );               
    localAddr.Output(aContext->iLocalHost);          
    
    aContext->iLocalPathMSRPDataPckg().iLocalHost.Copy( aContext->iLocalHost );
    
    TInt reason = aContext->Write(0, aContext->iLocalPathMSRPDataPckg);     
    aContext->CompleteClient(KErrNone);
        
    return this; // No state change.
    }
 
TStateBase* TStateIdle::handleConnectRequestL(CMSRPServerSubSession *aContext)
    {
    
    if(!aContext->iConnectionListner.set(*(aContext->iClientMessage)))
        {
        MSRPLOG("TStateIdle::handleConnectRequestL() iConnectionListner is already setup");
        MSRPLOG("TStateIdle::handleConnectRequestL() Invalid state to setup for ConnectionListner");
        return HandleStateErrorL(aContext);
        }  
    aContext->iClientMessage = NULL; 
    
    aContext->iConnectionListner.ReadL( 0, aContext->iConnectMSRPdataPckg );     
    
    aContext->iRemoteHost = aContext->iConnectMSRPdataPckg().iRemoteHost;
    aContext->iRemotePort = aContext->iConnectMSRPdataPckg().iRemotePort;
    
    aContext->iRemoteSessionID = 
            HBufC8::NewL( aContext->iConnectMSRPdataPckg().iRemoteSessionID.Length());
    
    *(aContext->iRemoteSessionID) = 
             aContext->iConnectMSRPdataPckg().iRemoteSessionID;
    
    // Request for a Connection.
    MMSRPConnection &connection = 
             aContext->ConnectionManager().getConnectionL(
                                  aContext->iRemoteHost
                                 ,aContext->iRemotePort);
    aContext->iConnection = &connection;    
    
    TBool connReady = initializeConnectionL(aContext);
    
    if(!connReady)
        { 
        MSRPLOG("TStateIdle:: Transtion to State EConnecting" );
        return aContext->StateFactory().getStateL(EConnecting);
        }
    else
        {
        MSRPLOG("TStateIdle:: Transtion to State EConnecting" );
        aContext->informConnectionReadyToClient();
        return aContext->StateFactory().getStateL(EWaitForClient);    
        }
    }
 
TStateBase* TStateIdle::handleListenRequestL(CMSRPServerSubSession *aContext)
    {  
    if(!aContext->iIncommingMessageListner.set(*(aContext->iClientMessage)))
        {
        MSRPLOG("TStateWaitForClient::EventL iIncomming listner is already setup");
        MSRPLOG("TStateWaitForClient::EventL Invalid state to setup a listner");
        return HandleStateErrorL(aContext);
        }    
    aContext->iClientMessage = NULL;    
    aContext->iIncommingMessageListner.ReadL( 0, aContext->iListenMSRPdataPckg );
    
    aContext->iRemoteHost = aContext->iListenMSRPdataPckg().iRemoteHost;
    aContext->iRemotePort = aContext->iListenMSRPdataPckg().iRemotePort; 
    aContext->iRemoteSessionID    = 
            HBufC8::NewL( aContext->iListenMSRPdataPckg().iRemoteSessionID.Length());
    
    *(aContext->iRemoteSessionID) = 
             aContext->iListenMSRPdataPckg().iRemoteSessionID;    
    
    // Request for a Connection.
    MMSRPConnection &connection   = 
             aContext->ConnectionManager().getConnectionL(
                                  aContext->iRemoteHost
                                 ,aContext->iRemotePort);
    aContext->iConnection = &connection;    
    
    TBool connReady = initializeConnectionL(aContext);
    
    if(!connReady)
        { 
        MSRPLOG("TStateIdle:: Transtion to State EConnecting" );
        return aContext->StateFactory().getStateL(EConnecting);
        }
    else
        {
        MSRPLOG("TStateIdle:: Transtion to State EConnecting" );
        aContext->informConnectionReadyToClient();
        return aContext->StateFactory().getStateL(EWaitForClient);    
        }
    } 
 
TBool TStateIdle::initializeConnectionL(CMSRPServerSubSession *aContext)
    {
     // Connect or Listen on Connection.
     // Return status based on connection is ready for use or not.
    TBool retVal = FALSE;
     
    if(EDownstream == aContext->iConnDirection)
        {
        if(3 == aContext->iConnection->ConnectL(*aContext))
            retVal = TRUE;              
        }
    else
        {
        if(3 == aContext->iConnection->ListenL(*aContext))
            retVal = TRUE;
        }
     
    return retVal;    
    }
 
TStates TStateConnecting::identity()
    {
    return EConnecting;
    }
  
TStateBase* TStateConnecting::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
    {
    MSRPLOG2("Entered TStateConnecting Event %d",aEvent);
    TStateBase *state = this;

    switch(aEvent)
        {
        case EMSRPProcessQueuedRequestsEvent:
        aContext->CompleteClient(KErrNone);              
        break;
        
        case EConnectionStateChangedEvent:
        state = handleConnectionStateChangedL(aContext);
        break;
        
        case EMSRPDataSendMessageEvent:
        aContext->QueueClientSendRequestsL();                         
        break;
                 
        default:                      
        MSRPLOG2("TStateConnecting::EventL :: Err!! Invalid state to have received %d",
                              aEvent);                     
        state = HandleStateErrorL(aContext);            
        break;              
        }

    return state;    
    }

TStates TStateWaitForClient::identity()
    {
    return EWaitForClient;
    }
 
TStateBase* TStateWaitForClient::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
    {
      // In the TStateWaitForClient stat the server subsession waits for the client to setup 
      // handlers to receive requests/responses/reports from the MSRP peer.
      // TODO - TStateWaitForClient can be removed. Replace this with a eventQueue implementation.
      
      MSRPLOG2("Entered TStateWaitForClient Event %d",aEvent);
  
      TStateBase *state = NULL;
      switch(aEvent)
          {
          case EMSRPProcessQueuedRequestsEvent:
              aContext->CompleteClient(KErrNone);              
              break;
          
          case EMSRPListenMessagesEvent:
              if(!aContext->iIncommingMessageListner.set(*(aContext->iClientMessage)))
                  {
                  MSRPLOG("TStateWaitForClient::EventL iIncomming listner is already setup");
                  MSRPLOG2("TStateWaitForClient::EventL Invalid state to have received %d", aEvent);                
                  }              
              aContext->iClientMessage = NULL;
              break;
              
          case EMSRPListenSendResultEvent:
              if(!aContext->iResponseListner.set(*(aContext->iClientMessage)))
                  {
                  MSRPLOG("TStateWaitForClient::EventL SendResult Listner is already setup");
                  MSRPLOG2("TStateWaitForClient::EventL Invalid state to have received %d", aEvent);                
                  }              
              aContext->iClientMessage = NULL;
              break;
  
          case EMSRPIncomingMessageReceivedEvent:
              // Queue any thing that comes.
              aContext->iInCommingMsgQ.Queue(*aContext->iReceivedMsg);                  
              break;  
              
          case EMSRPDataSendCompleteEvent:
          case EMSRPDataCancelledEvent:
              {
              MSRPLOG("TStateWaitForClient::EventL EMSRPDataCancelledEvent");

              if( aContext->iCurrentMsgHandler->IsMessageComplete() )
                  {
                  MSRPLOG("TStateWaitForClient::EventL moving to pending data");
                  aContext->iOutMsgQ.explicitRemove( aContext->iCurrentMsgHandler );    
                  aContext->iPendingDataSendCompleteQ.Queue( *aContext->iCurrentMsgHandler );
                  }
              aContext->iCurrentMsgHandler = NULL;
              break;
              }
              
          case EMSRPResponseSendCompleteEvent:
              {
              if( aContext->iReceivedResp->IsMessageComplete() )
                  {
                  aContext->iPendingDataIncCompleteQ.Queue( *aContext->iReceivedResp );
                  }
              aContext->iReceivedResp = NULL;
              break;
              }

          case EMSRPReportSendCompleteEvent:
              {
              if( aContext->iReceivedReport->IsMessageComplete() )
                  {
                  aContext->iPendingDataIncCompleteQ.Queue( *aContext->iReceivedReport );
                  }
              aContext->iReceivedReport = NULL;
              break;
              }

          case EMSRPDataSendMessageEvent:
              aContext->QueueClientSendRequestsL();                         
              break;
                              
          case EMSRPCancelReceivingEvent:          
          case EMSRPCancelSendRespListeningEvent:
               handleClientListnerCancelL(aContext, aEvent);
               break;
               
          case EMSRPDataSendMessageCancelEvent:
              {
              HandleClientCancelSendingL( aContext );
              break;
              }

          case EConnectionStateChangedEvent:
              state = handleConnectionStateChangedL(aContext);
              break;
              
          case EMSRPSendProgressEvent:
              {
              state = aContext->StateFactory().getStateL( EWaitForClient );
              // queue only if this msghandler is not already in the queue
              if ( aContext->iPendingSendProgressQ.FindElement( aContext->iSendProgressMsg ) == NULL )
                  {
                  aContext->iPendingSendProgressQ.Queue( *aContext->iSendProgressMsg );                  
                  }
              break;
              }
          case EMSRPReceiveProgressEvent:
              {
              // Queue any thing that comes.
              state = aContext->StateFactory().getStateL( EWaitForClient );
              // queue only if this msghandler is not already in the queue
              if ( aContext->iPendingReceiveProgressQ.FindElement( aContext->iReceiveProgressMsg ) == NULL )
                  {
                  aContext->iPendingReceiveProgressQ.Queue( *aContext->iReceiveProgressMsg );                  
                  }
              break;
              }
                  
          default:          
              // Any such error usually a client/server protocol voilation or connection/subsession 
              // protocol voilation. A bug to fix!!
              MSRPLOG2("TStateWaitForClient::EventL :: Err!! Invalid state was received %d",aEvent);                     
              state = HandleStateErrorL(aContext);
              break;              
          };

      if(NULL == state)
        {
          // State not set.
          if( !aContext->listnerSetupComplete() )
              {
              state = this;
              }
          else
              {
              state = handleQueuesL(aContext);
              }
        }
        return state;    
    }
    
TStates TStateActive::identity()
    {
    return EActive;
    } 
 
TStateBase* TStateActive::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
    {
     // TStateActive is entered when the subsession is in a connected state and  client setup 
     // is done.
 
     MSRPLOG2("Entered TStateActive Event %d",aEvent);
 
     TStateBase *state;
     switch(aEvent)
         {
         case EMSRPDataSendMessageEvent:
              state =  handleSendDataL(aContext);
              break; 
 
         case EMSRPIncomingMessageReceivedEvent:
              state =  handleInCommingMessagesL(aContext);
              break; 
 
         case EConnectionStateChangedEvent:
              state = handleConnectionStateChangedL(aContext);
              break;
  
         case EMSRPCancelReceivingEvent:          
         case EMSRPCancelSendRespListeningEvent:
              state =  handleClientListnerCancelL(aContext, aEvent);
              break;   
         
         case EMSRPDataSendMessageCancelEvent:
             {
             state = HandleClientCancelSendingL( aContext );
             break;
             }
              
         case EMSRPDataCancelledEvent:
         case EMSRPDataSendCompleteEvent:
             state = MessageSendCompleteL(aContext);
             break;
             
         case EMSRPResponseSendCompleteEvent:
             {
             state = handleResponseSentL(aContext);  
             break;
             }

         case EMSRPReportSendCompleteEvent:
             {
             state = handleReportSentL( aContext );  
             break;
             }

         case EMSRPReceiveProgressEvent:
             {
             aContext->ReceiveProgressToClientL( aContext->iReceiveProgressMsg );
             state = aContext->StateFactory().getStateL( EWaitForClient );
             break;
             }

         case EMSRPSendProgressEvent:
             {
             aContext->SendProgressToClientL( aContext->iSendProgressMsg );
             state = aContext->StateFactory().getStateL( EWaitForClient );
             break;
             }

         default: 
             // Any such error usually a client/server protocol voilation or connection/subsession 
             // protocol voilation. A bug to fix!!
             
             MSRPLOG2("TStateActive::EventL :: Err!! Invalid state to have received %d",aEvent);                     
             state =  HandleStateErrorL(aContext); 
             break; 
         }

     return state;     
    } 
 
TStateBase* TStateActive::handleSendDataL(CMSRPServerSubSession *aContext)
    {
    MSRPLOG("TStateActive::handleSendDataL()");     
    
    aContext->ReadSendDataPckgL(); 
    
    CMSRPMessageHandler *msgHandler 
     = CMSRPMessageHandler::NewL(aContext,
         aContext->iSendMSRPdataPckg().iExtMessageBuffer); 
    
    msgHandler->SendMessageL( *aContext->iConnection );   
    aContext->iOutMsgQ.Queue( *msgHandler );
    aContext->CompleteClient( KErrNone );
    
    return aContext->StateFactory().getStateL( EActive );
    }
   
TStateBase* TStateActive::handleResponseSentL( CMSRPServerSubSession* aContext )
    {
    MSRPLOG("-> TStateActive::handleResponseSentL");
    if( aContext->iReceivedResp->IsMessageComplete() )
        {
        if ( !aContext->iReceivedResp->SendReportL(aContext, *aContext->iConnection, EAllOk ) )
            {
            if ( aContext->iReceivedResp->IsTransmissionTerminated() )
                {
                if( aContext->iIncommingMessageListner.Check() )
                    {
                    aContext->iIncommingMessageListner.Complete( KErrCancel );
                    }
                }
            else
                {
                aContext->sendMsgToClientL( aContext->iReceivedResp );
                }
            return aContext->StateFactory().getStateL(EWaitForClient);
            }
        else
            {
            aContext->iReceivedResp = NULL;
            }
        }
    else
        {
        aContext->iReceivedResp->ResponseHandled();   
        aContext->iReceivedResp = NULL;
        }
    MSRPLOG("<- TStateBase::handleRequestsL");
    return aContext->StateFactory().getStateL( EActive );
    }

TStateBase* TStateActive::handleReportSentL( CMSRPServerSubSession* aContext)
    {
    MSRPLOG("-> TStateActive::handleReportSentL");
    aContext->sendReportToClientL( aContext->iReceivedReport );
    aContext->iPendingDataIncCompleteQ.Queue( *aContext->iReceivedReport );
    aContext->iReceivedReport = NULL;
    return aContext->StateFactory().getStateL( EWaitForClient );
    }

TStates TStateError::identity()
    {
    return EError;
    } 

TStateBase* TStateError::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
    {
    // Error State is entered when we receive an Error Response from the opposite
    // end point to which we sent a SEND request. When this state is entered the client
    // is expected to close the session without trying to send further messages.
    
    MSRPLOG2("Entered TStateError Event %d", aEvent);
    TStateBase *state;
    
    switch(aEvent)
        {
        case EMSRPListenMessagesEvent:
            aContext->CompleteClient(KErrCouldNotConnect); 
            state = this;
            break;
            
        case EMSRPListenSendResultEvent:
            aContext->CompleteClient( KErrGeneral ); 
            state = this;
            break;
            
        case EMSRPDataSendMessageEvent:
            aContext->CompleteClient(KErrNone); 
            state = this;
            break;
            
        case EMSRPCancelReceivingEvent:
        case EMSRPCancelSendRespListeningEvent:
            state = handleClientListnerCancelL(aContext, aEvent);
            break;
            
        case EMSRPIncomingMessageReceivedEvent:            
            aContext->iInCommingMsgQ.Queue(*aContext->iReceivedMsg);
            state = this;           
            break;
        
        case EConnectionStateChangedEvent:
            state = handleConnectionStateChangedL(aContext);
            break;
            
        default:
            MSRPLOG2("TStateError::EventL :: Err!! Invalid state to have received %d",aEvent);                     
            state =  HandleStateErrorL(aContext);            
            break;                  
        }    
    return state;   
    }
