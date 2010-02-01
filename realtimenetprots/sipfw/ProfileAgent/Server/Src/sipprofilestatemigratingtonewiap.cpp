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
// Name        : sipprofilestatemigratingtonewiap.cpp
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "sipprofilestatemigratingtonewiap.h"
#include "SipProfileServerCore.h"
#include "sipconcreteprofile.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "sipalrmonitor.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateMigratingToNewIAP*
CSIPProfileStateMigratingToNewIAP::NewL(CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateMigratingToNewIAP(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::CSIPProfileStateMigratingToNewIAP
// -----------------------------------------------------------------------------
//
CSIPProfileStateMigratingToNewIAP::CSIPProfileStateMigratingToNewIAP(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::~CSIPProfileStateMigratingToNewIAP
// -----------------------------------------------------------------------------
//
CSIPProfileStateMigratingToNewIAP::~CSIPProfileStateMigratingToNewIAP()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateMigratingToNewIAP::Name() const
	{
	return static_cast<CSIPConcreteProfile::TStatus>(EMigratingToNewIAP);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::EnableL
// EnableL is used by a profile that doesn't have a SNAP configured, and such
// profile should not be in this state.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::EnableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	TUint profileId(aItem.ProfileId());
	PROFILE_DEBUG3("ProfileStateMigratingToNewIAP::EnableL id",
				   profileId)

	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateMigratingToNewIAP::EnableSnapInUseL(profile)",
				   aItem.ProfileId())

	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::DisableL
// Actual registration has now been started, and must be stopped.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::DisableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG3("ProfileStateMigratingToNewIAP::DisableL id",
				   aItem.ProfileId())
	DoCommonDisable(aItem, aObserver);

	if (IsInitialRegisterDone(aItem))
		{
		// Work as in Registered state
		if (!aItem.IsReferred())
			{
			StartDeregisterL(aItem, *iUnregInProg);
			}
		}
	else
		{
		DisableWhileInProgressL(aItem, *iUnregInProg, *iUnregistered);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::SetNeighbourStates(
	CSIPProfileState& aUnregisteringOldIAP,
	CSIPProfileState& aWaitForIAP,
	CSIPProfileState& aRegistered,
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aUnregistered)
	{
	iUnregisteringOldIAP = &aUnregisteringOldIAP;
	iWaitForIAP = &aWaitForIAP;
	iRegistered = &aRegistered;
	iUnregInProg = &aUnregInProg;
	iUnregistered = &aUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::RegistrationStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateMigratingToNewIAP::RegStatusEventL id,status",
	    		   // Id of profile that most recently used profile agent
	    		   aItem.UsedProfile().Id(),
	    		   aStatus)
	__ASSERT_ALWAYS(!aItem.HasProfileUpdate(), User::Leave(KErrCorrupt));

	if (aStatus == CSIPConcreteProfile::ERegistered)
		{
		// Don't put old IAP's profile to iOldProfile, as ChangeStateL leads to
		// ProceedUpdatingProfileL using iOldProfile.
		CSIPConcreteProfile& profileWithOldIap = aItem.UseProfileWithNewIAP();

		// Pass profile with new IAP as parameter
		aItem.ProfileRegisteredL(ETrue);

		aItem.ChangeStateL(iUnregisteringOldIAP);

		// New IAP is registered, de-register the old IAP.
		iPluginDirector.DeregisterL(profileWithOldIap);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::ErrorOccurred
// Migrating fails, clear new IAP's profile. Pass ALR error (not ErrorOccurredL)
// to client. Old IAP's profile stays registered, so enter registered-state.
// -----------------------------------------------------------------------------
//
TBool
CSIPProfileStateMigratingToNewIAP::ErrorOccurred(CSIPProfileCacheItem& aItem,
											  	 TInt aError)
	{
	PROFILE_DEBUG4("ProfileStateMigratingToNewIAP::ErrorOccurred id,err",
				   aItem.UsedProfile().Id(),
				   aError)

	TUint32 failedIapId = aItem.UsedProfile().IapId();

	// Deletes iProfileWithNewIAP. TerminateHandling was already done in
	// CSIPProfileServerCore::HandleProfileError.
	DoCommonErrorOccurred(aItem);

	aItem.NewIAPFailed();
	TRAP_IGNORE(aItem.ChangeStateL(iRegistered))
	aItem.PassAlrErrorToClient(aError, failedIapId);
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::IapAvailableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::IapAvailableL(
    CSIPProfileCacheItem& aItem,
    TUint32 /*aSnapId*/,
    TUint32 aNewIapId)
	{
	PROFILE_DEBUG4("ProfileStateMigratingToNewIAP::IapAvailableL(profile,iap)",
				   aItem.ProfileId(),
				   aNewIapId)

	// If the old IAP is again the preferred IAP, the new IAP has dropped.
	// Abort migration to new IAP.
	if (aNewIapId == aItem.Profile().IapId())
		{
		iPluginDirector.TerminateHandling(aItem.UsedProfile());
		aItem.ClearMigrationProfiles();
		aItem.ChangeStateL(iRegistered);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::IsAlrState
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateMigratingToNewIAP::IsAlrState() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateMigratingToNewIAP::ShutdownInitiated
// De-register the currently registered profile (old IAP).
// -----------------------------------------------------------------------------
//
void CSIPProfileStateMigratingToNewIAP::ShutdownInitiated(
	CSIPProfileCacheItem& aItem)
	{
	TRAPD(err, iPluginDirector.DeregisterL(aItem.Profile()));
	if (err == KErrNone)
		{
		TRAP_IGNORE(aItem.ChangeStateL(iUnregInProg))
		}
	}
