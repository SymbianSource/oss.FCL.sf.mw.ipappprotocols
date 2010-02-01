/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipConnection.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0
*
*/



#ifndef __CSIPCONNECTION_H__

/**
* @internalComponent
*/
#define __CSIPCONNECTION_H__

// INCLUDES
#include "MTransportOwner.h"
#include "MIcmpReceiver.h"
#include "CSipConnectionMgr.h"
#include "msipconnectioncontext.h"
#include "MOutgoingRequestQueueContext.h"
#include "sipnetworkobserver.h"
#include "CNetworkManager.h"
#include "CStateModel.h"
#include <commsdattypesv1_1.h>

#include <stringpool.h>

// FORWARD DECLARATIONS
class MTransactionFinder;
class MSigCompController;
class MServerTaFactory;
class MTimerManager;
class MSIPServerResolverObserver;
class RSocketServ;
class CLocalAddrResolver;
class CTransportBase;
class CIcmpReceiver;
class CSIPServerResolver;
class CSender;
class COutgoingData;
class CTcpResponseSender;
class CSIPMessage;
class CSIPViaHeader;
class CURIContainer;
class COwnerSettingsList;
class CConnectionContainer;
class CNetworkManager;
class CStateModel;
class CNetworkInfo;
class CSIPNATTraversalController;
class COutgoingRequestQueue;
class CUri8;
class MSIPNATBindingObserver;


