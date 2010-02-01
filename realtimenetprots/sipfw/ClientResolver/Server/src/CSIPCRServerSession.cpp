// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSIPCRServerSession.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0 
//



// INCLUDES
#include <e32math.h>
#include "CSipCRServerSession.h"
#include "CSIPCRRoutingTable.h"
#include "CSIPClientResolver.h"
#include "CSIPCRITCUtility.h"
#include "CSIPCRSessionReceiver.h"
#include "CSipCRServer.h"
#include "sipCRclientserver.h"
#include "CSIPCRRequestItem.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "TSipClient.h"
#include "sipresolvedclient.h"
#include "sipcontenttypeheader.h"
#include "sipstrconsts.h"
#include "sipstrings.h"
#include "uricontainer.h"
#include "SIPCRSerializer.h"
#include "sipresolvedclient2.h"
#include "sipcrworkerao.h"
#include "sipcrrequestqueuecleanupdata.h"

const TUint K480ResponseCode = 480;
const TUint K503ResponseCode = 503;

_LIT(KWorkerThreadName, "SIPClientResolverSessionWorker");

// ----------------------------------------------------------------------------
// CSIPCRServerSession::NewL
// ----------------------------------------------------------------------------
//
CSIPCRServerSession* CSIPCRServerSession::NewL(CSIPCRServer& aCRServer)
    {
    CSIPCRServerSession* self = CSIPCRServerSession::NewLC(aCRServer);
    CleanupStack::Pop (self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::NewLC
// ----------------------------------------------------------------------------
//
CSIPCRServerSession* CSIPCRServerSession::NewLC(CSIPCRServer& aCRServer)
    {
    CSIPCRServerSession* self = new(ELeave)CSIPCRServerSession(aCRServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::CSIPCRServerSession
// ----------------------------------------------------------------------------
//
CSIPCRServerSession::CSIPCRServerSession(CSIPCRServer& aCRServer)
 : iClientUid(TUid::Null()),
   iClientUidSet(EFalse),
   iCRServer(aCRServer)
    {
    iCRServer.IncrementSessions();
    }    
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ConstructL()
    {
    User::LeaveIfError(iRequestQueueMutex.CreateLocal());
    iReceiver = CSIPCRSessionReceiver::NewL(iCRServer.ITCUtility());
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::~CSIPCRServerSession
// ----------------------------------------------------------------------------
//
CSIPCRServerSession::~CSIPCRServerSession ()
    {
    iWorkerAos.ResetAndDestroy();    
    delete iReceiver;
    iRequestQueue.ResetAndDestroy();
    iCRServer.DecrementSessions();
    if (iClientUidSet)
        {
        iCRServer.RoutingTable().Remove(iClientUid);   
        }
    iRequestQueueMutex.Close();
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::ServiceL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ServiceL(const RMessage2& aMessage)
    {
    DoServiceL(aMessage.Function(),aMessage);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::WorkerAoRequestCompleted
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::WorkerAoRequestCompleted(CSipCrWorkerAo* aWorkerAo)
    {
    TInt err = aWorkerAo->CompletionCode();
    TUint32 requestId = aWorkerAo->RequestId();
    if (err != KErrNone && requestId > 0)
        {
        TRAP_IGNORE(iReceiver->ErrorReceivedL(requestId,err))
        }
    TInt index = iWorkerAos.Find(aWorkerAo);
    if (index >= 0)
        {
        iWorkerAos.Remove(index);
        }
    delete aWorkerAo;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::RoutingEntryAddedL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::RoutingEntryAddedL(const TUid& aUid)
    {
    CSIPCRITCUtility::WaitForMutexLC(iRequestQueueMutex);    
    
    // Complete pending requests for aUid
    CSIPCRRequestItem* requestItem = NULL;
    for (TInt i=iRequestQueue.Count()-1; i >= 0; i--)
        {
        requestItem = iRequestQueue[i];
        if (requestItem->ChannelUid() == aUid)
            {
            TUint32 requestId = requestItem->RequestId();
            TUid nextHopUid(TUid::Null());
            if (iCRServer.RoutingTable().FindMatch(aUid,iClientUid,nextHopUid))
                {
                iReceiver->RequestCompletedL(requestId,nextHopUid);
                }
            else
                {
                iReceiver->ErrorReceivedL(requestId,KErrNotFound);
                }
            ProtectedRemoveFromRequestQueue( requestItem );
            delete requestItem;            
            }
        }
    
    CleanupStack::PopAndDestroy(); // signals iRequestQueueMutex
        
    CSipCrWorkerAo* workerAo = CSipCrWorkerAo::NewLC(*this);
    CreateWorkerThreadL(workerAo,ConnectWorkerThreadFunction);
    CleanupStack::Pop(workerAo);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::DoServiceL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::DoServiceL (TInt aFunction,
                                      const RMessage2& aMessage)
    {
    switch (aFunction)
        {
#ifndef	PLAT_SEC_TEST
        case ESIPCRIpcSetClientUid:
            SetUIDL(aMessage); break;
            
	    case ESIPCRIpcRegister:
	        RegisterL(iCRServer.ITCUtility().ReadUIDL(aMessage)); break;

	    case ESIPCRIpcDeregister:
	        DeregisterL(iCRServer.ITCUtility().ReadUIDL(aMessage)); break;

        case ESIPCRIpcClientReadyToReceive: // Async. Don't complete yet.
            ClientReadyToReceiveL(aMessage); return;
            
        case ESIPCRIpcClientCancelReceive:
            CancelClientReceiveL(); break;
            
        case ESIPCRIpcCancelRequest:
            CancelRequestL(aMessage); break;            
        
        case ESIPCRIpcCancelAllRequests:
            CancelAllRequests(); break;
            
        case ESIPCRIpcChannel:
            ChannelL(aMessage); break;
            
        case ESIPCRIpcChannelWithResolver:
            ChannelWithResolverL(aMessage); break;
            
        case ESIPCRIpcClientReceiveSipResponse:
            ClientReceiveSipResponseL(aMessage); break;
        
        default:
            break;
#endif
	    }
    iCRServer.ITCUtility().Complete(aMessage,KErrNone);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ChannelWithResolverL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ChannelWithResolverL(const RMessage2& aMessage)
    {
    CSIPRequest* request = CSIPRequest::NewLC();
    iCRServer.ITCUtility().ReadSIPRequestL(aMessage, *request);
    TUid resolverUid = iCRServer.ITCUtility().ReadResolverUidL(aMessage);
    TUint32 requestId = CreateRequestId();
    TSipClient clientUid(TUid::Null(),EFalse,EFalse);
    ChannelL(requestId,*request,clientUid,resolverUid);
    CleanupStack::PopAndDestroy(request);
    iCRServer.ITCUtility().WriteRequestIdL(aMessage,requestId);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::ChannelL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ChannelL(const RMessage2& aMessage)
    {
    CSIPRequest* request = CSIPRequest::NewLC();
    iCRServer.ITCUtility().ReadSIPRequestL(aMessage, *request);
    
    TUint32 requestId = CreateRequestId();
    iCRServer.ITCUtility().WriteRequestIdL(aMessage,requestId);
    
    TSipClient clientUid(TUid::Null(),EFalse,EFalse);
    
    CSIPResponse* response = ResolveClientL(*request,clientUid);
    if (response)
        {
        CleanupStack::PushL(response);
        iReceiver->AddErrorResponseL(requestId,response);
        CleanupStack::PopAndDestroy(response);
        }
    else
        {
        ChannelL(requestId,*request,clientUid,TUid::Null());
        }
    CleanupStack::PopAndDestroy(request);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::RegisterL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::RegisterL(const TUid& aChannelUid)
    {
    iCRServer.RoutingTable().AssociateL(iClientUid, aChannelUid);
    iCRServer.RoutingEntryAddedL(aChannelUid);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::DeregisterL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::DeregisterL(const TUid& aChannelUid)
    {
    iCRServer.RoutingTable().Remove(aChannelUid);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::SetUIDL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::SetUIDL(const RMessage2& aMessage)
    {
    iClientUid = iCRServer.ITCUtility().ReadUIDL (aMessage);
    iCRServer.RoutingTable().AddL(iClientUid);
    iClientUidSet = ETrue;
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ClientReadyToReceiveL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ClientReadyToReceiveL(const RMessage2& aMessage)
    {
    iReceiver->ClientReadyToReceiveL(aMessage);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ClientReceiveSipResponseL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ClientReceiveSipResponseL(const RMessage2& aMessage)
    {
    iReceiver->WriteSipResponseToClientL(aMessage);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::CancelRequestL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::CancelRequestL(const RMessage2& aMessage)
    {
    TUint32 requestId = iCRServer.ITCUtility().ReadRequestIdL(aMessage);
    TBool found = EFalse;
    
    CSIPCRITCUtility::WaitForMutexLC(iRequestQueueMutex);    
    
    for (TInt i=iRequestQueue.Count()-1; i >=0 && !found; i--)
        {
        CSIPCRRequestItem* requestItem = iRequestQueue[i];
        if (requestItem->RequestId() == requestId)
            {
            CancelConnectL( requestItem );
            ProtectedRemoveFromRequestQueue( requestItem );
            delete requestItem;             
            found = ETrue;
            }
        }
        
    CleanupStack::PopAndDestroy(); // signals iRequestQueueMutex   
          
    iReceiver->RemoveByRequestId(requestId);
    CSipCrWorkerAo* workerAo = CSipCrWorkerAo::NewLC(*this);
    CreateWorkerThreadL(workerAo,ConnectWorkerThreadFunction);
    CleanupStack::Pop(workerAo);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::CancelConnectL
// ----------------------------------------------------------------------------
//    
void CSIPCRServerSession::CancelConnectL( CSIPCRRequestItem* aRequestItem )
    {
    if ( aRequestItem->Client().PluginType() == TSipClient::ESecondGeneration )
    	{
    	if ( aRequestItem->ConnectCalled() )
    		{
    		CSIPResolvedClient2* resolvedClient2 =
        		iCRServer.LoadPlugin2LC( 
        		    aRequestItem->Client().ImplementationUid() );
            resolvedClient2->CancelConnect( aRequestItem->Client().Uid() );
        	CleanupStack::PopAndDestroy( resolvedClient2 );
    		}
    	}
    }
    
// ---------------------------------------------------------------------------
// CSIPCRServerSession::AddToRequestQueueL
// ---------------------------------------------------------------------------
//
template<class T> void CSIPCRServerSession::AddToRequestQueueL( 
    CSIPCRRequestItem* aRequestItem,
    T& aResolvedClient )
	{
	__ASSERT_ALWAYS ( aRequestItem, User::Leave( KErrArgument ) );

    // The request item needs to be in request queue 
    // in case the client connects immediately after calling ConnectL 
    // and the main thread examines the request queue 
    ProtectedAddToRequestQueueL( aRequestItem );
    TSipCrRequestQueueCleanupData cleanupData;
    cleanupData.iSession = this;
    cleanupData.iRequestItem = aRequestItem;
    TCleanupItem cleanupItem( RemoveFromRequestQueue, &cleanupData );
    CleanupStack::PushL(cleanupItem);
    TBool connectSucceeded = ConnectL( *aRequestItem, aResolvedClient );
    CleanupStack::Pop(); // cleanupItem, leave the item to queue
    if ( !connectSucceeded )
        {
        ProtectedRemoveFromRequestQueue( aRequestItem );
        delete aRequestItem;
        }  
	}

// ---------------------------------------------------------------------------
// CSIPCRServerSession::ConnectL
// ---------------------------------------------------------------------------
//
template<class T> TBool CSIPCRServerSession::ConnectL( 
    CSIPCRRequestItem& aRequestItem,
    T& aResolvedClient )
	{
	TBool success = ETrue;
    TRAPD (err, aResolvedClient.ConnectL(aRequestItem.ChannelUid()));
    if (err == KErrNoMemory)
        {
        User::Leave(err);
        }
    else if (err != KErrNone)
        {
        CSIPResponse* response = CSIPResponse::NewLC(K480ResponseCode,
            SIPStrings::StringF(SipStrConsts::EPhraseTemporarilyNotAvailable));
        iReceiver->AddErrorResponseL(aRequestItem.RequestId(),response);
        CleanupStack::Pop(response);
        success = EFalse;
        }
    else
        {
        aRequestItem.SetConnectCalled();    
        }        
    return success;
	}

// ----------------------------------------------------------------------------
// CSIPCRServerSession::CancelAllRequests
// ----------------------------------------------------------------------------
//    
void CSIPCRServerSession::CancelAllRequests()
    {
    iRequestQueueMutex.Wait(); // Protect iRequestQueue with a mutex
    iRequestQueue.ResetAndDestroy();
    iRequestQueueMutex.Signal();
    iReceiver->RemoveAll();
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::CancelClientReceiveL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::CancelClientReceiveL ()
    {
    iReceiver->CancelClientReceiveL();
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ResolveClientL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSIPCRServerSession::ResolveClientL(
    CSIPRequest& aRequest,
    TSipClient& aUid)
    {
    RArray<TSipClient> uids;
    CleanupClosePushL(uids);
    // Always execute the client resolving code in the main thread
    // Multithreading the resolving code would be error prone and
    // there is no performance reason to create a new thread for resolving.
    CSIPResponse* response = 
        iCRServer.ClientResolver().FindUidL(aRequest,uids);
    if (!response)
        {
        __ASSERT_ALWAYS(uids.Count() > 0, User::Leave(KErrNotFound));
        if (!FindUid(uids,aUid,ETrue,ETrue) &&
            !FindUid(uids,aUid,ETrue,EFalse) &&
            !FindUid(uids,aUid,EFalse,ETrue) &&
            !FindUid(uids,aUid,EFalse,EFalse))
            {
            response = CSIPResponse::NewL(K503ResponseCode,
                SIPStrings::StringF(SipStrConsts::EPhraseServiceUnavailable));
            }
        }
    CleanupStack::PopAndDestroy(1); // uids
    return response;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::ChannelL
// ----------------------------------------------------------------------------
//    
void CSIPCRServerSession::ChannelL(
    TUint32 aRequestId,
    CSIPRequest& aRequest,
    const TSipClient& aClient,
    const TUid& aResolverUid)
    {
    // SIP request cannot be passed as such to the worker thread because 
    // the main and worker threads are using a different string pool.
    // Instead SIP request is externalized in the main thread and 
    // internalized in the worker thread.
    CBufBase* serializedRequest = SIPCRSerializer::ExternalizeL(aRequest);
    CleanupStack::PushL(serializedRequest);
    HBufC8* requestBody = aRequest.TakeContentOwnershipL();
    CleanupStack::PushL(requestBody);
    
    CSipCrWorkerAo* workerAo = 
        CSipCrWorkerAo::NewL(
            aRequestId,serializedRequest,requestBody,
            aClient,aResolverUid,*this);
            
    CleanupStack::Pop(requestBody);
    CleanupStack::Pop(serializedRequest);    
    
    CleanupStack::PushL(workerAo);
    CreateWorkerThreadL(workerAo,ChannelWorkerThreadFunction);
    CleanupStack::Pop(workerAo);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ChannelImplementationL
// ----------------------------------------------------------------------------
//   
void CSIPCRServerSession::ChannelImplementationL( CSipCrWorkerAo& aWorkerAo )
    {    
    CSIPRequest* request = CSIPRequest::NewLC(); 
    SIPCRSerializer::InternalizeL( 
        aWorkerAo.SerializedSipRequest().Ptr( 0 ), *request );
    request->SetContent( aWorkerAo.GetSipRequestBody() );
    
    CSIPResolvedClient2* resolvedClient2 = NULL;
    CSIPResolvedClient* resolvedClient = NULL;
    
    if ( aWorkerAo.SipClient().PluginType() == TSipClient::EFirstGeneration )
    	{
    	resolvedClient =
        	iCRServer.LoadPlugin1LC( aWorkerAo.SipClient().Uid(),
        							 aWorkerAo.ResolverUid() );
	__ASSERT_ALWAYS ( resolvedClient , User::Leave( KErrNoMemory ) );
    	}
    else
    	{
    	resolvedClient2 = 
    	    iCRServer.LoadPlugin2LC( aWorkerAo.SipClient().ImplementationUid() );
	__ASSERT_ALWAYS ( resolvedClient2 , User::Leave( KErrNoMemory ) );
    	}
    
    TUid channelUid;   
    ChannelUidL( aWorkerAo.SipClient(), channelUid, request, resolvedClient );
    
    TUid nextHopUid( TUid::Null() );
    
    if ( iCRServer.RoutingTable().FindMatch( channelUid,
                                             iClientUid,
                                             nextHopUid ) )
        {
        iReceiver->RequestCompletedL( aWorkerAo.RequestId(),nextHopUid );
        }
    else // Client not running
        {
        CSIPCRRequestItem* requestItem = 
            CSIPCRRequestItem::NewLC( aWorkerAo.RequestId(), 
            						  aWorkerAo.SipClient(),
                                      channelUid,
                                      aWorkerAo.ResolverUid() ); 
        if ( resolvedClient ) 
         	{
         	AddToRequestQueueL( requestItem, *resolvedClient );
         	}
        else
        	{
        	AddToRequestQueueL( requestItem, *resolvedClient2 );
        	}
        CleanupStack::Pop( requestItem );
        }
    if ( resolvedClient )
    	{
    	CleanupStack::PopAndDestroy( resolvedClient );
    	}
    if ( resolvedClient2 )
    	{
    	CleanupStack::PopAndDestroy( resolvedClient2 );
    	}
    
    CleanupStack::PopAndDestroy( request );
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::ChannelUidL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ChannelUidL(
    const TSipClient& aClient,
    TUid& aChannelUid, 
    CSIPRequest* aRequest,
    CSIPResolvedClient* aResolvedClient )
    {
    if ( aClient.PluginType() == TSipClient::EFirstGeneration )
    	{
    	HBufC8* uri = aRequest->RequestURI()->ToTextL();
    	CleanupStack::PushL(uri);
    
    	CSIPContentTypeHeader* contenTypeHeader = 
        	static_cast<CSIPContentTypeHeader*>(
            	aRequest->Header(
                	SIPStrings::StringF(SipStrConsts::EContentTypeHeader),0));
    	                
    	if(aResolvedClient)
	{
		aChannelUid = aResolvedClient->ChannelL(aRequest->Method(), 
                                               *uri, 
                                               aRequest->AllHeadersL(),
                                               aRequest->Content(),
                                               contenTypeHeader);
	}
    	CleanupStack::PopAndDestroy(uri);
    	}
  	else
  		{
  		aChannelUid = aClient.Uid();
  		}
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::HandleNextPendingRequestL
// ----------------------------------------------------------------------------
//  
void CSIPCRServerSession::HandleNextPendingRequestL(
    CSipCrWorkerAo& /*aWorkerAo*/)
    {
    CSIPCRITCUtility::WaitForMutexLC(iRequestQueueMutex);

    if ( iRequestQueue.Count() > 0 && !( iRequestQueue[0]->ConnectCalled() ) )
        {
        CSIPCRRequestItem* requestItem = iRequestQueue[ 0 ];
    	if ( requestItem->Client().PluginType() == 
    	     TSipClient::EFirstGeneration )
    		{
    		CSIPResolvedClient* resolvedClient =
        	    iCRServer.LoadPlugin1LC( requestItem->Client().Uid(),
                                         requestItem->ResolverUid() );
            ConnectL( *requestItem, *resolvedClient );
        	CleanupStack::PopAndDestroy( resolvedClient );                    
    		}
        else
        	{
        	CSIPResolvedClient2* resolvedClient2 =
        	    iCRServer.LoadPlugin2LC( requestItem->Client().ImplementationUid() );
            ConnectL( *requestItem, *resolvedClient2 );
        	CleanupStack::PopAndDestroy( resolvedClient2 ); 
        	}
        }
    
    CleanupStack::PopAndDestroy(); // signals iRequestQueueMutex  
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::CreateRequestId
// ----------------------------------------------------------------------------
//
TUint32 CSIPCRServerSession::CreateRequestId()
    {
   	if (iRequestIdCounter < KMaxTUint32)
		{
		iRequestIdCounter++;
		}
	else
		{
		iRequestIdCounter = 1;
		}
	return iRequestIdCounter;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerSession::FindUid
// ----------------------------------------------------------------------------
//    
TBool CSIPCRServerSession::FindUid(
    const RArray<TSipClient>& aUids,
    TSipClient& aUid,
    TBool aRomBased,
    TBool aConnected)
    {
    TInt found = EFalse;
    for (TInt i=0; i < aUids.Count() && !found; i++)
        {
        const TSipClient& client = aUids[i];
        TBool connected = iCRServer.RoutingTable().HasUid(client.Uid());
        if ( client.RomBased() == aRomBased && connected == aConnected )
            {
            if ( aConnected || client.AllowStarting() )
                {
                aUid.SetUid( client.Uid() );
                aUid.SetAllowStarting( client.AllowStarting() );
                aUid.SetPluginType( client.PluginType() );
                aUid.SetRomBased( client.RomBased() );
                aUid.SetImplementationUid( client.ImplementationUid() );
                found = ETrue;
                }
            }
        }
    return found;    
    }
   
// ----------------------------------------------------------------------------
// CSIPCRServerSession::CreateWorkerThreadL
// ----------------------------------------------------------------------------
//   
void CSIPCRServerSession::CreateWorkerThreadL(
    CSipCrWorkerAo* aWorkerAo,
    ThreadFunctionPtr aThreadFunctionPtr)
    {
    TName threadName(KWorkerThreadName);
    threadName.AppendNum(static_cast<TUint64>(Math::Random()),EHex);
	RThread thread;
    TInt err = thread.Create(threadName,
                             aThreadFunctionPtr,
                             KDefaultStackSize,
                             NULL, // Use the same heap as the main thread
                             aWorkerAo);
    User::LeaveIfError(err);
    CleanupClosePushL(thread);
    aWorkerAo->StartThreadL(thread);
    err = iWorkerAos.Append(aWorkerAo);
    if (err != KErrNone)
        {
        aWorkerAo->Cancel();
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(&thread);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ChannelWorkerThreadFunction
// ----------------------------------------------------------------------------
//	
TInt CSIPCRServerSession::ChannelWorkerThreadFunction(TAny* aPtr)
    {
    return WorkerThreadFunctionImpl(aPtr,
        &CSIPCRServerSession::ChannelImplementationL);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ConnectWorkerThreadFunction
// ----------------------------------------------------------------------------
//  
TInt CSIPCRServerSession::ConnectWorkerThreadFunction(TAny* aPtr)
    {
    return WorkerThreadFunctionImpl(aPtr,
        &CSIPCRServerSession::HandleNextPendingRequestL);
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::WorkerThreadFunctionImpl
// ----------------------------------------------------------------------------
//    
TInt CSIPCRServerSession::WorkerThreadFunctionImpl(
    TAny* aPtr,
    WorkerFunctionPtr aWorkerFunctionPtr)
    {
    CSipCrWorkerAo* workerAo = 
        reinterpret_cast<CSipCrWorkerAo*>(aPtr);
    TInt err = KErrNoMemory;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack)
        {
        TRAP(err, SIPStrings::OpenL());
        if (!err)
            {
            TRAP(err, (workerAo->Session().*aWorkerFunctionPtr)(*workerAo));
            SIPStrings::Close();
            REComSession::FinalClose(); // For each thread separately
            }
        }
    delete cleanupStack;
    return err;
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ProtectedAddToRequestQueueL
// ----------------------------------------------------------------------------
//
void CSIPCRServerSession::ProtectedAddToRequestQueueL(
    CSIPCRRequestItem* aRequestItem)
    {
    CSIPCRITCUtility::WaitForMutexLC(iRequestQueueMutex);
    iRequestQueue.AppendL(aRequestItem);
    CleanupStack::PopAndDestroy(); // signals iRequestQueueMutex
    }

// ----------------------------------------------------------------------------
// CSIPCRServerSession::ProtectedRemoveFromRequestQueue
// ----------------------------------------------------------------------------
//    
void CSIPCRServerSession::ProtectedRemoveFromRequestQueue(
    CSIPCRRequestItem* aRequestItem)
    {
    iRequestQueueMutex.Wait();
    TInt index = iRequestQueue.Find(aRequestItem); 
    if (index >= 0)
        {
        iRequestQueue.Remove(index);
        }    
    iRequestQueueMutex.Signal();
    }

// -----------------------------------------------------------------------------
// CSIPCRServerSession::RemoveFromRequestQueue
// -----------------------------------------------------------------------------
//
void CSIPCRServerSession::RemoveFromRequestQueue(TAny* aCleanupData)
	{
    TSipCrRequestQueueCleanupData* cleanupData = 
        reinterpret_cast<TSipCrRequestQueueCleanupData*>(aCleanupData);
    cleanupData->iSession->ProtectedRemoveFromRequestQueue(
        cleanupData->iRequestItem);
	}

// End of File
