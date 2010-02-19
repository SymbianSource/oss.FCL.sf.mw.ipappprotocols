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
// Name        : sipprofilestatewaitforpermission.cpp
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "sipprofilestatewaitforpermission.h"
#include "sipconcreteprofile.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "SipProfileLog.h"
#include "sipprofile.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateWaitForPermission*
CSIPProfileStateWaitForPermission::NewL(CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateWaitForPermission(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::CSIPProfileStateWaitForPermission
// -----------------------------------------------------------------------------
//
CSIPProfileStateWaitForPermission::CSIPProfileStateWaitForPermission(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::~CSIPProfileStateWaitForPermission
// -----------------------------------------------------------------------------
//
CSIPProfileStateWaitForPermission::~CSIPProfileStateWaitForPermission()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateWaitForPermission::Name() const
	{
	return static_cast<CSIPConcreteProfile::TStatus>(EWaitForPermission);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::EnableL
// EnableL is used by a profile that doesn't have a SNAP configured, and it
// should not be in this state.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::EnableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	TUint profileId(aItem.ProfileId());
	PROFILE_DEBUG3("ProfileStateWaitForPermission::EnableL(profile id)",
				   profileId)

	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateWaitForPermission::EnableSnapInUseL id,snap",
				   aItem.ProfileId())

	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::DisableL
// Waiting for migration permission, so actual registration has not started yet.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::DisableL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG3("ProfileStateWaitForPermission::DisableL id",
				   aItem.ProfileId())

	DisableWhileWaitingIAPL(aItem, aObserver, *iUnregInProg, *iUnregistered);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::SetNeighbourStates(
	CSIPProfileState& aWaitForIAP,
	CSIPProfileState& aMigratingToNewIAP,
	CSIPProfileState& aRegInProg,
	CSIPProfileState& aRegistered,
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aUnregistered)
	{
	iWaitForIAP 	   = &aWaitForIAP;
	iMigratingToNewIAP = &aMigratingToNewIAP;
	iRegInProg 		   = &aRegInProg;
	iRegistered 	   = &aRegistered;
	iUnregInProg 	   = &aUnregInProg;
	iUnregistered 	   = &aUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::RegistrationStatusEventL.
// If aStatus is "register in progress" or unregistered, current IAP dropped.
// If unregistered, IsInitialRegisterDone would return EFalse, but if status
// CHANGES to unregistered, it was different before (registration existed). As
// it ends, allow migration automatically and wait migration result.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateWaitForPermission::RegStatusEventL id,status",
	    		   aItem.ProfileId(),
	    		   aStatus)

	if (aStatus == CSIPConcreteProfile::EUnregistered ||
		aStatus == CSIPConcreteProfile::ERegistrationInProgress)
		{
		aItem.SendUnregisteredStatusEventL();
		iPluginDirector.TerminateHandling(aItem.Profile());
		aItem.RegistrationEnded();
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::ErrorOccurred
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateWaitForPermission::ErrorOccurred(
    CSIPProfileCacheItem& aItem,
    TInt /*aError*/)
	{
	PROFILE_DEBUG3("ProfileStateWaitForPermission::ErrorOccurred(profile id)",
				   aItem.ProfileId())
	DoCommonErrorOccurred(aItem);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::IapAvailableL
// -----------------------------------------------------------------------------
//
void
CSIPProfileStateWaitForPermission::IapAvailableL(CSIPProfileCacheItem& aItem,
										        TUint32 aSnapId,
											    TUint32 aNewIapId)
	{
	PROFILE_DEBUG5("ProfileStateWaitForPermission::IapAvailableL id,snap,iap",
				   aItem.ProfileId(),
				   aSnapId,
				   aNewIapId)

	aItem.HandleNewIapL(aSnapId,
						aNewIapId,
						IsInitialRegisterDone(aItem),
						*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::ClientAllowsMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::ClientAllowsMigrationL(
	CSIPProfileCacheItem& aItem,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG4(
		"ProfileStateWaitForPermission::ClientAllowsMigrationL id,iap",
		aItem.ProfileId(),
		aIapId)

	SetClientPermissionL(aItem, aIapId, aObserver, ETrue);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::ClientDisallowsMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::ClientDisallowsMigrationL(
	CSIPProfileCacheItem& aItem,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG4(
		"ProfileStateWaitForPermission::ClientDisallowsMigrationL id,iap",
		aItem.ProfileId(),
		aIapId)

	SetClientPermissionL(aItem, aIapId, aObserver, EFalse);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::MigrationIsAllowedL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::MigrationIsAllowedL(
	CSIPProfileCacheItem& aItem,
	TUint32 aIapId)
	{
	PROFILE_DEBUG4("ProfileStateWaitForPermission::MigrationIsAllowedL: id,iap",
				   aItem.ProfileId(),
				   aIapId)
	// Handle only one update or migration to IAP event at a time.
	__ASSERT_ALWAYS(!aItem.HasProfileUpdate(), User::Leave(KErrAlreadyExists));
	TUint32 snapId(0);
	__ASSERT_ALWAYS(aItem.IsSNAPConfigured(snapId), User::Leave(KErrNotFound));

	if (IsInitialRegisterDone(aItem))
		{
		iPluginDirector.RegisterL(aItem.CloneProfileL(aIapId));
		aItem.ChangeStateL(iMigratingToNewIAP);
		aItem.PassMigrationStartedToClientL(snapId, aIapId);
		}
	else
		{
		// Don't pass MigrationStarted to clients.
		TRAPD(err, RegisterWithIapL(aItem, aIapId, *iRegInProg));
		if (err != KErrNone)
			{
			aItem.HandleProfileError(err, aItem.Profile());
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::MigrationIsDisallowedL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::MigrationIsDisallowedL(
	CSIPProfileCacheItem& aItem,
	TUint32 aIapId)
	{
	PROFILE_DEBUG4(
		"ProfileStateWaitForPermission::MigrationIsDisAllowedL: id,iap",
		aItem.ProfileId(),
		aIapId)
	// Migration never happens in this state
	__ASSERT_ALWAYS(!aItem.IAPMigrationInProgress(), User::Leave(KErrGeneral));

	if (IsInitialRegisterDone(aItem))
		{
		// The existing profile remains registered.		
		aItem.ChangeStateL(iRegistered);
		User::LeaveIfError(
		    aItem.PassAlrErrorToClient(KErrAccessDenied, aIapId));
		}
	else
		{
		// Wait for another IAP to become available
		aItem.ChangeStateL(iWaitForIAP);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::IsAlrState
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateWaitForPermission::IsAlrState() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::ShutdownInitiated
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::ShutdownInitiated(
	CSIPProfileCacheItem& aItem)
	{
	if (IsInitialRegisterDone(aItem))
		{
		TRAP_IGNORE(StartDeregisterL(aItem, *iUnregInProg))
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForPermission::SetClientPermissionL
// Profile update can't exist now.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForPermission::SetClientPermissionL(
	CSIPProfileCacheItem& aItem,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TBool aAllowMigration) const
	{
	TUint32 snapId(0);
	__ASSERT_ALWAYS(aItem.IsSNAPConfigured(snapId), User::Leave(KErrNotFound));
	__ASSERT_ALWAYS(!aItem.HasProfileUpdate(), User::Leave(KErrAlreadyExists));

	aItem.SetClientPermission(aIapId, &aObserver, aAllowMigration);
	}
