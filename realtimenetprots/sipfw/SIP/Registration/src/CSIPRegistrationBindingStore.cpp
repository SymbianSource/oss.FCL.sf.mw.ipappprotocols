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
// Name          : CSIPRegistrationBindingStore.cpp
// Part of       : Registration
// Version       : SIP/4.0 
//



#include "CSIPRegistrationBinding.h"
#include "CSIPRegistrationRefreshBinding.h"
#include "CSIPRegistrationBindingStore.h"
#include "CSIPResponseUtility.h"
#include "CSIPRegistrarStore.h"
#include "sipcontactheader.h"
#include "sipaddress.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include "siprouteheader.h"
#include "siptoheader.h"
#include "DeleteMgr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::NewL
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingStore* CSIPRegistrationBindingStore::NewL(
	CSIPRegistrarStore& aRegistrarStore)
	{
	CSIPRegistrationBindingStore* self = 
		CSIPRegistrationBindingStore::NewLC(aRegistrarStore);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingStore* CSIPRegistrationBindingStore::NewLC(
	CSIPRegistrarStore& aRegistrarStore)
	{
    CSIPRegistrationBindingStore* self = 
		new (ELeave) CSIPRegistrationBindingStore(aRegistrarStore);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::CSIPRegistrationBindingStore
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingStore::CSIPRegistrationBindingStore(
	CSIPRegistrarStore& aRegistrarStore)

  : iRegistrarStore     (aRegistrarStore),
    iNextRegistrationId (KMinRegistrationId)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingStore::ConstructL()
	{
	iDeleteMgr = CDeleteMgr::NewL();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::~CSIPRegistrationBindingStore
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingStore::~CSIPRegistrationBindingStore()
	{
	iBindings.ResetAndDestroy();
	delete iDeleteMgr;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::FindBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingBase* CSIPRegistrationBindingStore::FindBinding(
	const MRegistrationOwner& aRegistrationOwner,
	CSIPRequest&              aSIPRequest)
	{ 
	CSIPRegistrationBindingBase* returnValue = NULL;
	
	for (TInt i=iBindings.Count()-1; (i>=0 && !returnValue); i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];

		if (binding->CompareWithRegisterRequest(
			aRegistrationOwner, aSIPRequest))
			{
			returnValue = binding; 
			}
		}

	return returnValue;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::FindBinding
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingBase* CSIPRegistrationBindingStore::FindBinding(
	const TRegistrationId& aRegistrationId,
	const MRegistrationOwner* aOwner)
	{
	CSIPRegistrationBindingBase* returnValue = NULL;

	if (aRegistrationId == KEmptyRegistrationId)
		{
		return returnValue;
		}

	for (TInt i=iBindings.Count()-1; (i>=0 && !returnValue); i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
		 
		if (binding->RegistrationId() == aRegistrationId &&
		    (!aOwner || binding->Owner() == aOwner))
			{
			returnValue = binding;
			}
		}

	return returnValue;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::FindBindingForTransaction
// -----------------------------------------------------------------------------
//
const CSIPRegistrationBindingBase* 
CSIPRegistrationBindingStore::FindBindingForTransaction(
    const TTransactionId& aTransactionId)
    {
	for (TInt i=0; i < iBindings.Count(); i++)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
		if (binding->HasTransaction(aTransactionId))
			{
			return binding;
			}
		}
	return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::FindContactByFromL
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingStore::FindContactByFromL(
	const CSIPFromToHeaderBase* aFromToHeader,
	CSIPContactHeader&          aContact)
	{
	TBool found = EFalse;

	for (TInt i=iBindings.Count()-1; (i>=0 && !found); i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
		found = binding->FindContactByFromL(aFromToHeader, aContact);
		}
	
	return found;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::RemoveBinding
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingStore::RemoveBinding (
	const CSIPRegistrationBindingBase* aBinding)
	{
	TInt err   = KErrNotFound;
	TInt index = KErrNotFound;

	index  = iBindings.Find(aBinding);

	// binding found 
	if (index  >= 0)
		{
		err = iDeleteMgr->AddDeleteRequest(aBinding);

		if (err == KErrNone)
			{
			iBindings.Remove(index);
			}
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::RemoveBindingByIAPId
// remove bindings, which contain same IAPId as the parameter
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingStore::RemoveBindingByIAPId (
	const TUint32 aIapId)
	{
	for (TInt i=iBindings.Count()-1; i>=0; i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
		 
		if (binding->IAPId() == aIapId)
			{
			iBindings.Remove(i);
			delete binding;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::RemoveBinding
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingStore::RemoveBinding (
    const TRegistrationId& aRegistrationId,
    const MRegistrationOwner* aOwner)
    {
    TInt returnValue = KErrNotFound;

    CSIPRegistrationBindingBase* binding = FindBinding(aRegistrationId,aOwner);
    if (binding)
        {
        TInt index = iBindings.Find(binding); 
        if (index != KErrNotFound)
            {
            iBindings.Remove(index); 
            delete binding;
            binding = NULL;
            returnValue = KErrNone;
            }
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::ClearRegistrationOwner
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingStore::ClearRegistrationOwner(
	const MRegistrationOwner* aRegistrationOwner)
	{
	TInt err = KErrNotFound;

	for (TInt i=iBindings.Count()-1; i>=0; i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];

		if (binding->Owner() == aRegistrationOwner)
			{
			iBindings.Remove(i);
			delete binding;
			binding = NULL;
			err = KErrNone;
			}
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::RegistrarStore
// -----------------------------------------------------------------------------
//
CSIPRegistrarStore& CSIPRegistrationBindingStore::RegistrarStore()
	{
	return iRegistrarStore;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::NextRegistrationId
// -----------------------------------------------------------------------------
//
TRegistrationId CSIPRegistrationBindingStore::NextRegistrationId()
	{
	TRegistrationId registrationId = iNextRegistrationId;

    if (iNextRegistrationId == KMaxRegistrationId)
		{
		iNextRegistrationId = KMinRegistrationId;
		}
	else
		{
	    iNextRegistrationId++;
		}

	return registrationId;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::CheckRequestURI
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingStore::CheckRequestURI(CURIContainer& aRequestUri)
	{
	TBool matchFound = EFalse;

	for (TInt i=iBindings.Count()-1; i>=0 && !matchFound; i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
		CSIPURI* bindingUri = binding->Contact().SIPAddress()->URI().SIPURI();

        // Check only the user part. 
        // The host part may be have been changed 
        // by the proxy/registrar due to presence of NAT.
		if (aRequestUri.IsSIPURI() &&
		    bindingUri &&
		    bindingUri->IsSIPSURI() == aRequestUri.SIPURI()->IsSIPSURI() &&
		    bindingUri->User().CompareF(aRequestUri.SIPURI()->User()) == 0)
			{
			matchFound = ETrue;
			}
		}

	return matchFound;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::SetOutboundProxyL
// -----------------------------------------------------------------------------
//	
void CSIPRegistrationBindingStore::SetOutboundProxyL (
    const TRegistrationId& aRegistrationId,
    CSIPRouteHeader* aOutboundProxy,
    const MRegistrationOwner* aOwner)
    {
    CSIPRegistrationBindingBase* binding = FindBinding(aRegistrationId,aOwner);
    if  (!binding)
        {
        User::Leave(KErrNotFound);
        }
    binding->SetOutboundProxyL(aOutboundProxy);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::OutboundProxy
// -----------------------------------------------------------------------------
//
const CSIPRouteHeader* CSIPRegistrationBindingStore::OutboundProxy(
	TRegistrationId aRegistrationId)
	{
	CSIPRegistrationBindingBase* binding = FindBinding(aRegistrationId);

    CSIPRouteHeader* proxy = NULL;

    if (binding)
        {
        proxy = const_cast<CSIPRouteHeader*>(binding->OutboundProxy());
        }
        
	return proxy;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::HasOutboundProxy
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingStore::HasOutboundProxy(
	TRegistrationId aRegistrationId) 
	{
	TBool returnValue = EFalse; 

	CSIPRegistrationBindingBase* binding = FindBinding(aRegistrationId);

	if (binding && binding->HasOutboundProxy())
		{
		returnValue = ETrue;
		}

	return returnValue;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::IsOutboundProxy
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingStore::IsOutboundProxy (const CURIContainer& aUri)
    {
    TBool isOutboundProxy = EFalse;

	for (TInt i=iBindings.Count()-1; i>=0 && !isOutboundProxy; i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
		const CSIPRouteHeader* proxy = binding->OutboundProxy();
		if (binding->HasOutboundProxy() && 
		    proxy->SIPAddress().URI().IsSIPURI() &&
		    aUri.IsSIPURI())
            {
	    	const CSIPURI& sipuri = *aUri.SIPURI();
	    	RStringF maddr = SIPStrings::StringF(SipStrConsts::EMaddr);

	    	const TDesC8 *uriHost(NULL);
	    	if (sipuri.HasParam(maddr))
	    		{
	    		uriHost = &sipuri.ParamValue(maddr).DesC();
		   		}
			else
				{
				uriHost = &sipuri.HostPort().Host();
				}            
            TPtrC8 proxyHost = 
                proxy->SIPAddress().URI().SIPURI()->HostPort().Host();            
            if (proxyHost.CompareF(*uriHost) == 0)
                {
                isOutboundProxy = ETrue;
                }
            }
		}

	return isOutboundProxy;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::RemoveOutboundProxy
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingStore::RemoveOutboundProxy(
	const TRegistrationId& aRegistrationId,
	const MRegistrationOwner* aOwner)
	{
	TInt err = KErrNotFound;

	if (aRegistrationId == KEmptyRegistrationId)
		{
		return err;
		}

	for (TInt i=iBindings.Count()-1; (i>=0 && err==KErrNotFound); i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];

		if (binding->RegistrationId() == aRegistrationId &&
		    binding->Owner() == aOwner)
			{
			err = binding->RemoveOutboundProxy();
			}
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::CompareUri
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingStore::CompareUri(const CURIContainer& aProxy, CURIContainer& aUri)
	{
    const TUint KDefaultSipPort 	  = 5060;
    const TUint KDefaultSipPortForTLS = 5061;
    
	if(aProxy.SIPURI()->HostPort().Host() != aUri.SIPURI()->HostPort().Host())
		/* Hosts dont match */
		{
		return FALSE;
		}
	else if(aProxy.SIPURI()->HostPort().HasPort() != aUri.SIPURI()->HostPort().HasPort())
		/* If the port is KDefaultSipPort/KDefaultSipPortForTLS
		 * then it is optional to mention the port in the URI
		 * and the check can be ignored
		 */
		{
		if(aProxy.SIPURI()->IsSIPSURI() &&
				aProxy.SIPURI()->IsSIPSURI() == aUri.SIPURI()->IsSIPSURI())
	        {
	        if( aProxy.SIPURI()->HostPort().Port() != KDefaultSipPortForTLS &&
	        		aUri.SIPURI()->HostPort().Port() != KDefaultSipPortForTLS)
	        	{
	        	return FALSE;
	        	}
	        }
		else if( aProxy.SIPURI()->HostPort().Port() != KDefaultSipPort &&
				aUri.SIPURI()->HostPort().Port() != KDefaultSipPort)
			{
	        return FALSE;
	        }
		}
	else if(aProxy.SIPURI()->HostPort().HasPort() &&
			aProxy.SIPURI()->HostPort().Port() != aUri.SIPURI()->HostPort().Port())
		/* Ports dont match */
		{
		return FALSE;
		}
	return TRUE;
 	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::URIFailed
// -----------------------------------------------------------------------------
//	
void CSIPRegistrationBindingStore::URIFailed (CURIContainer& aUri)
    {
	for (TInt i=iBindings.Count()-1; i>=0; i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];
        const CSIPRouteHeader* proxy = binding->OutboundProxy();
        if(proxy)
        	{
        	TBool uriMatch = CompareUri(proxy->SIPAddress().URI(), aUri);
        	if (uriMatch &&
        			!binding->RegisterPending())
        	            {
        	            // Remove binding only the registration is not pending,
        	            // which means this failure was not related to a REGISTER request.
        	            // If a REGISTER request fails, we will get a 
        	            // TransactionEnded-callback from TU.
        	            __SIP_LOG("CSIPRegistrationBindingStore::URIFailed")
        				iBindings.Remove(i);
        				binding->OutboundProxyFailed();
        				delete binding;            
        	            }
        	}
		}   
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::RegisterPendingToAOR
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingStore::RegisterPendingToAOR (
	const CURIContainer& aAOR)
	{
	TBool registerPending = EFalse;

	for (TInt i=iBindings.Count()-1; i>=0 && !registerPending; i--)
		{
		CSIPRegistrationBindingBase* binding = iBindings[i];

		if (binding->AOR().SIPAddress().URI() == aAOR &&
		    binding->RegisterPending())
			{
			registerPending = ETrue;
			}
		}

	return registerPending;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::AddBindingL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingStore::AddBindingL(
	CSIPRegistrationBindingBase* aBinding)
	{
	iBindings.AppendL(aBinding);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingStore::DetachBinding
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingStore::DetachBinding (
	CSIPRegistrationBindingBase* aBinding)
	{
	TInt index = KErrNotFound;
	index  = iBindings.Find(aBinding);
	if (index != KErrNotFound)
		{
		iBindings.Remove(index);
		}
	}
