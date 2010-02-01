// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SIPHttpDigest.cpp
// Part of       : SIP API
// Version       : SIP/5.0
//



#include <badesca.h>
#include "siperr.h"
#include "sip.h"
#include "SipImplementation.h"
#include "sipclient.h"
#include "siphttpdigest.h"
#include "sipclienttransaction.h"
#include "siprefresh.h"

#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#endif


// -----------------------------------------------------------------------------
// CSIPHttpDigest::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHttpDigest*
CSIPHttpDigest::NewL(CSIP& aSIP, MSIPHttpDigestChallengeObserver& aObserver)
    {
    CSIPHttpDigest* self = CSIPHttpDigest::NewLC(aSIP, aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHttpDigest*
CSIPHttpDigest::NewLC(CSIP& aSIP, MSIPHttpDigestChallengeObserver& aObserver)
    {
    CSIPHttpDigest* self = new (ELeave) CSIPHttpDigest(aSIP);
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHttpDigest*
CSIPHttpDigest::NewL(CSIP& aSIP, MSIPHttpDigestChallengeObserver2& aObserver2)
    {
    CSIPHttpDigest* self = CSIPHttpDigest::NewLC(aSIP, aObserver2);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHttpDigest*
CSIPHttpDigest::NewLC(CSIP& aSIP, MSIPHttpDigestChallengeObserver2& aObserver2)
    {
    CSIPHttpDigest* self = new (ELeave) CSIPHttpDigest(aSIP);
    CleanupStack::PushL(self);
    self->ConstructL(aObserver2);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::CSIPHttpDigest
// -----------------------------------------------------------------------------
//
CSIPHttpDigest::CSIPHttpDigest(CSIP& aSIP) : iSIP(&aSIP)
	{
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPHttpDigest::ConstructL(MSIPHttpDigestChallengeObserver& aObserver)
	{
	TInt err =
		iSIP->Implementation().SIPClient().SetHttpDigestObserver(aObserver);
	if (err != KErrNone)
		{
		User::Leave(err);
		}

	iSIP->Implementation().AddDigestL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPHttpDigest::ConstructL(MSIPHttpDigestChallengeObserver2& aObserver2)
	{
	TInt err =
		iSIP->Implementation().SIPClient().SetHttpDigestObserver(aObserver2);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
		
	iSIP->Implementation().AddDigestL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::~CSIPHttpDigest
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHttpDigest::~CSIPHttpDigest()
	{
	if (iSIP)
		{
		iSIP->Implementation().SIPClient().RemoveHttpDigestObserver();
		iSIP->Implementation().RemoveDigest(*this);
		}
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::operator=
// -----------------------------------------------------------------------------
//
CSIPHttpDigest& CSIPHttpDigest::operator=(const CSIPHttpDigest& /*aHttpDigest*/)
    {
	//lint -e1529, -e1745
    return *this;
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetCredentialsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHttpDigest::SetCredentialsL(const TDesC8& aOutboundProxy,
				                              const TDesC8& aRealm,
			                          		  const TDesC8& aUsername,
			                          		  const TDesC8& aPasswd)
    {
    __ASSERT_ALWAYS(aOutboundProxy.Length() > 0, User::Leave(KErrArgument));

    SetCredentialParamsL(0, 0, aOutboundProxy, aRealm, aUsername, aPasswd);    
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetCredentialsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHttpDigest::SetCredentialsL(const TDesC8& aRealm,
			                           		  const TDesC8& aUsername,
			                          		  const TDesC8& aPasswd)
	{
	SetCredentialParamsL(0, 0, KNullDesC8, aRealm, aUsername, aPasswd);	
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetCredentialsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHttpDigest::SetCredentialsL(
    const CSIPClientTransaction& aTransaction,
    const TDesC8& aOutboundProxy,
    const TDesC8& aRealm,
    const TDesC8& aUsername,
    const TDesC8& aPasswd)
    {
    SetCredentialParamsL(aTransaction.RequestId(), 0, aOutboundProxy, 
                         aRealm, aUsername, aPasswd);
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetCredentialsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHttpDigest::SetCredentialsL(
    const CSIPRefresh& aRefresh,
    const TDesC8& aOutboundProxy,
    const TDesC8& aRealm,
    const TDesC8& aUsername,
    const TDesC8& aPasswd)
    {
    SetCredentialParamsL(0, aRefresh.RefreshId(), aOutboundProxy, 
                         aRealm, aUsername, aPasswd);
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::RemoveCredentials
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPHttpDigest::RemoveCredentials(const TDesC8& aRealm)
	{
	if (aRealm.Length() == 0)
		{
		return KErrArgument;
		}

	return RemoveCredentialParams(aRealm);    
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::RemoveCredentials
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPHttpDigest::RemoveCredentials()
	{
	return RemoveCredentialParams(KNullDesC8);
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::RemoveCredentialParams
// -----------------------------------------------------------------------------
//
TInt CSIPHttpDigest::RemoveCredentialParams(const TDesC8& aRealm) const
	{
    if (iSIP)
		{
		return iSIP->Implementation().SIPClient().RemoveCredentials(aRealm);
		}

	return KErrSIPResourceNotAvailable;
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHttpDigest::SetObserver(
    MSIPHttpDigestChallengeObserver& aObserver)
	{
	if (iSIP)
		{
		iSIP->Implementation().SIPClient().SetHttpDigestObserver(aObserver);
		}
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHttpDigest::SetObserver(
    MSIPHttpDigestChallengeObserver2& aObserver)
	{
	if (iSIP)
		{
		iSIP->Implementation().SIPClient().SetHttpDigestObserver(aObserver);
		}
	}
		
// -----------------------------------------------------------------------------
// CSIPHttpDigest::IgnoreChallenge
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPHttpDigest::IgnoreChallenge(const TDesC8& aRealm)
	{
	return IgnoreChallenge(0,0,aRealm);
	}
	
// -----------------------------------------------------------------------------
// CSIPHttpDigest::IgnoreChallenge
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPHttpDigest::IgnoreChallenge(
    const CSIPClientTransaction& aTransaction,
    const TDesC8& aRealm)
    {
    return IgnoreChallenge(aTransaction.RequestId(),0,aRealm);
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::IgnoreChallenge
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPHttpDigest::IgnoreChallenge(
    const CSIPRefresh& aRefresh,
    const TDesC8& aRealm)
    {
    return IgnoreChallenge(0,aRefresh.RefreshId(),aRealm);
    }

// -----------------------------------------------------------------------------
// CSIPHttpDigest::CSIPDeleted
// -----------------------------------------------------------------------------
//
void CSIPHttpDigest::CSIPDeleted()
	{
	iSIP = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::SetCredentialParamsL
// -----------------------------------------------------------------------------
//
void CSIPHttpDigest::SetCredentialParamsL(TUint32 aRequestId,
                                          TUint32 aRefreshId,
                                          const TDesC8& aOutboundProxy,
                                          const TDesC8& aRealm,
				                          const TDesC8& aUsername,
				                          const TDesC8& aPasswd) const
	{
	__ASSERT_ALWAYS(aRealm.Length() > 0 &&
					aUsername.Length() > 0 &&
					aPasswd.Length() > 0, User::Leave(KErrArgument));
	if (!iSIP)
		{
		User::Leave(KErrSIPResourceNotAvailable);
		}
	
	iSIP->Implementation().SIPClient().SetCredentialsL(aRequestId,
	                                                   aRefreshId,
	                                                   aRealm,
		             								   aUsername,
		             								   aPasswd,
		             								   aOutboundProxy);
	}

// -----------------------------------------------------------------------------
// CSIPHttpDigest::IgnoreChallenge
// -----------------------------------------------------------------------------
//	
TInt CSIPHttpDigest::IgnoreChallenge(
    TUint32 aRequestId,
    TUint32 aRefreshId,
    const TDesC8& aRealm)
    {
	if (aRealm.Length() == 0)
		{
		return KErrArgument;
		}

	if (iSIP)
		{
		return iSIP->Implementation().SIPClient().IgnoreChallenge(aRequestId,
		                                                          aRefreshId,
		                                                          aRealm);
		}

	return KErrSIPResourceNotAvailable;
    }
