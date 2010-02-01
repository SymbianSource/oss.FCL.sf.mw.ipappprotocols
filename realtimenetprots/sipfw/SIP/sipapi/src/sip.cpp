// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sip.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "sipstrings.h"
#include "sip.h"
#include "SipImplementation.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#undef EXPORT_C
#define EXPORT_C

#endif


// -----------------------------------------------------------------------------
// CSIP::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIP* CSIP::NewL(const TUid& aUid, MSIPObserver& aObserver)
	{
    CSIP* self = CSIP::NewLC(aUid, aObserver);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIP::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIP* CSIP::NewLC(const TUid& aUid, MSIPObserver& aObserver)
	{   
    CSIP* self = new (ELeave) CSIP();
    CleanupStack::PushL(self);
    self->ConstructL(aUid, aObserver);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIP::CSIP
// -----------------------------------------------------------------------------
//
CSIP::CSIP()
	{
	}

// -----------------------------------------------------------------------------
// CSIP::ConstructL
// -----------------------------------------------------------------------------
//
void CSIP::ConstructL(const TUid& aUid, MSIPObserver& aObserver)
	{
	SIPStrings::OpenL();

	iImplementation = CSIPImplementation::NewL(aUid, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIP::~CSIP
// -----------------------------------------------------------------------------
//
EXPORT_C CSIP::~CSIP()
	{
    delete iImplementation;
    
    SIPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CSIP::SupportedSecurityMechanismsL
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC8Array* CSIP::SupportedSecurityMechanismsL() const
    {
    __TEST_INVARIANT;
    return iImplementation->SupportedSecurityMechanismsL();
    }

// -----------------------------------------------------------------------------
// CSIP::IsSigCompSupportedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIP::IsSigCompSupportedL() const
    {
    __TEST_INVARIANT;
    return iImplementation->IsSigCompSupportedL();
    }

// -----------------------------------------------------------------------------
// CSIP::NegotiatedSecurityMechanismL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIP::NegotiatedSecurityMechanismL(const TDesC8& aHop)
	{
	__TEST_INVARIANT;
	return iImplementation->NegotiatedSecurityMechanismL(aHop);
	}

// -----------------------------------------------------------------------------
// CSIP::Connection
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConnection* CSIP::Connection(TUint32 aIapId) const
    {
    __TEST_INVARIANT;
    return iImplementation->Connection(aIapId);
	}

// -----------------------------------------------------------------------------
// CSIP::SetSecurityHandlingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIP::SetSecurityHandlingL(TBool aEnabled)
    {
    __TEST_INVARIANT;
    iImplementation->SetSecurityHandlingL(aEnabled);
    }

// -----------------------------------------------------------------------------
// CSIP::IsSecurityHandlingEnabled
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIP::IsSecurityHandlingEnabled() const
    {
    __TEST_INVARIANT;
    return iImplementation->IsSecurityHandlingEnabled();
    }

// -----------------------------------------------------------------------------
// CSIP::Implementation
// -----------------------------------------------------------------------------
//
CSIPImplementation& CSIP::Implementation()
	{
	__TEST_INVARIANT;
	return *iImplementation;
	}

// -----------------------------------------------------------------------------
// CSIP::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIP::__DbgTestInvariant() const
	{
    if (!iImplementation)
		{	
		User::Invariant();
		}
	}
