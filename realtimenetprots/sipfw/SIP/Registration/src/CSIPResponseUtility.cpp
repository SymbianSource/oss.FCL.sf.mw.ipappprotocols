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
// Name          : CSIPResponseUtility.cpp
// Part of       : Registration
// Version       : SIP/6.0 
//




#include "CSIPResponseUtility.h"
#include "sipresponse.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include "sipaddress.h"
#include "siptoheader.h"
#include "sipcontactheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "sipexpiresheader.h"
#include "sipviaheader.h"
#include "mregistrationcontext.h"
#include "SipAssert.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPResponseUtility::IsError
// -----------------------------------------------------------------------------
//
TBool CSIPResponseUtility::IsError(CSIPResponse& aSIPResponse)
	{
	return (aSIPResponse.IsErrorResponse());
	}

// -----------------------------------------------------------------------------
// CSIPResponseUtility::ContactHeader
// -----------------------------------------------------------------------------
//
CSIPContactHeader* CSIPResponseUtility::ContactHeader(
	CSIPContactHeader& aContactInBinding,
	CSIPResponse&      aResponse)
	{
	TSglQueIter<CSIPHeaderBase> contact = 
	    aResponse.Headers(SIPStrings::StringF(SipStrConsts::EContactHeader));

	if (aResponse.HeaderCount(
	        SIPStrings::StringF(SipStrConsts::EContactHeader)) == 1)
	    {
	    return static_cast<CSIPContactHeader*>(contact++);
	    }

	for (CSIPHeaderBase* header = contact++; header; header = contact++)
		{
		CSIPContactHeader* contactHeader = 
			static_cast<CSIPContactHeader*>(header);

		if (contactHeader->SIPAddress() &&
		    contactHeader->SIPAddress()->URI().IsSIPURI() &&
		    aContactInBinding.SIPAddress()->URI().IsSIPURI())
		    {
		    if (contactHeader->SIPAddress()->URI().SIPURI()->User() ==
		        aContactInBinding.SIPAddress()->URI().SIPURI()->User())
		        {
		        return contactHeader;
		        }
		    }
		else
		    {
		    if (contactHeader->SIPAddress() && 
				contactHeader->SIPAddress()->URI() ==
				aContactInBinding.SIPAddress()->URI())
			    {
			    return contactHeader;
			    }
			}
		}

	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPResponseUtility::CallIdL
// -----------------------------------------------------------------------------
//
HBufC8* CSIPResponseUtility::CallIdL(CSIPResponse& aSIPResponse)
	{
	CSIPCallIDHeader* callId = aSIPResponse.CallID();
	
	// panic if there is no CallId header in response
    __SIP_ASSERT_LEAVE (callId, KErrArgument);

	return (callId->ToTextValueL());
	}

// -----------------------------------------------------------------------------
// CSIPResponseUtility::CSeqValueL
// -----------------------------------------------------------------------------
//
TUint CSIPResponseUtility::CSeqValueL(CSIPResponse& aSIPResponse)
	{	
	CSIPCSeqHeader* cseq = aSIPResponse.CSeq();
	
	// panic if there is no CSeq header in response
    __SIP_ASSERT_LEAVE (cseq, KErrArgument);

	return cseq->Seq();
	}

// -----------------------------------------------------------------------------
// CSIPResponseUtility::ExpireValueL
// -----------------------------------------------------------------------------
//
TUint CSIPResponseUtility::ExpireValueL(CSIPContactHeader& aBindingContact,
										CSIPResponse&      aResponse)
	{
	const RStringF KExpiresHeaderName = 
	    SIPStrings::StringF(SipStrConsts::EExpiresHeader);
	TUint returnValue = 0;

	// get Contact header from response
	CSIPContactHeader* responseContact = 
	    ContactHeader(aBindingContact, aResponse);
	
	if (responseContact)
		{
		if (responseContact->ExpiresParameter() < 0)
			{
            if (aResponse.HasHeader(KExpiresHeaderName))
				{
				// copy the expires value from expires header to
				// expires parameter in contact, remove the expires header
				CSIPExpiresHeader* expiresHeader = 
					static_cast<CSIPExpiresHeader*>(
                        aResponse.Header(KExpiresHeaderName,0));

				responseContact->SetExpiresParameterL(
					expiresHeader->Value());

				if (aResponse.RemoveHeader(expiresHeader) == KErrNone)
					{
					delete expiresHeader; expiresHeader = 0;
					}
				}
			else
				{
				// copy the expires parameter from binding
				responseContact->SetExpiresParameterL(
					aBindingContact.ExpiresParameter());
				}
			}
        // if expires parameter is not zero and expires parameter in binding
		// less than expires parameter in response, change expires parameter
		// in response to lower value
		if (aBindingContact.ExpiresParameter() != 0 && 
			(aBindingContact.ExpiresParameter() < 
			 responseContact->ExpiresParameter()))
			{
			responseContact->SetExpiresParameterL(
				aBindingContact.ExpiresParameter());
			}

		returnValue = responseContact->ExpiresParameter();
		}

	return returnValue;
	}

// -----------------------------------------------------------------------------
// CSIPResponseUtility::Check2XXResponse
// -----------------------------------------------------------------------------
//
TInt CSIPResponseUtility::Check2XXResponse(MRegistrationContext& aRegistration,
		                                   CSIPResponse& aResponse)
	{
	TInt err = KErrNone;
	const RStringF KContactHeaderName = 
	    SIPStrings::StringF(SipStrConsts::EContactHeader);
    // error if there is no contact header in response
	if (!aResponse.HasHeader(KContactHeaderName))
		{
		return KErrSIPInvalidRegistrarResponse;
		}
	// error if contact header sip address in response is different as contact 
	// header sip address in response
	if (!ContactHeader(aRegistration.Contact(),aResponse))
		{
		return KErrSIPInvalidRegistrarResponse;
		}
	// if there is no expires parameter or expires header in response, 
	// copy te expires parameter from binding.
	// error if the expires parameter is 0 in response.
	TUint expireValue = 0;
	TRAP(err, expireValue = ExpireValueL(aRegistration.Contact(),aResponse))
	if (err)
		{
		return err;
		}
    if (expireValue == 0)
        {
        return KErrSIPInvalidRegistrarResponse;
        }
    // Invalid Service-Route must be indicated to SIP Profile Agent 
    // as a transport failure. This will lead to a new registration. 
    if (!aRegistration.ServiceRouteOk(aResponse))
        {
        return KErrSIPTransportFailure;
        }

	const CURIContainer& uriOfToInBinding  = 
	    aRegistration.AOR().SIPAddress().URI();
	
	CURIContainer& uriOfToInResponse = 
	    aResponse.To()->SIPAddress().URI();

	if (!(uriOfToInBinding == uriOfToInResponse))
	    {
	    err = KErrSIPInvalidRegistrarResponse;
	    }
	return err;
	}
