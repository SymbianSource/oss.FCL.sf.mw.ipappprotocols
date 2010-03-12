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
// Name        : sipprofilestate.cpp
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileState.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileState::CSIPProfileState
// -----------------------------------------------------------------------------
//
CSIPProfileState::CSIPProfileState(CSIPPluginDirector& aDirector) :
	iPluginDirector(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileState::~CSIPProfileState
// -----------------------------------------------------------------------------
//
CSIPProfileState::~CSIPProfileState()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileState::EnterL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::EnterL(CSIPProfileCacheItem& /*aItem*/,
							  TBool /*aMaySendRegStatusEvent*/,
							  CSIPProfileServerCore& /*aServerCore*/)
	{
	PROFILE_DEBUG1("CSIPProfileState::EnterL")
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::RegisterL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::RegisterL(CSIPProfileCacheItem& /*aItem*/)
	{
    PROFILE_DEBUG1("CSIPProfileState::RegisterL")
	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::RemoveL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::RemoveL(CSIPProfileCacheItem& /*aItem*/)
	{
	PROFILE_DEBUG1("CSIPProfileState::RemoveL")
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::CanBePermanentlyRemoved
// -----------------------------------------------------------------------------
//
TBool CSIPProfileState::CanBePermanentlyRemoved(
	const CSIPProfileCacheItem& /*aItem*/) const
	{
	PROFILE_DEBUG1("CSIPProfileState::CanBePermanentlyRemoved")
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::UpdateRegistrationL
// -----------------------------------------------------------------------------
//			
void CSIPProfileState::UpdateRegistrationL(
    CSIPProfileCacheItem& /*aItem*/,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
    {
    PROFILE_DEBUG1("CSIPProfileState::UpdateRegistrationL")
    User::Leave(KErrArgument);
    }

// -----------------------------------------------------------------------------
// CSIPProfileState::RegistrationStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::RegistrationStatusEventL(
	CSIPProfileCacheItem& /*aItem*/,
	CSIPConcreteProfile::TStatus /*aStatus*/)
	{
	PROFILE_DEBUG1("CSIPProfileState::RegistrationStatusEventL")
	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::IapAvailableL
// Migration is disallowed by default.
// -----------------------------------------------------------------------------
//
void CSIPProfileState::IapAvailableL(CSIPProfileCacheItem& aItem,
									 TUint32 /*aSnapId*/,
									 TUint32 aNewIapId)
	{
	PROFILE_DEBUG4("ProfileState::IapAvailableL(state,iap)",
				   Name(),
				   aNewIapId)

	aItem.SetIAPRefreshReminder();
	aItem.SetClientPermission(aNewIapId, NULL, EFalse);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::ClientAllowsMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::ClientAllowsMigrationL(
    CSIPProfileCacheItem& /*aItem*/,
	TUint32 /*aIapId*/,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::ClientDisallowsMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::ClientDisallowsMigrationL(
    CSIPProfileCacheItem& /*aItem*/,
	TUint32 /*aIapId*/,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::MigrationIsAllowedL
// By default, ignore the event.
// -----------------------------------------------------------------------------
//
void CSIPProfileState::MigrationIsAllowedL(CSIPProfileCacheItem& /*aItem*/,
										   TUint32 /*aIapId*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::MigrationIsDisallowedL
// By default, ignore the event.
// -----------------------------------------------------------------------------
//
void CSIPProfileState::MigrationIsDisallowedL(CSIPProfileCacheItem& /*aItem*/,
										      TUint32 /*aIapId*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DoCommonErrorOccurred
// -----------------------------------------------------------------------------
//
void CSIPProfileState::DoCommonErrorOccurred(CSIPProfileCacheItem& aItem) const
	{
	PROFILE_DEBUG4("ProfileState::DoCommonErrorOccurred state,id",
				   Name(),
				   aItem.ProfileId())

	aItem.ClearOldProfile();
	aItem.ClearMigrationProfiles();
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DoCommonEnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::DoCommonEnableL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver) const
	{
	PROFILE_DEBUG4("ProfileState::DoCommonEnableL state,id",
				   Name(),
				   aItem.ProfileId())
	__ASSERT_ALWAYS(!aItem.IsUser(aObserver), User::Leave(KErrAlreadyExists));
	__ASSERT_ALWAYS(aItem.IsActiveState(), User::Leave(KErrNotFound));

	aItem.MoveToUserL(aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DoCommonDisable
// -----------------------------------------------------------------------------
//
void CSIPProfileState::DoCommonDisable(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver) const
	{
	PROFILE_DEBUG4("ProfileState::DoCommonDisable state,id",
				   Name(),
				   aItem.ProfileId())

	aItem.RemoveUser(aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::ShouldRefreshIAPs
// Not all states handle "IAP available" event, so by default don't refresh IAPs
// -----------------------------------------------------------------------------
//
TBool CSIPProfileState::ShouldRefreshIAPs(CSIPProfileCacheItem& /*aItem*/) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::IsAlrState
// -----------------------------------------------------------------------------
//
TBool CSIPProfileState::IsAlrState() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::ShutdownInitiated
// No action.
// -----------------------------------------------------------------------------
//
void CSIPProfileState::ShutdownInitiated(CSIPProfileCacheItem& /*aItem*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::CanProfileBeUpdated
// PluginDirector may have a pending register in (un)registered progress state,
// but not in (un)register state.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileState::CanProfileBeUpdated(CSIPProfileCacheItem& aItem) const
	{
	return !IsAlrState() &&
		   !iPluginDirector.IsRegisterPending(aItem.UsedProfile());
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::IsInitialRegisterDone
// Used in "wait for IAP", "wait for permission" and "migrating" states, so use
// current profile (aItem.Profile()), not profile of new IAP
// (aItem.UsedProfile()).
//
// NOTE: if IsInitialRegisterDone is used also from non-ALR states, use code:
//		const CSIPConcreteProfile* profile = &aItem.Profile();
//		if (!IsAlrState())
//			{
//			profile = &aItem.UsedProfile();
//			}
//		TBool done = (iPluginDirector.State(status, *profile) == KErrNone...
// -----------------------------------------------------------------------------
//
TBool CSIPProfileState::IsInitialRegisterDone(CSIPProfileCacheItem& aItem) const
	{
	// Check that in ALR-related state
	__ASSERT_ALWAYS(IsAlrState(),
		User::Panic(_L("ProfileState::IsInitRegDone"), KErrNotReady));
	
	CSIPConcreteProfile::TStatus status(CSIPConcreteProfile::EUnregistered);
	TBool done = (iPluginDirector.State(status, aItem.Profile()) == KErrNone &&
		   		  (status == CSIPConcreteProfile::ERegistered ||
		   		   status == CSIPConcreteProfile::ERegistrationInProgress));
	PROFILE_DEBUG4("ProfileState::IsInitRegDone id,done",
				   aItem.ProfileId(),
				   done)
	return done;
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::RegisterWithIapL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::RegisterWithIapL(CSIPProfileCacheItem& aItem,
							  			TUint32 aIapId,
							  			CSIPProfileState& aRegInProg) const
	{
	CSIPConcreteProfile& profile = aItem.Profile();
	profile.SetIapId(aIapId);
	iPluginDirector.RegisterL(profile);
	aItem.ChangeStateL(&aRegInProg);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::StartDeregisterL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::StartDeregisterL(CSIPProfileCacheItem& aItem,
										CSIPProfileState& aUnregInProg) const
	{
	PROFILE_DEBUG4("ProfileState::StartDeregisterL state,id",
				   Name(),
				   aItem.ProfileId())

	iPluginDirector.DeregisterL(aItem.UsedProfile());
	aItem.ChangeStateL(&aUnregInProg);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DoUpdateL
// -----------------------------------------------------------------------------
//
TBool CSIPProfileState::DoUpdateL(CSIPConcreteProfile& aProfile,
								  CSIPConcreteProfile& aOldProfile) const
	{
	iPluginDirector.UpdateRegistrationL(aProfile, aOldProfile);
	// Get old profile's state
	CSIPConcreteProfile::TStatus state;
	iPluginDirector.State(state, aOldProfile);
	return state == CSIPConcreteProfile::EUnregistrationInProgress;
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::RefreshIAPsFailed
// -----------------------------------------------------------------------------
//
void CSIPProfileState::RefreshIAPsFailed(CSIPProfileCacheItem& aItem) const
	{
	PROFILE_DEBUG4("ProfileState::RefreshIAPsFailed state,id",
				   Name(),
				   aItem.ProfileId())

	iPluginDirector.TerminateHandling(aItem.UsedProfile());
	DoCommonErrorOccurred(aItem);
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DisableWhileWaitingIAPL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::DisableWhileWaitingIAPL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aUnregistered) const
	{
	PROFILE_DEBUG3("ProfileState::DisableWhileWaitingIAPL id",
				   aItem.ProfileId())

	DoCommonDisable(aItem, aObserver);

	if (!aItem.IsReferred())
		{
		if (IsInitialRegisterDone(aItem))
			{
			// Work as in Registered-state
			StartDeregisterL(aItem, aUnregInProg);
			}
		else
			{
			aItem.ChangeStateL(&aUnregistered);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DisableWhileInProgressL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::DisableWhileInProgressL(CSIPProfileCacheItem& aItem,
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aUnregistered) const
	{
	PROFILE_DEBUG3("ProfileState::DisableWhileInProgressL id",
				   aItem.ProfileId())

	if (!aItem.IsReferred() &&
		!iPluginDirector.IsRegisterPending(aItem.UsedProfile()))
		{
		CSIPConcreteProfile::TStatus status;
		iPluginDirector.State(status, aItem.UsedProfile());
		iPluginDirector.DeregisterL(aItem.UsedProfile());
		if (status == CSIPConcreteProfile::ERegistrationInProgress)
			{
			aItem.ChangeStateL(&aUnregistered);
			}
		else
			{
			aItem.ChangeStateL(&aUnregInProg);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::NoNewIapAvailable()
// -----------------------------------------------------------------------------
//
void CSIPProfileState::NoNewIapAvailable(CSIPProfileCacheItem& /*aItem*/) 
	{
	PROFILE_DEBUG1("CSIPProfileState::NoNewIapAvailable")
	}

// -----------------------------------------------------------------------------
// CSIPProfileState::DeregisterWhileRegInProgressL
// -----------------------------------------------------------------------------
//
void CSIPProfileState::DeregisterWhileRegInProgressL(CSIPProfileCacheItem& aItem,
    CSIPProfileState& aUnregistered)
    {
    if ( !iPluginDirector.IsRegisterPending( aItem.UsedProfile() ) )
        {
        CSIPConcreteProfile::TStatus status;
        iPluginDirector.State( status, aItem.UsedProfile() );
        iPluginDirector.DeregisterL( aItem.UsedProfile() );
        if ( status == CSIPConcreteProfile::ERegistrationInProgress )
            {
            aItem.ChangeStateL( &aUnregistered );
            }
        }
    }
