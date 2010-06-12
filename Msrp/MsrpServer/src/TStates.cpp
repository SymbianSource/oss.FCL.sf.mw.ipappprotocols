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

TStateBase * TStateFileShare::processIncommingMessageL(CMSRPServerSubSession *aContext, 
                 CMSRPMessageHandler* incommingMsg)
    {
    MSRPLOG("TStateFileShare::processIncommingMessage Enter!");
    TStateBase *state = this;
    CMSRPMessageHandler *incommingMsgHandler;
    if(NULL != incommingMsg)
        {
        aContext->iInCommingMsgQ.Queue(*incommingMsg);
        }
    incommingMsgHandler = aContext->iInCommingMsgQ.DeQueue();         
 
    while (incommingMsgHandler && state == this)
        {
        if(MMSRPIncomingMessage::EMSRPResponse == incommingMsgHandler->MessageType())
            {
            state = handlesResponseL(aContext,incommingMsgHandler);                    
            }
        else
            {
            state = handleRequestsL(aContext,incommingMsgHandler);
            }
        incommingMsgHandler = aContext->iInCommingMsgQ.DeQueue();
        }
      
    MSRPLOG("TStateFileShare::processIncommingMessage Exit!");
    return state;
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
    
    // Shift this to Outgoing Queue.
    aContext->iCurrentMsgHandler = msgHandler;
    return aContext->StateFactory().getStateL(EActiveSend);
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
        TBool error = 0;
        CleanupStack::PushL(incommingMsgHandler);
        TBool sendResult = outgoingMsgHandler->ConsumeResponseL(*incommingMsgHandler);
         
        if( sendResult )
            {
            error = aContext->sendResultToClientL(outgoingMsgHandler);
            }        
         
        if(outgoingMsgHandler->IsMessageComplete())
            {
            aContext->iOutMsgQ.explicitRemove(outgoingMsgHandler);    
            delete outgoingMsgHandler;
            }
                 
        CleanupStack::Pop(incommingMsgHandler); 
        if( error )
            {
            nextState = aContext->StateFactory().getStateL(EError);   
            }
        else
            {
            nextState = aContext->StateFactory().getStateL(EWaitForClient);     
            }            
        }
    
    delete incommingMsgHandler;    
    
    MSRPLOG("TStateBase::handlesResponseL Exit!");
    return nextState;    
    }
 
TStateBase* TStateBase::handleRequestsL(CMSRPServerSubSession *aContext,
                 CMSRPMessageHandler *incommingMsgHandler)
    {
    MMSRPIncomingMessage::TMSRPMessageType msgType = incommingMsgHandler->MessageType();
    
    if(MMSRPIncomingMessage::EMSRPMessage == msgType) // SEND request
        {
        MSRPLOG("SEND request received");
        TBool sendToClient = incommingMsgHandler->SendResponseL(aContext, 
                *aContext->iConnection, 0);        
        if(sendToClient)
            {
            aContext->sendMsgToClientL(incommingMsgHandler);
            }
        aContext->iPendingForDeletionQ.Queue(*incommingMsgHandler);
        return aContext->StateFactory().getStateL(EWaitForClient);
        }
    else if(MMSRPIncomingMessage::EMSRPReport == msgType) // Drop Reports
        {
        MSRPLOG("Reports not supported!!");
        delete incommingMsgHandler;
        return this;
        }
    else // It is an unrecognized request eg. AUTH
        {
        MSRPLOG("Unrecognized request received");
        TBool sendToClient = incommingMsgHandler->SendResponseL(aContext, 
                *aContext->iConnection, CMSRPResponse::EUnknownRequestMethod);        
        aContext->iPendingForDeletionQ.Queue(*incommingMsgHandler);
        return aContext->StateFactory().getStateL(EWaitForClient);
        }
    } 
 
TStateBase* TStateBase::handleClientListnerCancelL(CMSRPServerSubSession * aContext, 
        TMSRPFSMEvent aEvent)
    {
    if(aEvent == EMSRPCancelReceivingEvent)
        {
        // Confirm completion of the Cancel Request.
        aContext->CompleteClient(KErrNone);
        
        // Complete the Listner if that is active.
        if(aContext->iIncommingMessageListner.Check())
            aContext->iIncommingMessageListner.Complete(KErrNone);
        }    
    
    if(aEvent == EMSRPCancelSendRespListeningEvent)
        {
        // Confirm completion of the Cancel Request.
        aContext->CompleteClient(KErrNone);
        
        // Complete the Listner if that is active.
        if(aContext->iResponseListner.Check())
            aContext->iResponseListner.Complete(KErrNone);        
        }
    
    return aContext->StateFactory().getStateL(EWaitForClient);    
    }
  
