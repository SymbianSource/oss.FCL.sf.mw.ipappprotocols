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
// Name        : sipprofilecacheitem.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileCacheItem.h"
#include "SipProfileServerCore.h"
#include "sipextendedconcreteprofileobserver.h"
#include "SipProfileState.h"
#include "SipProfileLog.h"
#include "sipalrmigrationcontroller.h"
#include <sipprofile.h>
#include <cmmanager.h>
#include<e32const.h>

const TInt KSnapRetryCountThreshold = 3;
const TUint32 KDefaultSNAPIdentifier = KMaxTUint32;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem*
CSIPProfileCacheItem::NewL(CSIPProfileServerCore& aCore,
    					   CSIPProfileState* aUnregistered)
	{
    CSIPProfileCacheItem* self =
    	CSIPProfileCacheItem::NewLC(aCore, aUnregistered);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::NewLC
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem*
CSIPProfileCacheItem::NewLC(CSIPProfileServerCore& aCore,
    						CSIPProfileState* aUnregistered)
	{
	CSIPProfileCacheItem* self =
		new (ELeave) CSIPProfileCacheItem(aCore, aUnregistered);
    CleanupStack::PushL(self);
	self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::CSIPProfileCacheItem
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem::CSIPProfileCacheItem(CSIPProfileServerCore& aCore,
									  	   CSIPProfileState* aUnregistered) :
	iCacheState(ENormal),
	iCurrentState(aUnregistered),
	iServerCore(aCore),
	iMigrationDisallowed(EFalse),
	iDeltaTimer(0),
	iDeltaTimerCallBack(DoNewIapFailed, this),
	iSnapRetryCounter(0)
#ifdef CPPUNIT_TEST
    // Use array granularity 1, so each append allocates memory
    , iObservers(1),
    iUsers(1),
    iObserversWaitedForPermission(1)
#endif
	{
	iIsRfsInprogress = EFalse;
	iIsVpnInUse = EFalse;
	iInitialApnSelected = EFalse;
	iApnSwitchEnabled = EFalse;
	iIsOfflineInitiated = EFalse;
	iDeltaTimerEntry.Set(iDeltaTimerCallBack);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ConstructL()
	{
    iDeltaTimer = CDeltaTimer::NewL(CActive::EPriorityLow);
	}


// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::~CSIPProfileCacheItem
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem::~CSIPProfileCacheItem()
	{
	if (iDeltaTimer)
		{
		iDeltaTimer->Cancel();
		}
	delete iDeltaTimer;

	StopSnapMonitoring();

	iObservers.Reset();
	iObserversWaitedForPermission.Reset();
	iUsers.Reset();
	delete iQueuedProfile;
	delete iProfile;
	delete iOldProfile;
	delete iProfileWithNewIAP;
	delete iProfileWithOldIAP;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IapAvailable
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::IapAvailable(TUint32 aSnapId, TUint32 aNewIapId)
	{
	iDeltaTimer->Cancel();

	TRAPD(err, iCurrentState->IapAvailableL(*this, aSnapId, aNewIapId));
	if (err != KErrNone)
		{
		iServerCore.HandleProfileError(*this, err);
		}
	}
// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::MigrationIsAllowedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::MigrationIsAllowedL(TUint32 aIapId)
	{
	iCurrentState->MigrationIsAllowedL(*this, aIapId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::MigrationIsDisallowedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::MigrationIsDisallowedL(TUint32 aIapId)
	{
	iCurrentState->MigrationIsDisallowedL(*this, aIapId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ErrorOccurred
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ErrorOccurred(TInt aError)
	{
	HandleError(aError);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::NoNewIapAvailable
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::NoNewIapAvailable()
	{
	iCurrentState->NoNewIapAvailable(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RefreshIAPsFailed
// This is a fatal error, as profile no longer gets information of new IAPs.
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RefreshIAPsFailed()
	{
	iProfile->SetLastRegistrationError(KErrNoMemory);
	iCurrentState->RefreshIAPsFailed(*this);
	SwitchToUnregisteredState(EFalse);
	PassAlrErrorToClient(KErrNoMemory, Profile().IapId());
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetProfile
// Set iProfileId only once, then it never changes.
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetProfile(CSIPConcreteProfile* aProfile)
	{
	__ASSERT_ALWAYS(aProfile, User::Panic(_L("ProfileCacheItem:SetProfile"),
										  KErrArgument));
	delete iProfile;
	iProfile = aProfile;

	if (iProfileId == 0)
		{
		iProfileId = iProfile->Id();
		}

	CheckProfileEnabledState();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetProfileId
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetProfileId(TUint aProfileId)
	{
	iProfileId = aProfileId;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ProfileId
// -----------------------------------------------------------------------------
//
TUint CSIPProfileCacheItem::ProfileId() const
	{
	return iProfileId;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::Profile
// Return the CSIPConcreteProfile that is most recently registered.
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileCacheItem::Profile()
	{
	__ASSERT_ALWAYS(iProfile,
					User::Panic(_L("ProfileCacheItem:Profile"), KErrNotFound));
	return *iProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::Profile
// -----------------------------------------------------------------------------
//
const CSIPConcreteProfile& CSIPProfileCacheItem::Profile() const
	{
	__ASSERT_ALWAYS(iProfile,
					User::Panic(_L("ProfileCacheItem:ProfileC"), KErrNotFound));
	return *iProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::LatestProfile
// Return the CSIPConcreteProfile that includes the most recent updates.
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileCacheItem::LatestProfile()
	{
	return iQueuedProfile ? *iQueuedProfile : Profile();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ReferenceCount
// -----------------------------------------------------------------------------
//
TUint CSIPProfileCacheItem::ReferenceCount() const
	{
	return iUsers.Count();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::CacheOldProfile
// If update is done before a previous update completed, it contains also the
// earlier update's changes. Some states queue the update. If a new update
// comes, it is stored and the previous discarded.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::CacheOldProfile(CSIPConcreteProfile* aNewProfile,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	__ASSERT_ALWAYS(aNewProfile && aNewProfile->Id() == ProfileId(),
					User::Panic(_L("ProfileCacheItem:CacheOld"), KErrArgument));

	if (iCurrentState->CanProfileBeUpdated(*this))
		{
		StoreProfileToUpdate(aNewProfile);
		return ETrue;
		}

	delete iQueuedProfile;
	iQueuedProfile = aNewProfile;
	iQueuedObserver = &aObserver;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ClearOldProfile
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ClearOldProfile()
	{
	delete iOldProfile;
	iOldProfile = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::HasProfileUpdate
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::HasProfileUpdate() const
	{
	return iOldProfile != NULL;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::UsedProfile
// Return the CSIPConcreteProfile that most recently used profile agent to
// register, deregister or update.
//
// iProfileWithNewIAP and iProfileWithOldIAP exist only during ALR migration,
// and they never exist both at the same time.
// iProfileWithNewIAP exists only in state CSIPProfileStateMigratingToNewIAP and
// iProfileWithOldIAP exists only in state CSIPProfileStateUnregisteringOldIAP.
//
// iOldProfile exists during an update.
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileCacheItem::UsedProfile()
	{
	return iProfileWithNewIAP ? *iProfileWithNewIAP :
		   iProfileWithOldIAP ? *iProfileWithOldIAP :
		   iOldProfile ? *iOldProfile : Profile();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::UsedProfile
// -----------------------------------------------------------------------------
//
const CSIPConcreteProfile& CSIPProfileCacheItem::UsedProfile() const
	{
	return iProfileWithNewIAP ? *iProfileWithNewIAP :
		   iProfileWithOldIAP ? *iProfileWithOldIAP :
		   iOldProfile ? *iOldProfile : Profile();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::AddObserverL
// Don't add observer to iObserversWaitedForPermission. It has only observers
// that were notified of a new IAP.
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::AddObserverL(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iObservers.AppendL(&aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RemoveObserver
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RemoveObserver(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	TInt index = iObservers.Find(&aObserver);
	if (index != KErrNotFound)
		{
		iObservers.Remove(index);
		iObservers.Compress();
		}

	RemoveFromPendingObservers(aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::Observers
// -----------------------------------------------------------------------------
//
const RPointerArray<MSIPExtendedConcreteProfileObserver>&
CSIPProfileCacheItem::Observers() const
	{
	return iObservers;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsObserverOrUser
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsObserverOrUser(
	const MSIPExtendedConcreteProfileObserver& aObserver) const
	{
	return (iObservers.Find(&aObserver) != KErrNotFound) || IsUser(aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RemoveUser
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RemoveUser(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	TInt index = iUsers.Find(&aObserver);
	if (index != KErrNotFound)
		{
		iUsers.Remove(index);
		iUsers.Compress();
		CheckProfileEnabledState();
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RemoveAllUsers
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RemoveAllUsers()
	{
	iUsers.Reset();
    CheckProfileEnabledState();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsUser
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsUser(
	const MSIPExtendedConcreteProfileObserver& aObserver) const
	{
	return iUsers.Find(&aObserver) != KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::MoveToUserL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::MoveToUserL(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iUsers.AppendL(&aObserver);

	CheckProfileEnabledState();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsActiveState
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsActiveState() const
	{
	return iCacheState == ENormal;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::HasAorL
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::HasAorL(const TDesC8& aAOR) const
	{
    TUriParser8 otherUriparser;
    User::LeaveIfError(otherUriparser.Parse(aAOR));
    TInt err = otherUriparser.Equivalent(iProfile->AORUri8());
    if (err == KErrNoMemory)
        {
        User::Leave(err);
        }
    return err == KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::StartRegisterL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::StartRegisterL(CSIPProfileState& aWaitForIAP,
										  CSIPProfileState& aRegInProg,
										  TBool aGetIap)
	{
	TUint32 snapId(0);
		PROFILE_DEBUG3("ProfileCacheItem::StartRegisterL - SNAP ID", snapId)
	if (aGetIap && IsSNAPConfigured(snapId))
		{
		PROFILE_DEBUG3("ProfileCacheItem::StartRegisterL - MONITOR SNAP", snapId)
		MonitorSnapL(snapId, aWaitForIAP, aRegInProg);
		}
	else
		{
		PROFILE_DEBUG3("ProfileCacheItem::StartRegisterL - REGISTER", snapId)
		iCurrentState->RegisterL(*this);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::EnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::EnableL(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iCurrentState->EnableL(*this, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::EnableSnapInUseL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::EnableSnapInUseL(
	const MSIPExtendedConcreteProfileObserver& aObserver,
	TUint32 aSnapId)
	{
	iCurrentState->EnableSnapInUseL(*this, aObserver, aSnapId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::DisableL(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iCurrentState->DisableL(*this, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RemoveL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RemoveL()
	{
	iCacheState = EToBeRemoved;
	iCurrentState->RemoveL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ShutdownInitiated
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ShutdownInitiated()
	{
	iIsShutdownInitiated = ETrue;
	iCurrentState->ShutdownInitiated(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsShutdownInitiated
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsShutdownInitiated() const
	{
	return iIsShutdownInitiated;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RfsInprogress
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RfsInprogress(TBool aStatus)
    {
    iIsRfsInprogress = aStatus;
    iCurrentState->ShutdownInitiated(*this);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsRfsInprogress
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsRfsInprogress() const
    {
    return iIsRfsInprogress;
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::VpnInUse
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::VpnInUse(TBool aStatus)
    {
    iIsVpnInUse = aStatus;
    iCurrentState->ShutdownInitiated(*this);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsVpnInUse
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsVpnInUse() const
    {
    return iIsVpnInUse;
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetApnSelected
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetApnSelected(TBool aStatus)
    {
    iInitialApnSelected = aStatus;
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsInitialApnSelected
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsInitialApnSelected() const
    {
    return iInitialApnSelected;
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetApnSwitchStatus
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetApnSwitchStatus(TBool aStatus)
    {
    iApnSwitchEnabled = aStatus;
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsApnSwitchEnabled
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsApnSwitchEnabled() const
    {
    return iApnSwitchEnabled;
    }
    
// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::CanBePermanentlyRemoved
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::CanBePermanentlyRemoved() const
	{
	return iCurrentState->CanBePermanentlyRemoved(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::UpdateRegistrationL(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iCurrentState->UpdateRegistrationL(*this, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RegistrationStatusEvent
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RegistrationStatusEventL(
	CSIPConcreteProfile::TStatus aStatus,
	TUint32 aStatusId)
	{
	UsedProfile().SetContextId(aStatusId);
	iCurrentState->RegistrationStatusEventL(*this, aStatus);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ChangeStateL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ChangeStateL(CSIPProfileState* aNewState)
	{
	CSIPConcreteProfile::TStatus newState = aNewState->Name();
	PROFILE_DEBUG5("ProfileCacheItem::ChangeStateL id,old state,new state",
				   ProfileId(),
				   iCurrentState->Name(),
				   newState)

	TBool maySendRegStatusEvent = !iCurrentState->IsAlrState();
	if (!aNewState->IsAlrState())
		{
		iProfile->SetStatus(newState);
		}

	iCurrentState = aNewState;
	iCurrentState->EnterL(*this, maySendRegStatusEvent, iServerCore);

	ResumeL();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ResumeL
// Don't resume if migrating. Resume is only used with SNAP.
// If both a queued update and iMustRefreshIAPs exist, do update first.
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ResumeL()
	{
	PROFILE_DEBUG3("ProfileCacheItem::ResumeL id", ProfileId())

	TUint32 snapId(0);
	if (!IAPMigrationInProgress())
		{
		PROFILE_DEBUG1("ProfileCacheItem::ResumeL SNAP used,not migrating")

		if (iQueuedProfile && iCurrentState->CanProfileBeUpdated(*this))
			{
			ResumeQueuedUpdateL();
			}
		else if(IsSNAPConfigured(snapId))
			{
			PROFILE_DEBUG1("ProfileCacheItem::ResumeL don't resume now")

			// HasProfileUpdate is checked in ShouldRefreshIAPs
			if (iMustRefreshIAPs &&
				iCurrentState->ShouldRefreshIAPs(*this) &&
				iMigrationController)
				{
				iMustRefreshIAPs = EFalse;
				PROFILE_DEBUG3("Refresh IAPs, snapId=", snapId)
				iMigrationController->RefreshIapAvailabilityL(snapId);
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetIAPRefreshReminder
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetIAPRefreshReminder()
	{
	iMustRefreshIAPs = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::NewIAPFailed
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::NewIAPFailed()
	{
	PROFILE_DEBUG1("ProfileCacheItem::NewIAPFailed")

	iDeltaTimer->Remove(iDeltaTimerEntry);

	const TInt KIapFailedDelay(1);

	TTimeIntervalMicroSeconds32 interval(KIapFailedDelay);
	iDeltaTimer->Queue(interval, iDeltaTimerEntry);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SendUnregisteredStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SendUnregisteredStatusEventL()
	{
	iServerCore.SendUnregisteredStatusEventL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::HandleProfileError
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::HandleProfileError(TInt aError,
	CSIPConcreteProfile& aProfile)
	{
	PROFILE_DEBUG3("ProfileCacheItem::HandleProfileError", aError)
	iServerCore.SIPProfileErrorEvent(aProfile, aError);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::HandleError
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::HandleError(TInt aError)
	{
	PROFILE_DEBUG4("ProfileCacheItem::HandleError id,err", ProfileId(), aError)

	if (iCurrentState->ErrorOccurred(*this, aError))
		{
		PROFILE_DEBUG1("ProfileCacheItem::HandleError go unregistered")
		        
        if(SwitchToUnregisteredState(ETrue))
            {
            return EFalse;
            }		
		
		if (aError != KErrNone)
			{
			iProfile->SetLastRegistrationError(aError);
			}
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::Compare
// During linear search operations the search term is always passed as the first
// argument and the second argument is an element of the array being searched.
//
// As MSIPProfileAgentObserver calls can lead here, aSearched.iProfileId can be
// a temp id, used in iOldProfile, iProfileWithNewIAP, iProfileWithOldIAP.
// iQueuedProfile has the same profile id as current profile.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::Compare(const CSIPProfileCacheItem& aSearched,
									const CSIPProfileCacheItem& aArrayItem)
	{
	CSIPConcreteProfile* old    = aArrayItem.iOldProfile;
	CSIPConcreteProfile* newIap = aArrayItem.iProfileWithNewIAP;
	CSIPConcreteProfile* oldIap = aArrayItem.iProfileWithOldIAP;
	return (aArrayItem.iProfileId  == aSearched.iProfileId) ||
		   (old    && old->Id()    == aSearched.iProfileId) ||
		   (newIap && newIap->Id() == aSearched.iProfileId) ||
		   (oldIap && oldIap->Id() == aSearched.iProfileId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ClientAllowsMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ClientAllowsMigrationL(TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iCurrentState->ClientAllowsMigrationL(*this, aIapId, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ClientDisallowsMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ClientDisallowsMigrationL(TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	iCurrentState->ClientDisallowsMigrationL(*this, aIapId, aObserver);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetClientPermission
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetClientPermission(TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver* aObserver,
	TBool aAllowMigration)
	{
	PROFILE_DEBUG5("ProfileCacheItem::SetClientPermission id,iap,allow",
				   ProfileId(),
				   aIapId,
				   aAllowMigration)
	PROFILE_DEBUG3("pending observers=", iObserversWaitedForPermission.Count())

	if (!aAllowMigration)
		{
		iMigrationDisallowed = ETrue;
		}
	if (aObserver)
		{
		RemoveFromPendingObservers(*aObserver);
		}

	if (iObserversWaitedForPermission.Count() == 0 && iMigrationController)
		{
		PROFILE_DEBUG3("ProfileCacheItem::SetClientPermission allow",
 	    			   !iMigrationDisallowed)
		iMigrationController->SetMigrationPermission(*this,
													 !iMigrationDisallowed,
													 aIapId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsSNAPConfigured
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsSNAPConfigured(TUint32& aSnapId) const
	{
	TBool Val = (UsedProfile().ExtensionParameter(KSIPSnapId, aSnapId) == KErrNone);

	PROFILE_DEBUG3("Exceptional SNAP Entry verifier :", KDefaultSNAPIdentifier)
	if(Val && aSnapId == KDefaultSNAPIdentifier)
		{
		PROFILE_DEBUG3("Snap ID Found to be :", aSnapId)
		TRAPD(err, DefaultSNAPL(aSnapId));
		if(err)
			{
				Val = EFalse;
			}
		}
	PROFILE_DEBUG3("ProfileCacheItem::IsSNAPConfigured; return SNAP ID Value = ", aSnapId)
	PROFILE_DEBUG3("ProfileCacheItem::IsSNAPConfigured; return Value = ", Val)
	return Val;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::PassMigrationStartedToClientL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::PassMigrationStartedToClientL(TUint32 aSnapId,
														 TUint32 aIapId)
	{
	iServerCore.MigrationStartedL(*this, aSnapId, aIapId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::PassAlrErrorToClient
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCacheItem::PassAlrErrorToClient(TInt aError, TUint32 aIapId)
	{
	TUint32 snapId(0);
	if (!IsSNAPConfigured(snapId))
	    {
	    return KErrNotFound;
	    }
	return iServerCore.PassAlrErrorToClient(*this, aError, snapId, aIapId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::HandleNewIapL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::HandleNewIapL(TUint32 aSnapId,
									     TUint32 aIapId,
									     TBool aRegistrationExists,
									     CSIPProfileState& aWaitForPermission)
	{
	PROFILE_DEBUG5("ProfileCacheItem::HandleNewIapL id,snap,iap",
				   ProfileId(),
				   aSnapId,
				   aIapId)
	PROFILE_DEBUG3("reg exists=", aRegistrationExists)

	TUint32 snapId(0);
	__ASSERT_ALWAYS(IsSNAPConfigured(snapId), User::Leave(KErrNotFound));
	__ASSERT_ALWAYS(aSnapId == snapId, User::Leave(KErrArgument));

	iMigrationDisallowed = EFalse;

	// SetMigrationPermission can syncronously lead to MigrationIs(Dis)AllowedL
	// so enter state first.
	ChangeStateL(&aWaitForPermission);
	if (aRegistrationExists)
		{
		// Don't wait (dis)allow to an earlier IAP
		iObserversWaitedForPermission.Reset();

		for (TInt i = 0; i < iObservers.Count(); ++i)
	        {
	        if (iObservers[i]->IapAvailableL(ProfileId(), aSnapId, aIapId))
	        	{
	        	iObserversWaitedForPermission.AppendL(iObservers[i]);
	        	}
	        }
		PROFILE_DEBUG3("pending count=", iObserversWaitedForPermission.Count())
		if (iObserversWaitedForPermission.Count() == 0)
			{
			iMigrationController->SetMigrationPermission(*this, ETrue, aIapId);
			}
		}
	else
		{
		// Don't pass IAP-Available, MigrationStarted or MigrationCompleted to
		// clients. This profile's clients are not asked for permission, but
		// another profile with same SNAP can disallow migration.
		__ASSERT_ALWAYS(!iProfileWithNewIAP, User::Leave(KErrNotFound));
		iMigrationController->SetMigrationPermission(*this, ETrue, aIapId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::MonitorSnapL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::MonitorSnapL(TUint32 aSnapId,
										CSIPProfileState& aWaitForIAP,
										CSIPProfileState& aRegInProg)
	{
	if (iMigrationController && iMigrationController->SnapId() != aSnapId)
		{
		// Stop monitoring the old SNAP id
		StopSnapMonitoring();
		}

	if (!iMigrationController) 
		{
	    TUint32 bearerId = BearerID();
	    TSipSNAPConfigurationData aSnapData(aSnapId,bearerId);
		iMigrationController = &iServerCore.MigrationControllerL(aSnapData);
		PROFILE_DEBUG3("ProfileCacheItem::BearerFilter value is", bearerId)
		TUint32 iapId = iMigrationController->AttachProfileL(*this);
		if (iapId)
			{
			// Initial IAP obtained
			aRegInProg.RegisterWithIapL(*this, iapId, aRegInProg);
			}
		else
			{
			// No IAP available yet
			ChangeStateL(&aWaitForIAP);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ProfileRegisteredL
// If initial registration, don't pass MigrationCompleted.
// -----------------------------------------------------------------------------
//
void
CSIPProfileCacheItem::ProfileRegisteredL(TBool aMigrating)
	{
	TUint32 snapId(0);
	if (IsSNAPConfigured(snapId))
		{
		__ASSERT_ALWAYS(iMigrationController != NULL,
						User::Leave(KErrNotFound));

		TInt err = iMigrationController->SetIapAcceptance(*this, ETrue);
		if (err != KErrNone)
			{
			iServerCore.HandleProfileError(*this, err);
			return;
			}
		if (aMigrating)
			{
			iServerCore.MigrationCompletedL(*this, snapId, Profile().IapId());
			}
		// Send notification of the updated IAP
		iServerCore.SendProfileUpdatedEventL(*this, Profile());
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RegistrationEnded
// Can use the same profile id, as TerminateHandling cleared earlier profile
// from Profile Agent.
// If client uses SetClientPermission later, migration controller ignores it
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RegistrationEnded()
	{
	PROFILE_DEBUG3("ProfileCacheItem::RegistrationEnded id", ProfileId())

	// If this has already been done, does nothing.
	iMigrationController->SetMigrationPermission(*this, ETrue);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::CheckProfileEnabledState
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::CheckProfileEnabledState()
    {
    UsedProfile().SetEnabled(iUsers.Count() > 0);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsReferred
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsReferred() const
	{
	return ReferenceCount() > 0 || Profile().IsAutoRegistrationEnabled();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ProceedUpdatingProfileL
// If updating SNAP, de-register old SNAP's profile, then register new SNAP's
// profile. CSIPProfileStateUnregistered::EnterL calls StartRegisterL, ending
// old SNAP monitoring and starts monitoring new SNAP.
// -----------------------------------------------------------------------------
//
void
CSIPProfileCacheItem::ProceedUpdatingProfileL(CSIPProfileState& aUnregInProg)
	{
	if (HasProfileUpdate() && !IAPMigrationInProgress())
		{
		if (IsSnapIdUpdated(Profile()))
			{
			aUnregInProg.StartDeregisterL(*this, aUnregInProg);
			}
		else
			{
			if (aUnregInProg.DoUpdateL(Profile(), *iOldProfile))
				{
				ChangeStateL(&aUnregInProg);
				}
			else
				{
				ClearOldProfile();
				// Update is complete, resume any pending action
				ResumeL();
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::StopSnapMonitoring
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::StopSnapMonitoring()
	{
	if (iMigrationController)
		{
		iMigrationController->DetachProfile(*this);
		iMigrationController = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::CloneProfileL
// Use a new profile id for the new IAP, so Profile Agent treats it as different
// profile from the old IAP.
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileCacheItem::CloneProfileL(TUint aIapId)
	{
	PROFILE_DEBUG4("ProfileCacheItem::CloneProfileL id,iap",
				   ProfileId(),
				   aIapId)
	__ASSERT_ALWAYS(!iProfileWithNewIAP, User::Leave(KErrAlreadyExists));

	CSIPConcreteProfile* profile = Profile().CloneL();
	CleanupStack::PushL(profile);
	profile->SetId(iServerCore.GenerateProfileIdL());
	profile->SetStorageId(Profile().StorageId());

	// Use the same Contact user-part as the existing profile.
	// CSIPConcreteProfile::CloneL generated a new one.
	const TDesC8* user(NULL);
	User::LeaveIfError(Profile().ExtensionParameter(
		KSIPContactHeaderUser, user));
	profile->SetExtensionParameterL(KSIPContactHeaderUser, *user);
	CleanupStack::Pop(profile);
	profile->SetIapId(aIapId);

	iProfileWithNewIAP = profile;
	return *iProfileWithNewIAP;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ClearMigrationProfiles
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ClearMigrationProfiles()
	{
	delete iProfileWithNewIAP;
	iProfileWithNewIAP = NULL;
	delete iProfileWithOldIAP;
	iProfileWithOldIAP = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::UseProfileWithNewIAP
// The new IAP works. Prepare to de-register the old IAP.
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileCacheItem::UseProfileWithNewIAP()
	{
	__ASSERT_ALWAYS(!iProfileWithOldIAP,
					User::Panic(KNullDesC, KErrAlreadyExists));
	__ASSERT_ALWAYS(iProfileWithNewIAP != NULL,
					User::Panic(KNullDesC, KErrNotFound));

	PROFILE_DEBUG4("ProfileCacheItem::UseProfileWithNewIAP Swapping Id1, Id2",
					iProfile->Id(), iProfileWithNewIAP->Id() )

	iProfileWithNewIAP->SetStatus( CSIPConcreteProfile::ERegistered );

	// Swap profile id's. Only the original id can be passed to clients.
	TUint32 originalId( iProfile->Id() );
	iProfile->SetId( iProfileWithNewIAP->Id() );
	iProfileWithNewIAP->SetId( originalId );

	iProfileWithOldIAP = iProfile;
	iProfile = iProfileWithNewIAP;
	iProfileWithNewIAP = NULL;

	return *iProfileWithOldIAP;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IAPMigrationInProgress
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IAPMigrationInProgress() const
	{
	return iProfileWithNewIAP || iProfileWithOldIAP;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::StoreProfileToUpdate
// -----------------------------------------------------------------------------
//
void
CSIPProfileCacheItem::StoreProfileToUpdate(CSIPConcreteProfile* aNewProfile)
	{
	PROFILE_DEBUG3("ProfileCacheItem::StoreProfileToUpdate id", ProfileId())

	if (!iOldProfile)
		{
		iOldProfile = iProfile;
		iProfile = NULL;
		}
	SetProfile(aNewProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::ResumeQueuedUpdateL
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::ResumeQueuedUpdateL()
	{
	PROFILE_DEBUG3("ProfileCacheItem::ResumeQueuedUpdateL id", ProfileId())
	__ASSERT_ALWAYS(iQueuedProfile != NULL && iQueuedObserver != NULL,
				    User::Leave(KErrNotFound));
	__ASSERT_ALWAYS(!IAPMigrationInProgress(), User::Leave(KErrNotReady));

	CSIPConcreteProfile* profile = iQueuedProfile;
	TUint32 snapId(0);
	if (!IsSnapIdUpdated(*profile) && IsSNAPConfigured(snapId))
		{
		// Use the current IAP id
		profile->SetIapId(UsedProfile().IapId());
		}
	const MSIPExtendedConcreteProfileObserver* obs = iQueuedObserver;
	iQueuedProfile = NULL;
	iQueuedObserver = NULL;
	StoreProfileToUpdate(profile);

	iServerCore.UpdateRegistrationL(ProfileId(), *obs);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsSnapIdUpdated
// -----------------------------------------------------------------------------
//
TBool
CSIPProfileCacheItem::IsSnapIdUpdated(CSIPConcreteProfile& aNewProfile) const
	{
	TUint32 currentSnapId(0);
	IsSNAPConfigured(currentSnapId);

	TUint32 newSnapId(0);
    TBool res(EFalse);
	
	res = (aNewProfile.ExtensionParameter(KSIPSnapId, newSnapId) == KErrNone &&
			newSnapId != currentSnapId);
	PROFILE_DEBUG3("Exceptional SNAP Entry verifier :", KDefaultSNAPIdentifier)
	if(res && newSnapId == KDefaultSNAPIdentifier)
		{
		res=EFalse;
		}
	PROFILE_DEBUG3("CSIPProfileCacheItem::IsSnapIdUpdated CurrentSNAPID",currentSnapId)
    PROFILE_DEBUG3("CSIPProfileCacheItem::IsSnapIdUpdated NewSNAPID",newSnapId)
	return res;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::RemoveFromPendingObservers
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::RemoveFromPendingObservers(
	const MSIPExtendedConcreteProfileObserver& aObserver)
	{
	TInt index = iObserversWaitedForPermission.Find(&aObserver);
	if (index != KErrNotFound)
		{
		iObserversWaitedForPermission.Remove(index);
		iObserversWaitedForPermission.Compress();
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::DoNewIapFailed
// Ignore SetIapAcceptance return value, as already handling error (IAP failed).
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCacheItem::DoNewIapFailed(TAny* aPtr)
	{
	__ASSERT_DEBUG(aPtr, User::Panic(_L("ProfileCacheItem:DoNewIapFailed"),
									 KErrArgument));
	PROFILE_DEBUG1("ProfileCacheItem::DoNewIapFailed")

	if ( !aPtr )
		{
		return KErrArgument;
		}

	CSIPProfileCacheItem* self = static_cast<CSIPProfileCacheItem*>(aPtr);
	self->iMigrationController->ResetFlags();
	self->iMigrationController->SetIapAcceptance(*self, EFalse);
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SwitchToUnregisteredState
// Enter unregistered state. Don't use ChangeStateL to avoid loop where profile
// would register again (if auto-registration on).
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::SwitchToUnregisteredState(TBool aResume)
	{
	CSIPProfileState& unregistered = iServerCore.UnregisteredState();
	iProfile->SetStatus(unregistered.Name());
	iCurrentState = &unregistered;
	
    if(iQueuedProfile)
        {
        if(aResume)
            {
            TRAPD(err, ResumeL());
            if(KErrNone == err)
                return ETrue;
            }
        delete iProfile;
        iProfile = iQueuedProfile;
        iQueuedProfile = NULL;            
        }
	// No need to monitor SNAP anymore.
    SetSnapRetryCounter(0);
	StopSnapMonitoring();
	RemoveAllUsers();
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SetSnapRetryCounter
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::SetSnapRetryCounter(TInt aCounter)
	{
	PROFILE_DEBUG3("CSIPProfileCacheItem::SetSnapRetryCounter counter",
				   aCounter)
	iSnapRetryCounter = aCounter;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SnapRetryCounter
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCacheItem::SnapRetryCounter() const
	{
	PROFILE_DEBUG3("CSIPProfileCacheItem::SnapRetryCounter counter",
				   iSnapRetryCounter)
	return iSnapRetryCounter;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::SnapRetryCountReached
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::SnapRetryCountReached() const
	{
	TBool reached = iSnapRetryCounter >= KSnapRetryCountThreshold;
	PROFILE_DEBUG3("CSIPProfileCacheItem::SnapRetryCountReached reached",
				   reached)
	return reached;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::OfferedIapRejected
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::OfferedIapRejected()
	{
	PROFILE_DEBUG3("CSIPProfileCacheItem::OfferedIapRejected, status",
				   iProfile->Status())

	HandleError( KErrCancel );

	// State is informed as ERegistrationInProgress to get registration
	// error event event in client.
	iServerCore.SendErrorEvent(
		*this, CSIPConcreteProfile::ERegistrationInProgress, KErrCancel );
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::DefaultSNAPL
// This function will return the ID of the default SNAP.
// -----------------------------------------------------------------------------
//

void CSIPProfileCacheItem::DefaultSNAPL(TUint32& aSnapId) const

	{
		RCmManager cmManager;
		cmManager.OpenL();
		CleanupClosePushL(cmManager);
		TCmDefConnValue defConn;
		cmManager.ReadDefConnL( defConn );
		aSnapId = defConn.iId;	
		PROFILE_DEBUG3("CSIPProfileCacheItem::DefaultSNAPL with Value: ", aSnapId)
		CleanupStack::PopAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::OfflineInitiated
// -----------------------------------------------------------------------------
//
void CSIPProfileCacheItem::OfflineInitiated(TBool aOfflineInitiated)
    {
    if(aOfflineInitiated)
        {
        iIsOfflineInitiated = ETrue;
        iCurrentState->ShutdownInitiated(*this);
        }
    else
        iIsOfflineInitiated = EFalse;    
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::IsOfflineInitiated
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::IsOfflineInitiated() const
    {
    return iIsOfflineInitiated;
    }

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::BearerID
// This function will return the Bearer ID of the Profile.
// -----------------------------------------------------------------------------
//
TUint32 CSIPProfileCacheItem::BearerID()
{   TUint32 bearerId(0);
    LatestProfile().ExtensionParameter(KBearerType , bearerId);
    return bearerId;
}

// -----------------------------------------------------------------------------
// CSIPProfileCacheItem::HasQueuedUpdate
// This function will return true if profile updated and not yet processed
// false otherwise 
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCacheItem::HasQueuedUpdate() const
    {
    return (iQueuedProfile != NULL);
    }
