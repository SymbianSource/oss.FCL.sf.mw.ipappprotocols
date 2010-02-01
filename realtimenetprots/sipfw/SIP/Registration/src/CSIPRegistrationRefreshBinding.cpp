// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSIPRegistrationRefreshBinding.cpp
// Part of       : Registration
// Version       : SIP/5.0 
//




#include "CSIPRegistrationRefreshBinding.h"
#include "CSIPRegistrar.h"
#include "CSIPRegistrationBindingStore.h"
#include "CSIPResponseUtility.h"
#include "MRegistrationOwner.h"
#include "csipregistered.h"
#include "csipregistering.h"
#include "sipresponse.h"
#include "uricontainer.h"
#include "MSipRefreshMgr.h"
#include "SipLogs.h" 
#include "SipAssert.h"
#include "sipcontactheader.h"


// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegistrationRefreshBinding* CSIPRegistrationRefreshBinding::NewLC(
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
	TBool						  aCacheOutboundProxyIP)
	{
    CSIPRegistrationRefreshBinding* self =
		new (ELeave) CSIPRegistrationRefreshBinding(aTransactionUser,
		                                            aSigComp,
		                                            aSIPSec,
		                                            aBindingStore, 
											        aRegistrationOwner,
											        aSIPSecUser,
											        aTransportMgr,
													aRefreshMgr,
													aSendWithExpires,
													aCacheOutboundProxyIP);
	CleanupStack::PushL(self);
	self->ConstructL(aSIPRequest, aRemoteTarget);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::CSIPRegistrationRefreshBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationRefreshBinding::CSIPRegistrationRefreshBinding(
    MTransactionUser&             aTransactionUser,
	MSigComp&                     aSigComp,
	CSIPSec&                      aSIPSec,
	CSIPRegistrationBindingStore& aBindingStore,
	MRegistrationOwner*           aRegistrationOwner,
	MSIPSecUser&                  aSIPSecUser,
	MSIPTransportMgr&             aTransportMgr,
	MSipRefreshMgr&               aRefreshMgr,
	TBool						  aSendWithExpires,
	TBool						  aCacheOutboundProxyIP) 
	: CSIPRegistrationBindingBase (aTransactionUser, aSigComp, aSIPSec, 
	                               aBindingStore, aRegistrationOwner, 
	                               aSIPSecUser, aTransportMgr,aSendWithExpires,
	                               aCacheOutboundProxyIP),
    iRefreshMgr(aRefreshMgr),
    iIsSubsequentResponse(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::~CSIPRegistrationRefreshBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationRefreshBinding::~CSIPRegistrationRefreshBinding()
	{
	iRefreshMgr.RemoveRefreshesFor (this);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::RequestCredentialsL
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::RequestCredentialsL(
    CSIPResponse& aResponse,
    TTransactionId aTransactionId,
    TRefreshId /*aRefreshId*/)
    {
    iSIPSecUser.RequestCredentialsL(aResponse,aTransactionId,iRefreshId);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::RefreshId
// -----------------------------------------------------------------------------
//	
TRefreshId CSIPRegistrationRefreshBinding::RefreshId() const
    {
    return iRefreshId;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::IsRefresh
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationRefreshBinding::IsRefresh() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::RefreshReceivedL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::RefreshReceivedL (
	TTransactionId aTransactionId, 
	TRefreshId     aRefreshId,
	CSIPResponse*  aSIPResponse)
	{
	__SIP_ASSERT_LEAVE(aSIPResponse, KErrArgument);
	__SIP_ASSERT_LEAVE(aTransactionId != KEmptyTransactionId, KErrArgument);  
	__SIP_ASSERT_LEAVE(aRefreshId != KEmptyRefreshId, KErrArgument);  
	__SIP_ASSERT_LEAVE(aSIPResponse->To(), KErrArgument);
	
    __SIP_MESSAGE_LOG("Registration::RefreshReceivedL", *aSIPResponse)

	if (iCurrentState == iRegistered)
		{
		// update the transaction id for subsequent received response.
		iTransactionId = aTransactionId;
		}
	else
		{
		// when unregister, updating or subsequent response received, 
		// transaction id must be same.
		__SIP_ASSERT_LEAVE(aTransactionId == iTransactionId || 
						   aTransactionId == iRetryTransactionId, KErrArgument);
		}

	__TEST_INVARIANT;

	iCurrentState->ReceiveL(iTransactionId, aSIPResponse); 
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::RefreshError
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationRefreshBinding::RefreshError (
	TRefreshId     /*aRefreshId*/,
	TTransactionId aTransactionId,
	TInt           aReason)
	{
	__TEST_INVARIANT;
 
	if (iCurrentState == iRegistered)
		{
		// update the transaction id for subsequent received response.
		iTransactionId = aTransactionId;
		}
	else
		{
		// when unregister, updating or subsequent response received, 
		// transaction id must be same.
		__SIP_ASSERT_RETURN_VALUE(aTransactionId==iTransactionId,KErrArgument);
		}

	TInt res = KErrNone;
	if (aReason != KErrNone)
		{
	    res = RemoveBinding ();
		}
	if(res != KErrNone)
		{
		return res;
		}

	return iCurrentState->TransactionEnded(aTransactionId, aReason);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::NextRefreshCSeq
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationRefreshBinding::NextRefreshCSeq (TUint& aCSeq)
	{
	if (iRegistrar)
		{
		iRegistrar->IncreaseCSeqNumber();
		aCSeq = iRegistrar->CSeqNumber();
		}
	else
		{
		aCSeq++;
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::TransportParams
// From MRefreshOwner
// -----------------------------------------------------------------------------
//	
TSIPTransportParams CSIPRegistrationRefreshBinding::TransportParams(
    const TRegistrationId& /*aRegistrationId*/)
    {
    return FillTransportParams();  
    }
    
// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::SIPSecUser
// From MRefreshOwner
// -----------------------------------------------------------------------------
//	
const MSIPSecUser* CSIPRegistrationRefreshBinding::SIPSecUser(
    const TRegistrationId& /*aRegistrationId*/)
    {
    // Don't return a this-pointer. 
    // It could cause cyclic dependecies elsewhere in the code.
    return NULL; 
    }    

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::RefreshDurationL
// -----------------------------------------------------------------------------
//
TUint CSIPRegistrationRefreshBinding::RefreshDurationL()
	{
	return Contact().ExpiresParameter();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::DoRegisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::DoRegisterL(
	TTransactionId&  aTransactionId, 
	CSIPRequest*     aRequest,
	CSIPRouteHeader* aOutboundProxy)
	{
	if (aOutboundProxy)
		{
		SetOutboundProxyL(aOutboundProxy); 
		// If SendL fails the aOutboundProxy is detached from binding
		CleanupStack::PushL(TCleanupItem(DetachOutboundProxy, this));
		}
			
	if (iRegistrar)
		{
		// send refresh registration request, 
		// the ownership of request is not passed
		iRefreshMgr.RefreshL(aTransactionId,iRefreshId,iRegistrationId,
		                     aRequest,this,*this, 
		                     const_cast<CURIContainer&>(iRegistrar->Registrar()),
		                     EFalse,ETrue);
		}
	else
		{
		// the ownership of the request is not passed.
		iRefreshMgr.RefreshL(aTransactionId,iRefreshId,iRegistrationId,aRequest, 
		                     this,*this,*iRemoteTarget,EFalse,ETrue);
		}

	if (aOutboundProxy)
		{
		CleanupStack::Pop(); // TCleanupItem
		}
	
	
	if ( iCurrentState->Retrying() )
		{
		iRetryTransactionId = aTransactionId;
		}
		
	else 
		{
		iTransactionId = aTransactionId;
		}
		
	
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::DoUpdateL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::DoUpdateL(
	TTransactionId& aTransactionId, CSIPRequest* aRequest)
	{
	// send update refresh registration request, 
	// the ownership of request is not passed.
	iRefreshMgr.UpdateRefreshL(iTransactionId, iRefreshId, aRequest, 
	                           this, EFalse);

	aTransactionId = iTransactionId;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::DoUnregisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::DoUnregisterL(
	TTransactionId& aTransactionId, CSIPRequest* aRequest)
	{
	// the ownership of the request is not passed
	iRefreshMgr.TerminateRefreshL(iTransactionId, iRefreshId, aRequest, 
	                              this, EFalse);

	aTransactionId = iTransactionId;
	}
	
// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::SetOutboundProxyL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::SetOutboundProxyL(
	CSIPRouteHeader* aOutboundProxy)
	{
	__SIP_ASSERT_LEAVE (aOutboundProxy, KErrArgument);

    if (iRouteSet.Count() > 0)
        {
        iRefreshMgr.UpdateRouteSetsL(this,iRouteSet);
        }
    else
        {
        iRefreshMgr.RemoveRouteSets(this);    
        }
    CreateProxyTransportResourcesL(*aOutboundProxy);
    SetOutboundProxy(aOutboundProxy);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::ClearRefreshOwner
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::ClearRefreshOwner ()
	{
	iRefreshMgr.TerminateRefresh(iRefreshId,this);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::SetRefreshId
// -----------------------------------------------------------------------------
//
void CSIPRegistrationRefreshBinding::SetRefreshId(TRefreshId aRefreshId)
	{
	iRefreshId = aRefreshId;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationRefreshBinding::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPRegistrationRefreshBinding::__DbgTestInvariant() const
	{
	if (!iOwner)
		{	
		User::Invariant();
		}
	}