TStateBase* TStateBase::handleConnectionStateChangedL(CMSRPServerSubSession *aContext)
    {
    TStateBase* state = NULL;    
    TInt iConnectionEvent = aContext->iConnection->getConnectionState();
    
    MSRPLOG2("handleConnectionStateChanged::Connection Event %d \n",iConnectionEvent ); 
    
    switch(iConnectionEvent)
        {
        case -1: // Error Scenario         
            state = handleConnectionErrorsL(aContext);
            break;
        
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
        if( CMSRPServerSubSession::TInCommingMsgQueue == 
                           aContext->getQToProcess() )
            state  =  processIncommingMessageL( aContext );       
        else
            state  =  processPendingMessageQL( aContext );
        }   

    return state;
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
    aContext->iLocalPathMSRPDataPckg().iSessionID = *(aContext->iLocalSessionID);
    
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
    TStateBase *state;

    switch(aEvent)
        {
        case EConnectionStateChangedEvent:
        state = handleConnectionStateChangedL(aContext);
        break;
                 
        default:                      
        MSRPLOG2("TStateConnecting::EventL :: Err!! Invalid state to have received %d",
                              aEvent);                     
        state = HandleStateErrorL(aContext);            
        break;              
        }

    return state;    
    }

TStateBase* TStateWaitForClient::fileSendCompleteL(CMSRPServerSubSession *aContext)
    {
    CMSRPMessageHandler *outgoingMessageHandler = aContext->iOutMsgQ.getHead();
    
    if( outgoingMessageHandler  && outgoingMessageHandler->IsMessageComplete() )
        {
            aContext->iOutMsgQ.explicitRemove(outgoingMessageHandler);    
             delete outgoingMessageHandler;
             aContext->iSendCompleteNotify = TRUE;
         }
    return aContext->StateFactory().getStateL( EWaitForClient );   
    }   

TStateBase * TStateWaitForClient::handleResponseSentL( CMSRPServerSubSession *aContext)
    {
    CMSRPMessageHandler *oriMessageHandler = aContext->iReceiveFileMsgHdler;
    oriMessageHandler->UpdateResponseStateL(aContext->iReceivedResp);
    if(oriMessageHandler->FileTransferComplete() )
        {
        aContext->iReceiveCompleteNotify = TRUE;              
        }
    
    return aContext->StateFactory().getStateL(EWaitForClient);    
    }

TStates TStateWaitForClient::identity()
    {
    return EWaitForClient;
    }
 
TStateBase* TStateBase::handleSendFileL(CMSRPServerSubSession *aContext)
    {    
    MSRPLOG("TStateBase::handleSendFileL() enter");
    aContext->ReadSendDataPckgL(); 
    CMSRPMessageHandler *aMessageHandler = CMSRPMessageHandler::NewL(aContext,
            aContext->iSendMSRPdataPckg().iExtMessageBuffer); 
    
    aMessageHandler->SendFileL(*aContext->iConnection);                                        
    
    aContext->iOutMsgQ.Queue(*aMessageHandler);    
    aContext->CompleteClient( KErrNone );
    
    aContext->iFileShare = TRUE;
    MSRPLOG("TStateBase::handleSendFileL() exit"); 
    if(!aContext->listnerSetupComplete())
        {
        return aContext->StateFactory().getStateL(EWaitForClient);
        }
    
    return aContext->StateFactory().getStateL(EFileShare);
    }
 
