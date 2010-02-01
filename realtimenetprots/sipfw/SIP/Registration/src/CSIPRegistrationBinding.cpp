// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPRegistrationBinding.cpp
// Part of       : Registration
// Version       : SIP/4.0 
//




#include "CSIPRegistrationBinding.h"
#include "CSIPRegistrar.h"
#include "CSIPRegistrationBindingStore.h"
#include "CSIPResponseUtility.h"
#include "MRegistrationOwner.h"
#include "csipregistrationstate.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "siptoheader.h"
#include "sipaddress.h"
#include "sipminexpiresheader.h"
#include "sipcontactheader.h"
#include "uricontainer.h"
#include "SipLogs.h" 
#include "SipAssert.h"


// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegistrationBinding* CSIPRegistrationBinding::NewLC(
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
	TBool						  aSendWithExpires,
	TBool						  aCacheOutboundProxyIP)
	{
	CSIPRegistrationBinding* self = new (ELeave) CSIPRegistrationBinding(
		aSigComp, aSIPSec, aTimerMgr, aTransactionUser, aBindingStore,
		aRegistrationOwner, aSIPSecUser, aTransportMgr,aSendWithExpires,
		aCacheOutboundProxyIP);
	CleanupStack::PushL(self);
	self->ConstructL(aSIPRequest, aRemoteTarget);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::CSIPRegistrationBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationBinding::CSIPRegistrationBinding(
	MSigComp&                     aSigComp,
	CSIPSec&                      aSIPSec,
	MTimerManager&                aTimerManager, 
	MTransactionUser&             aTransactionUser,
	CSIPRegistrationBindingStore& aBindingStore,
	MRegistrationOwner*           aRegistrationOwner,
	MSIPSecUser&                  aSIPSecUser,
	MSIPTransportMgr&             aTransportMgr,
	TBool						  aSendWithExpires,
	TBool						  aCacheOutboundProxyIP)
  : CSIPRegistrationBindingBase (aTransactionUser, aSigComp, aSIPSec, 
                                 aBindingStore, aRegistrationOwner, 
                                 aSIPSecUser, aTransportMgr,aSendWithExpires,
                                 aCacheOutboundProxyIP),
    iTimerMgr         (aTimerManager)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::~CSIPRegistrationBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationBinding::~CSIPRegistrationBinding() 
	{
	iTransactionUser.ClearTransactionOwner(this);
	iTimerMgr.Stop(iTimerId);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::IsRefresh
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBinding::IsRefresh() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::DoRegisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::DoRegisterL(TTransactionId&  aTransactionId, 
		                                  CSIPRequest*     aSIPRequest,
						                  CSIPRouteHeader* aOutboundProxy)
	{
	if (aOutboundProxy)
		{
		SetOutboundProxyL(aOutboundProxy); 
		// If SendL fails the aOutboundProxy is detached from binding
		CleanupStack::PushL(TCleanupItem(DetachOutboundProxy, this));
		}

    TSIPTransportParams transportParams(FillTransportParams());
    
	if (iRegistrar)
		{
		iTransactionUser.SendL(aTransactionId, iRegistrationId, aSIPRequest, 
			this, const_cast<CURIContainer&>(iRegistrar->Registrar()), 
			transportParams, EFalse);
		}
	else
		{
		iTransactionUser.SendL(aTransactionId, iRegistrationId, aSIPRequest,   
			this, *iRemoteTarget, transportParams, EFalse);
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
// CSIPRegistrationBinding::DoUpdateL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::DoUpdateL(TTransactionId& aTransactionId, 
		                                CSIPRequest*    aSIPRequest)
	{
    TSIPTransportParams transportParams(FillTransportParams());	
	
	iTransactionUser.SendL(iTransactionId, iRegistrationId, 
		aSIPRequest, this, const_cast<CURIContainer&>(iRegistrar->Registrar()), 
		transportParams, EFalse);

	aTransactionId = iTransactionId;
	iTimerMgr.Stop(iTimerId);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::DoUnregisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::DoUnregisterL(TTransactionId& aTransactionId, 
		                                    CSIPRequest*    aSIPRequest)
	{
    TSIPTransportParams transportParams(FillTransportParams());	
	
	iTransactionUser.SendL(iTransactionId, iRegistrationId, 
		aSIPRequest, this, const_cast<CURIContainer&>(iRegistrar->Registrar()), 
		transportParams, EFalse);

	aTransactionId = iTransactionId;
	iTimerMgr.Stop(iTimerId);
	}


// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ReceiveL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::ReceiveL(TUint32        /*aIapId*/,
									   TTransactionId /*aTransactionId*/, 
									   CSIPRequest*   /*aSIPRequest*/)
	{
	// this function call is not supported
	User::Leave (KErrNotSupported);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ReceiveL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::ReceiveL(TTransactionId aTransactionId, 
									   CSIPResponse*  aSIPResponse)
	{
	__SIP_ASSERT_LEAVE(aSIPResponse, KErrArgument);
    __SIP_ASSERT_LEAVE(aSIPResponse->To(), KErrArgument);
	__SIP_ASSERT_LEAVE(aTransactionId != KEmptyTransactionId, KErrArgument);

	// panic if transaction id is not equal to the one has sent
	__SIP_ASSERT_LEAVE(aTransactionId == iTransactionId || 
					   aTransactionId == iRetryTransactionId, KErrArgument);

    __SIP_MESSAGE_LOG("Registrations: ReceiveL", *aSIPResponse)

	__TEST_INVARIANT;

	iCurrentState->ReceiveL(iTransactionId, aSIPResponse);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBinding::TransactionEnded(TUint32        /*aIapId*/,
                                               TTransactionId aTransactionId, 
											   TInt           aReason)
	{
	__SIP_ASSERT_RETURN_VALUE (aTransactionId != KEmptyTransactionId, 
		                       KErrArgument);
	__SIP_ASSERT_RETURN_VALUE (aTransactionId == iTransactionId, 
		                       KErrArgument);

	__TEST_INVARIANT;

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
// CSIPRegistrationBinding::NextCSeq
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBinding::NextCSeq(TUint& aCSeq)
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
// CSIPRegistrationBinding::SIPSecUser
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//    
const MSIPSecUser* CSIPRegistrationBinding::SIPSecUser() const
    {
    return this;
    } 	

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::TimerExpiredL(TTimerId aTimerId, 
											TAny*    /*aTimerParam*/)
	{	
	__SIP_ASSERT_LEAVE(iTimerId == aTimerId, KErrArgument);

	// if register is in registered or unregistered states, 
	// binding can be removed. 
	// otherwise, the registration is pending, do not delete the binding.
	if (!RegisterPending()) 
		{
		ClearTransactionOwner();
		User::LeaveIfError(RemoveBinding());
		}
	}


// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::StartTimerL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::StartTimerL(TUint aExpiresValue)
	{
	const TUint KMilliSecondsInSecond = 1000;
	const TUint KTimerValue = (aExpiresValue * KMilliSecondsInSecond); 
	iTimerId = iTimerMgr.StartL(this,KTimerValue);
	iContact->SetExpiresParameterL(aExpiresValue);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::ClearTransactionOwner
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::ClearTransactionOwner ( TBool aUseRetryId )
	{
	TTransactionId id( iTransactionId );

	if ( aUseRetryId )
		{
		// If retry-register is sent, we should clear retry transaction
		id = iRetryTransactionId;
		}
	iTransactionUser.ClearTransactionOwner( id );
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBinding::__DbgTestInvariant
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBinding::__DbgTestInvariant() const
	{
	if (!iOwner)
		{	
		User::Invariant();
		}

	}
