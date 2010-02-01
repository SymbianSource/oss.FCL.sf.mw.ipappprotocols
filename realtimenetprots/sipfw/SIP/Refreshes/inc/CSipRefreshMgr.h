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
* Name          : CSipRefreshMgr.h
* Part of       : SIPRefreshes
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREFRESHMGR_H
#define CSIPREFRESHMGR_H

#include "MSipRefreshMgr.h"
#include "MConnectionStateObserver.h"
#include "_sipcodecdefs.h"

class MTransactionUser;
class MTimerManager;
class CSIPSec;
class CDeleteMgr;
class CSipRefreshBase;

class CSipRefreshMgr : public CBase,
                       public MSipRefreshMgr,
					   public MConnectionStateObserver
	{
public: // Constructors and destructor
	
    static CSipRefreshMgr* NewL (MTransactionUser& aTU,
                                 MTimerManager& aTimerMgr,
                                 CSIPSec& aSIPSec);

    ~CSipRefreshMgr ();

public: // From MSipRefreshMgr

	void RefreshL (TTransactionId& aTransactionId,
	               TRefreshId&     aRefreshId,
	               TRegistrationId aRegistrationId,
				   CSIPRequest*    aRequest,
				   MRefreshOwner*  aOwner,
				   MSIPSecUser&    aSIPSecUser,
				   CURIContainer&  aRemoteTarget,
				   TBool           aDeleteRequest,
				   TBool           aPassAllResponsesToOwner);

	MTransactionHeaders* 
	    RefreshAndGetHeadersL (TTransactionId& aTransactionId,
	                           TRefreshId&     aRefreshId,
	                           TRegistrationId aRegistrationId,
						       CSIPRequest*    aRequest,
						       MRefreshOwner*  aOwner,
						       MSIPSecUser&    aSIPSecUser,
						       CURIContainer&  aRemoteTarget);

	void UpdateRefreshL (TTransactionId& aTransactionId,
	                     const TRefreshId& aRefreshId,
						 CSIPRequest* aRequest,
						 const MRefreshOwner* aOwner,
						 TBool aDeleteRequest);

	void TerminateRefreshL (TTransactionId& aTransactionId,
	                        const TRefreshId& aRefreshId,
							CSIPRequest* aRequest,
							const MRefreshOwner* aOwner,
							TBool aDeleteRequest);

	TInt TerminateRefresh (const TRefreshId& aRefreshId,
	                       const MRefreshOwner* aOwner);

    TInt RemoveRefreshesFor (const MRefreshOwner* aOwner);

	CSIPRequest* Request (const TRefreshId& aRefreshId);

	void UpdateRemoteTargetL (const MRefreshOwner* aOwner, 
		                      CURIContainer& aRemoteTarget);

	void UpdateHeaderL (const TRefreshId& aRefreshId,
	                    const MRefreshOwner* aOwner,
						CSIPHeaderBase* aSIPHeader );

	void UpdateRouteSetsL (const MRefreshOwner* aOwner, 
				           const RPointerArray<CSIPRouteHeader>& aRouteSet);

	void RemoveRouteSets (const MRefreshOwner* aOwner);

	void SetRegistrationIdsEmpty (const MRefreshOwner* aOwner);
	
	void SetIntervalL (const TRefreshId& aRefreshId,
	                   TInt aInterval);

	TInt IntervalL (const TRefreshId& aRefreshId);

    TTransactionId CurrentTransactionId(const TRefreshId& aRefreshId) const;
        
    const MSIPSecUser* SIPSecUser(const TRefreshId& aRefreshId) const;

public: // MConnectionStateObserver

	void ConnectionStateChangedL (TUint32 aIapId,
                                  CSIPConnection::TState aState);


public:	// New functions

	TInt RemoveRefreshBy (CSipRefreshBase* aRefresh);

	TRefreshId NextRefreshId ();

private: // Second phase constructors

    CSipRefreshMgr (MTransactionUser& aTU, 
                    MTimerManager& aTimerMgr,
                    CSIPSec& aSIPSec);

    void ConstructL ();

private: // New functions

	TInt FindIndex (const TRefreshId& aRefreshId,
	                const MRefreshOwner* aOwner,
	                TBool aCheckOwner=ETrue) const;	
	
	CSipRefreshBase& FindRefreshL (const TRefreshId& aRefreshId,
	                               const MRefreshOwner* aOwner,
	                               TBool aCheckOwner=ETrue);

	void RemoveRefreshByIAPId(const TUint aIAPId);

	// Needed for cleanup of a MTransactionHeaders:
	static void DeleteTransactionHeaders (TAny* aTransactionHeaders);

private: // Data

    MTransactionUser&              iTU;
    MTimerManager&                 iTimerMgr;
    CSIPSec&                       iSIPSec;
    CDeleteMgr*				       iDeleteMgr;
	TRefreshId				       iNextRefreshId;
	RPointerArray<CSipRefreshBase> iRefreshes;

private: // For testing purposes

	UNIT_TEST(CSipRefreshMgrTest)
	UNIT_TEST(CSipRegisterRefreshTest)
    UNIT_TEST(CSipAnyRefreshTest)
    };

#endif // CSIPREFRESHMGR_H

// End of File
