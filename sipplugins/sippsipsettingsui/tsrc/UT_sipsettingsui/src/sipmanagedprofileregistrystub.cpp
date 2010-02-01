/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  implementation
*
*/


//  INCLUDE FILES

#include "sip.h"
#include "sipconnection.h"
#include "sipprofileregistry.h"
#include "sipmanagedprofileregistry.h"
#include "sipprofileregistryobserver.h"
#include "sipprofile.h"
#include "sipmanagedprofile.h"
//#include "sipconcreteprofile.h"

// ============================ MEMBER FUNCTIONS ===============================
EXPORT_C void CSIPManagedProfileRegistry::SaveL(CSIPProfile& /*aSIPProfile*/)
    {
    
    }

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfileRegistry* CSIPManagedProfileRegistry::NewL(
	MSIPProfileRegistryObserver& aObserver)
	{
    CSIPManagedProfileRegistry* self = CSIPManagedProfileRegistry::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfileRegistry* CSIPManagedProfileRegistry::NewLC(
	MSIPProfileRegistryObserver& aObserver)
	{
	CSIPManagedProfileRegistry* self = new (ELeave) CSIPManagedProfileRegistry(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::~CSIPManagedProfileRegistry
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfileRegistry::~CSIPManagedProfileRegistry()
	{
	
	}
/*
// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::EnableL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPManagedProfileRegistry::EnableL(CSIPProfile& aSIPProfile,
						MSIPConnectionObserver& aObserver)
	{
	//__ASSERT_ALWAYS (!aSIPProfile.IsEnabled(), User::Leave (KErrArgument));
	//aSIPProfile.ConcreteProfile().SetLastRegistrationError(KErrNone);
	//iConnectionObserver = &aObserver;

	//EnableProfileL(aSIPProfile);
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::Disable
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPManagedProfileRegistry::Disable(CSIPProfile& /*aProfile)
	{
	TInt err = KErrNone;
	//aProfile.ConcreteProfile().SetLastRegistrationError(KErrNone);
	//TRAPD(err, DisableProfileL(aProfile));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::SIP
// -----------------------------------------------------------------------------
//
EXPORT_C CSIP& CSIPManagedProfileRegistry::SIP() const
	{
	return iSip;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::ConnectionL
// -----------------------------------------------------------------------------
//
//EXPORT_C CSIPConnection* CSIPManagedProfileRegistry::ConnectionL(
//						/*CSIPProfile& aSIPProfile)
//	{
	//TUint32 apId(0);
	//__ASSERT_ALWAYS (iConnectionObserver != 0, User::Leave (KErrArgument));
	//aSIPProfile.GetParameter(KSIPAccessPointId, apId);
	//CSIPConnection* connection = iSip.Connection(apId);
	//if (connection == 0)
	//	{
	//	connection = CSIPConnection::NewL(iSip, apId, 
	//		*iConnectionObserver);
	//	}
	//return connection;
//	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::IsEnabled
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPManagedProfileRegistry::IsEnabled(const CSIPProfile& /*aSIPProfile) const
	{
	//return aSIPProfile.IsEnabled();
	return ETrue;
	}
*/
// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::CSIPManagedProfileRegistry
// -----------------------------------------------------------------------------
//
CSIPManagedProfileRegistry::CSIPManagedProfileRegistry(
	MSIPProfileRegistryObserver& aObserver)
	: CSIPProfileRegistryBase( aObserver )
	{
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPManagedProfileRegistry::ConstructL()
	{
//	BaseConstructL();

	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry::NewInstanceL
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPManagedProfileRegistry::NewInstanceL()
	{
    return CSIPManagedProfile::NewL(this);
	}


TBool CSIPManagedProfileRegistry::IsInUseL(const CSIPProfile& /*aSIPProfile*/) const
    {
    return EFalse;
    }

CSIPManagedProfile* CSIPManagedProfileRegistry::CreateL(const TSIPProfileTypeInfo& /*aType*/)
	{
	return CSIPManagedProfile::NewL(this); 
	}

void CSIPManagedProfileRegistry::DestroyL(CSIPProfile& /*aSIPProfile*/)
	{
	
	}
