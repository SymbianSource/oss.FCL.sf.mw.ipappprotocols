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
// Name        : sipmanagedprofileregistry
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "sip.h"
#include "sipmanagedprofileregistry.h"
#include "sipprofileregistryobserver.h"
#include "sipmanagedprofile.h"
#include "sipconcreteprofile.h"
#include "SIPProfileITC.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfileRegistry* CSIPManagedProfileRegistry::NewL(
											MSIPProfileRegistryObserver& aObserver)
	{
    CSIPManagedProfileRegistry* self = CSIPManagedProfileRegistry::NewLC (aObserver);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfileRegistry* CSIPManagedProfileRegistry::NewLC(
											MSIPProfileRegistryObserver& aObserver)
	{
	CSIPManagedProfileRegistry* self = new(ELeave)CSIPManagedProfileRegistry(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::~CSIPManagedProfileRegistry
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfileRegistry::~CSIPManagedProfileRegistry()
	{
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::CreateL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfile* CSIPManagedProfileRegistry::CreateL(
	const TSIPProfileTypeInfo& aType)
	{
	CSIPConcreteProfile *concreteProfile = iITC->SIPCreateProfileL(aType);
	CleanupStack::PushL(concreteProfile);
	CSIPManagedProfile* profile = CSIPManagedProfile::NewL(0);
	CleanupStack::Pop(concreteProfile);
	profile->SetConcreteProfile(concreteProfile);
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::IsInUse
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPManagedProfileRegistry::IsInUseL(const CSIPProfile& aSIPProfile) const
	{
	return (UsageL(aSIPProfile) != 0);
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::SaveL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPManagedProfileRegistry::SaveL(CSIPProfile& aSIPProfile)
	{
	__ASSERT_ALWAYS (aSIPProfile.ConcreteProfile().ValidateProfileParamsL(),User::Leave (KErrArgument));
	aSIPProfile.ConcreteProfile().SetLastRegistrationError(KErrNone);
	if (aSIPProfile.ConcreteProfile().Id() == 0)
		{
		AddProfileL(aSIPProfile);
		aSIPProfile.SetRegistry(this);
		} 
	else
		{
		UpdateProfileL(aSIPProfile);
		}
	aSIPProfile.ConcreteProfile().SetDefault(EFalse);
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::DestroyL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPManagedProfileRegistry::DestroyL(CSIPProfile& aSIPProfile)
	{
	aSIPProfile.ConcreteProfile().SetLastRegistrationError(KErrNone);
	RemoveProfileL(aSIPProfile);
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::CSIPManagedProfileRegistry
// -----------------------------------------------------------------------------
//
CSIPManagedProfileRegistry::CSIPManagedProfileRegistry(
	MSIPProfileRegistryObserver& aObserver):
	CSIPProfileRegistryBase(aObserver)
	{
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPManagedProfileRegistry::ConstructL()
	{
	BaseConstructL();
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::NewInstanceL
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPManagedProfileRegistry::NewInstanceL()
	{
	CSIPManagedProfile* profile = CSIPManagedProfile::NewL(this);
    return profile;
	}
