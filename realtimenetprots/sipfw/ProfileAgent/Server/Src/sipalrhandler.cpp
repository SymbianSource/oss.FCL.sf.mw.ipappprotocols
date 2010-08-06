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
// Name          : sipalrhandler.cpp
// Part of       : SIP / SIP Profile Server
// Version       : SIP/6.0 
//



#include "sipalrhandler.h"
#include "sipalrmonitor.h"
#include "SipProfileCacheItem.h"
#include "sipprofileerrorhandler.h"


// -----------------------------------------------------------------------------
// CSipAlrHandler::NewL
// -----------------------------------------------------------------------------
//
CSipAlrHandler* CSipAlrHandler::NewL(
    MSipProfileErrorHandler& aErrorHandler,
    CSipSystemStateMonitor* aSystemStateMonitor)
	{
    return new(ELeave)CSipAlrHandler(aErrorHandler,aSystemStateMonitor);
	}

// -----------------------------------------------------------------------------
// CSipAlrHandler::CSipAlrHandler
// -----------------------------------------------------------------------------
//
CSipAlrHandler::CSipAlrHandler(
    MSipProfileErrorHandler& aErrorHandler,
    CSipSystemStateMonitor* aSystemStateMonitor) :
    iErrorHandler(aErrorHandler),
    iSystemStateMonitor(aSystemStateMonitor)
	{
	}

// -----------------------------------------------------------------------------
// CSipAlrHandler::~CSipAlrHandler
// -----------------------------------------------------------------------------
//
CSipAlrHandler::~CSipAlrHandler()
	{
	delete iAlrMonitor;
	}

// -----------------------------------------------------------------------------
// CSipAlrHandler::EnableProfileL
// If SNAP is not configured, work in the old way.
// -----------------------------------------------------------------------------
//
void CSipAlrHandler::EnableProfileL(CSIPProfileCacheItem& aProfile,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    TInt err(KErrNone);
    TUint32 snapId(0);
	if (aProfile.IsSNAPConfigured(snapId))
		{
		TRAP(err, aProfile.EnableSnapInUseL(aObserver, snapId));
		}
	else
		{
		TRAP(err, aProfile.EnableL(aObserver));
		}

	if (err != KErrNone)
		{
    	HandleErrorL(aProfile, err, err);
	    }
    }

// -----------------------------------------------------------------------------
// CSipAlrHandler::DisableProfileL
// -----------------------------------------------------------------------------
//
void CSipAlrHandler::DisableProfileL(CSIPProfileCacheItem& aProfile,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
	TRAPD(err, aProfile.DisableL(aObserver));
    if (err != KErrNone)
        {
        HandleErrorL(aProfile, err, KErrNone);
        }
    }

// -----------------------------------------------------------------------------
// CSipAlrHandler::AllowMigrationL
// -----------------------------------------------------------------------------
//
void CSipAlrHandler::AllowMigrationL(CSIPProfileCacheItem& aProfile,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    TRAPD(err, aProfile.ClientAllowsMigrationL(aIapId, aObserver));
    if (err != KErrNone)
	    {
	    HandleErrorL(aProfile, err, err);
	    }
    }

// -----------------------------------------------------------------------------
// CSipAlrHandler::DisallowMigrationL
// -----------------------------------------------------------------------------
//
void CSipAlrHandler::DisallowMigrationL(CSIPProfileCacheItem& aProfile,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    TRAPD(err, aProfile.ClientDisallowsMigrationL(aIapId, aObserver));
    if (err != KErrNone)
	    {
	    HandleErrorL(aProfile, err, err);
	    }
    }

// -----------------------------------------------------------------------------
// CSipAlrHandler::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//
void CSipAlrHandler::RefreshIapAvailabilityL(CSIPProfileCacheItem& aProfile)
    {
    TUint32 snapId(0);
    __ASSERT_ALWAYS(aProfile.IsSNAPConfigured(snapId),
    				User::Leave(KErrArgument));
    TSipSNAPConfigurationData aSnapData(snapId ,aProfile.BearerID());
    AlrMonitorL().RefreshIapAvailabilityL(aSnapData);
    }

// -----------------------------------------------------------------------------
// CSipAlrHandler::AlrMonitorL
// Create the monitor on demand. It must not be loaded on start-up if not needed
// -----------------------------------------------------------------------------
//
CSipAlrMonitor& CSipAlrHandler::AlrMonitorL()
    {
    if (!iAlrMonitor)
        {
        __ASSERT_ALWAYS(iSystemStateMonitor, User::Leave(KErrNotSupported));
		iAlrMonitor = CSipAlrMonitor::NewL(*iSystemStateMonitor);
        }
    return *iAlrMonitor;
    }

// -----------------------------------------------------------------------------
// CSipAlrHandler::HandleErrorL
// -----------------------------------------------------------------------------
//
void CSipAlrHandler::HandleErrorL(CSIPProfileCacheItem& aItem,
    							  TInt aErr,
    							  TInt aOwnError)
    {
    if (aItem.ReferenceCount() == 0 && 
        !aItem.Profile().IsAutoRegistrationEnabled())
        {
        iErrorHandler.HandleProfileError(aItem, aOwnError);
        }
    User::Leave(aErr);
    }