TStateBase* TStateBase::handleReceiveFileL(CMSRPServerSubSession *aContext)
     {     
     MSRPLOG("TStateBase::handleReceiveFileL() enter");
    
     aContext->ReadSendDataPckgL(); 
     
     aContext->iReceiveFileMsgHdler = CMSRPMessageHandler::NewL(aContext,
                             aContext->iSendMSRPdataPckg().iExtMessageBuffer); 
      
     
     aContext->iReceiveFileMsgHdler->ReceiveFileL();
     aContext->CompleteClient( KErrNone );
     aContext->iFileShare = TRUE;
     MSRPLOG("TStateBase::handleReceiveFileL() exit");
     if(!aContext->listnerSetupComplete())
         {
         return aContext->StateFactory().getStateL(EWaitForClient);
         }
     
     return aContext->StateFactory().getStateL(EFileShare);     
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
              // Data Send Complete received in the TWaitForClient state is 
              // not handeled. At Data Send Complete messae with Failure Report 
              // header set to "no" need to inform the client about completion.
              // Currently Queuing of completion events is not supported.
              if(aContext->iFileShare)
                  {
                  state = fileSendCompleteL(aContext);
                  break;
                  }
              MSRPLOG2("TStateWaitForClient::Not supported, Please check %d",aEvent);                     
              break;
              
          case EMSRPResponseSendCompleteEvent:
              if(aContext->iFileShare)
                  {
                  state = handleResponseSentL(aContext);  
                  break;
                  }
              break;
  
          case EMSRPDataSendMessageEvent:
              aContext->QueueClientSendRequestsL();                         
              break;
                              
          case EMSRPCancelReceivingEvent:          
          case EMSRPCancelSendRespListeningEvent:
               handleClientListnerCancelL(aContext, aEvent);
               break;

          case EConnectionStateChangedEvent:
              state = handleConnectionStateChangedL(aContext);
              break;
              
          case EMSRPSendProgressEvent:
          case EMSRPReceiveProgressEvent:
              //ignore event if no listener
              MSRPLOG("TStateWaitForClient::EventL Ignoring Progress Event")
              state = aContext->StateFactory().getStateL(EWaitForClient);
              break;

          case EMSRPSendFileEvent:
               state =  handleSendFileL(aContext);
               break;
               
          case EMSRPReceiveFileEvent :
               state =  handleReceiveFileL(aContext);
               break;    
                  
          default:          
              // Any such error usually a client/server protocol voilation or connection/subsession 
              // protocol voilation. A bug to fix!!
              
              MSRPLOG2("TStateWaitForClient::EventL :: Err!! Invalid state to have received %d",aEvent);                     
              state = HandleStateErrorL(aContext);
              break;              
          };

      if(NULL == state)
        {
          // State not set.
          if(!aContext->listnerSetupComplete())
              {
              state = this;
              }
          else if (aContext->iFileShare)
              {
                  state = aContext->StateFactory().getStateL(EFileShare);
                  
                  if(aContext->iReceiveCompleteNotify)
                      {
                      aContext->NotifyFileReceiveResultToClientL(NULL);
                      state = aContext->StateFactory().getStateL(EWaitForClient);
                      }
                  else if (aContext->iSendCompleteNotify)
                      {
                      aContext->NotifyFileSendResultToClientL(NULL);
                      state = aContext->StateFactory().getStateL(EWaitForClient);
                      }
                  else if(!aContext->QueuesEmpty())
                      state = state->handleQueuesL(aContext);                   
              }
          else
            state = handleQueuesL(aContext);                  
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
 
         case EMSRPSendFileEvent:
              state =  handleSendFileL(aContext);
              break;
              
         case EMSRPReceiveFileEvent :
              state =  handleReceiveFileL(aContext);
              break;
             
         case EConnectionStateChangedEvent:
              state = handleConnectionStateChangedL(aContext);
              break;
  
         case EMSRPCancelReceivingEvent:          
         case EMSRPCancelSendRespListeningEvent:
              state =  handleClientListnerCancelL(aContext, aEvent);
              break;             
 
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
    aContext->iCurrentMsgHandler = msgHandler;
    
    aContext->CompleteClient( KErrNone );
    
    return aContext->StateFactory().getStateL(EActiveSend);
    }
   
TStateBase* TStateBase::handleInCommingMessagesL(CMSRPServerSubSession *aContext)
     {
     CMSRPMessageHandler* incommingMsg = aContext->iReceivedMsg;
     aContext->iReceivedMsg = NULL;
     
     return processIncommingMessageL(aContext, incommingMsg);             
     }
 
TStates TStateFileShare::identity()
     {
     return EFileShare;
     } 
 
TStateBase* TStateFileShare::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
     {
     TStateBase *state = NULL;
     MSRPLOG2("Entered TStateFileShare Event %d",aEvent);
     switch(aEvent)
         {
         case EMSRPDataSendCompleteEvent: // maps to file send complete
              state = fileSendCompleteL(aContext);
              break;
             
         case EMSRPIncomingMessageReceivedEvent:  // incoming responses to file chunks
             state =  handleInCommingMessagesL(aContext);
              break;
              
         case EMSRPResponseSendCompleteEvent:
             state = handleResponseSentL(aContext);  
              break;
         
         case EMSRPSendProgressEvent:
             state = handleSendProgressL(aContext);
             break;
             
         case EMSRPReceiveProgressEvent:
             state = handleReceiveProgressL(aContext);             
             break;
                  
         case EConnectionStateChangedEvent :
             state = handleConnectionStateChangedL(aContext); 
             break;

              
         case EMSRPCancelReceivingEvent:
         case EMSRPCancelSendRespListeningEvent:
              state =  handleClientListnerCancelL(aContext, aEvent);
              break;  
         default:
               MSRPLOG2("TStateFileShare::EventL :: Err!! Invalid state to have received %d",aEvent);                     
              // state =  HandleStateErrorL(aContext);      //handle error state       
               break;
         
         }
         return state;
     }
 
