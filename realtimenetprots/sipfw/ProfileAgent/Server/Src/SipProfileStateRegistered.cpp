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
// Name        : sipprofilestateregistered
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileStateRegistered.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "SipProfileServerCore.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateRegistered* CSIPProfileStateRegistered::NewL(
	CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateRegistered(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::CSIPProfileStateRegistered
// -----------------------------------------------------------------------------
//
CSIPProfileStateRegistered::CSIPProfileStateRegistered(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::~CSIPProfileStateRegistered
// -----------------------------------------------------------------------------
//
CSIPProfileStateRegistered::~CSIPProfileStateRegistered()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateRegistered::Name() const
	{
	return CSIPConcreteProfile::ERegistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::SetNeighbourStates(
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aUnregistered,
	CSIPProfileState& aRegInProg,
	CSIPProfileState& aWaitForPermission)
	{
	iUnregInProg 	   = &aUnregInProg;
	iUnregistered 	   = &aUnregistered;
	iRegInProg 		   = &aRegInProg;
	iWaitForPermission = &aWaitForPermission;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::EnterL
// Checks if a deregister or update registration should be initiated.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::EnterL(CSIPProfileCacheItem& aItem,
										TBool aMaySendRegStatusEvent,
										CSIPProfileServerCore& aServerCore)
	{
	PROFILE_DEBUG3("ProfileStateRegistered::EnterL id", aItem.ProfileId())
	// Migration never happens in this state
	__ASSERT_ALWAYS(!aItem.IAPMigrationInProgress(), User::Leave(KErrGeneral));
	aItem.SetSnapRetryCounter(0);
	// Clear the error code here, because it might be set to KErrDisconnnected,
	// if the state was previously changed from registered to registering.
    aItem.Profile().SetLastRegistrationError( KErrNone ); 

	if (aItem.IsActiveState())
		{
		// Don't pass registration status event if migration was disallowed, as
		// application sees the profile stays registered.
		if (aMaySendRegStatusEvent)
			{
			aServerCore.SendRegistrationStatusEventL(aItem);
			}

		if (aItem.IsReferred())
			{
			aItem.ProceedUpdatingProfileL(*iUnregInProg);
			return;
			}
		}

	StartDeregisterL(aItem, *iUnregInProg);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::EnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::EnableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)     
	{
	PROFILE_DEBUG3("ProfileStateRegistered::EnableL id", aItem.ProfileId())

	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateRegistered::EnableSnapInUseL(profile id)",
				   aItem.ProfileId())

	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::DisableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)     
	{
	PROFILE_DEBUG3("ProfileStateRegistered::DisableL id", aItem.ProfileId())

	DoCommonDisable(aItem, aObserver);
	if (!aItem.IsReferred())
		{
		StartDeregisterL(aItem, *iUnregInProg);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::RemoveL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::RemoveL(CSIPProfileCacheItem& aItem)
	{
	PROFILE_DEBUG3("ProfileStateRegistered::RemoveL id", aItem.ProfileId())

	StartDeregisterL(aItem, *iUnregInProg);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::UpdateRegistrationL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	PROFILE_DEBUG3("ProfileStateRegistered::UpdateRegistrationL id",
				   aItem.ProfileId())

	if (aItem.IsReferred())
		{
		aItem.ProceedUpdatingProfileL(*iUnregInProg);
		}
	else
		{
		StartDeregisterL(aItem, *iUnregInProg);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::RegistrationStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateRegistered::RegStatusEventL id,status",
				   aItem.ProfileId(),
				   aStatus)

	if (aStatus == CSIPConcreteProfile::ERegistrationInProgress)
		{
		aItem.ChangeStateL(iRegInProg);
		// If state is changed REGISTERED->REGISTERING, we can assume that
		// the connection was lost.
		aItem.Profile().SetLastRegistrationError( KErrDisconnected ); 
	    aItem.SendUnregisteredStatusEventL();
		}
	if (aStatus == CSIPConcreteProfile::EUnregistered)
		{
		aItem.ChangeStateL(iUnregistered);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::ErrorOccurred
// CSIPProfileCacheItem::HandleError calls SetLastRegistrationError. No need to
// call it here.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateRegistered::ErrorOccurred(CSIPProfileCacheItem& aItem, 
											    TInt /*aError*/)
	{
	PROFILE_DEBUG3("ProfileStateRegistered::ErrorOccurred(profile id)",
				   aItem.ProfileId())

	DoCommonErrorOccurred(aItem);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::IapAvailableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::IapAvailableL(CSIPProfileCacheItem& aItem,
                                               TUint32 aSnapId,
                                               TUint32 aNewIapId)
    {
    if ( aNewIapId == aItem.Profile().IapId() )
        {
        // The IAP that we are already using was offered for migration.
        // Let's allow the migration in order to allow other profiles
        // to use the IAP.
        aItem.SetClientPermission(aNewIapId, NULL, ETrue);        
        }
    else if (aItem.HasProfileUpdate())
        {
        // Currently updating, disallow migration
        aItem.SetIAPRefreshReminder();
        aItem.SetClientPermission(aNewIapId, NULL, EFalse);
        }
    else
        {
        aItem.HandleNewIapL(aSnapId, aNewIapId, ETrue, *iWaitForPermission);
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::ShouldRefreshIAPs
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateRegistered::ShouldRefreshIAPs(
	CSIPProfileCacheItem& aItem) const
	{
	return !aItem.HasProfileUpdate();
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegistered::ShutdownInitiated
// Initiate de-registration. PluginDirector never has a pending register in
// registered state.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegistered::ShutdownInitiated(CSIPProfileCacheItem& aItem)
	{
	TRAP_IGNORE(StartDeregisterL(aItem, *iUnregInProg))
	}
