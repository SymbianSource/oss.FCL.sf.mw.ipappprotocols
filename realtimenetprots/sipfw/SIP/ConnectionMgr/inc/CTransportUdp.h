/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CTransportUdp.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTRANSPORTUDP_H__
#define __CTRANSPORTUDP_H__

#include "CTransport.h"
#include "MContext.h"

class CResponseHandler;
class CSenderUdp;
class CSIPMessage;
class CReceiver;
class CSocketContainer;
class CSIPNATTraversalController;
class MSocketUsagePermissionObserver;


class CTransportUdp : public CTransport, public MContext
	{
	public:
		static CTransportUdp* NewL(
		    RSocketServ& aServer, 
			TUint aPort,
			MTransactionFinder& aFinder, 
			MServerTaFactory& aTaFactory,
			MTransportOwner& aTransportOwner,
			MTimerManager& aTimerManager,
			CSIPServerResolver& aServerResolver,
			RConnection& aConnection,
			MSigCompController& aSigCompHandler,
			TUint aICMPErrorWaitTime,
			CNetworkInfo& aNetworkInfo,
			COwnerSettingsList& aOwnerSettingsList,
			const TSIPTransportParams& aTransportParams,
			CSIPNATTraversalController& aNATTraversal,
			TBool aOnlyForReceiving = EFalse );

		static CTransportUdp* NewLC(
		    RSocketServ& aServer, 
			TUint aPort,
			MTransactionFinder& aFinder, 
			MServerTaFactory& aTaFactory,
			MTransportOwner& aTransportOwner,
			MTimerManager& aTimerManager,
			CSIPServerResolver& aServerResolver,
			RConnection& aConnection,
			MSigCompController& aSigCompHandler,
			TUint aICMPErrorWaitTime,
			CNetworkInfo& aNetworkInfo,
			COwnerSettingsList& aOwnerSettingsList,
			const TSIPTransportParams& aTransportParams,
			CSIPNATTraversalController& aNATTraversal,
			TBool aOnlyForReceiving = EFalse );
			
		~CTransportUdp();

	public: // From CTransport
	
		TBool Handle(TUint32 aTransportId);
		TBool HandleMessage(const TSIPTransportParams& aParams,
		                    RStringF aProtocol,
							const TInetAddr& aRemoteAddr,
							TUint aLocalPort,
							TBool aIsStrict);
		TUint SourcePort();
		void SendToNetL(const TSIPTransportParams& aParams,
		                const TInetAddr& aAddress, 
		                CSIPMessage& aMessage,
		                TBool aForceUDP,
						TUint aOrigTransport, 
						TRequestStatus &aStatus);
		void SendToNetL(const TSIPTransportParams& aParams,
		                const TInetAddr& aAddress, 
		                TTransactionId aId,
						CSIPMessage& aMessage, 
						TRequestStatus &aStatus);
		void SendToNetL(const TSIPTransportParams& aParams,
		                const TInetAddr& aAddress,
						CSIPMessage& aMessage,
						TRequestStatus &aStatus);
		TBool CancelSend(TRequestStatus& aStatus);
		CTransportErrorHandler& ErrorHandler() {return *iErrorHandler;}
		void WaitL();
		TBool IsWaiting(TUint aProtocol, TUint aPort);
		void CancelAllRequests(CSIPConnection::TState aReason);
		TUint Protocol() {return KProtocolInetUdp;}	
		CSocketContainer& GetSocketContainerL( const TInetAddr& aRemoteAddr );
		void InformSendingStatus( const TInetAddr& aRemoteAddr );
		CSender* Sender(const TSIPTransportParams& aParams,
		                TUint aProtocol, 
		                const TInetAddr& aRemoteAddr);       
		                
	    TBool Remove( TUint32 aTransportId );
	               
	    TBool AddUserL( const TSIPTransportParams& aParams, TUint aReceivePort );
	    
	    RSocket* AcquireUdpSocket( 
	        MSocketUsagePermissionObserver* aObserver,
	        const TInetAddr& aRemoteAddr,
            TBool& aPermissionToUse );
                                        
        void UdpSocketReleased( MSocketUsagePermissionObserver* aObserver,
                                RSocket& aSocket ); 
		
	public:	// From MContext
	
		CSocketContainer& SocketContainer();
		CSocketContainer* SocketContainer(TUint aIPAddrFamily);
		void ReceivedDataL(HBufC8* aData,
						   const TInetAddr& aRemoteAddr,
						   TBool aCompressed);
		void ReceivedDataL(CSIPMessage* /*aMessage*/,
						   const TInetAddr& /*aRemoteAddr*/,
						   TInt /*aError*/,
						   TBool /*aCompressed*/) {}
		MSigCompController* SigCompHandler();
		void ResetSocketL(); 
		void ContinueL();
		TBool StopL();
		TBool ConnectionOpenL();
		TBool DisconnectedL();
		TInt Remove();
		void ReRouteL(TUint aProtocol, 
		              COutgoingData* aData);
		TUint32 IapId();
		void Sending( TBool aIsSending );
	    
	public:
		
		void ICMPErrorL(CSipConnectionMgr::TICMPError aICMPError,
						const TInetAddr& aSourceAddr);
		TBool CancelResponseSend(TTransactionId aId, TBool aCancelAlso2xxResponses);

		void RemoveResHandler(CResponseHandler* aHandler);
				
	private:
	
		CTransportUdp(
		    RSocketServ& aServer,
		    TUint aPort,
            MTransactionFinder& aFinder, 
            MServerTaFactory& aTaFactory,
            MTransportOwner& aTransportOwner,
            MTimerManager& aTimerManager,					
	        CSIPServerResolver& aServerResolver,
        	RConnection& aConnection,
            MSigCompController& aSigCompHandler,
            TUint aICMPErrorWaitTime,
            CNetworkInfo& aNetworkInfo,
        	COwnerSettingsList& aOwnerSettingsList,
        	const TSIPTransportParams& aTransportParams,
            CSIPNATTraversalController& aNATTraversal,
            TBool aOnlyForReceiving);

		void ConstructL();
		void RefreshSocketContainerL( TUint aFamily );
		
		CSocketContainer* CreateSocketContainerL( 
		    TUint aFamily, 
		    RSocket& aSocket );
    private: // Data
    
		RSocket iIPv4Socket;
		RSocket iIPv6Socket;
		RSocketServ* iSocketServ;
		RConnection* iConnection;
		CSocketContainer* iIPv4SocketContainer;
		CSocketContainer* iIPv6SocketContainer;
		CReceiver* iIPv4Receiver;
		CReceiver* iIPv6Receiver;
		CSenderUdp* iSender;
		CTransportErrorHandler* iErrorHandler;
		TUint iSourcePort;
		MTimerManager& iTimerManager;
		TSglQue<CResponseHandler> iList;
		TSglQueIter<CResponseHandler> iListIter;
		TUint iICMPErrorWaitTime;
		RArray<TUint> iTransportUsers;
		TBool iOnlyForReceiving;
		TBool iIsSending;
		MSocketUsagePermissionObserver* iSocketUsagePermissionObserver;	

	private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CTransportUdpTest;
	friend class CResHandlerTest;
#endif
	};
#endif // end of __CTRANSPORTUDP_H__

// End of File
