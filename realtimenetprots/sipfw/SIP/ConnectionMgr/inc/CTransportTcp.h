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
* Name          : CTransportTcp.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTRANSPORTTCP_H__
#define __CTRANSPORTTCP_H__

#include <in_sock.h>
#include "MContext.h"
#include "CTransport.h"
#include "Lwtimer.h"

class CSIPConnection;
class CSIPMessage;
class MTransactionFinder;
class MServerTaFactory;
class MTransportOwner;
class CTransportErrorHandler;
class CReceiverTcp;
class CSenderTcp;
class CSender;
class CTcpTransportState;
class MTimerManager;
class CNetworkInfo;
class CSocketContainer;
class CSIPNATTraversalController;

class CTransportTcp : public CTransport,
					  public MContext,
					  public MExpirationHandler
	{
	
public:

	static CTransportTcp* NewL(RSocketServ& aServer,
				MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				MTimerManager& aTimer,
				const TSIPTransportParams& aTransportParams,
				const TInetAddr& aRemoteAddr,
				TUint aTOne,
				CSIPServerResolver& aServerResolver,
				RConnection& aConnection,
				MSigCompController& aSigCompHandler,
				TUint aLocalSendPort,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				CSIPNATTraversalController& aNATTraversal);

	static CTransportTcp* NewLC(RSocketServ& aServer,
				MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				MTimerManager& aTimer,
				const TSIPTransportParams& aTransportParams,
				const TInetAddr& aRemoteAddr,
				TUint aTOne,
				CSIPServerResolver& aServerResolver,
				RConnection& aConnection,
				MSigCompController& aSigCompHandler,
				TUint aLocalSendPort,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				CSIPNATTraversalController& aNATTraversal);

    ~CTransportTcp();

public:	// From CTransport

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
	TBool CancelSend(TRequestStatus& aStatus);
	CTransportErrorHandler& ErrorHandler() {return *iErrorHandler;}
	void WaitL();
	TBool IsWaiting(TUint aProtocol, TUint aPort);
	void CancelAllRequests(CSIPConnection::TState aReason);
	TUint Protocol() {return KProtocolInetTcp;}
	CSocketContainer& GetSocketContainerL( const TInetAddr& aRemoteAddr );
	void InformSendingStatus( const TInetAddr& aRemoteAddr );
	CSender* Sender(const TSIPTransportParams& aParams,
	                TUint aProtocol, 
	                const TInetAddr& aRemoteAddr);
	void Disconnect(const TInetAddr& aRemoteHost);
	             
	TBool Shutdown( TUint32 aTransportId,
	                MSIPTransportRemovalObserver* aRemovalObserver );
	                
	TInetAddr* PersistentRemoteAddr( MSIPNATBindingObserver* aBindingObserver );
	
	             
public:	// From MContext

	CSocketContainer& SocketContainer();
    CSocketContainer* SocketContainer(TUint aIPAddrFamily);
	void ReceivedDataL(HBufC8* /*aData*/,
					   const TInetAddr& /*aRemoteAddr*/,
					   TBool /*aCompressed*/) {}
	void ReceivedDataL(CSIPMessage* aMessage,
					   const TInetAddr& aRemoteAddr,
					   TInt aError,
					   TBool aCompressed);
	MSigCompController* SigCompHandler();
	void ResetSocketL(); 
	void ContinueL();
	TBool StopL();
	TInt Remove();
	TBool ConnectionOpenL();
	TBool DisconnectedL();
	void ReRouteL(TUint aProtocol, 
	              COutgoingData* aData);
	TUint32 IapId();
	void Sending( TBool aIsSending );

public: // New methods

	void SetCurrentState(CTcpTransportState* aState);
	void ConnectSocket();
	void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam);
	void StartTimerL();
	void StopTimer();
	void GetRemoteAddr(TInetAddr& aAddr);
	CReceiverTcp* Receiver() {return iReceiver;}
	MTransportOwner* TransportOwner();	
    CSender* GetSender();
	
private:

	CTransportTcp(MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				const TSIPTransportParams& aTransportParams,
				CSIPServerResolver& aServerResolver,
				MSigCompController& aSigCompHandler,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				CSIPNATTraversalController& aNATTraversal);

    void ConstructL(RSocketServ& aServer,
				MTimerManager& aTimer,
				const TInetAddr& aRemoteAddr,
				TUint aTOne,
				RConnection& aConnection,
				TUint aLocalSendPort);
				
private:

    void SetShutdownObserver( MSIPTransportRemovalObserver* aRemovalObserver );
    void HandleShutdownCompletion();

private: // Data
				
	RSocket iSocket;
	RSocketServ* iSocketServ;
	CSocketContainer* iSocketContainer;
	CTransportErrorHandler* iErrorHandler;
	CReceiverTcp* iReceiver;
	CSenderTcp* iSender;
	CTcpTransportState* iCurrentState;
	CTcpTransportState* iConnectedState;
	CTcpTransportState* iConnectingState;
	TUint iTimerValue;
	MTimerManager* iTimer;
	TTimerId iMyTimerId;
	TInetAddr iRemoteAddr;
	RConnection* iConnection;
	TBool iIsSending;
	MSIPTransportRemovalObserver* iShutdownObserver;	
private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CTcpTransTest;
	friend class CTransportTcpTest;
#endif
	};

#endif // end of __CTRANSPORTTCP_H__

// End of File
