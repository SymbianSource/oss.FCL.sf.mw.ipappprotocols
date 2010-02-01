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
// Name          : SIPRequestUtility.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "uricontainer.h"
#include "siprouteheader.h"
#include "sipsupportedheader.h"
#include "sipaddress.h"
#include "sipmaxforwardsheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "SIPRequestUtility.h"
#include "RouteSet.h"


// -----------------------------------------------------------------------------
// SIPRequestUtility::CheckOutgoingRequestL
// Request-URI can be present if the request is sent by Refresh subsystem.
// -----------------------------------------------------------------------------
//
void SIPRequestUtility::CheckOutgoingRequestL(CSIPRequest& aReq)
	{
    __SIP_ASSERT_LEAVE(aReq.Method().DesC().Length() > 0,
    				   KErrSIPMalformedMessage);	    
    __SIP_ASSERT_LEAVE(aReq.To() != NULL, KErrSIPMalformedMessage);    
    __SIP_ASSERT_LEAVE(aReq.From() != NULL, KErrSIPMalformedMessage);    
	}

// -----------------------------------------------------------------------------
// SIPRequestUtility::SetRequestUriL
// -----------------------------------------------------------------------------
//
void SIPRequestUtility::SetRequestUriL(CSIPRequest& aReq,
									   const CURIContainer& aUri)
	{	
	CURIContainer* reqUri = CURIContainer::NewLC(aUri);
	aReq.SetRequestURIL(reqUri);
	CleanupStack::Pop(reqUri);
	}

// -----------------------------------------------------------------------------
// SIPRequestUtility::FillRouteAndRequestUriL
// -----------------------------------------------------------------------------
//
void
SIPRequestUtility::FillRouteAndRequestUriL(CSIPRequest& aReq,
										   const CRouteSet& aRouteSet,
								    	   const CURIContainer& aRemoteTarget)
	{
	aReq.DeleteHeaders(SIPStrings::StringF(SipStrConsts::ERouteHeader));

	if (aRouteSet.IsEmpty())
		{
		SetRequestUriL(aReq, aRemoteTarget);
		}
	else
		{
		if (aRouteSet.IsLrParamInTopRoute())
			{
			SetRequestUriL(aReq, aRemoteTarget);
			aRouteSet.CopyToRequestL(aReq);
			}
		else
			{
			//Put first URI in route set to Request-URI. Add rest of the route
			//set as Route headers. Add remote target as the last route header.
			SetRequestUriL(aReq, *aRouteSet.TopUri());
			aRouteSet.CopyToRequestL(aReq, 1);

			if (!aRouteSet.IsInSet(aRemoteTarget))
				{
				CSIPRouteHeader* route = CreateRouteHeaderLC(aRemoteTarget);
				aReq.AddHeaderL(route);
				CleanupStack::Pop(route);
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// SIPRequestUtility::CreateRouteHeaderLC
// -----------------------------------------------------------------------------
//
CSIPRouteHeader*
SIPRequestUtility::CreateRouteHeaderLC(const CURIContainer& aUri)
	{
	HBufC8* uri = aUri.ToTextL();
	CleanupStack::PushL(uri);
	CSIPAddress* addr = CSIPAddress::DecodeL(*uri);
	CleanupStack::PopAndDestroy(uri);	
	CleanupStack::PushL(addr);
	CSIPRouteHeader* route = CSIPRouteHeader::NewL(addr);
	CleanupStack::Pop(addr);
	CleanupStack::PushL(route);
	return route;
	}	

// -----------------------------------------------------------------------------
// SIPRequestUtility::FillNewMaxForwardsL
// -----------------------------------------------------------------------------
//
void SIPRequestUtility::FillNewMaxForwardsL(CSIPRequest& aReq)
	{	
	CSIPMaxForwardsHeader* maxForw =
		new (ELeave) CSIPMaxForwardsHeader(EMaxForwardsValue);
	CleanupStack::PushL(maxForw);
	aReq.AddHeaderL(maxForw);
	CleanupStack::Pop(maxForw);
	}

// -----------------------------------------------------------------------------
// SIPRequestUtility::FillSupportedSecAgreeL
// -----------------------------------------------------------------------------
//
void SIPRequestUtility::FillSupportedSecAgreeL(CSIPRequest& aReq)
	{
	CSIPSupportedHeader* supported = CSIPSupportedHeader::NewLC(
							SIPStrings::StringF(SipStrConsts::ESecAgreeTag));
	aReq.AddHeaderL(supported);
	CleanupStack::Pop(supported);
	}

// -----------------------------------------------------------------------------
// SIPRequestUtility::CleanupRouteHeaders
// -----------------------------------------------------------------------------
//
void SIPRequestUtility::CleanupRouteHeaders(TAny* aRouteHeaders)
	{
    __SIP_ASSERT_RETURN(aRouteHeaders, KErrArgument);

    reinterpret_cast<RPointerArray<CSIPHeaderBase>*>
        (aRouteHeaders)->ResetAndDestroy();
	}
