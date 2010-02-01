/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipRefreshBase.h
* Part of       : SIPRefreshes
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREFRESHBASE_H
#define CSIPREFRESHBASE_H

#include <e32base.h>
#include "SipStackServerDefs.h"
#include "MTransactionOwner.h"
#include "Lwtimer.h"
#include "MSIPSecUser.h"
#include "_sipcodecdefs.h"

class MTransactionUser;
class CSipRefreshMgr;
class MRefreshOwner;
class CSIPRequest;
class CSIPResponse;
class CURIContainer;
class CSIPHeaderBase;
class CSIPRouteHeader;
class MTransactionHeaders;
class CSIPSec;


class CSipRefreshBase : public CBase,
                        public MTransactionOwner,
                        public MExpirationHandler,
                        public MSIPSecUser
    {
public:

    virtual ~CSipRefreshBase ();

    void ClearTransactionOwner ();

    MRefreshOwner* Owner();

    const TRefreshId& Id () const;
    
    TTransactionId CurrentTransactionId() const;

    CSIPRequest* Request();
    
    void SetIntervalL(TInt aInterval);
    
    TInt Interval() const;
    
    void SetRequest(CSIPRequest* aRequest, TBool aDeleteRequest);

    void SendL (TTransactionId& aTransactionId, 
                CSIPRequest* aRequest);

    MTransactionHeaders* SendAndGetHeadersL (TTransactionId& aTransactionId, 
                                             CSIPRequest*    aRequest);

    void UpdateRemoteTargetL(CURIContainer& aRemoteTarget);

    void SetUpdatedHeader(CSIPHeaderBase* aHeader);

    void SetUpdatedRouteHeadersL(const RPointerArray<CSIPRouteHeader>& 
                                 aHeaderArray);

    void RemoveRouteHeaders();

    void UpdateRequestL (TTransactionId& aTransactionId,
                         CSIPRequest* aRequest);

    void TerminateRequestL (TTransactionId& aTransactionId,
                            CSIPRequest* aRequest);

    TUint IAPId () const;

    void ResetRegistrationId();

    void UpdateCSeqValueL(CSIPRequest& aRequest);

	void StopTimer();

public: // From MTransactionOwner

    void ReceiveL(TUint32        aIapId, 
                  TTransactionId aTransactionId, 
                  CSIPRequest*   aRequest);

    void ReceiveL(TTransactionId aTransactionId,
                  CSIPResponse* aResponse);

    TInt TransactionEnded(TUint32 aIapId, 
                          TTransactionId aTransactionId, 
                          TInt aReason);

    TInt NextCSeq(TUint& aCSeq);
    
    virtual const MSIPSecUser* SIPSecUser() const;

public: // From MExpirationHandler

    void TimerExpiredL (TTimerId aTimerId, TAny* aTimerParam);
    
public: // From MSIPSecUser

	TBool PassOnlyRealmsToUser() const;
	
    void RequestCredentialsL(const TDesC8& aRealm);
	
    void RequestCredentialsL(CSIPResponse& aResponse,
	                         TTransactionId aTransactionId,
	                         TRefreshId aRefreshId);    

    const MSIPSecUser* TrustedUser(TRegistrationId aRegistrationId);

    TBool ByPassSIPSec() const;

protected:

    CSipRefreshBase (MTransactionUser& aTU,
                     MTimerManager&    aTimerMgr,
                     CSIPSec&          aSIPSec,
                     CSipRefreshMgr&   aRefreshMgr,
                     TRegistrationId   aRegistrationId,
                     MRefreshOwner*    aOwner,
                     MSIPSecUser&      aSIPSecUser);

    void BaseConstructL(CURIContainer& aRemoteTarget,
                        CSIPRequest& aRequest);

    virtual void Forward2xxResponseToCallerL(CSIPResponse* aResponse) = 0;

    

    virtual void SetExpirationTimeToRequestL(CSIPRequest& aRequest,
                                             TUint aExpirationTimeInSeconds) = 0;

    virtual void SetExpirationTimeToResponseL(CSIPResponse& 
                                              aResponse,
                                              TUint 
                                              aExpirationTimeInSeconds) = 0;

    virtual void CheckUpdateRequestExpiresValueL(CSIPRequest& aRequest) = 0;

    virtual void CheckTerminateRequestExpiresValueL(CSIPRequest& aRequest) = 0;

    virtual TInt ExpirationTimeFromResponse(CSIPResponse& aResponse) = 0;

    virtual TInt ExpirationTimeFromRequestL(CSIPRequest& aRequest) = 0;
    
    void Received200ClassResponseL(CSIPResponse* aResponse);
    void ReceivedProvisonalResponseL(CSIPResponse* aResponse);
    void ReceivedErrorResponseL(CSIPResponse* aResponse);

    TInt ExpireTimeFromResponseExpiresHeader(CSIPResponse& aResponse);
    void SetExpireTimeToResponseExpiresHeaderL(CSIPResponse& 
                                               aResponse,
                                               TUint 
                                               aExpirationTimeInSeconds);

    void UpdateHeaderForL(CSIPRequest& aRequest);

    void UpdateRouteHeadersForL(CSIPRequest& aRequest);

    void RemoveRouteHeadersFor(CSIPRequest& aRequest);

    void SetToFromOriginalRequestL(CSIPRequest& aRequest);

    void SetFromFromOriginalRequestL(CSIPRequest& aRequest);

    void SetCallIdFromOriginalRequestL(CSIPRequest& aRequest);

    void SetCSeqFromOriginalRequestL(CSIPRequest& aRequest);

    void RefreshReceivedL(TTransactionId aTransactionId, 
                          CSIPResponse*  aResponse);

    void CheckUpdateRequestMethodL(CSIPRequest& aRequest);

    void CheckUpdateRequestHeadersL(CSIPRequest& aRequest);

private:

    void SendUpdateRequestL (TTransactionId& aTransactionId,
                             CSIPRequest*    aRequest);

    void StartTimerL(TInt aMilliSeconds);
    
    TInt CalculateInterval(TInt aValueInSeconds) const;

    TInt RefreshErrorOccured(TTransactionId aTransactionId, TInt aError);

protected: // Data

    CSipRefreshMgr&               iRefreshMgr;
    TRefreshId                    iRefreshId;
    CSIPRequest*                  iRequest;
    CURIContainer*                iRemoteTarget;    
    TTransactionId                iCurrentTransactionId;
    TBool                         iFirstRequest;
    MSIPSecUser&                  iSIPSecUser;
    MRefreshOwner*                iOwner;
    
private: // Data

    MTransactionUser&             iTU;
    MTimerManager&                iTimerMgr;
    CSIPSec&                      iSIPSec;
    TBool                         iTransactionOwnerCleared;
    TInt                          iIntervalInMilliSeconds;
    TBool                         iIntervalUpdated;   
    TRegistrationId               iRegistrationId;
    TTimerId                      iTimerId;
    CSIPHeaderBase*               iUpdatedHeader;
    RPointerArray<CSIPHeaderBase> iUpdatedRouteHeaders;
    TBool                         iOwnRequest;
    TBool                         iRemoveRouteHeaders;
    
private: // For testing purposes

    UNIT_TEST(CSipRefreshMgrTest)
    UNIT_TEST(CSipRegisterRefreshTest)
    };

#endif // CSIPREFRESHBASE_H

// End of File
