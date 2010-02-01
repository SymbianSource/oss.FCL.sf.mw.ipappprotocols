// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofilestateunregisteringoldiap.cpp
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "sipprofilestateunregisteringoldiap.h"
#include "sipconcreteprofile.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregisteringOldIAP*
CSIPProfileStateUnregisteringOldIAP::NewL(CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateUnregisteringOldIAP(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::CSIPProfileStateUnregisteringOldIAP
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregisteringOldIAP::CSIPProfileStateUnregisteringOldIAP(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::~CSIPProfileStateUnregisteringOldIAP
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregisteringOldIAP::~CSIPProfileStateUnregisteringOldIAP()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::Name
// Though the new IAP has been registered, return the actual state id.
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateUnregisteringOldIAP::Name() const
	{
	return static_cast<CSIPConcreteProfile::TStatus>(EUnregisteringOldIAP);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::EnableL
// EnableL is used by a profile that doesn't have a SNAP configured, and such
// profile should not be in this state.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregisteringOldIAP::EnableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	TUint profileId(aItem.ProfileId());
	PROFILE_DEBUG3("ProfileStateUnregisteringOldIAP::EnableL(profile id)",
				   profileId)

	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregisteringOldIAP::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateUnregisteringOldIAP::EnableSnapInUseL(profile)",
				   aItem.ProfileId())

	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregisteringOldIAP::DisableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG3("ProfileStateUnregisteringOldIAP::DisableL id",
				   aItem.ProfileId())

	DoCommonDisable(aItem, aObserver);
	if (!aItem.IsReferred())
		{
		// Quit de-registering old IAP, so that can de-register new IAP profile
		iPluginDirector.TerminateHandling(aItem.UsedProfile());
		aItem.ClearMigrationProfiles();

		StartDeregisterL(aItem, *iUnregInProg);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregisteringOldIAP::SetNeighbourStates(
	CSIPProfileState& aRegistered,
	CSIPProfileState& aUnregInProg)
	{
	iRegistered = &aRegistered;
	iUnregInProg = &aUnregInProg;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::RegistrationStatusEventL
// Wait until de-register of the old IAP completes.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregisteringOldIAP::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateUnregisteringOldIAP::RegStatusEventL id,status",
				   // Id of profile that most recently used profile agent
	    		   aItem.UsedProfile().Id(),
	    		   aStatus)
	__ASSERT_ALWAYS(!aItem.HasProfileUpdate(), User::Leave(KErrCorrupt));

	if (aStatus == CSIPConcreteProfile::EUnregistered)
		{
		// Release the old IAP's connection. UsedProfile() returns now old IAP's
		// profile.
		iPluginDirector.TerminateHandling(aItem.UsedProfile());
		aItem.ClearMigrationProfiles();
		aItem.ChangeStateL(iRegistered);
		}
	iPluginDirector.StartTimerForCleanup();	
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::ErrorOccurred
// De-registering old IAP fails, but the new IAP is registered, so enter
// registered-state.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateUnregisteringOldIAP::ErrorOccurred(
    CSIPProfileCacheItem& aItem,
    TInt /*aError*/)
	{
	PROFILE_DEBUG3("ProfileStateUnregisteringOldIAP::ErrorOccurred(profile)",
				   aItem.UsedProfile().Id())

	// Deletes iProfileWithOldIAP. TerminateHandling was already done in
	// CSIPProfileServerCore::HandleProfileError.
	DoCommonErrorOccurred(aItem);

	// If entering register state fails, handle it as error.
	TRAPD(err, aItem.ChangeStateL(iRegistered));
	return err != KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::IsAlrState
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateUnregisteringOldIAP::IsAlrState() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregisteringOldIAP::ShutdownInitiated
// De-register the currently registered profile (new IAP).
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregisteringOldIAP::ShutdownInitiated(
	CSIPProfileCacheItem& aItem)
	{
	TRAP_IGNORE(StartDeregisterL(aItem, *iUnregInProg))
	}
