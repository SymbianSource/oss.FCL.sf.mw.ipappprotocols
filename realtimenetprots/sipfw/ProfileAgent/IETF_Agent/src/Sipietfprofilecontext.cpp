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
// Name        : sipietfprofilecontext.cpp
// Part of     : sip ietf agent
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include "sipietfprofilecontext.h"
#include "sipprflstatebase.h"
#include "sipconcreteprofile.h"
#include "sipietfprofileagent.h"
#include "sipregistrationbinding.h"
#include "sipclienttransaction.h"
#include "siprouteheader.h"
#include "sipgendefs.h"
#include "siprefresh.h"
#include "sipmessagebuilder.h"
#include "sipaddress.h"
#include "siptoheader.h"
#include "siprouteheader.h"
#include "sipmessageelements.h"
#include "sipresponseelements.h"
#include "sipsecurityclientheader.h"
#include "sipconnection.h"
#include "SipProfileLog.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"
#include "sipcodecerr.h"
#include "sipcontactheader.h"
#include <sipprofile.h>
#include <bamdesca.h>
#include <e32math.h>
#include <uri8.h>
#include <uriutils.h>

_LIT8(KNATTraversalRequiredDes, "nat_traversal_required");
_LIT8(KSIPTls, "tls");
_LIT8(KTransportUdpParam, "transport=udp");
_LIT8(KTransportTcpParam, "transport=tcp");
_LIT8(KTransportTlsParam, "transport=tls");

