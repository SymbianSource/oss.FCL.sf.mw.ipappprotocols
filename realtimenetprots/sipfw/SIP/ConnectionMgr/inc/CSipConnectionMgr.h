/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipConnectionMgr.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPCONNECTIONMGR_H__
#define __CSIPCONNECTIONMGR_H__

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MUriResolver.h"
#include "SipStackServerDefs.h"
#include "MConnectionStateObserver.h"
#include "MSipConnectionMgr.h"
#include "sipclient.h"
#include "MSigComp.h"
#include "MLocalName.h"
#include "MSipConnectionOwner.h"
#include "MSIPTransportMgr.h"
#include <sipsystemstateobserver.h>
#include "MSipCellularDataUsageObserver.h"

// FORWARD DECLARATIONS
class CSipConnectionMgrStateBase;
class MTransactionFinder;
class MSIPResolvingResult;
class MTimerManager;
class CSipConnection;
class CSIPResponse;
class CSIPRequest;
class MSigCompController;
class CNetworkManager;
class MSIPServerResolverObserver;
class TSIPTransportParams;
class CNetworkInfo;
class MServerTaFactory;
class CSIPNATTraversalController;
class CSipSystemStateMonitor;
class CSipCellularDataUsageMonitor;


class CSipConnectionMgr : public CBase,
	                      public MSipConnectionMgr,
	                      public MSipUriResolver,
	                      public MSigComp, 
	                      public MLocalName,
	                      public MSipConnectionOwner,
	                      public MSIPTransportMgr,
	                      public MSipSystemStateObserver,
	                      public MSipCellularDataUsageObserver
	                      
	{
public:

		enum TICMPError
			{
			EHostUnreachable = 850,
			ENetUnreachable,
			EPortUnreachable,
			EProtocolUnreachable
			};

public:

	static CSipConnectionMgr* NewL(MTransactionFinder& aFinder, 
		MServerTaFactory& aTaFactory, MTimerManager& aTimer, TUint aTOne, 
		MConnectionStateObserver& aNetworkObserver);

	static CSipConnectionMgr* NewLC(MTransactionFinder& aFinder, 
		MServerTaFactory& aTaFactory, MTimerManager& aTimer, TUint aTOne, 
		MConnectionStateObserver& aNetworkObserver);

	~CSipConnectionMgr();

	void RefreshConnectionL(TUint32 aIapId);
	
	TInt ConnectionErrorL(TUint32 aIapId);


public: // From MSipConnectionMgr

    CSIPConnection::TState OpenL( const TSIPTransportParams& aParams );

    TBool IsOpen( const TSIPTransportParams& aParams );

	void Close( const TSIPTransportParams& aParams );
	
    void SetOptL( const TSIPTransportParams& aParams,
                  TUint aOptionName,
	              TUint aOptionLevel,
	              const TDesC8& aOption );
	         	                         
    void SetOptL( const TSIPTransportParams& aParams,
                  TUint aOptionName,
	              TUint aOptionLevel,
	              TInt aOption); 	

	void SendL( const TSIPTransportParams& aParams,
	            CSIPRequest& aRequest,
	            TBool aForceUDP,
	            const CUri8* aOutboundProxy,
			    const TInetAddr& aRemoteAddr, 
			    TRequestStatus& aStatus );

    void SendL( const TSIPTransportParams& aParams,
	            CSIPResponse& aResponse,
                TTransactionId aId,
			    TRequestStatus& aStatus );

    TInt CancelSend( TRequestStatus &aStatus );

	TInt CancelResponseSend( TTransactionId aId, TBool aCancelAlso2xxResponses );

    void TcpDisconnect( const TSIPTransportParams& aParams,
	                    const TInetAddr& aRemoteAddress );
	                    
    void FreeResources( MSIPNATBindingObserver& aSIPNATBindingObserver );


public: // From MSipUriResolver

    void GetByURIL( const CURIContainer& aURI,
                    RPointerArray<MSIPResolvingResult>& aResult,
					const TSIPTransportParams& aParams,
		            MSIPServerResolverObserver* aObserver );

	void CancelGetByUri( MSIPServerResolverObserver* aObserver );


public: // From MSigComp

	TBool IsSupported () const;

	TUint32 CreateCompartmentL( TUint32 aIapId );

    void RemoveCompartment( TUint32 aCompartmentId );

	TInt CompartmentCount();

	void SendFailedL( TUint32 aCompartmentId );


public: // From MLocalName

	TBool Match( const TDesC8& aAddress );
	
	
public: // From MSipConnectionOwner

    TBool IsSystemStarted() const;

	void Destroy( TUint32 aIapId );

	TBool IsCellularDataUsageAllowed() const;

public: // From MSIPTransportMgr

    void ReserveTransportL( TSIPTransportParams& aTransportParams,
                            TUint aLocalSendPort,
						    TUint aLocalReceivePort,
			                const TInetAddr& aRemoteAddr,
			                TUint aRemoteSendPort );
    
    void CreateTLSTransportL( TSIPTransportParams& aTransportParams );
    
    void RemoveTransport( TUint32 aTransportId );
    	                
	TInt RemoveTransport( TUint32 aTransportId,
	                      MSIPTransportRemovalObserver* aRemovalObserver );
	
	TInt GetLocalAddress( TUint32 aIapId, TInetAddr& aAddr );

public: // From MSipSystemStateObserver

    void SystemVariableUpdated( 
        CSipSystemStateMonitor::TSystemVariable aVariable,
		TInt aObjectId,
		TInt aValue );

public: //MSipCellularDataUsageObserver
void CellularDataUsageChangedL(MSipCellularDataUsageObserver::TCellularDataUsageStatus aStatus);



private: // New functions

	CSipConnection* CreateNewConnectionL( const TSIPTransportParams& aParams );
	TBool ConnectionExists( TUint32 aIapId );
	CSipConnection* Connection( TUint32 aIapId );
	void RemoveConnection( TUint32 aIapId );
	void RemoveAllConnections();
	TUint32 CreateTransportId();
	void LoadSigCompL();
	void LoadNATTraversalL();
	void LoadSystemStateMonitorL();

private: // Constructors

	CSipConnectionMgr( MTransactionFinder& aFinder, 
		MServerTaFactory& aTaFactory, MTimerManager& aTimer, TUint aTOne, 
		MConnectionStateObserver& aNetworkObserver );
	void ConstructL();


private: // Data
	
	CNetworkManager* iNetworkManager;

	MSigCompController* iSigCompController;
	MConnectionStateObserver* iTUNetStatusObserver;
	MTransactionFinder* iFinder; 
	MServerTaFactory* iTaFactory;
	MTimerManager* iTimer;	
	RSocketServ iSocketServ;
	TUint iTOne;
	TSglQue<CSipConnection> iConnectionList;
	TSglQueIter<CSipConnection> iConnectionListIter;
	TUint32 iTransportIdCounter;
	
	CNetworkInfo* iNetworkInfo; //owned
	
	CSIPNATTraversalController* iNATTraversal;
	
	CSipSystemStateMonitor* iSystemStateMonitor;
	
	CSipSystemStateMonitor::TSystemState iSystemState;

	CSipCellularDataUsageMonitor * iCellularDataUsageMonitor;

private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSipConnectionMgrTest;
#endif
    };
    
#endif // end of __CSIPCONNECTIONMGR_H__

// End of File
