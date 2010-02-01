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
* Name          : CSIPRegistrationBindingBase.h
* Part of       : Registration
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONBINDINGBASE_H
#define CSIPREGISTRATIONBINDINGBASE_H

#include <e32base.h>
#include "MTransactionUser.h"
#include "MSIPSecUser.h"
#include "SipStackServerDefs.h"
#include "sipnatbindingobserver.h"
#include "mregistrationcontext.h"
#include "TSIPTransportParams.h"
#include "_sipcodecdefs.h"

class CSIPRegistrar;
class CSIPRegistrationBindingStore;
class MRegistrationOwner;
class MSigComp;
class CSIPSec;
class MSIPTransportMgr;
class CURIContainer;
class CSIPURI;
class CSIPResponse;
class CSIPRequest;
class CSIPToHeader;
class CSIPFromToHeaderBase;
class CSIPFromHeader;
class CSIPContactHeader;
class CSIPRouteHeader;
class CSIPMinExpiresHeader;
class CSIPRegistrationState;
class CSIPRegistering;
class CSIPRegistered;
class CSIP423ResponseReceived;
class CSIPUnregistering;
class CSIPUnregistered;
class CSIPRegisterUpdating;
class CSIPPAssociatedURIHeader;


/**
 * base class of registration binding.
 */
