/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : CSipCRServerSession.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCRSERVERSESSION_H
#define CSIPCRSERVERSESSION_H

//  INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"
#include "TSipClient.h"

// FORWARD DECLARATIONS
class CSIPCRServer;
class CSIPCRSessionReceiver;
class CSIPCRRequestItem;
class CSIPRequest;
class CSIPResponse;
class CSIPCRITCUtility;
class CSIPResolvedClient;
class CSIPResolvedClient2;
class CSipCrWorkerAo;

// CLASS DEFINITIONS
class CSIPCRServerSession : public CSession2
	{
public: // Constructors and destructor

    static CSIPCRServerSession* NewL(CSIPCRServer& aCRServer);
    static CSIPCRServerSession* NewLC(CSIPCRServer& aCRServer);
    ~CSIPCRServerSession ();

public: // From CSession2

    void ServiceL(const RMessage2& aMessage);

public: // New functions
    
    void WorkerAoRequestCompleted(CSipCrWorkerAo* aWorkerAo);
    
    void RoutingEntryAddedL(const TUid& aUid);

private: // Second phase constructors

    CSIPCRServerSession(CSIPCRServer& aCRServer);
    void ConstructL();

private: // Helper functions

    void DoServiceL (TInt aFunction, const RMessage2& aMessage);

    void ChannelWithResolverL(const RMessage2& aMessage);
    
    void ChannelL(const RMessage2& aMessage);

	void RegisterL(const TUid& aChannelUid);

	void DeregisterL(const TUid& aChannelUid);

	void SetUIDL(const RMessage2& aMessage);

	void CancelRequestL(const RMessage2& aMessage);
	
	void CancelAllRequests();

    void CancelClientReceiveL();

    void ClientReadyToReceiveL(const RMessage2& aMessage);
    
    void ClientReceiveSipResponseL(const RMessage2& aMessage);

    CSIPResponse* ResolveClientL(CSIPRequest& aRequest, TSipClient& aUid);
    
    void ChannelL(TUint32 aRequestId,
                  CSIPRequest& aRequest,
                  const TSipClient& aClientUid,
                  const TUid& aResolverUid);
                  
    void ChannelImplementationL(CSipCrWorkerAo& aWorkerAo);                  
        
    void HandleNextPendingRequestL(CSipCrWorkerAo& aWorkerAo);
                      
	void ChannelUidL( const TSipClient& aClient,
	                  TUid& aChannelUid, 
					  CSIPRequest* aRequest, 
					  CSIPResolvedClient* aResolvedClient );
	                      
	TUint32 CreateRequestId();
	
	TBool FindUid(const RArray<TSipClient>& aUids, 
	              TSipClient& aUid, 
	              TBool aRomBased,
	              TBool aConnected);
	
	typedef TInt (*ThreadFunctionPtr)(TAny* aPtr);
	        
    void CreateWorkerThreadL(
        CSipCrWorkerAo* aWorkerAo,
        ThreadFunctionPtr aThreadFunctionPtr);
	
	static TInt ChannelWorkerThreadFunction(TAny* aPtr);
	
	static TInt ConnectWorkerThreadFunction(TAny* aPtr);
	
	typedef void 
	    (CSIPCRServerSession::*WorkerFunctionPtr)(CSipCrWorkerAo& aWorkerAo);
	
	static TInt WorkerThreadFunctionImpl(
	    TAny* aPtr,
        WorkerFunctionPtr aWorkerFunctionPtr);
	                                     
	void CancelConnectL(CSIPCRRequestItem* aRequestItem);
	
	template<class T> void AddToRequestQueueL( 
	    CSIPCRRequestItem* aRequestItem, 
        T& aResolvedClient);
										
	template<class T> TBool ConnectL( 
	    CSIPCRRequestItem& aRequestItem, 
        T& aResolvedClient);
		
    void ProtectedAddToRequestQueueL(CSIPCRRequestItem* aRequestItem);
    
    void ProtectedRemoveFromRequestQueue(CSIPCRRequestItem* aRequestItem);	
		
    static void RemoveFromRequestQueue(TAny* aCleanupData);	
							                       
private: // Data                    
  
    TUid iClientUid;
    TBool iClientUidSet;
    CSIPCRServer& iCRServer;
    CSIPCRSessionReceiver* iReceiver;    
    RPointerArray<CSIPCRRequestItem> iRequestQueue;
    TUint32 iRequestIdCounter;
    RPointerArray<CSipCrWorkerAo> iWorkerAos;
    RMutex iRequestQueueMutex;  
    
private: // For testing purposes

	UNIT_TEST(CSipCRServerSessionTest)
    };

#endif // CSIPCRSERVERSESSION_H

// End of File
