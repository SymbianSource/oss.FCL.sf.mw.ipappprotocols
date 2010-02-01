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
// Name        : sipprofilestatewaitforiap.cpp
// Part of     : Sip Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "sipprofilestatewaitforiap.h"
#include "SipProfileServerCore.h"
#include "sipconcreteprofile.h"
#include "SipProfileCacheItem.h"
#include "sipplugindirector.h"
#include "sipalrmonitor.h"
#include "SipProfileLog.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStateWaitForIAP*
CSIPProfileStateWaitForIAP::NewL(CSIPPluginDirector& aDirector)
    {
    return new (ELeave) CSIPProfileStateWaitForIAP(aDirector);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::CSIPProfileStateWaitForIAP
// -----------------------------------------------------------------------------
//
CSIPProfileStateWaitForIAP::CSIPProfileStateWaitForIAP(
	CSIPPluginDirector& aDirector):
	CSIPProfileState(aDirector)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::~CSIPProfileStateWaitForIAP
// -----------------------------------------------------------------------------
//
CSIPProfileStateWaitForIAP::~CSIPProfileStateWaitForIAP()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::Name
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileStateWaitForIAP::Name() const
	{
	return static_cast<CSIPConcreteProfile::TStatus>(EWaitForIAP);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::EnableL
// EnableL is used by a profile that doesn't have a SNAP configured, and it
// should not be in this state.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::EnableL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& /*aObserver*/)
	{
	TUint profileId(aItem.ProfileId());
	PROFILE_DEBUG3("ProfileStateWaitForIAP::EnableL(profile id)", profileId)

	User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::EnableSnapInUseL(
	CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 /*aSnapId*/)
	{
	PROFILE_DEBUG3("ProfileStateWaitForIAP::EnableSnapInUseL id,snap",
				   aItem.ProfileId())

	DoCommonEnableL(aItem, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::DisableL
// Waiting for IAP, so actual registration has not started yet.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::DisableL(CSIPProfileCacheItem& aItem,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	PROFILE_DEBUG3("ProfileStateWaitForIAP::DisableL id", aItem.ProfileId())

	DisableWhileWaitingIAPL(aItem, aObserver, *iUnregInProg, *iUnregistered);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::SetNeighbourStates(
	CSIPProfileState& aWaitForPermission,
	CSIPProfileState& aRegInProg,
	CSIPProfileState& aUnregInProg,
	CSIPProfileState& aUnregistered)
	{
	iWaitForPermission = &aWaitForPermission;
	iRegInProg = &aRegInProg;
	iUnregInProg = &aUnregInProg;
	iUnregistered = &aUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::RegistrationStatusEventL
// Called when an action has succeeded. But as registration was not started yet,
// this should not be called.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::RegistrationStatusEventL(
	CSIPProfileCacheItem& /*aItem*/,
	CSIPConcreteProfile::TStatus /*aStatus*/)
	{
	// No action
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::ErrorOccurred
// CSIPProfileCacheItem::HandleError calls SetLastRegistrationError. No need to
// call it here.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateWaitForIAP::ErrorOccurred(CSIPProfileCacheItem& aItem, 
											    TInt /*aError*/)
	{
	PROFILE_DEBUG3("ProfileStateWaitForIAP::ErrorOccurred(profile id)",
				   aItem.ProfileId())
	DoCommonErrorOccurred(aItem);
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::IapAvailableL
// Registration doesn't exist in this state. Don't ask permission from clients.
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::IapAvailableL(CSIPProfileCacheItem& aItem,
										       TUint32 aSnapId,
											   TUint32 aNewIapId)
	{
	PROFILE_DEBUG5("ProfileStateWaitForIAP::IapAvailableL id,snap,iap",
				   aItem.ProfileId(),
				   aSnapId,
				   aNewIapId)

	aItem.HandleNewIapL(aSnapId, aNewIapId, EFalse, *iWaitForPermission);
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::ShouldRefreshIAPs
// Even if aItem.HasProfileUpdate()==ETrue, refresh IAPs as update is queued in
// this state.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateWaitForIAP::ShouldRefreshIAPs(
	CSIPProfileCacheItem& /*aItem*/) const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::IsAlrState
// -----------------------------------------------------------------------------
//
TBool CSIPProfileStateWaitForIAP::IsAlrState() const
	{
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::NoNewIapAvailable
// -----------------------------------------------------------------------------
//
void CSIPProfileStateWaitForIAP::NoNewIapAvailable(CSIPProfileCacheItem& aItem) 
	{	
	TInt retryCounter = aItem.SnapRetryCounter();
	PROFILE_DEBUG3("CSIPProfileStateWaitForIAP::NoNewIapAvailable, retries", 
					retryCounter)

	aItem.SetSnapRetryCounter(aItem.SnapRetryCounter() + 1);	
	}	

// -----------------------------------------------------------------------------
// CSIPProfileStateWaitForIAP::CanBePermanentlyRemoved
// -----------------------------------------------------------------------------
//
TBool  CSIPProfileStateWaitForIAP::CanBePermanentlyRemoved
                                (const CSIPProfileCacheItem& /*aItem*/) const
    {
        return ETrue;
    }
