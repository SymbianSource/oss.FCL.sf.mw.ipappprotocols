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
// Name          : TSIPRegistrationUtility.cpp
// Part of       : Registration
// Version       : SIP/4.0 
//



#include "TSIPRegistrationUtility.h"
#include "MTransactionUser.h"
#include "MSigComp.h"
#include "siprequest.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcontactheader.h"
#include "sipaddress.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include "sipexpiresheader.h"
#include "siprouteheader.h"
#include "CSIPRegistrationBindingBase.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"

const TInt KOneHourInSeconds = 3600;

#define HEADER_IN_REQUEST(H, N) static_cast<H*>(aRequest.Header(N,0))

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::LocalAddressLC
// -----------------------------------------------------------------------------
//
HBufC8* TSIPRegistrationUtility::LocalAddressLC (const TUint       aIapId,
                                                 MTransactionUser& aTU)
	{
	// get local IP address by giving IAP id
	TInetAddr localAddress;
	aTU.GetLocalAddress(aIapId, localAddress);

	// create Local IP address
	const TInt KMaxIPAddrLength = 256;
	HBufC* ipAddressOutputBuf = HBufC::NewLC(KMaxIPAddrLength);
	TPtr ipAddressOutputPtr(ipAddressOutputBuf->Des());
	localAddress.Output(ipAddressOutputPtr);

	HBufC8* outPutBuf = HBufC8::NewL(ipAddressOutputPtr.Length());
	TPtr8 outPutPtr(outPutBuf->Des());
	outPutPtr.Copy(ipAddressOutputPtr);
	CleanupStack::PopAndDestroy(ipAddressOutputBuf);
    CleanupStack::PushL(outPutBuf);

	return outPutBuf;
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckRegisterRequestL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckRegisterRequestL (
	CSIPRequest&           aRequest,
	const CSIPRouteHeader* aOutboundProxy,
    const TUint32          aIapId,
	MTransactionUser&      aTU)
	{
	// panic if there is no To header in request in debug mode.
	// leaves with error code KErrArgument in release mode.
	__SIP_ASSERT_LEAVE(aRequest.To() != 0, KErrArgument);
	
	// panic if RegisterRequestNoFromHeader in debug mode.
	// leaves with error code KErrArgument in release mode.
	__SIP_ASSERT_LEAVE(aRequest.From() != 0, KErrArgument);

	CheckToHeaderL(*(aRequest.To()));
	CheckRegisterContactL(aRequest, aIapId, aTU);
	CheckRegisterExpiresValueL(aRequest);
	if (aOutboundProxy)
	    {
	    CheckSipsSchemeInAorAndOtherAddressL(
	        *aRequest.To(),aOutboundProxy->SIPAddress());
	    }
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckUpdateRegisterRequestL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckUpdateRegisterRequestL (
	CSIPRequest&                 aRequest, 
	CSIPRegistrationBindingBase& aBinding)
	{
	// panic if there is to header in update register request in debug
	// mode. leaves with error code KErrArgument in release build
    __SIP_ASSERT_LEAVE(!aRequest.To(), KErrArgument);

	// panic if there is from header in update register request in debug
	// mode. leaves with error code KErrArgument in release build
    __SIP_ASSERT_LEAVE(!aRequest.From(), KErrArgument);

	CheckUpdateRegisterContactL(aRequest, aBinding);
	CheckUpdateRegisterExpiresValueL(aRequest, aBinding);
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckUnregisterRequestL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckUnregisterRequestL (
	CSIPRequest&                 aRequest, 
	CSIPRegistrationBindingBase& aBinding)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);	
	
	// panic if there is to header in unregister request in debug mode,
	// Leaves with error code KErrArgument in release mode
    __SIP_ASSERT_LEAVE(!aRequest.To(), KErrArgument);

	// panic if there is from header in unregister request in debug mode,
	// Leaves with error code KErrArgument in release mode
    __SIP_ASSERT_LEAVE(!aRequest.From(), KErrArgument);

	// panic if there is contact header in unregister request in debug mode,
	// Leaves with error code KErrArgument in release mode
    __SIP_ASSERT_LEAVE(!aRequest.HasHeader(KContactHeaderName),
                       KErrArgument);

    // fill in contact header
	CSIPContactHeader* contact = 
		static_cast<CSIPContactHeader*>(aBinding.Contact().CloneL());
    CleanupStack::PushL(contact);
	contact->SetExpiresParameterL(0); //set expires parameter to 0
	aRequest.AddHeaderL(contact);
	CleanupStack::Pop(contact);
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckFetchingRequestL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckFetchingRequestL (CSIPRequest& aRequest)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);	
	
	// panic if there is contact header in binding in debug mode.
	// leaves with error code KErrArgument in release mode.
    __SIP_ASSERT_LEAVE(!(aRequest.HasHeader(KContactHeaderName)), 
		               KErrArgument);

	CheckToHeaderL (*(aRequest.To()));
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckOutboundProxyCompParameterL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckOutboundProxyCompParameterL(
	CSIPRouteHeader& aOutboundProxy,
	MSigComp&        aSigComp)
	{
	if (aOutboundProxy.SIPAddress().URI().IsSIPURI())
	    {
	    CSIPURI& uri = *(aOutboundProxy.SIPAddress().URI().SIPURI());
	
	    // when the uri of outboundProxy contains "comp" parameter and
	    // comp == "sigcomp", leave if sigComp asked from MSigComp 
	    // is not supported.
        RStringF comp = SIPStrings::StringF(SipStrConsts::EComp);
        RStringF sigcomp = SIPStrings::StringF(SipStrConsts::ESigComp);
	    if (uri.ParamValue(comp) == sigcomp && !aSigComp.IsSupported())
		    {
		    User::Leave(KErrNotSupported);
		    }
	    }
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckToHeaderL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckToHeaderL(CSIPToHeader& aTo)
	{
	CURIContainer& uri = aTo.SIPAddress().URI();
    if (uri.IsSIPURI())
        {
	    __ASSERT_ALWAYS(uri.SIPURI()->User().Length() > 0, 
	                    User::Leave(KErrArgument));
        }
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckRegisterContactL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckRegisterContactL(
    CSIPRequest&      aRequest,
	const TUint32     aIapId,
    MTransactionUser& aTU)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);	

	__SIP_ASSERT_LEAVE(aRequest.HeaderCount(KContactHeaderName) == 1,
		               KErrArgument);

	CSIPContactHeader* contact = 
		HEADER_IN_REQUEST(CSIPContactHeader, KContactHeaderName);

	__SIP_ASSERT_LEAVE(!contact->Star(), KErrArgument);
	__SIP_ASSERT_LEAVE(contact->SIPAddress() != 0, KErrArgument);

    CheckSipsSchemeInAorAndOtherAddressL(*aRequest.To(),*contact->SIPAddress());

	CheckSIPAddressInContactHeaderL(*contact, aIapId, aTU);
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckUpdateRegisterContactL
// -----------------------------------------------------------------------------
//	
void TSIPRegistrationUtility::CheckUpdateRegisterContactL(
    CSIPRequest&                 aRequest,
	CSIPRegistrationBindingBase& aBinding)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);
	
	// contact header is not empty in request
	if (aRequest.HasHeader(KContactHeaderName))
		{
		__SIP_ASSERT_LEAVE(aRequest.HeaderCount(KContactHeaderName) == 1, 
                           KErrArgument);      

		CSIPContactHeader* contactInRequest = 
			HEADER_IN_REQUEST(CSIPContactHeader, KContactHeaderName);
	
		__SIP_ASSERT_LEAVE(!contactInRequest->Star(), KErrArgument);
	
	    CheckSipsSchemeInAorAndOtherAddressL(
	        aBinding.AOR(),*contactInRequest->SIPAddress());
		
		if (contactInRequest->SIPAddress()->URI().IsSIPURI())
		    {
		    __ASSERT_ALWAYS(aBinding.Contact().SIPAddress()->URI().IsSIPURI(),
                            User::Leave(KErrArgument));

		    CSIPURI& uriInRequestContact = 
		        *(contactInRequest->SIPAddress()->URI().SIPURI());
		    
		    CSIPURI& uriInBindingContact = 
		        *(aBinding.Contact().SIPAddress()->URI().SIPURI());
			
		    CSIPHostPort& hostPortInRequestContact = 
			    uriInRequestContact.HostPort();
		    
		    CSIPHostPort& hostPortInBindingContact = 
			    uriInBindingContact.HostPort();
	
		    // if there is no host in contact of request
		    if (hostPortInRequestContact.Host().CompareF(
		            SIPStrings::StringF(SipStrConsts::ELocalHost).DesC()) == 0)
			    {
			    hostPortInRequestContact.SetHostL(hostPortInBindingContact.Host());
			    }
		    // if there is no port in contact of request
		    if (!hostPortInRequestContact.HasPort() && 
			    hostPortInBindingContact.HasPort())
			    {
			    hostPortInRequestContact.SetPort(hostPortInBindingContact.Port());
			    }
		    }
	
		// leave if the contact in binding is different as the one in request
		__ASSERT_ALWAYS (contactInRequest->SIPAddress()->URI() ==
		                 aBinding.Contact().SIPAddress()->URI(), 
			             User::Leave (KErrArgument));	
		}
	else 
		{
		// create contact header, fill in the sip address from contact 
		// in binding. add the contact header to request
		CSIPAddress* address = 
			CSIPAddress::NewLC(*(aBinding.Contact().SIPAddress()));
		CSIPContactHeader* contactHeader = CSIPContactHeader::NewL(address);
		CleanupStack::Pop(address);
		CleanupStack::PushL(contactHeader);
		aRequest.AddHeaderL(contactHeader);
		CleanupStack::Pop(contactHeader);
		}
	}
	
// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckRegisterExpiresValueL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckRegisterExpiresValueL(CSIPRequest& aRequest)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);

    const RStringF KExpiresHeaderName = 
        SIPStrings::StringF(SipStrConsts::EExpiresHeader);	
	
	CSIPContactHeader* contactHeader = 
		HEADER_IN_REQUEST(CSIPContactHeader, KContactHeaderName);

    // panic if there is no contact header == 0 in debug mode.
	// leaves with error code KErrArgument in release mode.
	__SIP_ASSERT_LEAVE(contactHeader, KErrArgument);

	if (contactHeader->ExpiresParameter() >= 0)
		{
		// panic RegisterRequestExpiresParameterIsZero in release mode.
		// leaves with error code KErrArgument in release mode.
		__SIP_ASSERT_LEAVE((contactHeader->ExpiresParameter() != 0),
			               KErrArgument);	
		}
    else if (!aRequest.HasHeader(KExpiresHeaderName))
		{
		// set default expires value one hour
		contactHeader->SetExpiresParameterL(KOneHourInSeconds);
		}
	else
		{
		CSIPExpiresHeader* expiresHeader = 
            HEADER_IN_REQUEST(CSIPExpiresHeader, KExpiresHeaderName);

		// panic expires header value is 0 in release mode.
		// leaves with error code KErrArgument in release mode.
		__SIP_ASSERT_LEAVE(expiresHeader->Value() != 0, KErrArgument);
		
		// copies expires value from expire header to expires parameter
		// in Contact, remove the expires header.
		contactHeader->SetExpiresParameterL(expiresHeader->Value());
		if (aRequest.RemoveHeader(expiresHeader) == KErrNone)
			{	
			delete expiresHeader; expiresHeader = 0; 
			}
		}
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::RemoveRegisterExpiresParam
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::RemoveRegisterExpiresParam(CSIPRequest& aRequest)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);
    
    //remove the expires param
	if (aRequest.HasHeader(KContactHeaderName))
		{
		CSIPContactHeader* contactHeader = 
				HEADER_IN_REQUEST(CSIPContactHeader, KContactHeaderName);
		if(contactHeader->ExpiresParameter() != KErrNotFound)
			{
			contactHeader->RemoveExpiresParam();
			}
		}
	}

// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckUpdateRegisterExpiresValueL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckUpdateRegisterExpiresValueL(
	CSIPRequest&                 aRequest,
	CSIPRegistrationBindingBase& aBinding)
	{
    const RStringF KContactHeaderName = 
        SIPStrings::StringF(SipStrConsts::EContactHeader);

    const RStringF KExpiresHeaderName = 
        SIPStrings::StringF(SipStrConsts::EExpiresHeader);	
	
	CSIPContactHeader* contactHeader = 
		HEADER_IN_REQUEST(CSIPContactHeader, KContactHeaderName);

	// panic is contactHeader == 0 in debug mode.
	// leaves with error code KErrArgument in release mode.
	__SIP_ASSERT_LEAVE(contactHeader, KErrArgument);

	if (contactHeader->ExpiresParameter() < 0)
		{
        if (!aRequest.HasHeader(KExpiresHeaderName))
			{
			// set default expires value from binding
			contactHeader->SetExpiresParameterL(
				aBinding.Contact().ExpiresParameter());
			}
		else
			{
			// copies expires value from expire header to expires parameter
			// in Contact, remove the expires header.
			CSIPExpiresHeader* expiresHeader = 
                HEADER_IN_REQUEST(CSIPExpiresHeader, KExpiresHeaderName);

			contactHeader->SetExpiresParameterL(expiresHeader->Value());
			if (aRequest.RemoveHeader(expiresHeader) == KErrNone)
				{	
				delete expiresHeader; expiresHeader = 0; 
				}
			}
		}
	}
	
// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckSIPAddressInContactHeaderL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckSIPAddressInContactHeaderL(
	CSIPContactHeader& aContact,
	const TUint32      aIapId,
    MTransactionUser&  aTransationUser)
    {
    if (aContact.SIPAddress()->URI().IsSIPURI())
        {
	    CSIPURI& contactUri = *(aContact.SIPAddress()->URI().SIPURI());
	    if (contactUri.HostPort().Host().CompareF(
	            SIPStrings::StringF(SipStrConsts::ELocalHost).DesC()) == 0)
		    {
		    // fills in the host part by coping the Local IP address
		    HBufC8* localAddr = 
		        TSIPRegistrationUtility::LocalAddressLC(
		            aIapId, aTransationUser);
		    contactUri.HostPort().SetHostL(*localAddr);
		    CleanupStack::PopAndDestroy(); //localAddr
		    }
        }
    }
    
// -----------------------------------------------------------------------------
// TSIPRegistrationUtility::CheckSipsSchemeInAorAndOtherAddressL
// -----------------------------------------------------------------------------
//
void TSIPRegistrationUtility::CheckSipsSchemeInAorAndOtherAddressL(
    const CSIPToHeader& aAor,
    const CSIPAddress& aAddress)
    {
    if (aAor.SIPAddress().URI().IsSIPURI() &&
        aAor.SIPAddress().URI().SIPURI()->IsSIPSURI())
        {
	    __ASSERT_ALWAYS(aAddress.URI().IsSIPURI(),
	                    User::Leave(KErrSIPInvalidURIType));
	    __ASSERT_ALWAYS(aAddress.URI().SIPURI()->IsSIPSURI(),
	                    User::Leave(KErrSIPInvalidURIType));       
        }
    }