TStateBase * TStateFileShare::handleSendProgressL( CMSRPServerSubSession *aContext)
     {
     CMSRPMessageHandler *outgoingMessageHandler = aContext->iOutMsgQ.getHead();
     aContext->SendProgressToClientL(outgoingMessageHandler);
     return aContext->StateFactory().getStateL( EWaitForClient ); 
     }
 
TStateBase * TStateFileShare::handleReceiveProgressL( CMSRPServerSubSession *aContext)
     {
     CMSRPMessageHandler *oriMessageHandler = aContext->iReceiveFileMsgHdler;     
     aContext->ReceiveProgressToClientL(oriMessageHandler);
     return aContext->StateFactory().getStateL( EWaitForClient );
     }
 
TStateBase * TStateFileShare::handleResponseSentL( CMSRPServerSubSession *aContext)
     {
     CMSRPMessageHandler *oriMessageHandler = aContext->iReceiveFileMsgHdler;
     oriMessageHandler->UpdateResponseStateL(aContext->iReceivedResp);
     

      if(oriMessageHandler->FileTransferComplete() )
         {
         //Notify client
         aContext->NotifyFileReceiveResultToClientL(oriMessageHandler);
         delete oriMessageHandler;
         aContext->iReceiveFileMsgHdler = NULL;
         return aContext->StateFactory().getStateL(EWaitForClient);
         }
     
     if(!aContext->listnerSetupComplete())
         {
         return aContext->StateFactory().getStateL(EWaitForClient);
         }
     
     return aContext->StateFactory().getStateL(EFileShare);
     
     }
  
TStateBase * TStateFileShare::handlesResponseL(CMSRPServerSubSession *aContext,
                  CMSRPMessageHandler *incommingMsgHandler)
      {
      TStateBase *nextState; 
      MSRPLOG("TStateFileShare::handlesFileResponseL Entered!");
      
      // Search the outgoing Queue to find the owner of  this response.
      CMSRPMessageHandler *outgoingMsgHandler = 
                         aContext->iOutMsgQ.getMatch(incommingMsgHandler);

      nextState = this;
      if(NULL != outgoingMsgHandler)
         {
         CleanupStack::PushL(incommingMsgHandler);
         outgoingMsgHandler->ConsumeFileResponseL(*incommingMsgHandler);
        
         if(outgoingMsgHandler->FileTransferComplete())    
            {
             //notify client
            aContext->NotifyFileSendResultToClientL(outgoingMsgHandler);
            aContext->iOutMsgQ.explicitRemove(outgoingMsgHandler);    
            delete outgoingMsgHandler;        
            nextState = aContext->StateFactory().getStateL(EWaitForClient);
            }    
        
         CleanupStack::Pop(incommingMsgHandler);
         }
      delete incommingMsgHandler;
      MSRPLOG("TStateFileShare::handlesResponseL Exit!");
      if(!aContext->listnerSetupComplete())
          {
          return aContext->StateFactory().getStateL(EWaitForClient);
          }
      return nextState; 
      }
  
TStateBase * TStateFileShare::handleRequestsL(CMSRPServerSubSession *aContext,
                 CMSRPMessageHandler *incommingMsgHandler)
     {     
     if(MMSRPIncomingMessage::EMSRPMessage == incommingMsgHandler->MessageType())
         {
         CMSRPMessageHandler *oriMessageHandler = aContext->iReceiveFileMsgHdler;
         
         if(oriMessageHandler->IsInFile())
             {   
             oriMessageHandler->WritetoFileL(incommingMsgHandler);
             incommingMsgHandler->SendResponseL(aContext, *aContext->iConnection,CMSRPResponse::EAllOk);
             if(!incommingMsgHandler->IfResponseReqL())
                 {
                 delete incommingMsgHandler;
                 if(oriMessageHandler->FileTransferComplete())
                     {
                     aContext->NotifyFileReceiveResultToClientL(oriMessageHandler);
                     delete oriMessageHandler;
         	         aContext->iReceiveFileMsgHdler = NULL;
                     return aContext->StateFactory().getStateL(EWaitForClient);   
                     }
                 }
           
             else
                 {
                 aContext->iPendingForDeletionQ.Queue(*incommingMsgHandler);
                 }              
              }
          }
     else
         {
         MSRPLOG("Reports not supported.!!");
         delete incommingMsgHandler;         
         }
     if(!aContext->listnerSetupComplete())
         {
         return aContext->StateFactory().getStateL(EWaitForClient);
         }
     return this; 
     }
 
