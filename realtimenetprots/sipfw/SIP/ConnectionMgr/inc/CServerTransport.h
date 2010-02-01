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
* Name          : CServerTransport.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CSERVERTRANSPORT_H__
#define __CSERVERTRANSPORT_H__

#include <e32base.h>
#include "Lwtimer.h"
#include "MContext.h"
#include <in_sock.h>
#include "sipconnection.h"
#include "TSIPTransportParams.h"

class MServerTransportOwner;
class CSIPMessage;
class CSender;
class CSenderTcp;
class CReceiverTcp;
class CSocketContainer;
class CSIPNATTraversalController;
class MSIPNATBindingObserver;
class MSIPTransportRemovalObserver;
class COwnerSettingsList;

class CServerTransport : public CBase,
						 public MContext,
						 public MExpirationHandler
	{
public:
	static CServerTransport* NewL(RSocketServ& aServer, 
								  MServerTransportOwner& aMyOwner,
								  MTimerManager& aTimer,
								  TUint aTOne,
								  CSIPNATTraversalController& aNATTraversal,
								  COwnerSettingsList& aSettingsList);

	static CServerTransport* NewLC(RSocketServ& aServer, 
								   MServerTransportOwner& aMyOwner,
								   MTimerManager& aTimer,
								   TUint aTOne,
								   CSIPNATTraversalController& aNATTraversal,
								   COwnerSettingsList& aSettingsList);
									
	~CServerTransport();

public:
	
	void SendL(const TSIPTransportParams& aParams,
	           CSIPMessage& aMessage, 
	           TRequestStatus &aStatus);
	TBool CancelSend(TRequestStatus& aStatus);
	TBool Handle(const TInetAddr& aRemoteAddr);
	void AcceptedL();
	
	TSIPTransportParams& TransportParams();
	
	void CancelAllRequests(TInt aReason);
	
	CSender* Sender();
	
	void SetPersistency( TBool aIsPersistent );
	
	TBool IsPersistent() const;
	
	void InformSendingStatus();
	
	TBool Removal( MSIPNATBindingObserver& aBindingObserver );
	
	TBool Shutdown( MSIPTransportRemovalObserver* aRemovalObserver );
	
	
public:	// From MExpirationHandler

	void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam);
	

public:	// From MContext

	MSigCompController* SigCompHandler();
	CSocketContainer& SocketContainer();
	CSocketContainer* SocketContainer(TUint aIPAddrFamily);
	void ReceivedDataL(HBufC8* /*aData*/,
					   const TInetAddr& /*aRemoteAddr*/,
					   TBool /*aCompressed*/){}
	void ReceivedDataL(CSIPMessage* aMessage,
					   const TInetAddr& aRemoteAddr,
					   TInt aError,
					   TBool aCompressed);
	void ResetSocketL(){}
	void ContinueL(){}
	TBool StopL(){ return EFalse; }
	TBool ConnectionOpenL(){ return ETrue; }
	TBool DisconnectedL();
	TInt Remove();
	TUint32 IapId();
	void Sending( TBool aIsSending );
	void ReRouteL(TUint aProtocol, 
	              COutgoingData* aData);

	static const TInt iOffset;
	TSglQueLink iLink;
	
private:
	CServerTransport(MServerTransportOwner& aMyOwner, 
					 MTimerManager& aTimer,
					 CSIPNATTraversalController& aNATTraversal,
					 COwnerSettingsList& aSettingsList );
	void ConstructL(RSocketServ& aServer, TUint aTOne);
	void StartTimerL();
	void StopTimer();
	void SetShutdownObserver( MSIPTransportRemovalObserver* aRemovalObserver );
	void HandleShutdownCompletion();
	void RemoveL();

private: // Data

	MServerTransportOwner& iMyOwner;
	MTimerManager& iTimer;
	CSIPNATTraversalController& iNATTraversal;
	COwnerSettingsList& iSettingsList;
	TTimerId iTimerId;
	TUint iTimerValue;
	RSocket iSocket;
	TBool iAccepted;
	TInetAddr iRemoteAddr;
	CSocketContainer* iSocketContainer;
	CReceiverTcp* iReceiver;
	CSenderTcp* iSender;
	TSIPTransportParams iTransportParams;
	TBool iIsPersistent;
	TBool iIsSending;
	MSIPTransportRemovalObserver* iShutdownObserver;
private: //For testing puposes
#ifdef CPPUNIT_TEST
	friend class CServerTest;
#endif
	};

#endif // end of __CSERVERTRANSPORT_H__

// End of File
