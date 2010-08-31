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
// Name          : sipalrmigrationcontroller.cpp
// Part of       : SIP / SIP Profile Server
// Version       : SIP/6.0 
//



#include "sipalrmigrationcontroller.h"
#include "sipalrmonitor.h"
#include "sipalrmigrationobserver.h"
#include "sipalrmigrationobserverinfo.h"
#include "SipProfileLog.h"


// -----------------------------------------------------------------------------
// CSipAlrMigrationController::NewLC
// -----------------------------------------------------------------------------
//
CSipAlrMigrationController*
CSipAlrMigrationController::NewLC(CSipAlrMonitor& aAlrMonitor, TSipSNAPConfigurationData aSnapData)
	{
	CSipAlrMigrationController* self =
		new (ELeave) CSipAlrMigrationController(aAlrMonitor, aSnapData);
    CleanupStack::PushL(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::CSipAlrMigrationController
// -----------------------------------------------------------------------------
//
CSipAlrMigrationController::CSipAlrMigrationController(
	CSipAlrMonitor& aAlrMonitor,
	TSipSNAPConfigurationData aSnapData) :
	iAlrMonitor(aAlrMonitor),
	iSnapData(aSnapData)
#ifdef CPPUNIT_TEST
    // Set the array granularity to 1, so it allocates memory in each append
    , iObservers(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::~CSipAlrMigrationController
// -----------------------------------------------------------------------------
//
CSipAlrMigrationController::~CSipAlrMigrationController()
	{
	PROFILE_DEBUG1("~CSipAlrMigrCtrl")

	iAlrMonitor.FreeResources(*this);
	iObservers.Close();
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::AlrEvent
// If migration is disallowed, the original IAP id is offered again.
// -----------------------------------------------------------------------------
//
void CSipAlrMigrationController::AlrEvent(MSipAlrObserver::TEvent aEvent,
										  TUint32 aSnapId,
										  TUint32 aIapId)
	{
	PROFILE_DEBUG3("CSipAlrMigrationController::AlrEvent", (TInt)(aEvent))

	switch (aEvent)
		{
		case MSipAlrObserver::EIapAvailable:
			if (aIapId == iAllowedIapId)
				{
				// Pass the failed IAP to observer, not the original IAP.
				SendMigrationResult(iOfferedIapId, EFalse);
				}
			else
				{
				IapAvailable(aSnapId, aIapId);
				}
			break;

		case MSipAlrObserver::EIapActive:
			iAllowedIapId = aIapId;
			SendMigrationResult(aIapId, ETrue);
		    break;

		case MSipAlrObserver::ENoNewIapAvailable:
			NoNewIapAvailable();
		    break;

		case MSipAlrObserver::ERefreshError:
			// Start from the end, as RefreshIAPsFailed() causes entries to
			// remove themselves from the array.
			for (TInt i = iObservers.Count() - 1; i >= 0; --i)
				{
				iObservers[i].iObserver.RefreshIAPsFailed();
				}
			break;

		case MSipAlrObserver::EOfferedIapRejected:
			OfferedIapRejected();
		    break;


        default:
        	User::Panic(_L("ALRMigrationCtrl::AlrEvent"), KErrArgument);
        }
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::SnapId
// -----------------------------------------------------------------------------
//
TUint32 CSipAlrMigrationController::SnapId() const
	{
	return iSnapData.iSnapId;
	}

TUint32 CSipAlrMigrationController::BearerId() const
    {
    return iSnapData.iBearerId;
    }

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::IsUsed
// -----------------------------------------------------------------------------
//
TBool CSipAlrMigrationController::IsUsed() const
	{
	return iObservers.Count() > 0;
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::AttachProfileL
// Initiate monitoring only after first observer exists. Otherwise IAP
// available event could not be forwarded to observer.
// -----------------------------------------------------------------------------
//
TUint32
CSipAlrMigrationController::AttachProfileL(MSipAlrMigrationObserver& aObserver)
    {
	TSipAlrMigrationObserverInfo observerInfo(aObserver);
	iObservers.AppendL(observerInfo);
    
    const TInt KFirstObserverExists = 1;
    if (iObservers.Count() == KFirstObserverExists)
    	{
        PROFILE_DEBUG3("CSipAlrMigrCtrl::AttachProfileL snapId", SnapId())
        PROFILE_DEBUG3("CSipAlrMigrCtrl::AttachProfileL BearerFiltered.. ", BearerId())
        iAlrMonitor.MonitorSnapL(iSnapData, *this);
    	return KNoIap;
    	}

    return iAllowedIapId;
    }

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::DetachProfile
// This is typically called from destructors, so don't leave.
// -----------------------------------------------------------------------------
//
void
CSipAlrMigrationController::DetachProfile(MSipAlrMigrationObserver& aObserver)
	{
	TInt index = FindIndex(aObserver);
	if (index != KErrNotFound)
		{
		iObservers.Remove(index);
		iObservers.Compress();
		}

	if (iObservers.Count() == 0)
		{
		PROFILE_DEBUG1("CSipAlrMigrCtrl::DetachProfile free resources")

		iAlrMonitor.FreeResources(*this);
		ResetFlags();
		}
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::IapAvailable
// Set all observers "waiting", before using IapAvailable. Even if the first
// observer syncronously uses SetMigrationPermission, ObserversWaitingPermission
// returns correct value.
// If an IAP comes available before each observer has (dis)allowed a previous
// IAP, pass the new IAP to observers and forget the older IAP.
// -----------------------------------------------------------------------------
//
void
CSipAlrMigrationController::IapAvailable(TUint32 aSnapId, TUint32 aIapId)
	{
	PROFILE_DEBUG4("CSipAlrMigrCtrl::IapAvailable snap,iap", aSnapId, aIapId)

	iOfferedIapId = aIapId;
	iMigrationDisallowed = EFalse;
	iRefreshIssued = EFalse;

	for (TInt i = 0; i < iObservers.Count(); ++i)
		{
		iObservers[i].iIsWaitingForPermission = ETrue;
		iObservers[i].iIsWaitingForAcceptance = ETrue;
		}

	for (TInt i = 0; i < iObservers.Count(); ++i)
		{
		iObservers[i].iObserver.IapAvailable(aSnapId, aIapId);
		}
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::SetMigrationPermission
// Handle only the latest IAP. If IAP==EMostRecentIap don't care if the observer
// has already used SetMigrationPermission. Otherwise do nothing if observer is
// not waiting, as it has already automatically called SetMigrationPermission.
// -----------------------------------------------------------------------------
//
void CSipAlrMigrationController::SetMigrationPermission(
	MSipAlrMigrationObserver& aObserver,
	TBool aAllow,
	TUint32 aIapId)
	{
	PROFILE_DEBUG5("CSipAlrMigrCtrl::SetMigrPermission iap,allow,offeredIap",
				   aIapId,
				   aAllow,
				   iOfferedIapId)

	TInt index = FindIndex(aObserver);
	// If no pending observers, result has been forwarded. Don't do it twice.
	if (ObserversWaitingPermission() &&
	    index != KErrNotFound &&
	    ((aIapId == EMostRecentIap) ||
		 (aIapId == iOfferedIapId &&
		  iObservers[index].iIsWaitingForPermission)))
		{
		iObservers[index].iIsWaitingForPermission = EFalse;
		if (!aAllow)
			{
			iMigrationDisallowed = ETrue;
			}

		if (!ObserversWaitingPermission())
			{
			PROFILE_DEBUG3("CSipAlrMigrCtrl::SetMigrPermission, allow=",
						   !iMigrationDisallowed)
			TInt err(KErrNone);
			if (iMigrationDisallowed)
				{
			    err = iAlrMonitor.DisallowMigration(iSnapData);
				}
			else
				{
			    err = iAlrMonitor.AllowMigration(iSnapData);
				}
			if (err != KErrNone)
				{
	        	for (TInt i = 0; i < iObservers.Count(); ++i)
					{
					iObservers[i].iObserver.ErrorOccurred(err);
					}
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//
void CSipAlrMigrationController::RefreshIapAvailabilityL(TUint32 aSnapId)
	{
	if (!iRefreshIssued && aSnapId == SnapId())
		{
	    
		iAlrMonitor.RefreshIapAvailabilityL(iSnapData);
		iRefreshIssued = ETrue;
		}
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::SetIapAcceptance
// Pass result to ALR monitor if any profile succeeds with new IAP or all failed
// -----------------------------------------------------------------------------
//
TInt CSipAlrMigrationController::SetIapAcceptance(
	MSipAlrMigrationObserver& aObserver,
	TBool aIapWorks)
	{
	PROFILE_DEBUG4("CSipAlrMigrCtrl::SetIapAcceptance, iap,works=",
				   iAllowedIapId,
				   aIapWorks)
    
	TInt index = FindIndex(aObserver);
	if (index != KErrNotFound && iObservers[index].iIsWaitingForAcceptance)
		{
		iObservers[index].iIsWaitingForAcceptance = EFalse;
		if (aIapWorks)
			{
			// IAP works, no need to wait other observers
			for (TInt i = 0; i < iObservers.Count(); ++i)
				{
				iObservers[i].iIsWaitingForAcceptance = EFalse;
				}
			PROFILE_DEBUG1("CSipAlrMigrCtrl::SetIapAcceptance accepted")
			return iAlrMonitor.NewIapAccepted(iSnapData);
			}

		if (!ObserversWaitingAcceptance())
			{
			// Nobody got IAP to work
			PROFILE_DEBUG1("CSipAlrMigrCtrl::SetIapAcceptance rejected")
			return iAlrMonitor.NewIapRejected(iSnapData);
			}
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::SendMigrationResult
// -----------------------------------------------------------------------------
//
void
CSipAlrMigrationController::SendMigrationResult(TUint32 aIapId, TBool aAllowed)
	{
	PROFILE_DEBUG5("CSipAlrMigrCtrl::SendMigrationResult iap,allow,obsCount",
				   aIapId,
				   aAllowed,
				   iObservers.Count())
	    
		// It may be possible that Profile Could not be registered (if migration is allowed). In such cases error handling
        // for profile will be done, which will move Profile into Un-registered State
        // As the profile moves into Un-registered state, profile is detached from
        // the list of Observer which AlrMigrationController maintains. This dynamically
        // changes the Observer count.
	    for (TInt i = iObservers.Count() -1; i >= 0; --i)
		{
		MSipAlrMigrationObserver& obs = iObservers[i].iObserver;
		TRAPD(err, if (aAllowed)
			{
			obs.MigrationIsAllowedL(aIapId);
			}
		else
			{
			obs.MigrationIsDisallowedL(aIapId);
			});

		if (err != KErrNone)
			{
			PROFILE_DEBUG4("MigrationIs(Dis)AllowedL leaves err,index", err, i)
			obs.ErrorOccurred(err);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::FindIndex
// -----------------------------------------------------------------------------
//
TInt
CSipAlrMigrationController::FindIndex(MSipAlrMigrationObserver& aObserver) const
	{
	TSipAlrMigrationObserverInfo observerInfo(aObserver);
	TIdentityRelation<TSipAlrMigrationObserverInfo>
		compare(TSipAlrMigrationObserverInfo::CompareObservers);
	return iObservers.Find(observerInfo, compare);
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::ObserversWaitingPermission
// -----------------------------------------------------------------------------
//
TBool CSipAlrMigrationController::ObserversWaitingPermission() const
	{
	for (TInt i = 0; i < iObservers.Count(); ++i)
		{
		if (iObservers[i].iIsWaitingForPermission)
			{
			return ETrue;
			}
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::ObserversWaitingAcceptance
// -----------------------------------------------------------------------------
//
TBool CSipAlrMigrationController::ObserversWaitingAcceptance() const
	{
	for (TInt i = 0; i < iObservers.Count(); ++i)
		{
		if (iObservers[i].iIsWaitingForAcceptance)
			{
			return ETrue;
			}
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::NoNewIapAvailable
// -----------------------------------------------------------------------------
//
void CSipAlrMigrationController::NoNewIapAvailable() const
	{
	for (TInt i = 0; i < iObservers.Count(); ++i)
		{
		iObservers[i].iObserver.NoNewIapAvailable();
		}		
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::ResetFlags
// -----------------------------------------------------------------------------
//
void CSipAlrMigrationController::ResetFlags()
	{
	iRefreshIssued = EFalse;
    iAllowedIapId = KNoIap;
    iOfferedIapId = KNoIap;
    iMigrationDisallowed = EFalse;
	}

// -----------------------------------------------------------------------------
// CSipAlrMigrationController::OfferedIapRejected
// -----------------------------------------------------------------------------
//
void CSipAlrMigrationController::OfferedIapRejected() const
	{
	// Start from the end, as RefreshIAPsFailed() causes entries to
	// remove themselves from the array.
	for (TInt i = iObservers.Count() - 1; i >= 0; --i)
		{
		iObservers[i].iObserver.OfferedIapRejected();
		}
	}