TStateBase* TStateFileShare::fileSendCompleteL(CMSRPServerSubSession *aContext)
     {
     CMSRPMessageHandler *outgoingMessageHandler = aContext->iOutMsgQ.getHead();
     
     if( outgoingMessageHandler  && outgoingMessageHandler->IsMessageComplete() )
         {
             //notify client
              aContext->NotifyFileSendResultToClientL( outgoingMessageHandler );
              aContext->iOutMsgQ.explicitRemove(outgoingMessageHandler);    
              delete outgoingMessageHandler;
              return aContext->StateFactory().getStateL( EWaitForClient ); 
          }
         // response needed keep it on the outmsg queue itself
     return aContext->StateFactory().getStateL( EFileShare );   
     }   
 
TStates TStateActiveSend::identity()
    {
    return EActiveSend;
    }
 
TStateBase* TStateActiveSend::EventL(TMSRPFSMEvent aEvent, CMSRPServerSubSession *aContext)
    {
     // ActiveSend state. The subsession is busy sending earlier data. Any further requests to 
     // send data from the client will be queued. Any Message received will be queued till 
     // EMSRPDataSendCompleteEvent is received. 
     // After this Message Received Queue will be processed to check for any 
     // incoming messages/responses.
     // After that Message send queue will be processed to see if there any pending messages to 
     // trasmitt.
     // If both the queues are empty move back to Active State.

     TStateBase *state;
     MSRPLOG2("Entered TStateActiveSend Event %d",aEvent); 
 
     switch(aEvent)
         {
         case EMSRPDataSendMessageEvent:
             aContext->QueueClientSendRequestsL();
             state = this;          
             break;
     
         case EMSRPDataSendCompleteEvent:
             state = MessageSendCompleteL(aContext);
             break;
 
         case EMSRPIncomingMessageReceivedEvent:
             // Queue any thing that comes.
             aContext->iInCommingMsgQ.Queue(*aContext->iReceivedMsg);
             state = this;           
             break;

         case EConnectionStateChangedEvent:
              state = handleConnectionStateChangedL(aContext);
              break;
              
         case EMSRPCancelReceivingEvent:          
         case EMSRPCancelSendRespListeningEvent:
             state =  handleClientListnerCancelL(aContext, aEvent);
             break;              
 
         default: 
             // Any such error usually a client/server protocol voilation or connection/subsession 
             // protocol voilation. A bug to fix!!
             
             MSRPLOG2("TStateActiveSend::EventL :: Err!! Invalid state to have received %d",aEvent);                     
             state =  HandleStateErrorL(aContext);            
             break; 
         } 
     return state;
    }
 
TStateBase * TStateActiveSend::MessageSendCompleteL(CMSRPServerSubSession *aContext)
    {
    // Handle send message complete event.
    if( NULL == aContext->iCurrentMsgHandler)
        {
        MSRPLOG( "TStateActiveSend::MessageSendCompleteL :: iCurrentMsgHandler is NULL \n" );                                     
        return this;
        }
        
    if( aContext->iCurrentMsgHandler->IsMessageComplete() )
        {
        // Messages with Failure Report set to "No" will never get a response.        
        TBool error = aContext->sendResultToClientL( aContext->iCurrentMsgHandler );        
        delete aContext->iCurrentMsgHandler;
        aContext->iCurrentMsgHandler = NULL;
        
        return aContext->StateFactory().getStateL( EWaitForClient );
        }
    else
        {
        // Message expects a response to come. Put this out the OutGoing Queue.
        aContext->iOutMsgQ.Queue( *(aContext->iCurrentMsgHandler) );
        aContext->iCurrentMsgHandler = NULL;
        
        // Handle any pending events in Queue.
        return handleQueuesL(aContext);     
        }
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
            aContext->CompleteClient(EInvalidAction); 
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
