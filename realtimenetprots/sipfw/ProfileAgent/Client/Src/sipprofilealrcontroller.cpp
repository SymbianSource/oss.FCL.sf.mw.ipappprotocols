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
// Name        : sipprofilealrcontroller
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
//



//  INCLUDE FILES
#include "sipprofilealrcontroller.h"
#include "sipprofilealrobserver.h"
#include "sipprofileregistrybase.h"
#include "SIPProfileITC.h"
#include "SIPProfileITCReceiver.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSipProfileAlrController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSipProfileAlrController * CSipProfileAlrController::NewL( 
	CSIPProfileRegistryBase& aRegistry, MSipProfileAlrObserver& aObserver )
	{
    CSipProfileAlrController* self = 
		CSipProfileAlrController::NewLC( aRegistry, aObserver );
    CleanupStack::Pop ( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CSipProfileAlrController::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSipProfileAlrController* CSipProfileAlrController::NewLC( 
	CSIPProfileRegistryBase& aRegistry, MSipProfileAlrObserver& aObserver )
	{
	CSipProfileAlrController* self = 
		new ( ELeave )	CSipProfileAlrController( aRegistry );
    CleanupStack::PushL ( self );
    self->ConstructL ( aObserver );
    return self;
	}

// -----------------------------------------------------------------------------
// CSipProfileAlrController::CSipProfileAlrController
// -----------------------------------------------------------------------------
//
CSipProfileAlrController::CSipProfileAlrController( 
	CSIPProfileRegistryBase& aRegistry )
	: iRegistry( aRegistry )
	{  
	}
		
// -----------------------------------------------------------------------------
// CSipProfileAlrController::~CSipProfileAlrController
// -----------------------------------------------------------------------------
//
CSipProfileAlrController::~CSipProfileAlrController()
	{
	iRegistry.ItcReceiver().SetALRObserver( NULL );
	iRegistry.Itc().RemoveALRObserver();
	}


// -----------------------------------------------------------------------------
// CSipProfileAlrController::ConstructL
// -----------------------------------------------------------------------------
//
void CSipProfileAlrController::ConstructL( MSipProfileAlrObserver& aObserver )
	{
	User::LeaveIfError(iRegistry.ItcReceiver().SetALRObserver( &aObserver ));
	iRegistry.Itc().AddALRObserverL();
	}
	
// -----------------------------------------------------------------------------
// CSipProfileAlrController::AllowMigrationL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSipProfileAlrController::AllowMigrationL( TUint32 aProfileId,
		                               		TUint32 aIapId )
	{
	iRegistry.Itc().AllowMigrationL( aProfileId, aIapId );
	}
	
// -----------------------------------------------------------------------------
// CSipProfileAlrController::DisallowMigration
// -----------------------------------------------------------------------------
//
EXPORT_C void CSipProfileAlrController::DisallowMigrationL( TUint32 aProfileId,
		                                	TUint32 aIapId )
	{
	iRegistry.Itc().DisallowMigrationL( aProfileId, aIapId );
	}

// -----------------------------------------------------------------------------
// CSipProfileAlrController::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSipProfileAlrController::RefreshIapAvailabilityL( 
											TUint32 aProfileId )
	{
	iRegistry.Itc().RefreshIapAvailabilityL( aProfileId );	
	}