class CSIPRegistrationBindingBase : public CBase,
									public MSIPSecUser,
									public MSIPNATBindingObserver,
									public MRegistrationContext
	{
public:

	virtual ~CSIPRegistrationBindingBase();


public: // From MSIPSecUser

	TBool PassOnlyRealmsToUser() const;
	
    void RequestCredentialsL(const TDesC8& aRealm);
	
    virtual void RequestCredentialsL(CSIPResponse& aResponse,
	                                 TTransactionId aTransactionId,
	                                 TRefreshId aRefreshId);
    
    const MSIPSecUser* TrustedUser(TRegistrationId aRegistrationId);

    TBool ByPassSIPSec() const;
    

public: // From MSIPNATBindingObserver

    void FlowFailure(TInt aError);


public: // From MRegistrationContext

	virtual TRefreshId RefreshId() const;

	void RegisterL(TTransactionId&  aTransactionId,
		           CSIPRequest*     aRequest,
				   CSIPRouteHeader* aOutboundProxy);
				   
	void UpdateL(TTransactionId& aTransactionId,
		         CSIPRequest*    aRequest);

	void UnregisterL(TTransactionId& aTransactionId,
		             CSIPRequest*    aRequest);

	CSIPRegistrar* Registrar();

    void FillToFromL(CSIPRequest& aRequest);
    
	void SetExpiresValueL(CSIPRequest& aRequest);

	CSIPRegistrationState& CurrentState();

	void ChangeState(CSIPRegistrationState* aState);

	const CSIPToHeader& AOR() const;
	
	void UpdateContactFromRequestL();

	CSIPContactHeader& Contact();

	CSIPRequest* Request();

	const MRegistrationOwner* Owner() const;
	
	void SelfDetach(TBool aRefresh);

	void SelfDetach(TBool aRefresh, TBool aUseRetryId);
	void Received2XXResponseL(CSIPResponse& aResponse,
			TTransactionId aTransactionId);


	void ReceivedErrorResponseL();

	void Received423ResponseL(CSIPResponse& aResponse);

	TRegistrationId RegistrationId() const;

	void ClearRequest();

	void SetRouteHeaderFromResponseL(CSIPResponse& aResponse);
	
	TBool ServiceRouteOk(CSIPResponse& aResponse) const;
	             
		             
public: // New virtual functions

	virtual TBool IsRefresh() const = 0;

	virtual void DoRegisterL(TTransactionId& aTransactionId,
		                     CSIPRequest* aSIPRequest,
						     CSIPRouteHeader* aOutboundProxy)=0;

	virtual void DoUpdateL(TTransactionId& aTransactionId,
		                   CSIPRequest* aSIPRequest)=0;

	virtual void DoUnregisterL(TTransactionId& aTransactionId,
		                       CSIPRequest* aSIPRequest)=0;
		                       
	virtual void SetOutboundProxyL(CSIPRouteHeader* aOutboundProxy);                       


public: // New functions

	void OutboundProxyFailed();

	const CSIPRouteHeader* OutboundProxy();

	TBool HasOutboundProxy() const;

	TInt RemoveOutboundProxy();
	
	TBool FindContactByFromL(const CSIPFromToHeaderBase* aFromToHeader,
		                     CSIPContactHeader& aContact);

	TBool CompareWithRegisterRequest(const MRegistrationOwner& aOwner,
								     CSIPRequest& aSIPRequest);

	RPointerArray<CSIPRouteHeader>& RouteSet();

	TUint32 IAPId() const;

    TUint32 SigCompCompartmentId() const;

	// Take ownership
	void SetRequest(CSIPRequest* aSIPRequest);

	void SetOutboundProxy(CSIPRouteHeader* aOutboundProxy);	

	TBool RegisterPending() const;

	void DetachFromStore();

    const CSIPPAssociatedURIHeader* PAssociatedURIHeader() const;

    TSIPTransportParams FillTransportParams();
    
    TBool HasTransaction(const TTransactionId& aTransactionId) const;
    
    TBool IsCacheOutboundProxyIPEnabled();
    
    CSIPRouteHeader& OutboundProxyIPL();

protected:

	void ConstructL(CSIPRequest* aSIPRequest, 
	                CURIContainer& aRemoteTarget);

	CSIPRegistrationBindingBase(MTransactionUser& aTransactionUser,
	                            MSigComp& aSigComp,
	                            CSIPSec& aSIPSec,
	                            CSIPRegistrationBindingStore& aBindingStore,
	                            MRegistrationOwner* aRegistrationOwner,
	                            MSIPSecUser& aSIPSecUser,
	                            MSIPTransportMgr& aTransportMgr,
	                            TBool aSendWithExpires,
	                            TBool aCacheOutboundProxyIP);

protected:

    void CreateProxyTransportResourcesL(CSIPRouteHeader& aOutboundProxy);
	static void DetachOutboundProxy(TAny* aBinding);
	TInt RemoveBinding();

private:

	void InitializeStatesL();
	void UpdateBindingFromResponseL(CSIPResponse& aResponse);
	void SetMinExpiresToRequestL (CSIPResponse& aResponse);
	void SetToHeaderL(CSIPRequest& aSIPRequest);
	void SetFromHeaderL(CSIPRequest& aSIPRequest);
	void SetContactHeaderL(CSIPRequest& aSIPRequest);
	void SetPAssociatedURIFromResponseL(CSIPResponse& aResponse);
	void FreeProxyTransportResources();
	TBool HasSigCompParameter(const CSIPURI& aSipUri) const;	
	TInt SetOutboundProxyIPL(const TTransactionId& aTransactionId);
	HBufC8* ConvertInetAddrToTextL(const TInetAddr& aAddr);
	
protected: // Data	

    MTransactionUser&              iTransactionUser;
	MRegistrationOwner*            iOwner;
	TRegistrationId                iRegistrationId;
	TTransactionId                 iTransactionId;
	TTransactionId                 iRetryTransactionId;
	TUint32                        iSigCompCompartmentId;
	TUint32                        iTransportId;
	CSIPRegistrar*                 iRegistrar;
	CURIContainer*                 iRemoteTarget;
    CSIPContactHeader*             iContact;
	RPointerArray<CSIPRouteHeader> iRouteSet;
	MSIPSecUser&                   iSIPSecUser;
	
	// States
	CSIPRegistrationState*         iCurrentState;
	CSIPRegistering*               iRegistering;
	CSIPRegistered*                iRegistered;
	CSIP423ResponseReceived*       iResponse423Received;
	CSIPRegisterUpdating*          iUpdating;
	CSIPUnregistering*             iUnregistering;
	CSIPUnregistered*              iUnregistered; 
	
private: // Data

	MSigComp&                      iSigComp;
	CSIPSec&                       iSIPSec;
	CSIPRegistrationBindingStore&  iBindingStore;
	MSIPTransportMgr&              iTransportMgr;
	CSIPToHeader*                  iTo;
	CSIPFromHeader*                iFrom;
	CSIPRouteHeader*               iOutboundProxy;
	CSIPPAssociatedURIHeader*      iPAssociatedURIHeader;
	CSIPRequest*                   iRequest;
	TBool						   iSendWithExpires;
	CSIPRouteHeader*			   iOutboundProxyIP;
	TBool						   iCacheOutboundProxyIP;

private: // For testing purposes

	UNIT_TEST(CSIPRegistrationMgrTest)
    UNIT_TEST(CSIPRegistrationBindingStoreTest)
	UNIT_TEST(CSIPRegistrationBindingTest)
	};

#endif // CSIPREGISTRATIONBINDINGBASE_H

// End of File
