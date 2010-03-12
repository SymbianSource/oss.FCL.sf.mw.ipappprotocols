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
// Name        : sipprofilestatereginprog.cpp
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileStateRegInProg.h"
#include "sipconcreteprofile.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "SipProfileLog.h"
#include "siperr.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateRegInProg*
CSIPProfileStateRegInProg::NewL(CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateRegInProg(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::CSIPProfileStateRegInProg
// -----------------------------------------------------------------------------
//
CSIPProfileStateRegInProg::CSIPProfileStateRegInProg(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::~CSIPProfileStateRegInProg
// -----------------------------------------------------------------------------
//
CSIPProfileStateRegInProg::~CSIPProfileStateRegInProg()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateRegInProg::Name() const
	{
	return CSIPConcreteProfile::ERegistrationInProgress;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::SetNeighbourStates(
	CSIPProfileState& aRegistered,
	CSIPProfileState& aUnregistered,
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aWaitForIAP,
	CSIPProfileState& aWaitForPermission)
	{
	iRegistered = &aRegistered;
	iUnregistered = &aUnregistered;
	iUnregInProg = &aUnregInProg;
	iWaitForIAP = &aWaitForIAP;
	iWaitForPermission = &aWaitForPermission;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::EnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::EnableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)     
	{
	PROFILE_DEBUG3("ProfileStateRegInProg::EnableL id", aItem.ProfileId())
	iPluginDirector.RegisterL(aItem.Profile());
	if(!aItem.IsUser(aObserver))
		{
		DoCommonEnableL(aItem,aObserver);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateRegInProg::EnableSnapInUseL(profile id)",
				   aItem.ProfileId())
	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::DisableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)     
	{
	PROFILE_DEBUG3("ProfileStateRegInProg::DisableL id", aItem.ProfileId())

	DoCommonDisable(aItem, aObserver);
	DisableWhileInProgressL(aItem, *iUnregInProg, *iUnregistered);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::UpdateRegistrationL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	PROFILE_DEBUG3("ProfileStateRegInProg::UpdateRegistrationL id", 
				   aItem.ProfileId())

	if (!iPluginDirector.IsRegisterPending(aItem.UsedProfile()))
		{
		iPluginDirector.DeregisterL(aItem.UsedProfile());
		aItem.ChangeStateL(iUnregistered);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::RegistrationStatusEventL
// Agent uses RegistrationStatusEventL if an action ((de)register..) succeeds,
// ErrorOccurred if not. Handle now only ERegistered (=registration done).
// Pass "profile updated" before "registration status changed" to client.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::RegistrationStatusEventL(
	CSIPProfileCacheItem& aItem,
	CSIPConcreteProfile::TStatus aStatus)
	{
	PROFILE_DEBUG4("ProfileStateRegInProg::RegStatusEventL id,status",
				   aItem.ProfileId(),
				   aStatus)

	if (aStatus == CSIPConcreteProfile::ERegistered)
		{
		aItem.ProfileRegisteredL(EFalse);
		aItem.ChangeStateL(iRegistered);
		}
	else
		{	
		if (aStatus == CSIPConcreteProfile::EUnregistrationInProgress)
			{
			aItem.ChangeStateL(iUnregInProg);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::ErrorOccurred
// If SNAP's initial register causes KErrCouldNotConnect, get another IAP.
// Also if SNAP's initial register causes KErrSIPICMPFailure, then do normal error
// handling because there is no point in getting New IAP for this error 
// CSIPProfileCacheItem::HandleError has called SetLastRegistrationError.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateRegInProg::ErrorOccurred(CSIPProfileCacheItem& aItem, 
											   TInt aError)
	{
	PROFILE_DEBUG4("ProfileStateRegInProg::ErrorOccurred id,err", 
				   aItem.ProfileId(),
				   aError)

	TUint32 dummySnapId(0);
	if ( (aError == KErrCouldNotConnect || 
		  aError == KErrTimedOut || 
		  aError == KErrSIPResolvingFailure ||
		  aError == KErrSIPTransportFailure) 
		  && aItem.IsSNAPConfigured(dummySnapId))
		{
		TRAPD(err, aItem.ChangeStateL(iWaitForIAP));
		if (err == KErrNone)
			{
			aItem.NewIAPFailed();						
			return EFalse;
			}
		}
			
	DoCommonErrorOccurred(aItem);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::IapAvailableL
// Handle a new IAP even if an update is queued, as update can't be done before
// successful register. Quit trying old IAP, as after registering new IAP,
// de-registering old IAP gives error, as it isn't registered.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::IapAvailableL(CSIPProfileCacheItem& aItem,
										      TUint32 aSnapId,
											  TUint32 aNewIapId)
	{
	PROFILE_DEBUG5("ProfileStateRegInProg::IapAvailableL id,snap,iap",
				   aItem.ProfileId(),
				   aSnapId,
				   aNewIapId)

	iPluginDirector.TerminateHandling(aItem.Profile());
	aItem.HandleNewIapL(aSnapId, aNewIapId, EFalse, *iWaitForPermission);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateRegInProg::ShutdownInitiated
// Initiate de-registration. PluginDirector never has a pending register in
// registered state.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateRegInProg::ShutdownInitiated( CSIPProfileCacheItem& aItem )
	{
      TRAP_IGNORE( DeregisterWhileRegInProgressL( aItem, *iUnregistered) )
	}
