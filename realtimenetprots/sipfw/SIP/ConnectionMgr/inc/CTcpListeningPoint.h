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
* Name          : CTcpListeningPoint.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CTCPLISTENINGPOINT_H__
#define __CTCPLISTENINGPOINT_H__

#include <in_sock.h>
#include <stringpool.h>
#include "MContext.h"
#include "CTransport.h"
#include "MServerTransportOwner.h"
#include "MListenerOwner.h"

class MTransactionFinder;
class MServerTaFactory;
class MTransportOwner;
class CTransErrorHandler;
class MTimerManager;
class CServerTransport;
class CListener;
class CSIPMessage;
class CSIPConnection;
class CSIPNATTraversalController;


class CTcpListeningPoint : public CTransport,
						   public MServerTransportOwner,
						   public MListenerOwner 
	{
public: // Constructors and destructor

	static CTcpListeningPoint* NewL( RSocketServ& aServer,
				MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				MTimerManager& aTimer,
				TUint aPort,
				TUint aTOne,
				CSIPServerResolver& aServerResolver,
				RConnection& aConnection,
				MSigCompController& aSigCompHandler,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				const TSIPTransportParams& aTransportParams,
				CSIPNATTraversalController& aNATTraversal );

	static CTcpListeningPoint* NewLC( RSocketServ& aServer,
				MTransactionFinder& aFinder, 
				MServerTaFactory& aTaFactory,
				MTransportOwner& aTransportOwner,
				MTimerManager& aTimer,
				TUint aPort,
				TUint aTOne,
				CSIPServerResolver& aServerResolver,
				RConnection& aConnection,
				MSigCompController& aSigCompHandler,
				CNetworkInfo& aNetworkInfo,
				COwnerSettingsList& aSettingsList,
				const TSIPTransportParams& aTransportParams,
				CSIPNATTraversalController& aNATTraversal );

    ~CTcpListeningPoint();
				
public:	// From CTransport

	TBool Handle( TUint32 aTransportId );
	TBool HandleMessage( const TSIPTransportParams& aParams,
	                     RStringF aProtocol, 
	                     const TInetAddr& aRemoteAddr,
	                     TUint aLocalPort,
	                     TBool aIsStrict );
	TUint SourcePort(){ return iSourcePort; }
	void SendToNetL( const TSIPTransportParams& aParams,
	                 const TInetAddr& aAddress, 
	                 CSIPMessage& aMessage,
	                 TBool aForceUDP,
					 TUint aOrigTransport, 
					 TRequestStatus &aStatus );
	TBool CancelSend( TRequestStatus& aStatus );
	CTransportErrorHandler& ErrorHandler() { return *iErrorHandler; }
	void WaitL();
	TBool IsWaiting( TUint aProtocol, TUint aPort );
	void CancelAllRequests( CSIPConnection::TState aReason );
	TUint Protocol() { return KProtocolInetTcp; }
	CSocketContainer& GetSocketContainerL( const TInetAddr& aRemoteAddr );
	void InformSendingStatus( const TInetAddr& aRemoteAddr );
	void SetPersistencyL( const TInetAddr& aRemoteAddr, 
	                      TBool aIsPersistent,
	                      MSIPNATBindingObserver* aBindingObserver );
	CSender* Sender( const TSIPTransportParams& aParams,
	                 TUint aProtocol, 
	                 const TInetAddr& aRemoteAddr );
	void Disconnect( const TInetAddr& aRemoteHost );       
	TBool Remove( TUint32 aTransportId );
	TBool Remove( MSIPNATBindingObserver& aSIPNATBindingObserver );
	TBool Shutdown( TUint32 aTransportId,
	                MSIPTransportRemovalObserver* aRemovalObserver );
	TBool AddUserL( const TSIPTransportParams& aParams, TUint aReceivePort );
	
public: // From MServerTransportOwner

	void RemoveL( CServerTransport* aTransport,
				  const TInetAddr& aRemoteAddr,
				  MSIPTransportRemovalObserver* aRemovalObserver );
	void ReceiveL( CSIPMessage* aMessage,
				   const TInetAddr& aRemoteAddr,
				   TInt aError,
				   TBool aCompressed );
	void ReRouteL( COutgoingData* aData );
	MSigCompController* SigCompHandler();
	TUint32 IapId();
	TBool IsReservedTransportOwner() const;
	
public:	// From MListenerOwner

	void AcceptedL();
	void AcceptErrorL( TInt aError );
	void GetLocalAddrL( TInetAddr& aAddr );
	
private: // Private contructors

    void ConstructL();

	CTcpListeningPoint( RSocketServ& aServer,
                    	MTransactionFinder& aFinder, 
                    	MServerTaFactory& aTaFactory,
                    	MTransportOwner& aTransportOwner,
                    	MTimerManager& aTimer,
                    	TUint aPort,
                    	TUint aTOne,
                    	CSIPServerResolver& aServerResolver,
                    	RConnection& aConnection,
                    	MSigCompController& aSigCompHandler,
                    	CNetworkInfo& aNetworkInfo,
                    	COwnerSettingsList& aSettingsList,
                    	const TSIPTransportParams& aTransportParams,
                    	CSIPNATTraversalController& aNATTraversal );

private:

    CServerTransport* FindServerTransport( const TInetAddr& aRemoteAddr );
    TBool IsRemoteAllowed( CServerTransport& aTransport );
    void RemoveWaitingAccept();
    
    void HandleShutdownCompletion( MSIPTransportRemovalObserver* aRemovalObserver,
                                   TUint32 aTransportId );
    TBool RemovalAllowed();
    void RemoveServerTransport( CServerTransport* aServerTransport );
    
private: // Data

	RSocketServ* iSocketServ;
	CTransportErrorHandler* iErrorHandler;
	MTimerManager* iTimer;
	CListener* iListener;
	TUint iSourcePort;
	TUint iTimerOne;
	TBool iErrorOn;
	RConnection* iConnection;
	
	TSglQue<CServerTransport> iList;
	TSglQueIter<CServerTransport> iListIter;
	
	CServerTransport* iWaitingAccept;
	
	RArray<TUint> iTransportUsers;
	
	TInt iShutdownCount;

private:
#ifdef CPPUNIT_TEST
	friend class CTcpTransTest;
	friend class CServerTest;
#endif
	};

#endif // end of __CTCPLISTENINGPOINT_H__

// End of File
