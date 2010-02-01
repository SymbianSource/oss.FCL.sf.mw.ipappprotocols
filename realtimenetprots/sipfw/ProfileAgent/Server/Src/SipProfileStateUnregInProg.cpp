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
// Name        : sipprofilestateunreginprog
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileStateUnregInProg.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregInProg* CSIPProfileStateUnregInProg::NewL(
	CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateUnregInProg(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::CSIPProfileStateUnregInProg
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregInProg::CSIPProfileStateUnregInProg(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::~CSIPProfileStateUnregInProg
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregInProg::~CSIPProfileStateUnregInProg()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateUnregInProg::Name() const
	{
	return CSIPConcreteProfile::EUnregistrationInProgress;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CSIPProfileStateUnregInProg::SetNeighbourStates(CSIPProfileState& aUnregistered,
												CSIPProfileState& aRegistered)
	{
	iUnregistered = &aUnregistered;
	iRegistered = &aRegistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::EnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregInProg::EnableL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)     
	{
	PROFILE_DEBUG3("ProfileStateUnregInProg::EnableL id", aItem.ProfileId())
	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregInProg::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateUnregInProg::EnableSnapInUseL id,snap",
				   aItem.ProfileId())
	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregInProg::DisableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)     
	{
	PROFILE_DEBUG3("ProfileStateUnregInProg::DisableL id", aItem.ProfileId())

	DoCommonDisable(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregInProg::UpdateRegistrationL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	TUint profileId(aItem.ProfileId());
	PROFILE_DEBUG3("ProfileStateUnregInProg::UpdateRegistrationL id", profileId)
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::RegistrationStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregInProg::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateUnregInProg::RegStatusEventL id,status",
				   aItem.ProfileId(),
				   aStatus)

	if (aStatus == CSIPConcreteProfile::ERegistered)
		{
		aItem.ChangeStateL(iRegistered);
		}
	else if (aStatus == CSIPConcreteProfile::EUnregistered)
		{
		aItem.ChangeStateL(iUnregistered);
		}
	else
		{
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregInProg::ErrorOccurred
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateUnregInProg::ErrorOccurred(CSIPProfileCacheItem& aItem, 
											     TInt /*aError*/)
	{
	PROFILE_DEBUG3("ProfileStateUnregInProg::ErrorOccurred(profile id)",
				   aItem.ProfileId())
	DoCommonErrorOccurred(aItem);
	return ETrue;
	}
