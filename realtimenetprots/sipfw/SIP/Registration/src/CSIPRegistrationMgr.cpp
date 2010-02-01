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
// Name          : CSIPRegistrationMgr.cpp
// Part of       : Registration
// Version       : SIP/4.0 
//




#include "CSIPRegistrationMgr.h"
#include "CSIPRegistrarStore.h"
#include "CSIPRegistrar.h"
#include "CSIPRegistrationBindingStore.h"
#include "CSIPRegistrationBinding.h"
#include "CSIPRegistrationRefreshBinding.h"
#include "TSIPRegistrationUtility.h"
#include "csipregistrationstate.h"
#include "MRegistrationOwner.h"
#include "MTransactionUser.h"
#include "MSipRefreshMgr.h"
#include "siprequest.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sippassociateduriheader.h"
#include "sipaddress.h"
#include "sipcontactheader.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "siphostport.h"
#include "siprouteheader.h"
#include "SipLogs.h"
#include "siperr.h"
#include "SipAssert.h"
#include "sipcodecerr.h"
#include "TSIPTransportParams.h"


// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::NewL
// -----------------------------------------------------------------------------
//
CSIPRegistrationMgr* CSIPRegistrationMgr::NewL(MTransactionUser& aTU, 
											   MSipRefreshMgr&   aRefreshMgr,
											   MTimerManager&    aTimerManager,
											   CSIPSec&          aSIPSec,
											   MSigComp&         aSigComp,
											   MSIPTransportMgr& aTransportMgr)
	{
	CSIPRegistrationMgr* self = 
	    new (ELeave) CSIPRegistrationMgr(aTU, aRefreshMgr, aTimerManager, 
	                                     aSIPSec, aSigComp, aTransportMgr);
    CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::CSIPRegistrationMgr
// -----------------------------------------------------------------------------
//
CSIPRegistrationMgr::CSIPRegistrationMgr(MTransactionUser& aTU, 
										 MSipRefreshMgr&   aRefreshMgr,
										 MTimerManager&    aTimerManager,
										 CSIPSec&          aSIPSec,
										 MSigComp&         aSigComp,
										 MSIPTransportMgr& aTransportMgr) 
	: iTU             (aTU), 
      iRefreshMgr     (aRefreshMgr),
      iTimerMgr       (aTimerManager),
      iSIPSec         (aSIPSec),
      iSigComp        (aSigComp),
      iTransportMgr   (aTransportMgr)
	{ 
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::ConstructL()
	{
	//ask the local address from transaction user.
	iRegistrarStore = CSIPRegistrarStore::NewL();
	iBindingStore   = CSIPRegistrationBindingStore::NewL(*iRegistrarStore);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::~CSIPRegistrationMgr
// -----------------------------------------------------------------------------
//
CSIPRegistrationMgr::~CSIPRegistrationMgr()
	{
	delete iBindingStore;
	delete iRegistrarStore;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::RegisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::RegisterL (TTransactionId& aTransactionId,
                                     TRegistrationId& aRegistrationId,
                                     CSIPRequest* aRequest,
                                     CURIContainer& aRemoteTarget,
                                     MRegistrationOwner* aRegistrationOwner,
                                     MSIPSecUser& aSIPSecUser,
                                     CSIPRouteHeader* aOutboundProxy,
                                     TBool aRefresh,
                                     TBool aSendWithExpires,
                                     TBool aCacheOutboundProxyIP)
	{
	// panic if aRequest is 0 in debug mode. leaves in release mode.
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	// panic if aRegistrationOwner is 0 in debug mode. leaves in release mode.
	__SIP_ASSERT_LEAVE(aRegistrationOwner, KErrArgument);

    __SIP_MESSAGE_LOG("Registration::RegisterL", *aRequest)

    TUint32 iapId = aRegistrationOwner->TransportParams().IapId(); 
	TSIPRegistrationUtility::CheckRegisterRequestL(
	    *aRequest,aOutboundProxy,iapId,iTU);

	if (aOutboundProxy)
		{
		__ASSERT_ALWAYS(aOutboundProxy->SIPAddress().URI().IsSIPURI(),
		                User::Leave(KErrSipCodecURIScheme));
		TSIPRegistrationUtility::CheckOutboundProxyCompParameterL(
			*aOutboundProxy, iSigComp);
		}

	// check if there are pending bindings to same AOR (registrar)
	if(iBindingStore->RegisterPendingToAOR(
		aRequest->To()->SIPAddress().URI()))
		{
		User::Leave(KErrSIPRequestPending);
		}

	// find existing binding by comparing URI of To, Form and Contact
	CSIPRegistrationBindingBase* binding =
		iBindingStore->FindBinding(*aRegistrationOwner, *aRequest); 

	// leave if binding already exists
	__ASSERT_ALWAYS (!binding, User::Leave (KErrAlreadyExists));

    if (aRemoteTarget.IsSIPURI())
        {
        aRemoteTarget.SIPURI()->DeleteUserInfo(); // user and password
        }

	if (aRefresh) // create refresh binding
		{
		binding = CSIPRegistrationRefreshBinding::NewLC(
			aRequest, iTU, iSigComp, iSIPSec, iRefreshMgr, 
			*iBindingStore, aRegistrationOwner, aSIPSecUser, 
			iTransportMgr, aRemoteTarget,aSendWithExpires,
			aCacheOutboundProxyIP);	
		}
	else // create non refresh binding
		{
		binding = CSIPRegistrationBinding::NewLC(
			aRequest, iSigComp, iSIPSec, iTimerMgr, iTU,
			*iBindingStore, aRegistrationOwner, aSIPSecUser,
			iTransportMgr, aRemoteTarget,aSendWithExpires,
			aCacheOutboundProxyIP);	
		}	
	TCleanupItem cleanupItem (DetachBinding, binding);
	iBindingStore->AddBindingL(binding);
	// if register fails, cleanupItem detach the binding from binding store.
	CleanupStack::PushL(cleanupItem);
	// send request
	binding->CurrentState().RegisterL(aTransactionId, aRequest, aOutboundProxy);
	CleanupStack::Pop(); // cleanupItem
	CleanupStack::Pop(binding);
	aRegistrationId = binding->RegistrationId();
	binding->SetRequest(aRequest); // take ownership of request	
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::UpdateRegistrationL (
    const TRegistrationId& aRegistrationId,
    TTransactionId& aTransactionId,
    CSIPRequest* aRequest,
    const MRegistrationOwner* aOwner)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

    __SIP_MESSAGE_LOG("Registration::UpdateRegistrationL", *aRequest)

	CSIPRegistrationBindingBase* binding =
		iBindingStore->FindBinding(aRegistrationId,aOwner); 

	// leave if binding doesn't exist
	__ASSERT_ALWAYS (binding, User::Leave(KErrSIPInvalidRegistrationState));

	TSIPRegistrationUtility::CheckUpdateRegisterRequestL(*aRequest, *binding);

	// if UpdateL fails the binding must be detached from binding store
	// and deleted. the cleanupItem detaches binding from binding store.
	TCleanupItem cleanupItem(DetachBinding, binding);
	CleanupStack::PushL(binding);
	CleanupStack::PushL(cleanupItem);
	// send request
	binding->CurrentState().UpdateL(aTransactionId, aRequest);
	CleanupStack::Pop(); // cleanupItem
	CleanupStack::Pop(binding);
	binding->SetRequest(aRequest); // take ownership of aRequest
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::UnRegisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::UnRegisterL (const TRegistrationId& aRegistrationId,
                                       TTransactionId& aTransactionId,
                                       CSIPRequest* aRequest,
                                       const MRegistrationOwner* aOwner)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	
    __SIP_MESSAGE_LOG("Registration::UnRegisterL", *aRequest)
		
	CSIPRegistrationBindingBase* binding =
		iBindingStore->FindBinding(aRegistrationId,aOwner); 

	// leave if binding doesn't exist
	__ASSERT_ALWAYS (binding, User::Leave(KErrSIPInvalidRegistrationState));

	TSIPRegistrationUtility::CheckUnregisterRequestL(*aRequest, *binding);

	// if UnregisterL fails the binding must be detached from binding store
	// and deleted. the cleanupItem detaches binding from binding store.
	TCleanupItem cleanupItem(DetachBinding, binding);
	CleanupStack::PushL(binding);
	CleanupStack::PushL(cleanupItem);
	// send request
	binding->CurrentState().UnregisterL(aTransactionId, aRequest);
	CleanupStack::Pop(); // cleanupItem
	CleanupStack::Pop(binding);

	binding->SetRequest(aRequest); // take ownership of aRequest
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::SetOutboundProxyL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::SetOutboundProxyL (
    const TRegistrationId& aRegistrationId,
    CSIPRouteHeader* aOutboundProxy,
    const MRegistrationOwner* aOwner)
    {
	__SIP_ASSERT_LEAVE(aOutboundProxy, KErrArgument);
	
	__ASSERT_ALWAYS(aOutboundProxy->SIPAddress().URI().IsSIPURI(),
		            User::Leave(KErrSipCodecURIScheme));	
	
	iBindingStore->SetOutboundProxyL(aRegistrationId,aOutboundProxy,aOwner);  
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::RemoveOutboundProxy
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationMgr::RemoveOutboundProxy (
	const TRegistrationId& aRegistrationId,
	const MRegistrationOwner* aOwner)
	{
	return (iBindingStore->RemoveOutboundProxy(aRegistrationId,aOwner));
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::FetchRegistrationsL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::FetchRegistrationsL (
	const TSIPTransportParams& aTransportParams,
	TTransactionId& aTransactionId,
	CSIPRequest* aRequest,
	CURIContainer& aRemoteTarget,
	MTransactionOwner* aTransactionOwner)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	__SIP_ASSERT_LEAVE(aTransactionOwner, KErrArgument);
	
    __SIP_MESSAGE_LOG("Registration::FetchRegistrationsL", *aRequest)

	TSIPRegistrationUtility::CheckFetchingRequestL(*aRequest);

    if (aRemoteTarget.IsSIPURI())
        {
        aRemoteTarget.SIPURI()->DeleteUserInfo(); // user and password
        }

	iTU.SendL(aTransactionId, KEmptyRegistrationId, aRequest, 
		      aTransactionOwner, aRemoteTarget, aTransportParams, ETrue);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::GetRouteL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::GetRouteL (
	const TRegistrationId&          aRegistrationId,
    RPointerArray<CSIPRouteHeader>& aRouteSet,
	TBool                           aAddOutboundProxy)
	{
	CSIPRegistrationBindingBase* binding = 
		iBindingStore->FindBinding(aRegistrationId);

	if (binding)
		{
		const CSIPRouteHeader* proxy = binding->OutboundProxy();
		if (proxy && aAddOutboundProxy)
			{
            aRouteSet.AppendL(proxy);
			}

		for (TInt i = 0; i < binding->RouteSet().Count(); i++)
			{
			CSIPRouteHeader* routeHeader = (binding->RouteSet())[i];
			aRouteSet.AppendL(routeHeader);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::CreateFromLC
// -----------------------------------------------------------------------------
//
CSIPFromHeader* CSIPRegistrationMgr::CreateFromLC (
    const TRegistrationId& aRegistrationId)
    {
    CSIPRegistrationBindingBase* binding = 
		iBindingStore->FindBinding(aRegistrationId);
    
    __ASSERT_ALWAYS(binding, User::Leave(KErrNotFound));
    
    CSIPFromHeader* from = NULL;
    if (binding->PAssociatedURIHeader())
        {
        CSIPAddress* sipAddress = 
            CSIPAddress::NewLC(binding->PAssociatedURIHeader()->SIPAddress());
        from = CSIPFromHeader::NewL(sipAddress);
        CleanupStack::Pop(sipAddress);
        CleanupStack::PushL(from);            
        }
    else
        {
        from = CSIPFromHeader::NewLC(binding->AOR());        
        }
    return from;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::ClearRegistrationOwner
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationMgr::ClearRegistrationOwner (
	const MRegistrationOwner* aRegistrationOwner)
	{
	return (iBindingStore->ClearRegistrationOwner(aRegistrationOwner));
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::TerminateRegistration
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationMgr::TerminateRegistration (
    const TRegistrationId& aRegistrationId,
    const MRegistrationOwner* aOwner)
    {
    return iBindingStore->RemoveBinding(aRegistrationId,aOwner);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::CheckRequestURI
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationMgr::CheckRequestURI(CURIContainer& aRequestUri)
	{
	return (iBindingStore->CheckRequestURI(aRequestUri));
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::OutboundProxy
// -----------------------------------------------------------------------------
//
const CSIPRouteHeader* CSIPRegistrationMgr::OutboundProxy(
	TRegistrationId aRegistrationId)
	{
	return iBindingStore->OutboundProxy(aRegistrationId);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::HasOutboundProxy
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationMgr::HasOutboundProxy(
	TRegistrationId aRegistrationId) const
	{
	return iBindingStore->HasOutboundProxy(aRegistrationId);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::IsOutboundProxy
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationMgr::IsOutboundProxy (const CURIContainer& aUri)
    {
    return iBindingStore->IsOutboundProxy(aUri);
    }
    
// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::URIFailed
// -----------------------------------------------------------------------------
//    
void CSIPRegistrationMgr::URIFailed (CURIContainer& aUri)
    {
    iBindingStore->URIFailed(aUri);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::ContactL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::ContactL(TUint32                     aIapId,
                                   TRegistrationId             aRegistrationId,
								   const CSIPFromToHeaderBase* aFromToHeader,
								   CSIPContactHeader&          aContact)
	{
	__SIP_ASSERT_LEAVE(aFromToHeader != 0, KErrArgument);
	__SIP_ASSERT_LEAVE(aContact.SIPAddress() != 0, KErrArgument);
	
	if (aContact.SIPAddress()->URI().IsSIPURI())
	    {   
	    CSIPURI& contactSIPURI = *(aContact.SIPAddress()->URI().SIPURI());

        CSIPRegistrationBindingBase* binding = 
		    iBindingStore->FindBinding(aRegistrationId);
        if (binding)
            {
            CSIPURI* bindingContactURI = 
                binding->Contact().SIPAddress()->URI().SIPURI();
            __SIP_ASSERT_LEAVE(bindingContactURI != 0, KErrArgument);
            CSIPHostPort* hostPort = 
                CSIPHostPort::NewLC(bindingContactURI->HostPort());
            contactSIPURI.SetHostPortL(hostPort);   
            CleanupStack::Pop(hostPort);
            }
        else
            {
            FillHostWithLocalIPL(aIapId,contactSIPURI);
            }

	    if (contactSIPURI.User().Length() == 0 &&
	        !iBindingStore->FindContactByFromL(aFromToHeader,aContact))
            {
            const CSIPURI* uriOfFromTo = 
                (const_cast<CSIPFromToHeaderBase*>(
                    aFromToHeader))->SIPAddress().URI().SIPURI();
			
            __ASSERT_ALWAYS (uriOfFromTo != 0, User::Leave(KErrArgument));
            __ASSERT_ALWAYS (uriOfFromTo->User().Length() > 0, 
				             User::Leave(KErrSIPInvalidContact));

            contactSIPURI.SetUserL(uriOfFromTo->User());
		    }
	    }
	}
	
// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::Contact
// -----------------------------------------------------------------------------
//
const CSIPContactHeader* CSIPRegistrationMgr::Contact (
    const TRegistrationId& aRegistrationId)
    {
    CSIPRegistrationBindingBase* binding = 
		iBindingStore->FindBinding(aRegistrationId);

    if (binding)
        {
        return &(binding->Contact());
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::FillTransportParams
// -----------------------------------------------------------------------------
//    
TBool CSIPRegistrationMgr::FillTransportParams(
    const TRegistrationId& aRegistrationId,
    TSIPTransportParams& aTransportParams)
    {
    TBool found = EFalse;
    CSIPRegistrationBindingBase* binding = 
		iBindingStore->FindBinding(aRegistrationId);
    if (binding)
        {
        found = ETrue;
        aTransportParams = binding->FillTransportParams();
        }
    return found;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::SIPSecUser
// -----------------------------------------------------------------------------
//    
const MSIPSecUser* CSIPRegistrationMgr::SIPSecUser(
    const TRegistrationId& aRegistrationId)
    {
    return iBindingStore->FindBinding(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::SIPSecUserForTransaction
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSIPRegistrationMgr::SIPSecUserForTransaction(
    const TTransactionId& aTransactionId)
    {
    return iBindingStore->FindBindingForTransaction(aTransactionId);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::ConnectionStateChangedL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::ConnectionStateChangedL (
	TUint32                aIapId,
    CSIPConnection::TState aState)
	{	
    if (aState == CSIPConnection::EInactive || 
        aState == CSIPConnection::EUnavailable)
		{
		// remove bindings, which has same IAPId.
		iBindingStore->RemoveBindingByIAPId(aIapId); 
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::FillHostWithLocalIPL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::FillHostWithLocalIPL (TUint32 aIapId,
                                                CSIPURI& aSipUri)
    {
    HBufC8* localAddr = TSIPRegistrationUtility::LocalAddressLC(aIapId, iTU);
    aSipUri.HostPort().SetHostL(*localAddr);
	CleanupStack::PopAndDestroy(localAddr);    
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::DetachBinding
// -----------------------------------------------------------------------------
//
void CSIPRegistrationMgr::DetachBinding (TAny* aBinding)
	{
	 __SIP_ASSERT_RETURN(aBinding, KErrGeneral);

	CSIPRegistrationBindingBase* binding = 
		reinterpret_cast<CSIPRegistrationBinding*>(aBinding);

	binding->DetachFromStore();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::IsCacheOutboundProxyIPEanbled
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationMgr::IsCacheOutboundProxyIPEanbled(
			TRegistrationId aRegistrationId) const
	{
	CSIPRegistrationBindingBase* binding = 
			    iBindingStore->FindBinding(aRegistrationId);
	if (binding)
		{
		return binding->IsCacheOutboundProxyIPEnabled();
		}
	return EFalse;
	}
// -----------------------------------------------------------------------------
// CSIPRegistrationMgr::OutboundProxyIPL
// -----------------------------------------------------------------------------
//
CSIPRouteHeader& CSIPRegistrationMgr::OutboundProxyIPL(
			TRegistrationId aRegistrationId) const
	{
	CSIPRegistrationBindingBase* binding = 
				    iBindingStore->FindBinding(aRegistrationId);
	__SIP_ASSERT_LEAVE(binding, KErrArgument);
	return binding->OutboundProxyIPL();
	}
