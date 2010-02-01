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
#include "sipprofileregistryobserver.h"
#include "sipprofile.h"
//#include "sipconcreteprofile.h"

// STUB stuff

void CSIPProfileBehaviorTls::OpenL()
	{
    CSIPProfileBehaviorTls* self = new (ELeave) CSIPProfileBehaviorTls();
    Dll::SetTls( self );
	}

void CSIPProfileBehaviorTls::Close ()
	{
	CSIPProfileBehaviorTls* self = Storage();
    delete self;
    Dll::SetTls( NULL );
	}

	
CSIPProfileBehaviorTls* CSIPProfileBehaviorTls::Storage()
	{
	return static_cast<CSIPProfileBehaviorTls*>(Dll::Tls());
	}

void CSIPProfileBehaviorTls::Reset()
    {
    iRegistryBehavior = 0;
    iProfileBehavior = 0;
    iConnectionBehavior = 0;
    }
    
CSIPProfileBehaviorTls::CSIPProfileBehaviorTls()
	{
	}
	
CSIPProfileBehaviorTls::~CSIPProfileBehaviorTls()
	{
	}
	

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileRegistry* CSIPProfileRegistry::NewL(
	CSIP& aSip,
	MSIPProfileRegistryObserver& aObserver)
	{
    CSIPProfileRegistry* self = CSIPProfileRegistry::NewLC(aSip, aObserver);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileRegistry* CSIPProfileRegistry::NewLC(
	CSIP& aSip,
	MSIPProfileRegistryObserver& aObserver)
	{
	CSIPProfileRegistry* self = new (ELeave) CSIPProfileRegistry(aSip, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::~CSIPProfileRegistry
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileRegistry::~CSIPProfileRegistry()
	{
	
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::EnableL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileRegistry::EnableL(CSIPProfile& /*aSIPProfile*/,
						MSIPConnectionObserver& /*&aObserver*/)
	{
	//__ASSERT_ALWAYS (!aSIPProfile.IsEnabled(), User::Leave (KErrArgument));
	//aSIPProfile.ConcreteProfile().SetLastRegistrationError(KErrNone);
	//iConnectionObserver = &aObserver;

	//EnableProfileL(aSIPProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::Disable
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPProfileRegistry::Disable(CSIPProfile& /*aProfile*/)
	{
	TInt err = KErrNone;
	iProfileDisabled = ETrue;
	//aProfile.ConcreteProfile().SetLastRegistrationError(KErrNone);
	//TRAPD(err, DisableProfileL(aProfile));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::SIP
// -----------------------------------------------------------------------------
//
EXPORT_C CSIP& CSIPProfileRegistry::SIP() const
	{
	return iSip;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::ConnectionL
// -----------------------------------------------------------------------------
//
//EXPORT_C CSIPConnection* CSIPProfileRegistry::ConnectionL(
//						/*CSIPProfile& aSIPProfile*/)
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
// CSIPProfileRegistry::IsEnabled
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileRegistry::IsEnabled(const CSIPProfile& /*aSIPProfile*/) const
	{
	//iProfileEnabled = ETrue;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::CSIPProfileRegistry
// -----------------------------------------------------------------------------
//
CSIPProfileRegistry::CSIPProfileRegistry(
	CSIP& aSip,
	MSIPProfileRegistryObserver& aObserver):
	CSIPProfileRegistryBase(aObserver), iSip(aSip)
	{
	
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistry::ConstructL()
	{
	CSIPProfileBehaviorTls* tls = CSIPProfileBehaviorTls::Storage();
	if ( tls )
	    {
	    if ( tls->iRegistryBehavior == KRegistryLeaveAtConstruct )
	        {
	        User::Leave( KErrGeneral );
	        }
	    }
	BaseConstructL();

	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistry::NewInstanceL
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPProfileRegistry::NewInstanceL()
	{
    return CSIPProfile::NewL(this);
	}

//////////////////////////
// -----------------------------------------------------------------------------
// CSIPProfileRegistryObserver
// -----------------------------------------------------------------------------

void CSIPProfileRegistryObserver::ConstructL()
    {
    }

CSIPProfileRegistryObserver* CSIPProfileRegistryObserver::NewLC()
    {
    CSIPProfileRegistryObserver* self = new ( ELeave ) 
        CSIPProfileRegistryObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CSIPProfileRegistryObserver* CSIPProfileRegistryObserver::NewL()
    {
    CSIPProfileRegistryObserver* self = new ( ELeave ) 
        CSIPProfileRegistryObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CSIPProfileRegistryObserver::CSIPProfileRegistryObserver()
    {
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CSIPProfileRegistryObserver::~CSIPProfileRegistryObserver()
   {
   }



void CSIPProfileRegistryObserver::ProfileRegistryEventOccurred
		(TUint32 /*aProfileId*/, TEvent /*aEvent*/)
	{
		
	}

void CSIPProfileRegistryObserver::ProfileRegistryErrorOccurred
		(TUint32 /*aProfileId*/, TInt /*aError*/)
	{
		
	}