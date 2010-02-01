/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPRegistrationRefreshBinding.h
* Part of       : Registration
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONREFRESHBINDING_H
#define CSIPREGISTRATIONREFRESHBINDING_H

#include <e32base.h>
#include "CSIPRegistrationBindingBase.h"
#include "MRefreshOwner.h"

class MSipRefreshMgr;

/**
 * Class contains registration refreshing binding for  information.
 */
class CSIPRegistrationRefreshBinding : public CSIPRegistrationBindingBase,
								       public MRefreshOwner
	{
public: // Constructors and destructor

	static CSIPRegistrationRefreshBinding* NewLC(
		CSIPRequest*                  aSIPRequest,
		MTransactionUser&             aTransactionUser,
		MSigComp&                     aSigComp,
		CSIPSec&                      aSIPSec,
		MSipRefreshMgr&               aRefreshMgr,
		CSIPRegistrationBindingStore& aBindingStore,
		MRegistrationOwner*           aRegistrationOwner,
		MSIPSecUser&                  aSIPSecUser,
		MSIPTransportMgr&             aTransportMgr,
		CURIContainer&                aRemoteTarget,
		TBool 						  aSendWithExpires,
		TBool						  aCacheOutboundProxyIP);

	~CSIPRegistrationRefreshBinding();


public: // From MSIPSecUser
	
    void RequestCredentialsL(CSIPResponse& aResponse,
	                         TTransactionId aTransactionId,
	                         TRefreshId aRefreshId);

public: // From CSIPRegistrationBindingBase

    TRefreshId RefreshId() const;

	TBool IsRefresh() const;
	
	void DoRegisterL(TTransactionId&  aTransactionId, 
		             CSIPRequest*     aRequest,
				     CSIPRouteHeader* aOutboundProxy);

	void DoUpdateL(TTransactionId& aTransactionId, 
		           CSIPRequest*    aRequest);

	void DoUnregisterL(TTransactionId& aTransactionId, 
		               CSIPRequest*    aRequest);
	               
    void SetOutboundProxyL(CSIPRouteHeader* aOutboundProxy);
    
    
public: // From MRefreshOwner:     	
	
	void RefreshReceivedL (TTransactionId aTransactionId, 
		                   TRefreshId     aRefreshId,
	                       CSIPResponse*  aSIPResponse);

	TInt RefreshError (TRefreshId     aRefreshId, 
		               TTransactionId aTransactionId, 
					   TInt           aReason);

	TInt NextRefreshCSeq (TUint& aCSeq);
	
	TSIPTransportParams TransportParams(const TRegistrationId& aRegistrationId);

    const MSIPSecUser* SIPSecUser(const TRegistrationId& aRegistrationId);
	
    TUint RefreshDurationL();
    
public: // New functions	

	void SetRefreshId(TRefreshId aRefreshId);
	
	void ClearRefreshOwner ();

 
private:
      
	CSIPRegistrationRefreshBinding(
	    MTransactionUser&             aTransactionUser,
		MSigComp&                     aSigComp,
		CSIPSec&                      aSIPSec,
		CSIPRegistrationBindingStore& aBindingStore,
		MRegistrationOwner*           aRegistrationOwner,
		MSIPSecUser&                  aSIPSecUser,
		MSIPTransportMgr&             aTransportMgr,
		MSipRefreshMgr&               aRefreshMgr,
		TBool 						  aSendWithExpires,
		TBool						  aCacheOutboundProxyIP);

private: // Data

	MSipRefreshMgr& iRefreshMgr;
	TRefreshId iRefreshId;
	TBool iIsSubsequentResponse;

private: // For testing purposes

	void __DbgTestInvariant() const;

    
	UNIT_TEST(CSIPRegistrationBindingTest)
	};

#endif // CSIPREGISTRATIONREFRESHBINDING_H

// End of File
