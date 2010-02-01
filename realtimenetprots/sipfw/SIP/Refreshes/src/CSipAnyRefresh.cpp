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
// Name          : CSipAnyRefresh.cpp
// Part of       : SIPRefreshes
// Version       : SIP/4.0 
//



#include "SipStackServerDefs.h"
#include "MTransactionUser.h"
#include "MRefreshOwner.h"
#include "CSipRefreshMgr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcontactheader.h"
#include "sipaddress.h"
#include "sipcseqheader.h"
#include "sipexpiresheader.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "SipLogs.h"
#include "sipcallidheader.h"
#include "sipuri.h"
#include "siprouteheader.h"
#include "siprecordrouteheader.h"
#include "CSipAnyRefresh.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSipAnyRefresh::NewLC
// -----------------------------------------------------------------------------
//
CSipAnyRefresh* CSipAnyRefresh::NewLC (MTransactionUser& aTU,
                                       MTimerManager& aTimerMgr,
                                       CSIPSec& aSIPSec,
                                       CSipRefreshMgr& aRefreshMgr,
                                       TRegistrationId aRegistrationId,
                                       CSIPRequest* aRequest,
                                       MRefreshOwner* aOwner,
                                       MSIPSecUser& aSIPSecUser,
                                       CURIContainer& aRemoteTarget,
                                       TBool aPassAllResponsesToOwner)
	{
	__SIP_ASSERT_LEAVE (aOwner, KErrArgument);

	CSipAnyRefresh* self = new(ELeave)CSipAnyRefresh(aTU, aTimerMgr, aSIPSec,
	                                                 aRefreshMgr, 
	                                                 aRegistrationId, aOwner,
													 aSIPSecUser,
													 aPassAllResponsesToOwner);
	CleanupStack::PushL (self);
	self->ConstructL(aRequest, aRemoteTarget);
	return self;
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::~CSipAnyRefresh
// -----------------------------------------------------------------------------
//
CSipAnyRefresh::~CSipAnyRefresh ()
	{
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::CSipAnyRefresh
// -----------------------------------------------------------------------------
//
CSipAnyRefresh::CSipAnyRefresh (MTransactionUser& aTU,
                                MTimerManager&    aTimerMgr,
                                CSIPSec&          aSIPSec,
				                CSipRefreshMgr&   aRefreshMgr,
				                TRegistrationId   aRegistrationId,
                                MRefreshOwner*    aOwner,
                                MSIPSecUser&      aSIPSecUser,
								TBool             aPassAllResponsesToOwner)
 : CSipRefreshBase (aTU, aTimerMgr, aSIPSec, aRefreshMgr, 
                    aRegistrationId, aOwner, aSIPSecUser),
   iPassAllResponsesToOwner (aPassAllResponsesToOwner)
	{
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::ConstructL
// -----------------------------------------------------------------------------
//
void CSipAnyRefresh::ConstructL (CSIPRequest* aRequest,
                                 CURIContainer& aRemoteTarget)
	{
	__SIP_ASSERT_LEAVE (aRequest, KErrArgument);

	__SIP_ASSERT_LEAVE (
	    aRequest->Method() != SIPStrings::StringF(SipStrConsts::ERegister),
	    KErrArgument);

	CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>(
        aRequest->Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));

	__SIP_ASSERT_LEAVE (expires, KErrArgument);

	__SIP_ASSERT_LEAVE (expires->Value() > 0, KErrArgument);
	
	CSipRefreshBase::BaseConstructL(aRemoteTarget,*aRequest);
	}


// -----------------------------------------------------------------------------
// CSipAnyRefresh::Forward2xxResponseToCallerL
// -----------------------------------------------------------------------------
//
void CSipAnyRefresh::Forward2xxResponseToCallerL (CSIPResponse* aResponse)
	{
	if (iFirstRequest || iPassAllResponsesToOwner) 
		{
		RefreshReceivedL(iCurrentTransactionId, aResponse);
		}
    else
		{
		delete aResponse;
		}
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::SetExpirationTimeToRequestL
// -----------------------------------------------------------------------------
//
void CSipAnyRefresh::SetExpirationTimeToRequestL(CSIPRequest& aRequest,
                                                 TUint aExpirationTimeInSeconds)
	{
	CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>(
        aRequest.Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));	
	
	__SIP_ASSERT_LEAVE (expires, KErrArgument);

	expires->SetValue(aExpirationTimeInSeconds);
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::SetExpirationTimeToResponseL
// -----------------------------------------------------------------------------
//
void CSipAnyRefresh::SetExpirationTimeToResponseL(CSIPResponse& 
												  aResponse, 
		                                          TUint         
										   	      aExpirationTimeInSeconds)
	{
	SetExpireTimeToResponseExpiresHeaderL(aResponse, aExpirationTimeInSeconds);
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::ExpirationTimeFromRequestL
// -----------------------------------------------------------------------------
//
TInt CSipAnyRefresh::ExpirationTimeFromRequestL (CSIPRequest& aRequest)
	{
	CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>(
        aRequest.Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));	
	
	__SIP_ASSERT_RETURN_VALUE (expires, KErrNotFound);
	
	return (expires->Value());
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::ExpirationTimeFromResponse
// -----------------------------------------------------------------------------
//
TInt CSipAnyRefresh::ExpirationTimeFromResponse (CSIPResponse& aResponse)
	{
	return ExpireTimeFromResponseExpiresHeader(aResponse);
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::CheckUpdateRequestExpiresValueL
// -----------------------------------------------------------------------------
//
void CSipAnyRefresh::CheckUpdateRequestExpiresValueL(CSIPRequest& aRequest)
	{
	// do not check expires header value after this, the refresh will be 
	// terminated when error happens or receive any final response.
	CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>(
        aRequest.Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));	
	
	__SIP_ASSERT_LEAVE (expires, KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSipAnyRefresh::CheckTerminateRequestExpiresValueL
// -----------------------------------------------------------------------------
//
void CSipAnyRefresh::CheckTerminateRequestExpiresValueL(CSIPRequest& aRequest)
	{
	CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>(
        aRequest.Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));	
	if (expires)
		{
		expires->SetValue(0);
		}
	else
		{
		expires = new(ELeave)CSIPExpiresHeader(0);
		CleanupStack::PushL(expires);
		aRequest.AddHeaderL(expires);
		CleanupStack::Pop(expires);
		}
	}
