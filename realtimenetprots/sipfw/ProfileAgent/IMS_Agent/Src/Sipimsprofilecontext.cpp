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
// Name        : sipimsprofilecontext.cpp
// Part of     : sip ims agent
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "sipimsprofilecontext.h"
#include "sipprflstatebase.h"
#include "sipconcreteprofile.h"
#include "sipmanagedprofile.h"
#include "sipimsprofileagent.h"
#include "sipregistrationbinding.h"
#include "sipclienttransaction.h"
#include "sipgendefs.h"
#include "siprefresh.h"
#include "sipmessagebuilder.h"
#include "sipaddress.h"
#include "siptoheader.h"
#include "siprouteheader.h"
#include "sipmessageelements.h"
#include "sipresponseelements.h"
#include "sipsecurityclientheader.h"
#include "sipextensionheader.h"
#include "sippassociateduriheader.h"
#include "sipsupportedheader.h"
#include "SipProfileLog.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"
#include "sip.h"
#include "sipcontactheader.h"
#include "CSIPRegEventHandler.h"
#include "sipdialogassocbase.h"
#include "Sipimsprofilesimrecord.h"
#include "sipunsupportedheader.h"
#include "sipretryafterheader.h"

_LIT8(KSIPIpSec3gpp, "ipsec-3gpp");
_LIT8(KTransportUdpParam, "transport=udp");
_LIT8(KTransportTcpParam, "transport=tcp");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext* CSIPIMSProfileContext::NewLC(
    CSIPNotifyXmlBodyParser& aXMLParser,
	CSIP& aSIP,
	CSIPIMSConnectionContext& aConnection, 
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer,
	CSIPProfileSIMRecord& aSIMRecord,
	CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPIMSProfileContext* self = 
	    new(ELeave)CSIPIMSProfileContext(aXMLParser, aSIP, aConnection, 
	        aObserver, aInitState, aProfile, aDeltaTimer, aSIMRecord,aConfigExtension);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext* CSIPIMSProfileContext::NewL(
    CSIPNotifyXmlBodyParser& aXMLParser,
	CSIP& aSIP,
	CSIPIMSConnectionContext& aConnection, 
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer,
	CSIPProfileSIMRecord& aSIMRecord,
	CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPIMSProfileContext* self = 
	    CSIPIMSProfileContext::NewLC(aXMLParser, aSIP, aConnection, aObserver, 
	        aInitState, aProfile, aDeltaTimer, aSIMRecord,aConfigExtension);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CSIPIMSProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext::CSIPIMSProfileContext(
    CSIPNotifyXmlBodyParser& aXMLParser,
	CSIP& aSIP,
	CSIPIMSConnectionContext& aConnection,
	MSIPProfileAgentObserver& aObserver,
	CSIPPrflStateBase& aInitState,
	CSIPConcreteProfile& aProfile,
	CDeltaTimer& aDeltaTimer,
	CSIPProfileSIMRecord& aSIMRecord,
	CSipProfileAgentConfigExtension& aConfigExtension)
	: CSIPProfileContextBase(aSIP,
	                         *(aConnection.Connection()),
	                         aObserver,
	                         aInitState,
	                         aProfile,
	                         aDeltaTimer),	
	  iXMLParser(aXMLParser),
	  iConnectionContext(aConnection),
	  iSIMRecord(aSIMRecord),
	  iConnectionDropped(EFalse),
	  iDeactivatedByNetwork(EFalse),
	  iRetriedRegister(EFalse),
	  iRetryTimerUse(EFalse),
	  iUseDynamicProxyForTry(EFalse),
	  iConfigExtension(aConfigExtension)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ConstructL()
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::ConstructL()
    {
    iPathStr = SIPStrings::Pool().OpenFStringL(KSIPpath());
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::~CSIPIMSProfileContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext::~CSIPIMSProfileContext()
	{
	delete iRegEventHandler;
	delete iPAURI;
	iPathStr.Close();
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::AgentObserver()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MSIPProfileAgentObserver& CSIPIMSProfileContext::AgentObserver() const
	{
	const MSIPProfileAgentObserver& obs = *this;
	return const_cast<MSIPProfileAgentObserver&>(obs);
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::DestroyRegistration()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::DestroyRegistration()
	{
	PROFILE_DEBUG3("SIPIMSProfileContext::DestroyRegistration", ProfileId())
	delete iRegistration;
	iRegistration = NULL;
	RemoveRegEventHandler();
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CreateRegistrationL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::CreateRegistrationL()
	{
	ASSERT(iProfile);
    RStringF transport(SIPStrings::StringF(SipStrConsts::EEmpty));
    CleanupClosePushL(transport);	
	TUint pops = 0;
	CSIPRouteHeader* proxy = 0;
	CUri8* remoteUri = 0;
	CSIPRefresh* refresh = CSIPRefresh::NewLC();
	pops++;
	CSIPToHeader* to = 0;
	if (iConfiguredType == CSIPIMSProfileContext::EEarlyIMSType)
		{
		to = SIPMessageBuilder::CreateToLC(iSIMRecord.SIMPublicIdentity());
		}
		
	else if (iConfiguredType == CSIPIMSProfileContext::EIMSReleaseType)
		{
		to = SIPMessageBuilder::CreateToLC(iSIMRecord.PublicIdentity());
		}	
	else 
		{
		//CSIPIMSProfileContext::EClientConfiguredType
		to = SIPMessageBuilder::CreateToLC(iProfile->AOR());
		}
	pops++;
    
	const TDesC8& proxyAddr = ProxyAddressL();

	if (proxyAddr.Length()) //there's an outbound proxy defined
		{
		(iProfile->IsSigCompEnabled())?
		proxy = SIPMessageBuilder::CreateRouteLC(proxyAddr, ETrue):
		proxy = SIPMessageBuilder::CreateRouteLC(proxyAddr);
		pops++;	
		RStringF proxyTransport = TransportProtocol(proxy->SIPAddress().Uri8());	
		if (proxyTransport.DesC().Length() > 0)
			{
			transport = proxyTransport.Copy();
			}		
		}
	if (iConfiguredType == CSIPIMSProfileContext::EClientConfiguredType)
	    {
		if (iProfile->Server(KSIPRegistrar).Length()) // registrar defined
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
	    }
	else
		{
		HBufC8* temp = NULL;
		if (iConfiguredType == CSIPIMSProfileContext::EEarlyIMSType)
			{
		 	temp = iSIMRecord.AddSIPPrefixLC(
		 	    iSIMRecord.SIMHomeNetworkDomainName());
			}			
		if (iConfiguredType == CSIPIMSProfileContext::EIMSReleaseType)
			{
			temp = iSIMRecord.AddSIPPrefixLC(
			    iSIMRecord.HomeNetworkDomainName());
			}
	    if (temp)
	        {
			TUriParser8 parser;
			User::LeaveIfError(parser.Parse(*temp));
            remoteUri = CUri8::NewL(parser);
            CleanupStack::PopAndDestroy(temp);
            CleanupStack::PushL(remoteUri);
			pops++;
	        }
		}
	TBool sigcomp = iProfile->IsSigCompEnabled();

	const TDesC8* user = NULL;
	User::LeaveIfError(
		iProfile->ExtensionParameter(KSIPContactHeaderUser,user));	
	
	CSIPContactHeader* contact =
	    SIPMessageBuilder::CreateContactLC(*user,
	    		iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EIms,
				CSipProfileAgentConfigExtension::EProfileRegistrationValue),
	    		iProfile->ContactHeaderParams(),
	    		transport, sigcomp);
	pops++;
	
	iRegistration = 
	    CSIPRegistrationBinding::NewL(*iConnectionContext.Connection(), 
		                              to, contact, refresh, proxy, remoteUri);
	CleanupStack::Pop(pops);
	CleanupStack::PopAndDestroy(1); // transport
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CreateMsgElementsLC()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPMessageElements* CSIPIMSProfileContext::CreateMsgElementsLC()
	{
	ASSERT(iProfile);
	CSIPMessageElements* elements = CSIPMessageElements::NewLC();
    RPointerArray<CSIPHeaderBase> headers(CreateSIPHeadersL(*iProfile));
	CSIPHeaderBase::PushLC(&headers);

	if(iConfiguredType != CSIPIMSProfileContext::EEarlyIMSType)
		{
		CSIPExtensionHeader* authorization= NULL;
		if(iConfiguredType == CSIPIMSProfileContext::EClientConfiguredType)
			{
			authorization=
				(iProfile->PrivateIdentity().Length()>0)?
			    SIPMessageBuilder::CreateAuthorizationLC(
				    iProfile->Server(KSIPRegistrar),
				    iProfile->ServerParameter(KSIPRegistrar, KSIPDigestRealm),
				    iProfile->PrivateIdentity())
			    :
			    SIPMessageBuilder::CreateAuthorizationLC(
				    iProfile->Server(KSIPRegistrar),
				    iProfile->ServerParameter(KSIPRegistrar, KSIPDigestRealm),
				    iProfile->ServerParameter(KSIPRegistrar, KSIPDigestUserName));	
			}
		else
			{
			HBufC8* temp = 
			    iSIMRecord.AddSIPPrefixLC(iSIMRecord.HomeNetworkDomainName());
			authorization=
			    SIPMessageBuilder::CreateAuthorizationLC(
				    *temp,
				    iSIMRecord.HomeNetworkDomainName(),
				    iSIMRecord.PrivateIdentity());
			CleanupStack::Pop(authorization);
			CleanupStack::PopAndDestroy(temp);
			CleanupStack::PushL(authorization);
			}
		headers.AppendL(authorization);
		CleanupStack::Pop(authorization);
		}

	CSIPSupportedHeader* supported = CSIPSupportedHeader::NewLC(iPathStr);
	headers.AppendL(supported);
	CleanupStack::Pop(supported);
	AddSecurityClientHeaderL(headers,*iProfile);
    elements->SetUserHeadersL(headers);
	CleanupStack::PopAndDestroy(&headers); 
	   
	return elements;
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CreateDeRegisterElementsL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPMessageElements* CSIPIMSProfileContext::CreateDeRegisterElementsL()
	{
	ASSERT(iProfile);	
	CSIPMessageElements* elements = CSIPMessageElements::NewLC();
    RPointerArray<CSIPHeaderBase> headers(CreateSIPHeadersL(*iProfile));
	CSIPHeaderBase::PushLC(&headers);	
	
	if (iConfiguredType == CSIPIMSProfileContext::EClientConfiguredType)
		{
		CSIPExtensionHeader* authorization= NULL;
		if(iProfile->PrivateIdentity().Length()>0)
			{
			authorization = SIPMessageBuilder::CreateAuthorizationLC(
				iProfile->Server(KSIPRegistrar),
				iProfile->ServerParameter(KSIPRegistrar, KSIPDigestRealm),
				iProfile->PrivateIdentity());
			}
		else
			{
			authorization =	SIPMessageBuilder::CreateAuthorizationLC(
				iProfile->Server(KSIPRegistrar),
				iProfile->ServerParameter(KSIPRegistrar, KSIPDigestRealm),
				iProfile->ServerParameter(KSIPRegistrar, KSIPDigestUserName));
			}
		headers.AppendL(authorization);
		CleanupStack::Pop(authorization);
		}

	AddSecurityClientHeaderL(headers,*iProfile);
    elements->SetUserHeadersL(headers);
	CleanupStack::PopAndDestroy(&headers); 
	CleanupStack::Pop(elements);
   
	return elements;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::UpdateContactHeaderParamsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::UpdateContactHeaderParamsL(
	CSIPConcreteProfile& aNewProfile)
	{
	ASSERT(iProfile);	
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
		CleanupStack::PopAndDestroy();//name
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
			CleanupStack::PopAndDestroy(); // value
			}
		else
			{
			contact.SetParamL(name);	
			}
		CleanupStack::PopAndDestroy(); // name	
		}
	
	CSIPMessageElements* elements = CreateMsgElementsForUpdateLC(aNewProfile);
	SetTransaction(Registration()->UpdateL(
			iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EIms,
			CSipProfileAgentConfigExtension::EProfileRegistrationValue),
			elements));
	CleanupStack::Pop(elements);			
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RegEventSubscriptionActive()
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::RegEventSubscriptionActive()
    {
    // Reg-event failure notifications after this must not lead to registration 
    // (see CSIPIMSProfileContext::RegEventSubscriptionFailedL)
    iConnectionDropped = EFalse;
    iDeactivatedByNetwork = EFalse;
    if (iProfile && iRegistration)
        {
        iObserver.SIPProfileStatusEvent(*iProfile,iRegistration->ContextId());
        }
    }
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ExpirationTimeUpdatedL()
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::ExpirationTimeUpdatedL(TInt aExpirationTime)
	{
	__ASSERT_ALWAYS(aExpirationTime > 0 , User::Leave(KErrArgument));
	__ASSERT_ALWAYS(iRegistration != NULL, User::Leave(KErrNotReady));
	
	CSIPRefresh* refresh = iRegistration->SIPRefresh();
	if(refresh)
		{
		refresh->SetIntervalL(aExpirationTime);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ReRegister()
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::ReRegister()
    {
    TBool handled = ETrue;
    CSIPProfileContextBase::ErrorOccured(KErrTotalLossOfPrecision,
                                         *Registration(),handled);    
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RegistrationDeactivated()
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::RegistrationDeactivated()
    {
    if (!iDeactivatedByNetwork)
        {
        // For the first deactivation indicate with an error 
        // that an initial reqistration must be performed
        iDeactivatedByNetwork = ETrue;
        ReRegister();     
        }
    else
        {
        // If the network deactivates registration twice in a row,
        // the profile registration fails
        RegistrationTerminated();
        }
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RegistrationTerminated()
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::RegistrationTerminated()
    {
    iCurrentState = iInitState;
    SetRetryPossible(EFalse);
    // A specific error code for network initiated de-registration
    iObserver.SIPProfileErrorEvent(*iProfile,KErrDisconnected);
    }
     
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RegEventSubscriptionFailedL()
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::RegEventSubscriptionFailedL()
    {
    if (iConnectionDropped)
        {
        // Perform an initial registration again
        iConnectionDropped = EFalse;
        ReRegister();
        }
    else
        {
        iRegEventSubscriptionFailed = ETrue;
        iCurrentState->DeregisterL(*this);
        RemoveRegEventHandler();       
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::SIPProfileStatusEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::SIPProfileStatusEvent(
	CSIPConcreteProfile& aProfile,
	TUint32 aContextId)
	{
	PROFILE_DEBUG4("CSIPIMSProfileContext::SIPProfileStatusEvent",\
		           aProfile.Id(), aContextId)
	if (CurrentState() == MSIPProfileContext::ERegistered)
		{
        TRAPD(err, HandleProfileRegisteredEventL())
        if (err)
            {
            RemoveRegEventHandler();           
            SIPProfileErrorEvent(aProfile,err);
            return;
            }
        // Defer the notification about the profile state change 
        // until the subscription to reg-event is active.
        // See CSIPIMSProfileContext::RegEventSubscriptionActive.
		}
    else
        {
	    if (iRegEventSubscriptionFailed &&
	        CurrentState() == MSIPProfileContext::EInit)
	        {
	        // Registration state event subscription has failed,
	        // de-REGISTER was sent and 
	        // 200 OK for de-REGISTER was just received.
	        // Indicate profile registration status as failed.
	        iRegEventSubscriptionFailed = EFalse;
	        iObserver.SIPProfileErrorEvent(aProfile,KErrGeneral);
	        }
	    else
	        {
	        iObserver.SIPProfileStatusEvent(aProfile,aContextId);
	        }        
        }
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::SIPProfileErrorEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::SIPProfileErrorEvent(
    CSIPConcreteProfile& aProfile,
    TInt aError)
	{
	PROFILE_DEBUG4("CSIPIMSProfileContext::ErrorEvent", aProfile.Id(), aError)
	iObserver.SIPProfileErrorEvent(aProfile,aError);
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ProceedRegistration
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::ProceedRegistration(
	CSIPConcreteProfile& aProfile,
	TInt aError)
	{
	PROFILE_DEBUG3("CSIPIMSProfileContext::ProceedRegistration", aProfile.Id())
	return iObserver.ProceedRegistration(aProfile, aError);
	}    			

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::GetFailedProfilesL
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::GetFailedProfilesL(
    const TSIPProfileTypeInfo& /*aType*/,
    RPointerArray<CSIPConcreteProfile>& /*aFailedProfiles*/) const
	{
	}  

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::DeregisterL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::DeregisterL()
	{
	PROFILE_DEBUG4("CSIPIMSProfileContext::DeregisterL", ProfileId()\
		, iCurrentState->Name())
	iCurrentState->DeregisterL(*this);
	RemoveRegEventHandler();
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::UpdateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::UpdateL(
	CSIPConcreteProfile& aNewProfile)
	{
	PROFILE_DEBUG4("CSIPIMSProfileContext::UpdateL", ProfileId()\
		, iCurrentState->Name())
	iCurrentState->UpdateL(*this, aNewProfile);
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IncomingResponse()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::IncomingResponse(
	CSIPClientTransaction& aTransaction,
	CSIPRegistrationBinding& aRegistration,
	TBool& aHandled)
	{
	TUint status=0;
	if (iClientTx && iRegistration && 
		aTransaction==*iClientTx && aRegistration==*iRegistration)
		{
		PROFILE_DEBUG3("SIPIMSProfileContext::IncomingResponse", ProfileId())
		aHandled = ETrue;
		TBool retry = EFalse;
		if (aTransaction.ResponseElements())
			{
			status = aTransaction.ResponseElements()->StatusCode();
			retry = RetryRegister( &aTransaction,  status);
			if ( !retry )
				{
				if (status == K420BadExtension && 
			    	iConfiguredType == CSIPIMSProfileContext::EIMSReleaseType && 
					IsUnsupportedHeaderValue(
				    	SIPStrings::StringF(SipStrConsts::ESecAgreeTag)))
					{
					TUint32 confValue = 0;
					TInt result = 0;
					result = iProfile->ExtensionParameter( KSIPAllowIMSRegistration,
													confValue );
					if (result != KErrNotFound)
						{
						if (confValue == 1 ||confValue == 2)
							{
							SetRetryPossible(EFalse);
							}
						}
					else
						{
						iConfiguredType = CSIPIMSProfileContext::EEarlyIMSType;
					   	SetRetryPossible(ETrue);
						}
					}
				else if (RequireProxyResolving())
					{
					if (status >= K200Ok && status < K300MultipleChoices)
						{
						delete iProxies;
						iProxies = NULL;
						iUseDynamicProxyForTry = ETrue;
				    	Received2XXRegisterResponse();
						}
					else
						{
						PROFILE_DEBUG1("SIPIMSProfileContext::registration failed")
						RetryPossible(status);
						}
					}
		    	else
		        	{
		        	// Silence PC-Lint
		        	}
		        }
			}
		
		if (retry)
			{
			iCurrentState->ErrorOccured(*this, status);
			}
		else
			{
			TRAPD(err, StoreMsgElementsL())
			if (err)
		    	{
		    	CSIPProfileContextBase::ErrorOccured(err,aRegistration,aHandled);
		    	}
			else
		    	{
		    	iCurrentState->ResponseReceived(*this, aTransaction);
		    	}
			}		
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RetryRegister()
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::RetryRegister(
    CSIPClientTransaction* aTransaction,
    TInt aError )
	{
	TBool retry = EFalse;
	if (iProfile &&
	    AgentObserver().ProceedRegistration(*iProfile, aError) &&
	   (!iRetriedRegister && 
	   (CurrentState() == MSIPProfileContext::ERegistrationInProgress || 
	   CurrentState() == MSIPProfileContext::ERegistered)))
		{
		if ( aError == K503ServiceUnavailable || 
		     aError == K408TimeOut ||
			 aError == K480TemporarilyUnavailable || 
			 aError == K500ServerInternalError || 
			 aError == K504ServerTimeOut || 
			 aError == KErrTimedOut ||
			 ((aError == KErrSIPOutboundProxyNotResponding || 
			 aError == KErrSIPResolvingFailure ||
			 aError == KErrSIPTransportFailure ||
			 aError == KErrSIPICMPFailure  )&&
			 iConnection.State() != CSIPConnection::ESuspended))
			{
			SetRetryPossible(ETrue);
			iRetriedRegister = ETrue;
			iRetryCounterSum++;
			retry = ETrue;
			if ( aTransaction )
				{
				CheckRetryAfter( aTransaction );
				}
			}
		}
	else
		{
		SetRetryPossible(EFalse);
		iRetriedRegister = EFalse;
		iRetryTimerUse = EFalse;
		iRetryCounterSum = 0;
		}
	return retry;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ShouldRetryRegistration
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileContext::ShouldRetryRegistration( TInt aError )
	{
	return (iProfile &&
	        AgentObserver().ProceedRegistration(*iProfile, aError) &&
	        (aError == K503ServiceUnavailable ||
	        aError == K408TimeOut ||
	        aError == K480TemporarilyUnavailable ||
	        aError == K500ServerInternalError ||
	        aError == K504ServerTimeOut || 
		    aError == KErrTimedOut ||
		    ((aError == KErrSIPResolvingFailure || 
		      aError == KErrSIPTransportFailure ||
		      aError == KErrSIPICMPFailure ||
		      aError == KErrSIPOutboundProxyNotResponding ) && 
			 iConnection.State() != CSIPConnection::ESuspended)));
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RetryAfterTimer
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileContext::RetryAfterTimer()
	{
	return ( iRetryCounterSum == 1 && iRetriedRegister );
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ResolveProxyL
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::ResolveProxyL()
    {
    iConnectionContext.ResolveL();
    }
    
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CancelProxyResolving
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::CancelProxyResolving()
    {
    iConnectionContext.CleanProxyResolving();
    }
 
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ProxiesAlreadyResolved()
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::ProxiesAlreadyResolved()
    {
    if ( iProfile->DynamicProxy() != KNullDesC8 &&
         iProxies == NULL &&
         iUseDynamicProxyForTry)
    	{
    	iUseDynamicProxyForTry = EFalse;
    	return ETrue;
    	}
    return (iProxies && iProxies->MdcaCount() > 0);
    }
  
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CheckRetryAfter
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::CheckRetryAfter( CSIPClientTransaction* aTransaction )
	{
	iRetryAfterTime = 0;
	if ( aTransaction && aTransaction->ResponseElements() )
		{
		const RPointerArray<CSIPHeaderBase>& headers =
			aTransaction->ResponseElements()->MessageElements().UserHeaders();
		TBool firstfound = EFalse;
		for ( TInt i=0; i < headers.Count() && !firstfound; i++ )
			{
			if (headers[i]->Name() == 
			    SIPStrings::StringF( SipStrConsts::ERetryAfter ))
				{
				iRetryTimerUse = ETrue;
				CSIPRetryAfterHeader* retryAfter = 
					(static_cast<CSIPRetryAfterHeader*>(headers[i]));
				iRetryAfterTime = retryAfter->RetryAfter();
				firstfound = ETrue;
				}
			}	
		}
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RetryTimerInUse()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::RetryTimerInUse()
	{
	return (iRetryTimerUse && iRetryAfterTime);
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RetryAfterTime()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CSIPIMSProfileContext::RetryAfterTime()
	{
	return iRetryAfterTime;
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::InitializeRetryTimerValue()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::InitializeRetryTimerValue()
	{
	iRetryAfterTime = 0;
	iRetryTimerUse = EFalse;
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::HasTransaction
// From CSIPProfileContextBase
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::HasTransaction(
    const CSIPClientTransaction& aTransaction) const
    {
    if (iRegEventHandler && iRegEventHandler->HasTransaction(aTransaction))
        {
        return ETrue;
        }
    return CSIPProfileContextBase::HasTransaction(aTransaction);
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::HasRefresh
// From CSIPProfileContextBase
// -----------------------------------------------------------------------------
//    
TBool CSIPIMSProfileContext::HasRefresh(const CSIPRefresh& aRefresh) const
    {
    if (iRegEventHandler && iRegEventHandler->HasRefresh(aRefresh))
        {
        return ETrue;
        }
    return CSIPProfileContextBase::HasRefresh(aRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::RegistrarUsername
// From CSIPProfileContextBase
// -----------------------------------------------------------------------------
//		
const TDesC8& CSIPIMSProfileContext::RegistrarUsername() const
    {
    if (iProfile)
        {
        if (iProfile->PrivateIdentity().Length() > 0)
            {
            return iProfile->PrivateIdentity();
            }
        else
            {
            return iProfile->ServerParameter(KSIPRegistrar, KSIPDigestUserName);
            }
        }
    return KNullDesC8;
    }	
		
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IncomingRequest()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::IncomingRequest(
    CSIPServerTransaction* aTransaction,
    CSIPDialog& aDialog,
	TBool& aHandled)
	{
	if(iRegEventHandler && iRegEventHandler->HasDialog(aDialog))
		{
		TRAPD(err, iRegEventHandler->RequestReceivedL(aTransaction, aDialog));
		if (err)
		    {
		    delete aTransaction;
		    }
		aHandled = ETrue;
		}
	}	
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IncomingResponse()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::IncomingResponse(
    CSIPClientTransaction& aTransaction,
    TBool& aHandled)
	{
	if(iRegEventHandler && iRegEventHandler->HasTransaction(aTransaction))
		{
		TRAP_IGNORE(iRegEventHandler->ResponseReceivedL(aTransaction))
		aHandled = ETrue;
		}
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IncomingResponse()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::IncomingResponse(
    CSIPClientTransaction& aTransaction,
    CSIPDialogAssocBase& aDialogAssoc,
    TBool& aHandled)
	{
	if(iRegEventHandler && iRegEventHandler->HasDialog(aDialogAssoc.Dialog()))
		{
		TRAP_IGNORE(iRegEventHandler->ResponseReceivedL(aTransaction))
		aHandled = ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ErrorOccured()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::ErrorOccured(
    CSIPDialogAssocBase& aDialogAssoc,
    TInt aError,
    TBool& aHandled)
	{
	if(iRegEventHandler && iRegEventHandler->HasDialog(aDialogAssoc.Dialog()))
		{
		iRegEventHandler->ErrorOccured(aError,aDialogAssoc.Dialog());
		aHandled = ETrue;
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ErrorOccured()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::ErrorOccured( 
    CSIPTransactionBase& aTransaction,
    CSIPDialogAssocBase& aDialogAssoc,
    TInt aError,
    TBool& aHandled)
	{
	if(iRegEventHandler && iRegEventHandler->HasDialog(aDialogAssoc.Dialog()))
		{
	    iRegEventHandler->ErrorOccured(aError,aTransaction);
		aHandled = ETrue;
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IsHttpDigestSettingsConfigured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::IsHttpDigestSettingsConfigured(
    const CSIPConcreteProfile& aProfile)
	{
	if ((aProfile.PrivateIdentity().Length() || 
	     aProfile.ServerParameter(KSIPRegistrar, KSIPDigestUserName).Length()) 
	     &&
	     aProfile.ServerParameter(KSIPRegistrar, KSIPDigestPassword).Length())
		{
		return ETrue;
		}
	return EFalse;
	}	
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IsSupportedSecurityMechanismL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSProfileContext::IsSupportedSecurityMechanismL(
    CSIP& aSIP, 
    const TDesC8& aValue)
	{
	TBool result = EFalse;
	CDesC8Array* mechanisms = aSIP.SupportedSecurityMechanismsL();
    CleanupStack::PushL(mechanisms);
	for (TInt i = 0; mechanisms && (i < mechanisms->MdcaCount()); i++)
		{
		if(mechanisms->MdcaPoint(i).Compare(aValue) == 0)
			{
			result = ETrue;
			}
		}
	CleanupStack::PopAndDestroy(mechanisms);
	return result;
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::SetConfiguredType()
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::SetConfiguredType(TConfiguredType aConfiguredType)
	{
	iConfiguredType = aConfiguredType;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ConfiguredType()
// -----------------------------------------------------------------------------
//	
CSIPIMSProfileContext::TConfiguredType CSIPIMSProfileContext::ConfiguredType()
	{
	return iConfiguredType;
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::ConnectionStateChangedImpl
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::ConnectionStateChangedImpl(
	CSIPConnection::TState aState)
	{
	PROFILE_DEBUG4("CSIPIMSProfileContext::ConnectionStateChangedImpl"
		,ProfileId(), aState)
    if (aState == CSIPConnection::EActive)
        {
        if (iRegEventHandler &&
	        iConnectionContext.PreviousSIPConnectionState() == 
	        CSIPConnection::ESuspended)
            {
            TRAP_IGNORE(iRegEventHandler->RefreshL())
            }
        }
    else
        {
        if (iConnectionContext.PreviousSIPConnectionState() == 
            CSIPConnection::EActive)
            {
            iConnectionDropped = ETrue;
            }
        }
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::SetRegisteredAORsL
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::SetRegisteredAORsL()
    {
    if (iProfile)
        {
        CDesC8ArrayFlat* array = new(ELeave)CDesC8ArrayFlat(1);
    	CleanupStack::PushL(array);    
    	if (!iPAURI)
    		{
	        if (iConfiguredType == 
	            CSIPIMSProfileContext::EClientConfiguredType ||
	            iSIMRecord.IsISIMPresent())
                {
                // SUBSCRIBE to reg-event with the AOR used in REGISTER
                TUriParser8 parser;
                if (iConfiguredType == CSIPIMSProfileContext::EIMSReleaseType)
                	{
                	User::LeaveIfError(parser.Parse(iSIMRecord.PublicIdentity()));
                	}
                else
                	{
                	User::LeaveIfError(parser.Parse(iProfile->AOR()));
                	}
			    TInt err = parser.Validate();
			    if (err != KErrNone && err != KErrNotSupported)
			        {
			        User::Leave(err);
			        }
                iPAURI = CUri8::NewL(parser);
                }
            else
                {
                // USIM or SIM used. 
                // Temporary ID cannot be used in reg-event subscription. 
                User::Leave(KErrNotSupported);
                }
		    }    	    
        array->AppendL(iPAURI->Uri().UriDes());
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
// CSIPIMSProfileContext::RemoveRegEventHandler
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::RemoveRegEventHandler()
    {
    delete iRegEventHandler;
    iRegEventHandler = NULL;
    }
	
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::StoreMsgElementsL
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::StoreMsgElementsL()
	{
	delete iPAURI;
	iPAURI = 0;	
	if (iClientTx && 
	    iClientTx->ResponseElements() &&
		iClientTx->ResponseElements()->StatusCode() >= K200Ok &&
		iClientTx->ResponseElements()->StatusCode() < K300MultipleChoices)
		{
		const RPointerArray<CSIPHeaderBase>& headers =
			iClientTx->ResponseElements()->MessageElements().UserHeaders();
		TBool firstfound = EFalse;
		for (TInt i=0; i < headers.Count() && !firstfound; i++)
			{
			if (headers[i]->Name() == 
			    SIPStrings::StringF(SipStrConsts::EPAssociatedURIHeader))
				{
				CSIPPAssociatedURIHeader* associatedURI = 
					(static_cast<CSIPPAssociatedURIHeader*>(headers[i]));
			    const TUriC8& uri = associatedURI->SIPAddress().Uri8().Uri();
			    TInt err = uri.Validate();
			    if (err != KErrNone && err != KErrNotSupported)
			        {
			        User::Leave(err);
			        }
				iPAURI = CUri8::NewL(uri);
				firstfound = ETrue;
				}
			}	
		}
	}	
		
// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::IsUnsupportedHeaderValue()
// -----------------------------------------------------------------------------
//	
TBool CSIPIMSProfileContext::IsUnsupportedHeaderValue(RStringF aValue)
	{
	const RPointerArray<CSIPHeaderBase>& headers =
			iClientTx->ResponseElements()->MessageElements().UserHeaders();
	for (TInt i = 0; i < headers.Count();i++)
		{
		if (headers[i]->Name() == 
				SIPStrings::StringF(SipStrConsts::EUnsupportedHeader))
			{
			CSIPUnsupportedHeader* unsupported = 
					(static_cast<CSIPUnsupportedHeader*>(headers[i]));
			if (unsupported->Value() == aValue)
				{
				return ETrue;
				}
			else
				{
				return EFalse;
				}
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::AddSecurityClientHeaderL
// -----------------------------------------------------------------------------
//
void CSIPIMSProfileContext::AddSecurityClientHeaderL(
    RPointerArray<CSIPHeaderBase>& aHeaders,
    const CSIPConcreteProfile& aProfile)
	{
	if (aProfile.IsSecurityNegotiationEnabled() &&
		iConfiguredType != CSIPIMSProfileContext::EEarlyIMSType)
		{
		TPtrC8 mechanism(KSIPIpSec3gpp);
	    if (IsHttpDigestSettingsConfigured(aProfile))
		    {
		    mechanism.Set(KSIPdigest);
		    }
	    CSIPSecurityClientHeader* header = 
	        CSIPSecurityClientHeader::NewLC(mechanism);
		aHeaders.AppendL(header);
		CleanupStack::Pop(header);
		}
	}	

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::HandleProfileRegisteredEventL
// -----------------------------------------------------------------------------
//	
void CSIPIMSProfileContext::HandleProfileRegisteredEventL()
    {
    if (iPAURI)
        {
        CSIPRegEventHandler* regEventHandler =  
            CSIPRegEventHandler::NewL(iXMLParser,
                                      iConnectionContext.LocalIPAddress(),
		                              iDeltaTimer,iConnection,
		                              *iRegistration,*iPAURI,*this,iConfigExtension);
        CleanupStack::PushL(regEventHandler);
        regEventHandler->SubscribeL();
        delete iRegEventHandler;
        iRegEventHandler = regEventHandler;
    	CleanupStack::Pop(regEventHandler);            
        delete iPAURI;
        iPAURI = 0;    
        }
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CreateMsgElementsForUpdateLC
// -----------------------------------------------------------------------------
//
CSIPMessageElements* CSIPIMSProfileContext::CreateMsgElementsForUpdateLC(
    CSIPConcreteProfile& aProfile)
	{
	CSIPMessageElements* elements = CSIPMessageElements::NewLC();
	RPointerArray<CSIPHeaderBase> headers(CreateSIPHeadersL(aProfile));
	CSIPHeaderBase::PushLC(&headers);
	AddSecurityClientHeaderL(headers,aProfile);
	elements->SetUserHeadersL(headers);
	CleanupStack::PopAndDestroy(&headers);	
	return elements;
	}

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::CreateSIPHeadersL
// -----------------------------------------------------------------------------
//	
RPointerArray<CSIPHeaderBase> CSIPIMSProfileContext::CreateSIPHeadersL(
    const CSIPConcreteProfile& aProfile)
    {
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
	    
	CleanupStack::Pop(&headers); 
	return headers;
    }

// -----------------------------------------------------------------------------
// CSIPIMSProfileContext::TransportProtocol
// -----------------------------------------------------------------------------
//
RStringF CSIPIMSProfileContext::TransportProtocol(const CUri8& aUri) const
    {
    RStringF transport(SIPStrings::StringF(SipStrConsts::EEmpty));
    if (aUri.Uri().Extract(EUriPath).FindF(KTransportUdpParam) >= 0)
        {
        transport = SIPStrings::StringF(SipStrConsts::EUdp); 
        }
    if (aUri.Uri().Extract(EUriPath).FindF(KTransportTcpParam) >= 0)
        {
        transport = SIPStrings::StringF(SipStrConsts::ETcp);
        }
    return transport;
    }
