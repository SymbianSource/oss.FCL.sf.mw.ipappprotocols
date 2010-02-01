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
// Name          : sipconnection.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "SipConnectionImplementation.h"
#include "sip.h"
#include "siperr.h"
#include "siprequestelements.h"
#include "sipregistrationcontext.h"
#include "sipclientconnection.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIPConnection::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection*
CSIPConnection::NewL(CSIP& aSip,
					 TUint32 aIapId,
					 MSIPConnectionObserver& aObserver)
	{
    CSIPConnection* self = CSIPConnection::NewLC(aSip, aIapId, aObserver);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPConnection::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection*
CSIPConnection::NewLC(CSIP& aSIP,
					  TUint32 aIapId,
					  MSIPConnectionObserver& aObserver)
	{
	CSIPConnection* self = new (ELeave) CSIPConnection();
    CleanupStack::PushL(self);
    self->ConstructL(aSIP, aIapId, aObserver);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPConnection::CSIPConnection
// -----------------------------------------------------------------------------
//
CSIPConnection::CSIPConnection()
    {
	}

// -----------------------------------------------------------------------------
// CSIPConnection::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPConnection::ConstructL(CSIP& aSIP,
                                TUint32 aIapId,
                                MSIPConnectionObserver& aObserver)
	{
	iImplementation = 
		CSIPConnectionImplementation::NewL(aSIP, *this, aIapId, aObserver);		
	}

// -----------------------------------------------------------------------------
// CSIPConnection::~CSIPConnection
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection::~CSIPConnection()
	{
	delete iImplementation;	
	}

// -----------------------------------------------------------------------------
// CSIPConnection::State
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection::TState CSIPConnection::State() const
    {
    __TEST_INVARIANT;
    return iImplementation->State();
    }

// -----------------------------------------------------------------------------
// CSIPConnection::SendRequestL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPConnection::SendRequestL(CSIPRequestElements* aElements,                                
                             const MSIPRegistrationContext& aContext)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aContext.IsContextActive(),
                    User::Leave(KErrSIPInvalidRegistrationState));

    TUint32 registrationId = aContext.ContextId();
    return iImplementation->SendRequestL(aElements, &registrationId, NULL);
    }
				
// -----------------------------------------------------------------------------
// CSIPConnection::SendRequestL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPConnection::SendRequestL(CSIPRequestElements* aElements,
                             const MSIPRegistrationContext& aContext,
							 CSIPRefresh& aRefresh)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aContext.IsContextActive(),
                    User::Leave(KErrSIPInvalidRegistrationState));
    
    TUint32 registrationId = aContext.ContextId();
    return iImplementation->SendRequestL(aElements, &registrationId, &aRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPConnection::SendRequestL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPConnection::SendRequestL(CSIPRequestElements* aElements)
    {
    __TEST_INVARIANT;
    return iImplementation->SendRequestL(aElements, NULL, NULL);
    }

// -----------------------------------------------------------------------------
// CSIPConnection::SendRequestL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPConnection::SendRequestL(CSIPRequestElements* aElements,
                             CSIPRefresh& aRefresh)
    {
    __TEST_INVARIANT;
    return iImplementation->SendRequestL(aElements, NULL, &aRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPConnection::FetchRegistrationsL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientTransaction*
CSIPConnection::FetchRegistrationsL(CSIPRequestElements* aElements)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aElements, User::Leave(KErrArgument));    
    __ASSERT_ALWAYS(aElements->Method().DesC().Length() == 0,
                    User::Leave(KErrArgument));
    
    return iImplementation->FetchRegistrationsL(aElements);
    }

// -----------------------------------------------------------------------------
// CSIPConnection::IapId
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CSIPConnection::IapId() const
    {
    __TEST_INVARIANT;
    return iImplementation->IapId();
    }

// -----------------------------------------------------------------------------
// CSIPConnection::SIP
// -----------------------------------------------------------------------------
//
EXPORT_C CSIP* CSIPConnection::SIP()
	{
	__TEST_INVARIANT;
	return iImplementation->SIP();
	}
 
// -----------------------------------------------------------------------------
// CSIPConnection::SIP
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIP* CSIPConnection::SIP() const
	{
	__TEST_INVARIANT;
	return iImplementation->SIP();
	}        

// -----------------------------------------------------------------------------
// CSIPConnection::SetOptL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConnection::SetOptL(TUint aOptionName,
			                      	  TUint aOptionLevel,
			                      	  const TDesC8& aOption)
	{
	__TEST_INVARIANT;
	return iImplementation->SetOptL(aOptionName, aOptionLevel, aOption);
	}

// -----------------------------------------------------------------------------
// CSIPConnection::SetOptL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConnection::SetOptL(TUint aOptionName,
			                          TUint aOptionLevel,
			                          TInt aOption)
	{
	__TEST_INVARIANT;
	return iImplementation->SetOptL(aOptionName, aOptionLevel, aOption);
	}

// -----------------------------------------------------------------------------
// CSIPConnection::GetLocalAddrL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPConnection::GetLocalAddrL(TInetAddr& aAddr) const
    {
    __TEST_INVARIANT;
    iImplementation->ClientConnectionL().GetLocalAddrL(aAddr);
    }

// -----------------------------------------------------------------------------
// CSIPConnection::Implementation
// -----------------------------------------------------------------------------
//
CSIPConnectionImplementation& CSIPConnection::Implementation()
	{	
	__TEST_INVARIANT;
	return *iImplementation;
	}

// -----------------------------------------------------------------------------
// CSIPConnection::RefreshConnection
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConnection::RefreshConnection() const
	{
	iImplementation->ClientConnection()->RefreshConnection();
	}

// -----------------------------------------------------------------------------
// CSIPConnection::GetConnectionErrorL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConnection::GetConnectionErrorL(TInt &aError) const
	{
	aError = iImplementation->ClientConnection()->ConnectionErrorL();
	}

// -----------------------------------------------------------------------------
// CSIPConnection::__DbgTestInvariant
// -----------------------------------------------------------------------------

void CSIPConnection::__DbgTestInvariant() const
	{
    if (!iImplementation)
		{	
		User::Invariant();
		}
	}
