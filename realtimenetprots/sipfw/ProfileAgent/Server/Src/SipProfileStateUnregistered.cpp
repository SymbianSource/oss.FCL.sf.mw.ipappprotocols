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
// Name        : sipprofilestateunregistered
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileStateUnregistered.h"
#include "SipProfileServerCore.h"
#include "sipconcreteprofile.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "sipalrmonitor.h"
#include "SipProfileLog.h"
#include <featmgr.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregistered* CSIPProfileStateUnregistered::NewL(
	CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateUnregistered(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::CSIPProfileStateUnregistered
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregistered::CSIPProfileStateUnregistered(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::~CSIPProfileStateUnregistered
// -----------------------------------------------------------------------------
//
CSIPProfileStateUnregistered::~CSIPProfileStateUnregistered()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateUnregistered::Name() const
	{
	return CSIPConcreteProfile::EUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void
CSIPProfileStateUnregistered::SetNeighbourStates(CSIPProfileState& aRegInProg,
												 CSIPProfileState& aWaitForIAP)
	{
	iRegInProg = &aRegInProg;
	iWaitForIAP = &aWaitForIAP;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::EnterL
// Check if should initiate registration.
// -----------------------------------------------------------------------------
//
void
CSIPProfileStateUnregistered::EnterL(CSIPProfileCacheItem& aItem,
									 TBool /*aMaySendRegStatusEvent*/,
									 CSIPProfileServerCore& /*aServerCore*/)
	{
	TBool updating = aItem.HasProfileUpdate();
	// Use current IAP if updating profile, but SNAP stays same.
	// Call IsSnapIdUpdated before ClearOldProfile.
	TBool getIap = !updating || aItem.IsSnapIdUpdated(aItem.Profile());
	aItem.SetSnapRetryCounter(0);

	PROFILE_DEBUG5("ProfileStateUnregistered::EnterL id,updating,getIap",
				   aItem.ProfileId(),
				   updating,
				   getIap)
	__ASSERT_ALWAYS(
		aItem.Profile().Status() == CSIPConcreteProfile::EUnregistered,
	 	User::Leave(KErrGeneral));
    
    // aItem.ClearOldProfile() can't be called here, because 
    // iPluginDirector.TerminateHandling is tring to do something
    // to oldProfile still, and it got crashes
	aItem.ClearMigrationProfiles();
	if (aItem.IsActiveState())
		{
		aItem.SendUnregisteredStatusEventL();
		TBool isVpnInUse = (FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn )
                                && aItem.IsVpnInUse());
		
        if ( aItem.IsReferred() && !aItem.IsShutdownInitiated() && !aItem.IsOfflineInitiated() &&
                !aItem.IsRfsInprogress() && !isVpnInUse)
            {
            aItem.ClearOldProfile();
            aItem.StartRegisterL(*iWaitForIAP, *iRegInProg, getIap);
            // CSIPProfileCacheItem::MonitorSnapL stops ALR monitor later if
            // updating SNAP.           
            return;
            }
        }
	// Stay unregistered, no need to monitor SNAP
	aItem.StopSnapMonitoring();
	iPluginDirector.TerminateHandling(aItem.UsedProfile());
	iPluginDirector.StartTimerForCleanup();
	aItem.ClearOldProfile();
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::RegisterL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::RegisterL(CSIPProfileCacheItem& aItem)
	{
	PROFILE_DEBUG3("ProfileStateUnregistered::RegisterL id", aItem.ProfileId())

	iPluginDirector.RegisterL(aItem.Profile());
	aItem.ChangeStateL(iRegInProg);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::RegistrationStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateUnregistered::RegStatusEventL",
				   aItem.ProfileId(),
				   aStatus)

	if (aStatus == CSIPConcreteProfile::ERegistrationInProgress)
		{
		aItem.ChangeStateL(iRegInProg);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::EnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::EnableL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG3("ProfileStateUnregistered::EnableL id", aItem.ProfileId())

	RegisterL(aItem);
	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 aSnapId)
	{
	PROFILE_DEBUG4("ProfileStateUnregistered::EnableSnapInUseL(profile,snap)",
				   aItem.ProfileId(),
				   aSnapId)

	aItem.MonitorSnapL(aSnapId, *iWaitForIAP, *iRegInProg);
	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::DisableL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG3("ProfileStateUnregistered::DisableL id", aItem.ProfileId())

	DoCommonDisable(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::CanBePermanentlyRemoved
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateUnregistered::CanBePermanentlyRemoved(
	const CSIPProfileCacheItem& aItem) const
	{
	TUint profileId(aItem.ProfileId());
	PROFILE_DEBUG3("ProfileStateUnregistered::CanBePermanentlyRemoved id",
				   profileId)
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::UpdateRegistrationL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	PROFILE_DEBUG3("ProfileStateUnregistered::UpdateRegistrationL id",
				   aItem.ProfileId())
	aItem.ClearOldProfile();

	CSIPConcreteProfile::TStatus state;
	__ASSERT_ALWAYS(
		iPluginDirector.State(state, aItem.Profile()) == KErrNotFound ||
		state == CSIPConcreteProfile::EUnregistered, User::Leave(KErrArgument));
        TBool isVpnInUse = (FeatureManager::FeatureSupported( KFeatureIdFfImsDeregistrationInVpn )
                             && aItem.IsVpnInUse());
        
        if (aItem.IsReferred() && !aItem.IsShutdownInitiated() 
                      && !aItem.IsRfsInprogress() && !isVpnInUse)
            {
            aItem.StartRegisterL(*iWaitForIAP, *iRegInProg, ETrue);
            }	
        }

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::ErrorOccurred
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateUnregistered::ErrorOccurred(CSIPProfileCacheItem& aItem, 
											  	  TInt /*aError*/)
	{
	PROFILE_DEBUG3("ProfileStateUnregistered::ErrorOccurred(profile id)",
				   aItem.ProfileId())
	DoCommonErrorOccurred(aItem);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateUnregistered::IapAvailableL
// Disallow migration during profile update.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateUnregistered::IapAvailableL(CSIPProfileCacheItem& aItem,
										         TUint32 /*aSnapId*/,
											     TUint32 aNewIapId)
	{
	PROFILE_DEBUG4("ProfileStateUnregistered::IapAvailableL(profile,iap)",
				   aItem.ProfileId(),
				   aNewIapId)

	TBool allowMigration = !aItem.HasProfileUpdate();
	if (!allowMigration)
		{
		aItem.SetIAPRefreshReminder();
		}
	aItem.SetClientPermission(aNewIapId, NULL, allowMigration);
	}
