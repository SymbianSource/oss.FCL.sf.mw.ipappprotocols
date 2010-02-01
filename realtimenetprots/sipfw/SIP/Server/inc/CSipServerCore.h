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
* Name          : CSipServerCore.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSERVERCORE_H
#define CSIPSERVERCORE_H

#include <e32base.h>
#include "MConnectionStateObserver.h"
#include "_sipcodecdefs.h"

class CSipCSServer;
class CSipCSServerITC;
class CSipCSServerCloseTimer;
class CTimerManager;
class CTransactionUser;
class CSipDialogMgr;
class MSipRegistrations;
class CSIPRegistrationMgr;
class MSipRefreshMgr;
class CSipRefreshMgr;
class CSIPSec;
class CSIPRequestHandlerBase;
class MSigComp;
class CSipConnectionMgr;
class MTransactionOwner;
class CSIPRoutingRequestStore;


class CSipServerCore : public CBase,
                       public MConnectionStateObserver
	{
public: // Constructors and destructor 
    
    static CSipServerCore* NewL();
    static CSipServerCore* NewLC();
    ~CSipServerCore ();

public: // From MConnectionStateObserver:

	void ConnectionStateChangedL(TUint32 aIapId, 
                                 CSIPConnection::TState aState);

public: // New functions

	void IncrementSessions();
	void DecrementSessions();
	TUint32 CreateTransportOwnerId();

	CServer2& Server();
	CObjectConIx& ObjectConIx();
    CSipCSServerITC& ITC();

    CTransactionUser& TU();
    CSipDialogMgr& DialogMgr();
    CSIPRegistrationMgr& RegistrationMgr();
    MSipRegistrations& Registrations();
    MSipRefreshMgr& RefreshMgr();
    CSIPSec& SIPSec();
    CSIPRequestHandlerBase& RequestHandler();
    MSigComp& SigComp();
    CSipConnectionMgr& ConnectionMgr();
	MTransactionOwner* FindOwner(TUint32 aIapId, const TUid& aUid);

	/**
	* Refresh the connection on the IAP ID.
	*@param aIapId IAP ID on which the connection has to be refreshed.
	*/
	void RefreshConnectionL(TUint32 aIapId);

	/**
	* Gets the connection error on the IAP ID.
	* @param aIapId IAP ID for which the connection error has to be retrieved.
	* @param aError will be filled with the error code.
	*/
	void GetConnectionErrorL(TUint32 aIapId,TInt& aError);


private: // New functions

	void LoadRequestHandlerL();

private: // Second phase constructors

    CSipServerCore ();
    void ConstructL ();

private: // Data:

	CSipCSServer*             iServer;			      
    CActiveScheduler*         iActiveScheduler;
    CSipCSServerITC*          iITC;
    CObjectConIx*             iContainerIndex;
	CSipCSServerCloseTimer*   iCloseTimer;  
    TUint                     iSessionCount;
    CTimerManager*            iTimerMgr;
    CTransactionUser*         iTU;
    CSipDialogMgr*            iDialogMgr;
    CSIPRegistrationMgr*      iRegistrationMgr;
    CSipRefreshMgr*           iRefreshMgr;
    CSIPRequestHandlerBase*   iRequestHandler;
    CSIPRoutingRequestStore*  iRoutingRequestStore;
    TUint32                   iTransportOwnerIdCounter;
	TBool					  iFeatMgrInitialized;
    
private: // For testing purposes

	UNIT_TEST(CSipCSSessionTest)
	};

#endif // CSIPSERVERCORE_H

// End of File