// CLASS DEFINITION
class CSipConnection
	: public CBase,
	  public MTransportOwner,
	  public MSIPConnectionContext,
	  public MOutgoingRequestQueueContext,
	  public MIcmpErrorObserver
	{
	public: // Constructors and destructor

	    /**
		* Constructs, adds the pointer onto the cleanup stack
		* and returns a pointer to, a new CSipConnection object;
		* leaves on failure.
		*
		* @param aFinder	A reference to the MTransactionFinder object
		* @param aTaFactory	A reference to the MServerTaFactory object
		* @param aTimer		A reference to the MTimerManager object
		* @param aTOne		T1 timervalue
		* @param aParams	info about transport (iapId etc.)
		* @param aServer	socket server		
		* @param aSigCompHandler The handler for signaling compression
		* @param aTUNetStatusObserver Callback where the connection state
		*					changes are notified
		* @param aNetworkManager Monitors network's state
		* @param aOwner		Owner of the CSipConnection
		* @param aNetworkInfo Network information store
		*
		* @return CSipConnection
		*/
		static CSipConnection*
			NewLC( MTransactionFinder& aFinder,
				   MServerTaFactory& aTaFactory,
				   MTimerManager& aTimer,
				   TUint aTOne,
				   const TSIPTransportParams& aParams,
				   RSocketServ& aServer,
				   MSigCompController& aSigCompHandler,
				   MConnectionStateObserver& aTUNetStatusObserver,
				   CNetworkManager& aNetworkManager,
				   MSipConnectionOwner& aOwner,
				   CNetworkInfo& aNetworkInfo,
				   CSIPNATTraversalController& aNATTraversal );

		/// Destructor
		~CSipConnection();


	public: // New functions, ConnectionMgr offers through MSipConnectionMgr
	
        void SetOptL( const TSIPTransportParams& aParams,
                      TUint aOptionName,
	                  TUint aOptionLevel,
	                  const TDesC8& aOption );
	         	                         
        void SetOptL( const TSIPTransportParams& aParams,
                      TUint aOptionName,
	                  TUint aOptionLevel,
	                  TInt aOption ); 	

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


	public: // From MSIPConnectionContext

		void SendToTransportL( const TSIPTransportParams& aParams,
		                       CSIPResponse& aResponse,
							   TTransactionId aId,
							   TRequestStatus &aStatus );

		void SendToTransportL( const TSIPTransportParams& aParams,
		                       CSIPRequest& aRequest,
		                       TBool aForceUDP,
		                       const CUri8* aOutboundProxy,
							   const TInetAddr& aAddr,
							   TRequestStatus &aStatus );

		TInt OpenConnection();

		void CloseConnection();

		void CreateTransportResourcesL();
        void CreateDefaultTransportsL();

		void ReleaseTransportResources( TBool aReleaseAllResources );
		
		TBool DoTransportResourcesExist() const;

		void ResolveLocalAddress( TInetAddr& aAddress );

		void StartMonitoringL( TInt aError );

		void StopMonitoring();
		
		TBool ContinueMonitoring( TInt aError );
		
		TInt MonitorConnectionClosure();

		void StateModelChanged( MStateModel::TState aState );
		
		void Destroy();

		TInt ConnectionType();
			
				
	public: // From MOutgoingRequestQueueContext
	
	    void ContinueSendToTransportL( const TSIPTransportParams& aParams,
        		                       CSIPRequest& aRequest,
        		                       TBool aForceUDP,
        							   const TInetAddr& aRemoteAddr,
        							   TUint aLocalPort,
        							   TUint aOrigTransport,
        							   TRequestStatus& aStatus,
        							   TBool aPublicAddrResolved );
        							   
        void RequestFailed( TRequestStatus& aStatus, TInt aError );
        
        RSocket& GetUdpSocketL( const TSIPTransportParams& aParams, 
                                const TInetAddr& aRemoteAddr,
                                TUint& aLocalPort,
                                MSocketUsagePermissionObserver* aObserver,
                                TBool& aPermissionToUse );
        
        void UdpSocketFree( const TSIPTransportParams& aParams,
                            const TInetAddr& aRemoteAddr,
                            TUint aLocalPort,
                            MSocketUsagePermissionObserver* aObserver,
                            RSocket& aSocket );
                              
        		
	public: // From MTransportOwner
	
	    void AddListenerL( const TSIPTransportParams& aParams, 
	                       TUint aProtocol, 
	                       TUint aPort );
	    
	    TInt RemoveTransport( CTransportBase* aTransport );

		void ReRouteL( TUint aProtocol, 
		               COutgoingData* aData );
		              
		void TcpConnected( const TInetAddr& aAddr );	
		
        TBool IsLocalAddr(TInetAddr& aAddr) const; 
		
		void GetLocalAddrL( TInetAddr& aAddr, 
		                    const TInetAddr* aNextHop );

        TBool GetLocalIPAddr( TUint aFamily, TInetAddr& aAddr );
		                    
		void FillWithMatchingAddrFamily( TInetAddr& aAddr, 
		                                 const TInetAddr* aNextHop );		                    
		
		TUint32 IapId() const;


	public: // New functions

		CSIPConnection::TState State();
		void SystemStarted();
		void CellularUsageAllowed();
		void GetByURIL( const TSIPTransportParams& aParams,
		                const CURIContainer& aURI,
		                RPointerArray<MSIPResolvingResult>& aResult,
		                MSIPServerResolverObserver* aObserver );

		void CancelGetByUri( MSIPServerResolverObserver* aObserver );
		TInt GetLocalAddress( TInetAddr& aAddr );
		TBool Match( const TDesC8& aAddress );
		void ResetTransportsL();		
		void CancelAllRequests( CSIPConnection::TState aReason );		
		void IcmpError( const TInetAddr& aAddress );		
		TBool Handle( TUint32 aIapId ) const;		
		TInt DecreaseUserCount( const TSIPTransportParams& aParams );
		void IncreaseUserCount();
		void ReserveTransportL( const TSIPTransportParams& aOldTransportParams,
		                        const TSIPTransportParams& aTransportParams,
                                TUint aLocalSendPort,
						        TUint aLocalReceivePort,
			                    const TInetAddr& aRemoteAddr,
			                    TUint aRemoteSendPort );       
	    void ReserveTLSTransportL( TSIPTransportParams& aTransportParams );      
	    TBool RemoveReservedTransport( TUint32 aTransportId,
	                                   MSIPTransportRemovalObserver* aRemovalObserver,
                                       TInt& aNumAsyncRemovals );		
		TInt ConnectionError();
		TInt DetermineConnectionTypeL();
		static const TInt iConnectionOffset;
		TSglQueLink iConnectionLink;

	private:

		void ConstructL();
		CSipConnection( MTransactionFinder& aFinder,
						MServerTaFactory& aTaFactory,
						MTimerManager& aTimer,
						TUint aTOne,
						const TSIPTransportParams& aParams,
						RSocketServ& aServer,
						MSigCompController& aSigCompHandler,
						MConnectionStateObserver& aTUNetStatusObserver,
						CNetworkManager& aNetworkManager,
						MSipConnectionOwner& aOwner,
						CNetworkInfo& aNetworkInfo,
						CSIPNATTraversalController& aNATTraversal );

		CTransportBase* FindTransportL( const TSIPTransportParams& aParams,
		                                CSIPMessage* aMessage, 
		                                TTransactionId aId,
		                                TBool aIsStrict );
		CTransportBase* FindTransportL( const TSIPTransportParams& aParams,
		                                CSIPMessage* aMessage,
									    const TInetAddr& aRemoteAddr,
									    TUint aLocalPort = 0,
									    TBool aIsStrict = ETrue );
	    CTransportBase* FindTransport( const TSIPTransportParams& aParams,
                                       RStringF aProtocol,
                                       const TInetAddr& aRemoteAddr,
                                       TUint aLocalPort = 0,
                                       TBool aIsStrict = ETrue );
                                       
		TBool IsListened( TInt aProtocol, TUint aPort );
		void AddTransportL( CTransportBase* aTransport );
		TBool IsInList( CTransportBase* aTransport );
		void RemoveAllTransports();
		TBool IsTransportUdp( CSIPMessage* aMessage );
		TUint TransportProtocolL( CSIPMessage& aMessage );
		
		void AddTcpListenerL( const TSIPTransportParams& aParams, 
		                      TUint aPort, 
		                      TBool aIsReserved = EFalse );
		
		/**
         * returns pointer to created transport, ownership is not transferred
         */                         
		CTransportBase* AddUdpListenerL( const TSIPTransportParams& aParams, 
		                                 TUint aPort, 
		                                 TBool aIsReserved = EFalse,
		                                 TBool aOnlyForReceiving = EFalse );

        void AddTcpListenerUserL( const TSIPTransportParams& aOldParams,
                                  const TSIPTransportParams& aParams,
                                  TUint aReceivePort );
         
                             
        void AddUdpListenerUserL( const TSIPTransportParams& aOldParams,
                                  const TSIPTransportParams& aParams,
                                  TUint aReceivePort );
                                  
		void CreateConnectedTransportPointL( const TSIPTransportParams& aParams,
                                             CSIPMessage& aMessage, 
                                             const TInetAddr& aAddress );                                     
		void CreateTcpTransportPointL( const TSIPTransportParams& aParams,
		                               const TInetAddr& aAddress );
		                               
		void CreateTlsTransportPointL( const TSIPTransportParams& aParams,
		                               const TInetAddr& aAddress );
		CSender* GetSender( const TSIPTransportParams& aParams,
		                    TUint aProtocol, 
		                    const TInetAddr& aAddr );
		TUint UpdateTransportIfNeededL( CSIPMessage& aMessage, 
		                                const TSIPTransportParams& aParams,
                                        TBool aForceUDP,
                                        const TInetAddr& aAddr );
		void RemoveTcpSender( CTcpResponseSender* aThis );
		void RemoveAllTcpSender();
		CSIPViaHeader* TopViaHeader( CSIPMessage* aMessage );
		void SetState( MStateModel::TState aState );
		RConnection& Connection(); 
		void NotifyIcmpErrorL( const TInetAddr& aAddress );
		TBool IsConnectionActive() const;
		void UpdateURIL( CURIContainer& aURI, 
		                 const TInetAddr& aAddress, 
		                 TUint aProtocol ) const;

	private: // Data

		MTransactionFinder& iFinder;
		MServerTaFactory& iTaFactory;
		MConnectionStateObserver& iTUNetStatusObserver;
		CLocalAddrResolver* iLocalAddrResolver;

		MSigCompController& iSigCompHandler;

		RSocketServ& iServer;
		TSglQue<CTransportBase> iList;
		TSglQueIter<CTransportBase> iListIter;
		TSglQue<CTcpResponseSender> iTcpSenderList;
		TSglQueIter<CTcpResponseSender> iTcpSenderListIter;
		TUint iTOne;
		CIcmpReceiver* iIcmpReceiver;
		CSIPServerResolver* iServerResolver;
		TInt iUserCount;

		CNetworkManager& iNetworkManager;
		CConnectionContainer* iConnection;
		CStateModel* iStateModel;
		MTimerManager& iTimer;
		MSipConnectionOwner& iOwner;
		TUint32 iIapId;
		CSIPConnection::TState iState;
		
		COwnerSettingsList* iSettingsList;
		
		COutgoingRequestQueue* iRequestQueue;
		
		CNetworkInfo& iNetworkInfo;
		
		CSIPNATTraversalController& iNATTraversal;

		TBool iWaitingForDataUsage;

		friend class CTcpResponseSender;

	private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSipConnectionTest;
		friend class CSipConnectionMgrTest;
		friend class CTcpResponseHandlerTest;
#endif
	};


#endif // end of __CSipConnection_H__