const TUint KMaxRandomLimit = 50;
const TUint KBaseTimeInSec	= 30;
const TUint KWaitTimeCoefficient = 2;
const TUint KRandomDivider		= 100;
const TUint KMaxRetryForOneAddress = 9;

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext* CSIPIetfProfileContext::NewLC(
	CSIP& aSIP,
	CSIPIetfConnectionContext& aConnection, 
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer,
	CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPIetfProfileContext* self = new (ELeave) CSIPIetfProfileContext(aSIP,
		aConnection, aObserver, aInitState, aProfile, aDeltaTimer,aConfigExtension);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext* CSIPIetfProfileContext::NewL(
	CSIP& aSIP,
	CSIPIetfConnectionContext& aConnection, 
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer,
	CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPIetfProfileContext* self = CSIPIetfProfileContext::NewLC(
		aSIP, aConnection, aObserver, aInitState, aProfile, aDeltaTimer,aConfigExtension);
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::CSIPIetfProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext::CSIPIetfProfileContext(
	CSIP& aSIP,
	CSIPIetfConnectionContext& aConnection,
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer,
	CSipProfileAgentConfigExtension& aConfigExtension)
	: CSIPProfileContextBase(aSIP,
	                         *(aConnection.Connection()),
	                         aObserver,
	                         aInitState,
	                         aProfile,
	                         aDeltaTimer),
	  iConnectionContext(aConnection),
	  iRetryTimerUse(EFalse),
	  iConfigExtension(aConfigExtension)
	{
	iRandomSeed = 0;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPIetfProfileContext::ConstructL()
    {
    iStrNATTraversalRequired = 
        SIPStrings::Pool().OpenFStringL(KNATTraversalRequiredDes);
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::~CSIPIetfProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext::~CSIPIetfProfileContext()
	{
	iStrNATTraversalRequired.Close();
	}
	
// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::DestroyRegistration()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::DestroyRegistration()
	{
	PROFILE_DEBUG3("SIPIetfProfileContext::DestroyRegistration", ProfileId())
	delete iRegistration;
	iRegistration = 0;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::CreateRegistrationL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::CreateRegistrationL()
	{
	ASSERT(iProfile);
    RStringF transport(SIPStrings::StringF(SipStrConsts::EEmpty));
    CleanupClosePushL(transport);
	CSIPRouteHeader* proxy = 0;
	CUri8* remoteUri = 0;
	CSIPRefresh* refresh = CSIPRefresh::NewLC();
	CSIPToHeader* to = SIPMessageBuilder::CreateToLC(iProfile->AOR());
	to->SetParamL(iStrNATTraversalRequired);
	TUint pops = 2; //initial amount to pop from CleanupStack
    
	const TDesC8& proxyAddr = ProxyAddressL();
	HBufC8* sipsproxy = NULL;
	if(IsAddressSIPSURIL(iProfile->AOR()) && proxyAddr.Length() > 0)
		{
		if (!IsProxyResolvingEnabled())
			{
			__ASSERT_ALWAYS (IsAddressSIPSURIL(proxyAddr), 
						User::Leave(KErrArgument));
			}
		else
			{
			sipsproxy = SetAddressToSIPSURIL(proxyAddr);
			CleanupStack::PushL(sipsproxy);
			pops++;
			}
		__ASSERT_ALWAYS (!iProfile->IsSecurityNegotiationEnabled(), 
					User::Leave(KErrArgument));	
		}
		
	if (!sipsproxy)
		{
		sipsproxy = proxyAddr.AllocLC();
		pops++;
		}

	if (sipsproxy->Length()) //there's an outbound proxy defined
		{
		(iProfile->IsSigCompEnabled())?
		proxy = SIPMessageBuilder::CreateRouteLC(*sipsproxy, ETrue):
		proxy = SIPMessageBuilder::CreateRouteLC(*sipsproxy);
		pops++;
		RStringF proxyTransport = TransportProtocol(proxy->SIPAddress().Uri8());	
		if (proxyTransport.DesC().Length() > 0)
			{
			transport = proxyTransport.Copy();
			}		
		}
		
	if (iProfile->Server(KSIPRegistrar).Length()) //there's a registrar defined
		{
        TUriParser8 parser;
        User::LeaveIfError(parser.Parse(iProfile->Server(KSIPRegistrar)));
        remoteUri = CUri8::NewLC(parser);			
	    pops++;
		if (!proxy)
			{
			RStringF registrarTransport = TransportProtocol(*remoteUri);		
		    if (registrarTransport.DesC().Length() > 0)
			    {
			    transport = registrarTransport.Copy();
			    }
			}
        }		

	TBool sigcomp = iProfile->IsSigCompEnabled();
	
	const TDesC8* user = NULL;
	User::LeaveIfError(
		iProfile->ExtensionParameter(KSIPContactHeaderUser,user));
	
	CSIPContactHeader* contact =
	    SIPMessageBuilder::CreateContactLC(*user, 
	    		iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EInternet,
				CSipProfileAgentConfigExtension::EProfileRegistrationValue),
	    		iProfile->ContactHeaderParams(),
	    		transport, sigcomp);
    pops++;

	if (IsAddressSIPSURIL(iProfile->AOR()))
		{
		CUri8* contactUriClone = 
		    CUri8::NewLC(contact->SIPAddress()->Uri8().Uri());
        contactUriClone->SetComponentL(
            SIPStrings::StringF(SipStrConsts::ESips).DesC(),EUriScheme);
        contactUriClone->SetComponentL(*user,EUriUserinfo);            	
		contact->SIPAddress()->SetUri8L(contactUriClone);
		CleanupStack::Pop(contactUriClone);
		}
	
	iRegistration = CSIPRegistrationBinding::NewL(iConnection, to, contact,
	                                              refresh, proxy, remoteUri);
	CleanupStack::Pop(pops);
	delete sipsproxy;
	CleanupStack::PopAndDestroy(1); // transport
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::CreateMsgElementsLC()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPMessageElements* CSIPIetfProfileContext::CreateMsgElementsLC()
	{
	ASSERT(iProfile);
	
	CSIPMessageElements* elements = CSIPMessageElements::NewLC();
	
	RPointerArray<CSIPHeaderBase> headers;
	CSIPHeaderBase::PushLC(&headers);
	if (SetTlsToSecurityClientL())
		{
		CSIPSecurityClientHeader* sclient = 
		    CSIPSecurityClientHeader::NewLC(KSIPTls);
		headers.AppendL(sclient);
		CleanupStack::Pop(sclient);
		}
		
	if (iProfile->IsSecurityNegotiationEnabled())
		{
		CSIPSecurityClientHeader* sclient = 
		    CSIPSecurityClientHeader::NewLC(KSIPdigest);
		headers.AppendL(sclient);
		CleanupStack::Pop(sclient);
		}
		
	TInt sipHeaderCount = iProfile->SIPHeaders().MdcaCount();
	for (TInt i=0; i < sipHeaderCount; i++)
	    {
	    TPtrC8 headerDes(iProfile->SIPHeaders().MdcaPoint(i));
	    CSIPHeaderBase* header = SIPMessageBuilder::CreateHeaderLC(headerDes);
	    headers.AppendL(header);
	    CleanupStack::Pop(header);
	    }   
	
	elements->SetUserHeadersL(headers); // // An empty array is also allowed
	CleanupStack::Pop(1); // headers
	    
	return elements;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::CreateDeRegisterElementsL()
// -----------------------------------------------------------------------------
//	
CSIPMessageElements* CSIPIetfProfileContext::CreateDeRegisterElementsL()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::UpdateContactHeaderParamsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::UpdateContactHeaderParamsL(
    CSIPConcreteProfile& aNewProfile)
	{
	CSIPContactHeader& contact = Registration()->ContactHeader();
    TPtrC8 paramName;
	TPtrC8 paramValue;

	for (TInt i = Profile()->ContactHeaderParams().MdcaCount()-1;i >= 0; i--)
		{
		TPtrC8 param(Profile()->ContactHeaderParams().MdcaPoint(i));
		SIPMessageBuilder::ParseParamL(param,paramName,paramValue);		
		RStringF name = SIPStrings::Pool().OpenFStringL(paramName);
		CleanupClosePushL(name);
		contact.DeleteParam(name);
		CleanupStack::PopAndDestroy(1); // name
		}

	for (TInt i = 0; &(aNewProfile.ContactHeaderParams()) && 
				     (i < aNewProfile.ContactHeaderParams().MdcaCount()); i++)
		{
		TPtrC8 param(aNewProfile.ContactHeaderParams().MdcaPoint(i));
		TBool hasValue = 
		    SIPMessageBuilder::ParseParamL(param,paramName,paramValue);
		RStringF name = SIPStrings::Pool().OpenFStringL(paramName);
		CleanupClosePushL(name);
		if (hasValue)
			{
			RStringF value = SIPStrings::Pool().OpenFStringL(paramValue);
			CleanupClosePushL(value);
			contact.SetParamL(name,value);
			CleanupStack::PopAndDestroy(1); // value
			}
		else
			{
			contact.SetParamL(name);	
			}
		CleanupStack::PopAndDestroy(1); // name	
		}
	
	CSIPMessageElements* elements = CreateMsgElementsForUpdateLC(aNewProfile);
	SetTransaction(Registration()->UpdateL(
			iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EInternet,
			CSipProfileAgentConfigExtension::EProfileRegistrationValue),
			elements));
	CleanupStack::Pop(elements);
	}
	
// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::SetRegisteredAORsL()
// -----------------------------------------------------------------------------
//	
void CSIPIetfProfileContext::SetRegisteredAORsL()
    {
    if (iProfile)
        {
        CDesC8ArrayFlat* array = new(ELeave)CDesC8ArrayFlat(1);
	    CleanupStack::PushL(array);    
        array->AppendL(iProfile->AOR());
	    iProfile->SetRegisteredAORsL(*array);
	    CleanupStack::PopAndDestroy(array);
	    if (iRegistration && iRegistration->RegisteredContact())
	        {
	        HBufC8* contact = 
	            iRegistration->RegisteredContact()->ToTextValueLC();
	        iProfile->SetExtensionParameterL(KSIPRegisteredContact,*contact);
	        CleanupStack::PopAndDestroy(contact);
	        }
        }
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::ResolveProxyL
// -----------------------------------------------------------------------------
//	
void CSIPIetfProfileContext::ResolveProxyL()
    {
    iConnectionContext.ResolveL();
    }
       
// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::CancelProxyResolving
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::CancelProxyResolving()
    {
    iConnectionContext.CleanProxyResolving();
    }	

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::RetryAfterTimer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CSIPIetfProfileContext::RetryAfterTimer()
	{
	return ( iRetryCounterSum == 0 && iRetryTimerUse );
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::RetryRegister
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CSIPIetfProfileContext::RetryRegister( 
    CSIPClientTransaction* /*aTransaction*/,
    TInt aError )
	{
	PROFILE_DEBUG4("CSIPIetfProfileContext::RetryRegister",
	               aError, iRetryCounterSum)

	TBool retry = EFalse;
		
	if (iProfile &&
	    AgentObserver().ProceedRegistration(*iProfile, aError) &&
	    (CurrentState() == MSIPProfileContext::ERegistrationInProgress ||
	     CurrentState() == MSIPProfileContext::ERegistered) &&
		(aError == K408TimeOut ||
		 aError == K500ServerInternalError ||
		 aError == KErrTimedOut || 
		 (( aError == KErrSIPOutboundProxyNotResponding || 
		 aError == KErrSIPResolvingFailure || 
		 aError == KErrSIPTransportFailure ||
		 aError == KErrSIPICMPFailure ) && 
		 iConnection.State() != CSIPConnection::ESuspended)))		
	    {
		if ( iRetryCounterSum + 1 < KMaxRetryForOneAddress &&
		     iRetryCounter < KMaxRetryForOneAddress )
			{
			retry = ETrue;
			}
		else
			{
			if (iRetryCounterSum < KMaxRetryForOneAddress && 
			    iRetryCounter + 1 >= KMaxRetryForOneAddress ||
			    iRetryCounter >=  KMaxRetryForOneAddress)
				{
				iRetryCounter = 0;
				retry = RetryPossible(aError);
				}
			else
				{
				retry = ETrue;
				}
			}
		iRetryTimerUse = retry;
		SetRetryPossible(retry);
		}
	else
		{
		iRetryCounter = 0;
		iRetryCounterSum = 0;
		iRetryTimerUse = EFalse;
		}
	if (!retry)
		{
		iRetryCounter = 0;
		iRetryCounterSum = 0;
		}
	return retry;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::ShouldRetryRegistration
// -----------------------------------------------------------------------------
//	
TBool CSIPIetfProfileContext::ShouldRetryRegistration( TInt aError )
	{
	return (iProfile && 
	        AgentObserver().ProceedRegistration(*iProfile, aError) &&
	        (aError == K503ServiceUnavailable || 
	        aError == K408TimeOut ||
	        aError == K500ServerInternalError ||
		    aError == KErrTimedOut ||
		    ((aError == KErrSIPResolvingFailure || 
		      aError == KErrSIPTransportFailure ||
		      aError == KErrSIPICMPFailure ||
		      aError == KErrSIPOutboundProxyNotResponding ) && 
			 iConnection.State() != CSIPConnection::ESuspended)));
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::InitializeRetryTimerValue
// -----------------------------------------------------------------------------
//	
void CSIPIetfProfileContext::InitializeRetryTimerValue()
	{
	iRetryTimerUse = 0;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::ConnectionStateChangedImpl
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::ConnectionStateChangedImpl(
    CSIPConnection::TState /*aState*/)
    {
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::DeregisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::DeregisterL()
	{
	PROFILE_DEBUG4("CSIPIetfProfileContext::DeregisterL", ProfileId()\
		, iCurrentState->Name())
	iCurrentState->DeregisterL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::UpdateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::UpdateL(
	CSIPConcreteProfile& aNewProfile)
	{
	PROFILE_DEBUG4("CSIPIetfProfileContext::UpdateL", ProfileId()\
		, iCurrentState->Name())
	iCurrentState->UpdateL(*this, aNewProfile);
	}
	
// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::IncomingResponse()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfProfileContext::IncomingResponse(
    CSIPClientTransaction& aTransaction,
    CSIPRegistrationBinding& aRegistration,
    TBool& aHandled)
    {
    if (iClientTx && iRegistration && 
        aTransaction==*iClientTx && aRegistration==*iRegistration)
        {
        PROFILE_DEBUG3("SIPIetfProfileContext::IncomingResponse", ProfileId())
        aHandled = ETrue;
        const CSIPResponseElements* response = aTransaction.ResponseElements();
        TInt responseCode  = KErrGeneral;
        TBool retry = EFalse;
        if (response)
            {
                responseCode = response->StatusCode();
                
                retry = RetryRegister( &aTransaction,  responseCode);
                if( !retry )
                {
                    if (responseCode >= K300MultipleChoices)
                    {
                    PROFILE_DEBUG1("IETFProfileContext: registration failed")
                    RetryPossible(responseCode);
                    }
                    else
                    {
                        if (responseCode >= K200Ok)
                        {
                        PROFILE_DEBUG1("IETFProfileContext: registration complete")
                        iRetryCounter = 0;
                        iRetryCounterSum = 0;
                        Received2XXRegisterResponse();
                        }
                    }
                }
            }
        
            if( retry )
            {
            iCurrentState->ErrorOccured(*this, responseCode);
            }
            else
            {
            iCurrentState->ResponseReceived(*this, aTransaction);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::RandomPercent()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CSIPIetfProfileContext::RandomPercent()
	{
	const TInt rand = Math::Rand(iRandomSeed);
	TInt result = 0;
	result = KMaxRandomLimit + (rand%KMaxRandomLimit);
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::DelayTime()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CSIPIetfProfileContext::DelayTime()
	{
	PROFILE_DEBUG1("CSIPIetfProfileContext::DelayTime")
	TReal res;
	iRetryCounter++;
	iRetryCounterSum++;
	Math::Pow(res,KWaitTimeCoefficient,iRetryCounter);
	TUint respond = res;
	TUint waitTime = KBaseTimeInSec*respond;
	if (waitTime > 1800)
		{
		waitTime = 1800;
		}
	return ((waitTime*RandomPercent())/KRandomDivider);
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::RetryTimerInUse()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileContext::RetryTimerInUse()
	{
	return iRetryTimerUse;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::IsAddressSIPSURIL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileContext::IsAddressSIPSURIL(const TDesC8& aValue) const
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrArgument));
    TUriParser8 uriParser;
    User::LeaveIfError(uriParser.Parse(aValue));
    TPtrC8 sips(SIPStrings::StringF(SipStrConsts::ESips).DesC());
    return (uriParser.Extract(EUriScheme).CompareF(sips) == 0);
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::SetAddressToSIPSURIL()
// -----------------------------------------------------------------------------
//
HBufC8* CSIPIetfProfileContext::SetAddressToSIPSURIL(const TDesC8& aValue)
	{
    TUriParser8 uriParser;
    User::LeaveIfError(uriParser.Parse(aValue));
    CUri8* uri = CUri8::NewLC(uriParser);	
	uri->SetComponentL(
	    SIPStrings::StringF(SipStrConsts::ESips).DesC(),EUriScheme);
	HBufC8* value = uri->Uri().UriDes().AllocL();
	CleanupStack::PopAndDestroy(uri);
	return value;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::SetTlsToSecurityClientL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileContext::SetTlsToSecurityClientL()
	{
	TBool tlsSet = EFalse;
	if(!IsAddressSIPSURIL(iProfile->AOR()) && 
	   iProfile->IsSecurityNegotiationEnabled())
		{
		const TDesC8& proxyAddr = ProxyAddressL();
		if (proxyAddr.Length() > 0)
			{
			if(IsConfiguredOutboundProxySIPURIL(proxyAddr)|| 
			   IsDynamicNumericIPAddress(proxyAddr))
				{
				tlsSet = ETrue;
				}
			}
		}
	return tlsSet;
	}
	
// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::IsDynamicNumericIPAddress()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileContext::IsDynamicNumericIPAddress( const TDesC8& aValue )
	{
	return ( IsProxyResolvingEnabled() && 
		 !( UriUtils::HostType( aValue ) == UriUtils::ETextHost ) );
	}	

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::IsConfiguredOutboundProxySIPURIL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfProfileContext::IsConfiguredOutboundProxySIPURIL( const TDesC8& aValue )
	{
	return ( !IsProxyResolvingEnabled() && !IsAddressSIPSURIL(aValue) );
	}	

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::CreateMsgElementsForUpdateLC
// add SIP headers to REGISTER request
// -----------------------------------------------------------------------------
//
CSIPMessageElements* CSIPIetfProfileContext::CreateMsgElementsForUpdateLC(
    CSIPConcreteProfile& aProfile)
	{
	CSIPMessageElements* elements = CSIPMessageElements::NewLC();
	
	RPointerArray<CSIPHeaderBase> headers;
	CSIPHeaderBase::PushLC(&headers);

	TInt sipHeaderCount = aProfile.SIPHeaders().MdcaCount();
	for (TInt i=0; i < sipHeaderCount; i++)
	    {
	    TPtrC8 headerDes(aProfile.SIPHeaders().MdcaPoint(i));
	    CSIPHeaderBase* header = SIPMessageBuilder::CreateHeaderLC(headerDes);
	    headers.AppendL(header);
	    CleanupStack::Pop(header);
	    }   
	    
	elements->SetUserHeadersL(headers); // An empty array is also allowed
	CleanupStack::Pop(1); // headers
	    
	return elements;
	}

// -----------------------------------------------------------------------------
// CSIPIetfProfileContext::TransportProtocol
// -----------------------------------------------------------------------------
//
RStringF CSIPIetfProfileContext::TransportProtocol(const CUri8& aUri) const
    {
    if (aUri.Uri().Extract(EUriPath).FindF(KTransportUdpParam) >= 0)
        {
        return SIPStrings::StringF(SipStrConsts::EUdp);
        }
    if (aUri.Uri().Extract(EUriPath).FindF(KTransportTcpParam) >= 0)
        {
        return SIPStrings::StringF(SipStrConsts::ETcp);
        }
    if (aUri.Uri().Extract(EUriPath).FindF(KTransportTlsParam) >= 0)
        {
        return SIPStrings::StringF(SipStrConsts::ETLS);
        }
    return SIPStrings::StringF(SipStrConsts::EEmpty);
    }
