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
* Name          : CSIPRegistrationBinding.h
* Part of       : Registration
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREGISTRATIONBINDING_H
#define CSIPREGISTRATIONBINDING_H

#include <e32base.h>

#include "CSIPRegistrationBindingBase.h"
#include "MTransactionOwner.h"
#include "Lwtimer.h"

class CURIContainer;
class MTransactionUser;
class MSigComp;

/**
 * Class contains registration binding information.
 */
class CSIPRegistrationBinding : public CSIPRegistrationBindingBase,
                                public MTransactionOwner,
                                public MExpirationHandler                       
    {
public: // Constructors and destructor

    static CSIPRegistrationBinding* NewLC(
        CSIPRequest*                  aSIPRequest,
        MSigComp&                     aSigComp,
        CSIPSec&                      aSIPSec,
        MTimerManager&                aTimerMgr,
        MTransactionUser&             aTransactionUser,
        CSIPRegistrationBindingStore& aBindingStore,
        MRegistrationOwner*           aRegistrationOwner,
        MSIPSecUser&                  aSIPSecUser,
        MSIPTransportMgr&             aTransportMgr,
        CURIContainer&                aRemoteTarget,
        TBool 						  aSendWithExpires,
        TBool						  aCacheOutboundProxyIP);

    ~CSIPRegistrationBinding();

    
public: // From CSIPRegistrationBindingBase

    TBool IsRefresh() const;
    
    void DoRegisterL(TTransactionId&  aTransactionId, 
                     CSIPRequest*     aSIPRequest,
                     CSIPRouteHeader* aOutboundProxy);

    void DoUpdateL(TTransactionId& aTransactionId, 
                   CSIPRequest*    aSIPRequest);

    void DoUnregisterL(TTransactionId& aTransactionId, 
                       CSIPRequest*    aSIPRequest);    


public: // From MTransactionOwner    
    
    void ReceiveL(TUint32 aIapId, 
                  TTransactionId aTransactionId, 
                  CSIPRequest* aSIPRequest);

    void ReceiveL(TTransactionId aTransactionId,
                  CSIPResponse* aSIPResponse);

    TInt TransactionEnded(TUint32 aIapId,
                          TTransactionId aTransactionId,
                          TInt aReason);

    TInt NextCSeq(TUint& aCSeq);
    
    const MSIPSecUser* SIPSecUser() const;


public: // From MExpirationHandler

    void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam);


public: // New functions                       
                       
    void StartTimerL(TUint aExpiresValue);
    
    void ClearTransactionOwner( TBool aUseRetryId=EFalse );

private:
      
    CSIPRegistrationBinding(MSigComp&                     aSigComp,
                            CSIPSec&                      aSIPSec,
                            MTimerManager&                aTimerManager, 
                            MTransactionUser&             aTransactionUser,
                            CSIPRegistrationBindingStore& aBindingStore,
                            MRegistrationOwner*           aRegistrationOwner,
                            MSIPSecUser&                  aSIPSecUser,
                            MSIPTransportMgr&             aTransportMgr,
                            TBool 						  aSendWithExpires,
                            TBool						  aCacheOutboundProxyIP);

private: // Data

    MTimerManager&    iTimerMgr;
    TTimerId          iTimerId;

private: // For testing purposes

	void __DbgTestInvariant() const;

    UNIT_TEST(CSIPRegistrationBindingTest)
    UNIT_TEST(CSIPRegistrationBindingStoreTest)
    };

#endif // CSIPREGISTRATIONBINDING_H

// End of File
