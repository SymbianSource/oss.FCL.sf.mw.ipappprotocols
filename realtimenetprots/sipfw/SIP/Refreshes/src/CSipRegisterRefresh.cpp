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
// Name          : CSipRegisterRefresh.cpp
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
#include "siptoheader.h"
#include "sipfromheader.h"
#include "SipLogs.h"
#include "sipcallidheader.h"
#include "sipuri.h"
#include "siprouteheader.h"
#include "siprecordrouteheader.h"
#include "SipAssert.h"
#include "CSipRegisterRefresh.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPResponseUtility.h"


// -----------------------------------------------------------------------------
// CSipRegisterRefresh::NewLC
// -----------------------------------------------------------------------------
//
CSipRegisterRefresh* CSipRegisterRefresh::NewLC(
	MTransactionUser& aTU,
    MTimerManager& aTimerMgr,
    CSIPSec& aSIPSec,
	CSipRefreshMgr& aRefreshMgr,
	TRegistrationId aRegistrationId,
	CSIPRequest* aRequest,
    MRefreshOwner* aOwner,
    MSIPSecUser& aSIPSecUser,
	CURIContainer& aRemoteTarget)
	{
	__SIP_ASSERT_LEAVE (aOwner, KErrArgument);

	CSipRegisterRefresh* self = 
	    new(ELeave)CSipRegisterRefresh(aTU, aTimerMgr, aSIPSec, aRefreshMgr,
	                                   aRegistrationId, aOwner, aSIPSecUser);

	CleanupStack::PushL (self);
	self->ConstructL(aRequest, aRemoteTarget);
	return self;
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::~CSipRegisterRefresh
// -----------------------------------------------------------------------------
//
CSipRegisterRefresh::~CSipRegisterRefresh ()
	{
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::CSipRegisterRefresh
// -----------------------------------------------------------------------------
//
CSipRegisterRefresh::CSipRegisterRefresh (MTransactionUser& aTU,
                                          MTimerManager& aTimerMgr,
                                          CSIPSec& aSIPSec,
                                          CSipRefreshMgr& aRefreshMgr,
                                          TRegistrationId aRegistrationId,
                                          MRefreshOwner* aOwner,
                                          MSIPSecUser& aSIPSecUser)
 : CSipRefreshBase (aTU, aTimerMgr, aSIPSec, aRefreshMgr, 
                    aRegistrationId, aOwner, aSIPSecUser)
	{
	iContactHeaderName = SIPStrings::StringF(SipStrConsts::EContactHeader);
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::ConstructL
// -----------------------------------------------------------------------------
//
void CSipRegisterRefresh::ConstructL (CSIPRequest* aRequest, 
									  CURIContainer& aRemoteTarget)
	{
	__SIP_ASSERT_LEAVE (aRequest, KErrArgument);

	__SIP_ASSERT_LEAVE (
	    aRequest->Method() == SIPStrings::StringF(SipStrConsts::ERegister),
	    KErrArgument);

	CSIPContactHeader* contact = 
	    static_cast<CSIPContactHeader*>(aRequest->Header(iContactHeaderName,0));

	__SIP_ASSERT_LEAVE (contact, KErrArgument);
	
	CSipRefreshBase::BaseConstructL(aRemoteTarget,*aRequest);
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::SIPSecUser
// From MTransactionOwner
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSipRegisterRefresh::SIPSecUser() const
    {
    // In case of REGISTER, the refresh must not act as a middle man.
    // This is to make the MSIPSecUser::TrustedUser() work correctly.
    return &iSIPSecUser;
    }

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::Forward2xxResponseToCallerL
// -----------------------------------------------------------------------------
//
void CSipRegisterRefresh::Forward2xxResponseToCallerL (CSIPResponse* aResponse)
	{
	RefreshReceivedL(iCurrentTransactionId, aResponse);
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::SetExpirationTimeToRequestL 
// -----------------------------------------------------------------------------
//
void CSipRegisterRefresh::SetExpirationTimeToRequestL (
    CSIPRequest& aRequest,
	TUint aExpirationTimeInSeconds)
	{
	CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(
        aRequest.Header(iContactHeaderName,0));

	__SIP_ASSERT_LEAVE (contact, KErrArgument);	
	
	contact->SetExpiresParameterL(aExpirationTimeInSeconds);
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::ExpirationTimeFromResponseContact
// -----------------------------------------------------------------------------
//
TInt CSipRegisterRefresh::ExpirationTimeFromResponseContact (
	CSIPResponse& aResponse)
	{
	TSglQueIter<CSIPHeaderBase> iter = 
        iRequest->Headers(iContactHeaderName);
	if (iter != 0)
		{
		CSIPHeaderBase* header = iter;
		CSIPContactHeader* reqContact = static_cast<CSIPContactHeader*>(header);
		
		// search the contact header of the response which match to one 
		// defined in the request
		CSIPContactHeader* resContact = 
		    CSIPResponseUtility::ContactHeader( *reqContact, aResponse );
		
		if ( resContact && resContact->ExpiresParameter() >= 0 )
		    {
		    return resContact->ExpiresParameter();
		    }
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::SetExpirationTimeToResponseL
// -----------------------------------------------------------------------------
//
void CSipRegisterRefresh::SetExpirationTimeToResponseL(
	CSIPResponse& aResponse, 
	TUint         aExpirationTimeInSeconds)
	{
	TBool setExpiresInContact = EFalse;

	if (aResponse.HasHeader(iContactHeaderName))
		{
		__SIP_ASSERT_LEAVE (
		    iRequest->HasHeader(iContactHeaderName), KErrGeneral);
		    
		CSIPContactHeader* reqContact = 
		    static_cast<CSIPContactHeader*>(
		        iRequest->Header(iContactHeaderName,0));
	
	    CSIPContactHeader* resContact = 
		    CSIPResponseUtility::ContactHeader( *reqContact, aResponse );
		
		if ( resContact )
		    {
		    resContact->SetExpiresParameterL(aExpirationTimeInSeconds);
		    setExpiresInContact = ETrue;
		    }
		}

	if (!setExpiresInContact)
		{
		SetExpireTimeToResponseExpiresHeaderL(aResponse, 
			                                  aExpirationTimeInSeconds);
		}
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::ExpirationTimeFromRequestL
// -----------------------------------------------------------------------------
//
TInt CSipRegisterRefresh::ExpirationTimeFromRequestL(CSIPRequest& /*aRequest*/)
	{
	return iOwner->RefreshDurationL();
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::ExpirationTimeFromResponse
// -----------------------------------------------------------------------------
//
TInt CSipRegisterRefresh::ExpirationTimeFromResponse (CSIPResponse& aResponse)
	{
	TInt time = 0;
	
	time = ExpirationTimeFromResponseContact(aResponse);
		
	if(time == KErrNotFound) 
		{
		time = ExpireTimeFromResponseExpiresHeader(aResponse);
		}

	return time;
	}

// -----------------------------------------------------------------------------
// CSipRegisterRefresh::CheckUpdateRequestExpiresValueL
// -----------------------------------------------------------------------------
//
void CSipRegisterRefresh::CheckUpdateRequestExpiresValueL(
    CSIPRequest& aRequest)
	{
	CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(
        aRequest.Header(iContactHeaderName,0));

	__SIP_ASSERT_LEAVE (contact, KErrArgument);
	}
 
// -----------------------------------------------------------------------------
// CSipRegisterRefresh::CheckTerminateRequestExpiresValueL
// -----------------------------------------------------------------------------
//
void CSipRegisterRefresh::CheckTerminateRequestExpiresValueL(
	CSIPRequest& aRequest)
	{
	CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(
        aRequest.Header(iContactHeaderName,0));

	__SIP_ASSERT_LEAVE (contact, KErrGeneral);

	contact->SetExpiresParameterL(0);
	}
