// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPRegistrationBindingBase.cpp
// Part of       : Registration
// Version       : SIP/6.0
//



#include "CSIPRegistrationRefreshBinding.h"
#include "CSIPRegistrationBinding.h"
#include "TSIPRegistrationUtility.h"
#include "CSIPRegistrar.h"
#include "CSIPRegistrarStore.h"
#include "CSIPRegistrationBindingStore.h"
#include "CSIPResponseUtility.h"
#include "MRegistrationOwner.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipaddress.h"
#include "sipcontactheader.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include "siprouteheader.h"
#include "sipminexpiresheader.h"
#include "sipservicerouteheader.h"
#include "sippassociateduriheader.h"
#include "MSigComp.h"
#include "sipsec.h"
#include "siperr.h"
#include "SipLogs.h"
#include "SipAssert.h"
#include "sipcodecutils.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "csipregistering.h"
#include "csip423responsereceived.h"
#include "csipregistered.h"
#include "csipregisterupdating.h"
#include "csipunregistering.h"
#include "csipunregistered.h"
#include "TSIPTransportParams.h"
#include "MSIPTransportMgr.h"
#include <utf.h>


#define HEADER(H, M, N) static_cast<H*>((M).Header(N,0))
const TUint KDefaultSipPort = 5060;

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::ConstructL(CSIPRequest* aSIPRequest,
											 CURIContainer& aRemoteTarget)
	{
	__ASSERT_ALWAYS (aSIPRequest, User::Leave (KErrArgument));
    __ASSERT_ALWAYS (aSIPRequest->To(), User::Leave (KErrArgument));
    __ASSERT_ALWAYS (aSIPRequest->From(), User::Leave (KErrArgument));

	InitializeStatesL();

	// copy to header from request
	SetToHeaderL(*aSIPRequest);

	// copy from header from request
	SetFromHeaderL(*aSIPRequest);

	// copy contact header from request.
	SetContactHeaderL(*aSIPRequest);
		
	iRegistrationId = iBindingStore.NextRegistrationId();

	iRegistrar = iBindingStore.RegistrarStore().FindRegistrar(aRemoteTarget);

	if (!iRegistrar) // registrar not found
		{
		iRemoteTarget = CURIContainer::NewL(aRemoteTarget);
		}
	else
		{
		iRegistrar->AddRegistrationIdL(iRegistrationId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::InitializeStatesL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::InitializeStatesL()
	{
	iRegistering = CSIPRegistering::NewL(*this);
	iRegistered = CSIPRegistered::NewL(*this);
	iResponse423Received = CSIP423ResponseReceived::NewL(*this);
	iUpdating = CSIPRegisterUpdating::NewL(*this);
	iUnregistering = CSIPUnregistering::NewL(*this);
	iUnregistered = CSIPUnregistered::NewL(*this);

	// set registering neighbour states.
	iRegistering->SetNeighbourStates(iResponse423Received, iRegistered,
		iUnregistered);
	// set current state to registering.
	iCurrentState = iRegistering;
	// set registered neighbour states.
	iRegistered->SetNeighbourStates(iUpdating, iUnregistering, iUnregistered,
		iResponse423Received);
	// set response 423 received neighbour states.
	iResponse423Received->SetNeighbourStates(iRegistered, iUnregistered);
	// set unregistering states.
	iUnregistering->SetNeighbourStates(iUnregistered);
	// set updating states.
	iUpdating->SetNeighbourStates(iResponse423Received, iRegistered,
		iUnregistered);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::CSIPRegistrationBindingBase
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingBase::CSIPRegistrationBindingBase (
    MTransactionUser&             aTransactionUser,
	MSigComp&                     aSigComp,
	CSIPSec&                      aSIPSec,
	CSIPRegistrationBindingStore& aBindingStore,
	MRegistrationOwner*           aRegistrationOwner,
	MSIPSecUser&                  aSIPSecUser,
	MSIPTransportMgr&             aTransportMgr,
	TBool 						  aSendWithExpires,
	TBool 						  aCacheOutboundProxyIP)
  : iTransactionUser     (aTransactionUser),
    iOwner               (aRegistrationOwner),
    iRegistrationId      (KEmptyRegistrationId),
	iSIPSecUser          (aSIPSecUser),    
    iSigComp             (aSigComp),
	iSIPSec              (aSIPSec),
    iBindingStore        (aBindingStore),
	iTransportMgr        (aTransportMgr),
	iSendWithExpires     (aSendWithExpires),
	iCacheOutboundProxyIP(aCacheOutboundProxyIP)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::~CSIPRegistrationBindingBase
// -----------------------------------------------------------------------------
//
CSIPRegistrationBindingBase::~CSIPRegistrationBindingBase()
	{
	if (iRegistrar)
		{
		iRegistrar->BindingRemoved(iRegistrationId);
		}
	FreeProxyTransportResources();
	delete iPAssociatedURIHeader;
	delete iOutboundProxy;
	delete iOutboundProxyIP;
	delete iTo;
	delete iFrom;
	delete iContact;
	delete iRemoteTarget;
	iRouteSet.ResetAndDestroy();
	delete iRequest;

	// delete register states
	delete iRegistering;
	delete iRegistered;
	delete iResponse423Received;
	delete iUpdating;
	delete iUnregistering;
	delete iUnregistered;
	iTransportMgr.RemoveTransport(iTransportId);
	iSIPSec.ClearCache(this);
	iTransactionUser.FreeResources(*this);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::PassOnlyRealmsToUser
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::PassOnlyRealmsToUser() const
    {
    return iSIPSecUser.PassOnlyRealmsToUser();
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RequestCredentialsL
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::RequestCredentialsL(const TDesC8& aRealm)
    {
    iSIPSecUser.RequestCredentialsL(aRealm);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RequestCredentialsL
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::RequestCredentialsL(
    CSIPResponse& aResponse,
    TTransactionId aTransactionId,
    TRefreshId aRefreshId)
    {
    iSIPSecUser.RequestCredentialsL(aResponse,aTransactionId,aRefreshId);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::TrustedUser
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSIPRegistrationBindingBase::TrustedUser(
    TRegistrationId /*aRegistrationId*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ByPassSIPSec
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::ByPassSIPSec() const
    {
    return iSIPSecUser.ByPassSIPSec();
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::FlowFailure
// From MSIPNATBindingObserver
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::FlowFailure(TInt /*aError*/)
    {
    iOwner->RegistrationError(iTransactionId,iRegistrationId,
                              KErrSIPTransportFailure);
    ChangeState(iUnregistered);
    iTransactionUser.FreeResources(*this);
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RefreshId
// -----------------------------------------------------------------------------
//	
TRefreshId CSIPRegistrationBindingBase::RefreshId() const
    {
    return KEmptyRefreshId;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetRequest
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetRequest (CSIPRequest* aSIPRequest)
	{
	delete iRequest;
	iRequest = aSIPRequest;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::Owner
// -----------------------------------------------------------------------------
//
const MRegistrationOwner* CSIPRegistrationBindingBase::Owner() const
	{
	return iOwner;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RegisterPending
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::RegisterPending () const
	{
	// return ETrue when binding is in following states.
	return (iCurrentState == iRegistering ||
		    iCurrentState == iUpdating ||
			iCurrentState == iUnregistering ||
			iCurrentState == iResponse423Received);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::UpdateBindingFromResponseL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::UpdateBindingFromResponseL(
	CSIPResponse& aResponse)
	{
	// no related registrar, create one, add registrar pointer to binding
	if (!iRegistrar)
		{
		HBufC8* callId = CSIPResponseUtility::CallIdL(aResponse);
		CleanupStack::PushL(callId);
		iRegistrar = iBindingStore.RegistrarStore().CreateRegistrarL(
			*iRemoteTarget, *callId,
			CSIPResponseUtility::CSeqValueL(aResponse), iRegistrationId);
		CleanupStack::PopAndDestroy(callId);
		delete iRemoteTarget;
		iRemoteTarget = NULL;
		}

    const RStringF KTagParam = SIPStrings::StringF(SipStrConsts::ETag);
	if ((aResponse.From()) && (aResponse.From()->HasParam(KTagParam)))
		{
		iFrom->SetParamL(KTagParam,aResponse.From()->ParamValue(KTagParam));
		}
	SetRouteHeaderFromResponseL(aResponse);
	SetPAssociatedURIFromResponseL(aResponse);
	}


// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetToHeaderL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetToHeaderL(CSIPRequest& aSIPRequest)
	{
    CSIPToHeader* to = aSIPRequest.To();
	CSIPToHeader* tempTo = CSIPToHeader::NewL(*to);
	delete iTo;
	iTo = tempTo;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetFromHeaderL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetFromHeaderL(CSIPRequest& aSIPRequest)
	{
	CSIPFromHeader* fromHeader = aSIPRequest.From();

	CSIPFromHeader* temp = CSIPFromHeader::NewL(*fromHeader);
	delete iFrom;
	iFrom = temp;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetContactHeaderL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetContactHeaderL(CSIPRequest& aSIPRequest)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);	
	
    CSIPContactHeader* contact =
        HEADER(CSIPContactHeader, aSIPRequest, KContactHeaderName);

	CSIPContactHeader* tmp = static_cast<CSIPContactHeader*>(contact->CloneL());
	CleanupStack::PushL(tmp);
	if(iContact && tmp->ExpiresParameter() == KErrNotFound)
		tmp->SetExpiresParameterL(iContact->ExpiresParameter());
	delete iContact;
	CleanupStack::Pop(tmp);
	iContact = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetExpiresValueL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetExpiresValueL(CSIPRequest& aSIPRequest)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);	
	
	CSIPContactHeader* contactHeader =
        HEADER(CSIPContactHeader, aSIPRequest, KContactHeaderName);

	iContact->SetExpiresParameterL(contactHeader->ExpiresParameter());
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetMinExpiresToRequestL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetMinExpiresToRequestL (
	CSIPResponse& aResponse)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);
        	
	// the request must not be empty
	__SIP_ASSERT_LEAVE(iRequest, KErrArgument);
	__SIP_ASSERT_LEAVE(iRequest->HasHeader(KContactHeaderName),
		               KErrArgument);
    // get the Min expires header from response
	CSIPMinExpiresHeader* minExpiresHeader = HEADER(
		CSIPMinExpiresHeader, aResponse, 
		SIPStrings::StringF(SipStrConsts::EMinExpiresHeader));
	// get contact header from request
	CSIPContactHeader* contactInRequest = HEADER(
	    CSIPContactHeader, *iRequest, KContactHeaderName);
	// set the min expires value to contact in the original request
	contactInRequest->SetExpiresParameterL(minExpiresHeader->Value());
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetRouteHeaderFromResponseL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetRouteHeaderFromResponseL(
    CSIPResponse& aResponse)
	{
	iRouteSet.ResetAndDestroy();

	TSglQueIter<CSIPHeaderBase> iter =
        aResponse.Headers(
            SIPStrings::StringF(SipStrConsts::EServiceRouteHeader));

	for (CSIPHeaderBase* header = iter++; header; header = iter++)
		{
		CSIPServiceRouteHeader* routeHeader =
			static_cast<CSIPServiceRouteHeader*>(header);
		CSIPRouteHeader* newRouteHeader = 
		    SIPCodecUtils::CreateRouteHeaderLC(*routeHeader);
        iRouteSet.AppendL(newRouteHeader);
		CleanupStack::Pop(newRouteHeader);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ServiceRouteOk
// -----------------------------------------------------------------------------
//	
TBool CSIPRegistrationBindingBase::ServiceRouteOk(
    CSIPResponse& aResponse) const
    {
    // Check if the last Service-Route-header has changed
    TBool ok = ETrue;
    if (iRouteSet.Count() > 0)
        {
        TInt headerCount = 
            aResponse.HeaderCount(
                SIPStrings::StringF(SipStrConsts::EServiceRouteHeader));
        if (headerCount > 0)
            {
            CSIPRouteHeaderBase* myLastRoute = iRouteSet[iRouteSet.Count()-1];
            
            CSIPRouteHeaderBase* newLastRoute = 
                static_cast<CSIPRouteHeaderBase*>(
                    aResponse.Header(
                        SIPStrings::StringF(SipStrConsts::EServiceRouteHeader),
                        headerCount-1));      
            
            ok = (myLastRoute->SIPAddress() == newLastRoute->SIPAddress()); 
            }
        }
    return ok;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetPAssociatedURIFromResponseL
// -----------------------------------------------------------------------------
//	
void CSIPRegistrationBindingBase::SetPAssociatedURIFromResponseL(
    CSIPResponse& aResponse)
    {
    CSIPHeaderBase* headerInResponse = 
        aResponse.Header(
            SIPStrings::StringF(SipStrConsts::EPAssociatedURIHeader),0);
    CSIPHeaderBase* header = NULL;
    if (headerInResponse)
        {
        header = headerInResponse->CloneL();
        }
    delete iPAssociatedURIHeader;
    iPAssociatedURIHeader = static_cast<CSIPPAssociatedURIHeader*>(header);
    }
    
// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::PAssociatedURIHeader
// -----------------------------------------------------------------------------
//    
const CSIPPAssociatedURIHeader* 
CSIPRegistrationBindingBase::PAssociatedURIHeader() const
    {
    return iPAssociatedURIHeader;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::FillTransportParams
// -----------------------------------------------------------------------------
//
TSIPTransportParams CSIPRegistrationBindingBase::FillTransportParams()
    {
    TSIPTransportParams transportParams(iOwner->TransportParams());
    transportParams.SetTransportId(iTransportId);
    transportParams.SetCompartmentId(iSigCompCompartmentId);
    transportParams.SetNATBindingObserver(this);
    return transportParams; 
    }    
	
// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::HasTransaction
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::HasTransaction(
    const TTransactionId& aTransactionId) const
    {
    return (iTransactionId == aTransactionId || 
            iRetryTransactionId == aTransactionId);
    }	
	
// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::OutboundProxyFailed
// -----------------------------------------------------------------------------
//	
void CSIPRegistrationBindingBase::OutboundProxyFailed()
    {
    if (!RegisterPending())
        {
        iCurrentState->TransactionEnded(KEmptyTransactionId,
                                        KErrSIPOutboundProxyNotResponding);
        }
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RouteSet
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPRouteHeader>& CSIPRegistrationBindingBase::RouteSet()
	{
	return iRouteSet;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::IAPId
// -----------------------------------------------------------------------------
//
TUint32 CSIPRegistrationBindingBase::IAPId() const
	{
	return iOwner->TransportParams().IapId();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SigCompCompartmentId
// -----------------------------------------------------------------------------
//
TUint32 CSIPRegistrationBindingBase::SigCompCompartmentId() const
	{
	return iSigCompCompartmentId;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::Contact
// -----------------------------------------------------------------------------
//
CSIPContactHeader& CSIPRegistrationBindingBase::Contact()
	{
	return *iContact;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::AOR
// -----------------------------------------------------------------------------
//
const CSIPToHeader& CSIPRegistrationBindingBase::AOR () const
    {
    return *iTo;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::FindContactByFromL
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::FindContactByFromL(
	const CSIPFromToHeaderBase* aFromToHeader,
	CSIPContactHeader&      	aContact)
	{
	TBool found = EFalse;

	const CURIContainer& uriOfFromTo  = (const_cast<CSIPFromToHeaderBase*>(
		aFromToHeader))->SIPAddress().URI();

	// if URI of To header in binding == URI of aFromToHeader header
	if (uriOfFromTo.IsSIPURI() &&
	    iTo->SIPAddress().URI().IsSIPURI() &&
	    iTo->SIPAddress().URI() == uriOfFromTo)
		{
		CSIPURI& sipuri = *aContact.SIPAddress()->URI().SIPURI();
		CSIPURI& sipuriInBinding = *iContact->SIPAddress()->URI().SIPURI();

		if (sipuri.User().Length() > 0)
			{
			if (sipuri.User().Compare(sipuriInBinding.User()) == 0)
				{
				// fill in host part
				CSIPHostPort* hostPort =
					CSIPHostPort::NewLC(sipuriInBinding.HostPort());
				sipuri.SetHostPortL(hostPort);
				CleanupStack::Pop(hostPort);
				found = ETrue;
				}
			}
		else
			{
			// fill in user part
			sipuri.SetUserL(sipuriInBinding.User());
			// fill in host part
			CSIPHostPort* hostPort =
				CSIPHostPort::NewLC(sipuriInBinding.HostPort());
			sipuri.SetHostPortL(hostPort);
			CleanupStack::Pop(hostPort);
			found = ETrue;
			}
		}

	return found;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::CompareWithRegisterRequest
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::CompareWithRegisterRequest(
	const MRegistrationOwner& aRegistrationOwner,
	CSIPRequest&              aSIPRequest)
	{
	TBool returnValue = EFalse;

	if (iOwner == &aRegistrationOwner)
		{
		// get To header from request
		CSIPToHeader* toHeader = aSIPRequest.To();
		CURIContainer& uriOfToInReq = toHeader->SIPAddress().URI();
		
		// get From header from request
		CSIPFromHeader* fromHeader = aSIPRequest.From();
		CURIContainer& uriOfFromInReq  = fromHeader->SIPAddress().URI();

		// get Contact header from request
        CSIPContactHeader* contactHeader = HEADER(
			CSIPContactHeader, aSIPRequest,
			SIPStrings::StringF(SipStrConsts::EContactHeader));

		CURIContainer& uriOfContactInReq = contactHeader->SIPAddress()->URI();
		CURIContainer& uriOfTo = iTo->SIPAddress().URI();
		CURIContainer& uriOfFrom = iFrom->SIPAddress().URI();
		CURIContainer& uriOfContact = iContact->SIPAddress()->URI();

		if ((uriOfToInReq      == uriOfTo) &&
			(uriOfFromInReq    == uriOfFrom) &&
			(uriOfContactInReq == uriOfContact))
			{
			returnValue = ETrue;
			}
		}

	return returnValue;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RegistrationId
// -----------------------------------------------------------------------------
//
TRegistrationId CSIPRegistrationBindingBase::RegistrationId() const
	{
	return iRegistrationId;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::OutboundProxy
// -----------------------------------------------------------------------------
//
const CSIPRouteHeader* CSIPRegistrationBindingBase::OutboundProxy()
	{
	return iOutboundProxy;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::HasOutboundProxy
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::HasOutboundProxy() const
	{
	return (iOutboundProxy != 0);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetOutboundProxy
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetOutboundProxy(
	CSIPRouteHeader* aOutboundProxy)
	{
	__SIP_ASSERT_RETURN (aOutboundProxy, KErrArgument);

	delete iOutboundProxy;
	iOutboundProxy = aOutboundProxy;
	}
	
// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetOutboundProxyL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SetOutboundProxyL(
	CSIPRouteHeader* aOutboundProxy)
	{
	__SIP_ASSERT_LEAVE (aOutboundProxy, KErrArgument);

    CreateProxyTransportResourcesL(*aOutboundProxy);
    SetOutboundProxy(aOutboundProxy);
	}	

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RemoveOutboundProxy
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingBase::RemoveOutboundProxy()
	{
	TInt err = KErrNotFound;
	if (iOutboundProxy)
		{
		FreeProxyTransportResources();
		delete iOutboundProxy;
		iOutboundProxy = NULL;
		err = KErrNone;
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RemoveBinding
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingBase::RemoveBinding ()
	{
	TInt err = iBindingStore.RemoveBinding (this);

	if (err == KErrNone && iRegistrar)
		{
		if(iRegistrar->BindingRemoved(iRegistrationId))
			{
			iRegistrar = NULL;
			}
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::CreateProxyTransportResourcesL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::CreateProxyTransportResourcesL (
	CSIPRouteHeader& aOutboundProxy)
	{
	CURIContainer& uri = aOutboundProxy.SIPAddress().URI();
	TUint32 oldCompartmentId = iSigCompCompartmentId;
	if (uri.IsSIPURI() && HasSigCompParameter(*uri.SIPURI()))
		{
		iSigCompCompartmentId = iSigComp.CreateCompartmentL(IAPId());
		}
    else
        {
        iSigCompCompartmentId = 0;
        }
    iSigComp.RemoveCompartment(oldCompartmentId);
    TUint32 oldTransportId = iTransportId;
    if (uri.IsSIPURI() && uri.SIPURI()->IsSIPSURI())
        {
		TSIPTransportParams transportParams(FillTransportParams());
		iTransportMgr.CreateTLSTransportL(transportParams);
		iTransportId = transportParams.TransportId();
        }
    else
        {
        iTransportId = 0;
        }
    iTransportMgr.RemoveTransport(oldTransportId);   
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::FreeProxyTransportResources
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::FreeProxyTransportResources()
	{
	if (iSigCompCompartmentId)
		{
        iSigComp.RemoveCompartment(iSigCompCompartmentId);
        iSigCompCompartmentId = 0;
		}
	if (iTransportId)
		{
        iTransportMgr.RemoveTransport(iTransportId);
        iTransportId = 0;
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::HasSigCompParameter
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::HasSigCompParameter (
	const CSIPURI& aUri) const
	{
    RStringF comp = SIPStrings::StringF(SipStrConsts::EComp);
    RStringF sigcomp = SIPStrings::StringF(SipStrConsts::ESigComp);
	return (iSigComp.IsSupported() && aUri.ParamValue(comp) == sigcomp);
	}


// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::RegisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::RegisterL(TTransactionId&  aTransactionId,
		                                    CSIPRequest*     aRequest,
						                    CSIPRouteHeader* aOutboundProxy)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	if(!iSendWithExpires)
		TSIPRegistrationUtility::RemoveRegisterExpiresParam(*aRequest);
	DoRegisterL(aTransactionId, aRequest, aOutboundProxy);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::UpdateL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::UpdateL(TTransactionId& aTransactionId,
		                                  CSIPRequest*    aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	if(!iSendWithExpires)
			TSIPRegistrationUtility::RemoveRegisterExpiresParam(*aRequest);
	DoUpdateL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::UnregisterL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::UnregisterL(TTransactionId& aTransactionId,
		                                      CSIPRequest*    aRequest)
	{
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	DoUnregisterL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::Registrar
// -----------------------------------------------------------------------------
//
CSIPRegistrar* CSIPRegistrationBindingBase::Registrar()
	{
	return iRegistrar;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::FillToFromL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::FillToFromL(CSIPRequest& aRequest)
	{
	// fill in to header from binding
	CSIPToHeader* to = aRequest.To();
	if (!to)
		{
		to = CSIPToHeader::NewLC(*iTo);
		aRequest.AddHeaderL(to);
		CleanupStack::Pop(to);
		}

	// fill in from header from binding
	CSIPFromHeader* from = aRequest.From();
	if (!from)
		{
		from = CSIPFromHeader::NewLC(*iFrom);
		aRequest.AddHeaderL(from);
		CleanupStack::Pop(from);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::CurrentState
// -----------------------------------------------------------------------------
//
CSIPRegistrationState& CSIPRegistrationBindingBase::CurrentState()
	{
	return *iCurrentState;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ChangeState
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::ChangeState(CSIPRegistrationState* aState)
	{
	iCurrentState = aState;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::Request
// -----------------------------------------------------------------------------
//
CSIPRequest* CSIPRegistrationBindingBase::Request()
	{
	return iRequest;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::UpdateContactFromRequestL
// -----------------------------------------------------------------------------
//	
void CSIPRegistrationBindingBase::UpdateContactFromRequestL()
    {
    if (iRequest)
        {
        SetContactHeaderL(*iRequest);
        }
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SelfDetach
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SelfDetach(TBool aRefresh)
	{
	if (IsRefresh())
		{
		if (aRefresh)
			{
			static_cast<CSIPRegistrationRefreshBinding*>(this)->
				ClearRefreshOwner();
			}
		}
	else
		{
		static_cast<CSIPRegistrationBinding*>(this)->ClearTransactionOwner();
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SelfDetach
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::SelfDetach(TBool aRefresh, TBool aUseRetryId)
	{
	if( !IsRefresh() && aUseRetryId )
		{
		static_cast<CSIPRegistrationBinding*>(this)->
			ClearTransactionOwner( aUseRetryId );
		}
	else
		{
		SelfDetach( aRefresh );
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::Received2XXResponseL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::Received2XXResponseL(CSIPResponse& aResponse,
			TTransactionId aTransactionId)
	{
	if (iRequest)
	    {
	    // Update the From- and To-headers in case some parameters have changed
	    SetToHeaderL(*iRequest);
	    SetFromHeaderL(*iRequest);
	    }
	
	UpdateBindingFromResponseL(aResponse);
	if (HasOutboundProxy() && iCacheOutboundProxyIP)
		SetOutboundProxyIPL(aTransactionId);		
	if (!IsRefresh())
		{
		// start timer for non refresh binding
		static_cast<CSIPRegistrationBinding*>(this)->StartTimerL(
				CSIPResponseUtility::ExpireValueL(*iContact, aResponse));
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ClearRequest
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::ClearRequest()
	{
	if (!IsRefresh() && iRequest)
		{
		delete iRequest;
		iRequest = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ReceivedErrorResponseL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::ReceivedErrorResponseL()
	{
	// return KErrNotFound when binding could not found,
	// return KErrNone when binding is found and is added to delete manager
	TInt err = RemoveBinding ();

	// leaves if binding is found and it is not added to delete manager
	if (err != KErrNotFound && err != KErrNone)
		{
		User::Leave(err);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::Received423ResponseL
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::Received423ResponseL(CSIPResponse& aResponse)
	{
	// set min expires value to request contact header.
	SetMinExpiresToRequestL(aResponse);
	// detaching "this" registration from lower layer.
	SelfDetach(ETrue);
	// set refresh id to KEmptyRefreshId if it's refresh binding
	if (IsRefresh())
		{
		static_cast<CSIPRegistrationRefreshBinding*>(this)->SetRefreshId(
			KEmptyRefreshId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::DetachFromStore
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::DetachFromStore()
	{
	iBindingStore.DetachBinding(this);
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::DetachOutboundProxy
// -----------------------------------------------------------------------------
//
void CSIPRegistrationBindingBase::DetachOutboundProxy(TAny* aBinding)
	{
	__SIP_ASSERT_RETURN(aBinding, KErrGeneral);

	CSIPRegistrationBindingBase* binding =
		reinterpret_cast<CSIPRegistrationBindingBase*>(aBinding);

	binding->FreeProxyTransportResources();
	binding->iOutboundProxy = NULL; // detach the OutboundProxy
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::SetOutboundProxyIPL
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrationBindingBase::SetOutboundProxyIPL(const TTransactionId& aTransactionId)
	{   
	TInetAddr outboundProxyIP;
	TInt error = iTransactionUser.NextHopIP(aTransactionId, outboundProxyIP);
	if (!error)
		{
		CSIPAddress* sipAddress = CSIPAddress::NewLC(iOutboundProxy->SIPAddress());
		
		HBufC8* addressText = ConvertInetAddrToTextL(outboundProxyIP);
		CleanupStack::PushL(addressText);
		CSIPHostPort* hostport = CSIPHostPort::DecodeL( *addressText );
		CleanupStack::PopAndDestroy(addressText);
		CleanupStack::PushL(hostport);
		
		TUint port = outboundProxyIP.Port() > 0 ?
				outboundProxyIP.Port() : KDefaultSipPort;
		hostport->SetPort(port);
		
		sipAddress->URI().SIPURI()->SetHostPortL(hostport);
		
		sipAddress->URI().SIPURI()->SetSIPS(
				iOutboundProxy->SIPAddress().URI().SIPURI()->IsSIPSURI());
		
		CleanupStack::Pop(hostport);
		delete iOutboundProxyIP;
		iOutboundProxyIP = NULL;
		iOutboundProxyIP = CSIPRouteHeader::NewL(sipAddress);
		CleanupStack::Pop(sipAddress);
		}
	return error;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::ConvertInetAddrToTextL
// -----------------------------------------------------------------------------
//        
HBufC8* CSIPRegistrationBindingBase::ConvertInetAddrToTextL(const TInetAddr& aAddr)
    {
    const TInt KMaxIPAddrLen = 256;
    HBufC* addressOut = HBufC::NewLC(KMaxIPAddrLen);
	TPtr outputPtr(addressOut->Des());
	aAddr.Output(outputPtr);
	
	HBufC8* addressResult = HBufC8::NewL(addressOut->Length());
	TPtr8 resultPtr(addressResult->Des());
	CnvUtfConverter::ConvertFromUnicodeToUtf8(resultPtr, *addressOut);
	CleanupStack::PopAndDestroy(addressOut);
	
	return addressResult;
    }

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::IsCacheOutboundProxyIPEnabled
// -----------------------------------------------------------------------------
//
TBool CSIPRegistrationBindingBase::IsCacheOutboundProxyIPEnabled()
	{
	return iCacheOutboundProxyIP;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrationBindingBase::OutboundProxyIPL
// -----------------------------------------------------------------------------
//
CSIPRouteHeader& CSIPRegistrationBindingBase::OutboundProxyIPL()
	{
	__ASSERT_ALWAYS (iOutboundProxyIP, User::Leave (KErrNotSupported));
	return *iOutboundProxyIP;
	}

