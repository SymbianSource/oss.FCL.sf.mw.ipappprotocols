/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include "teststubs.h"
#include "sipprofilestub.h"
#include <sipmanagedprofile.h>
#include <SIPProfileRegistryObserver.h>
#include <sipprofileregistry.h>
#include <sipprofileregistrybase.h>
#include <SIPManagedProfileRegistry.h>
#include <sipconcreteprofile.h>
#include <sipprofiletypeinfo.h>
#include <sipstrings.h>

// -----------------------------------------------------------------------------
// CSIPProfileRegistryObserver
// -----------------------------------------------------------------------------
//	
void CSIPProfileRegistryObserver::ProfileRegistryEventOccurred
		(TUint32 /*aProfileId*/, TEvent /*aEvent*/)
	{
		
	}

void CSIPProfileRegistryObserver::ProfileRegistryErrorOccurred
		(TUint32 /*aProfileId*/, TInt /*aError*/)
	{
		
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile
// -----------------------------------------------------------------------------
//	

CSIPManagedProfile* CSIPManagedProfile::NewL
		(CSIPManagedProfileRegistry* aSIPRegistry)
	{
    CSIPManagedProfile* self = CSIPManagedProfile::NewLC (aSIPRegistry);
    CleanupStack::Pop(self);
    return self;
	}

CSIPManagedProfile* CSIPManagedProfile::NewLC
		(CSIPManagedProfileRegistry* aSIPRegistry)
	{
	CSIPManagedProfile* self = new(ELeave)CSIPManagedProfile(aSIPRegistry);
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}

CSIPManagedProfile::~CSIPManagedProfile()
	{
	}
	
CSIPManagedProfile::CSIPManagedProfile(CSIPManagedProfileRegistry* aSIPRegistry):
	CSIPProfile(aSIPRegistry)
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
void CSIPManagedProfile::SetParameterL(TUint32 aParam, const TDesC8& aVal)
	{
	switch (aParam)
		{
		case KSIPProviderName:
			iProviderNameValue=aVal;
			break;
		case KSIPUserAor:
			iUserAorValue=aVal;
			break;
		case KSIPPrivateIdentity:
		case KSIPProfileId:
		case KSIPAccessPointId:
		case KSIPSigComp:
		case KSIPSecurityNegotiation:
		case KSIPAutoRegistration:
		case KSIPProfileRegistered:
		case KSIPRegisteredAors:
		case KSIPNegotiatedSecurityMechanism:
		case KSIPDefaultProfile:
		case KSIPContactHeaderParams:
		case KSIPRegistrar:
		case KSIPOutboundProxy:
		case KSIPDigestUserName:
		case KSIPDigestRealm:
		case KSIPServerAddress:
		case KSIPDigestPassword:
		case KSIPHeaders:
		case KSIPSoIpTOS:
			User::Leave(KErrNotFound);
		case KSIPContactHeaderUser:
		    User::Leave(KErrAccessDenied);
		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
void CSIPManagedProfile::SetParameterL(TUint32 aParam, TUint32 aVal)
	{
	switch (aParam)
		{
		case KSIPAccessPointId:
			iAccessPointIdValue=aVal;
			break;		
		case KSIPProfileId:
		case KSIPProviderName:		
		case KSIPSigComp:
		case KSIPSecurityNegotiation:
		case KSIPAutoRegistration:
		case KSIPUserAor:
		case KSIPProfileRegistered:
		case KSIPRegisteredAors:
		case KSIPNegotiatedSecurityMechanism:
		case KSIPDefaultProfile:
		case KSIPContactHeaderParams:
		case KSIPRegistrar:
		case KSIPOutboundProxy:
		case KSIPDigestUserName:
		case KSIPDigestRealm:
		case KSIPServerAddress:
		case KSIPPrivateIdentity:
		case KSIPDigestPassword:
		case KSIPHeaders:
		case KSIPContactHeaderUser:
			User::Leave(KErrNotFound);
		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
void CSIPManagedProfile::SetParameterL(TUint32 aParam, TBool aVal)
	{
	switch (aParam)
		{
		case KSIPSigComp:
			iSigCompValue=aVal;
			break;
		case KSIPAutoRegistration:
			iAutoRegistrationValue=aVal;
			break;
		case KSIPSecurityNegotiation:
			iSecurityNegotiationValue=aVal;
			break;		
		case KSIPDefaultProfile:
		case KSIPProfileId:
		case KSIPProviderName:
		case KSIPAccessPointId:		
		case KSIPUserAor:
		case KSIPProfileRegistered:
		case KSIPRegisteredAors:
		case KSIPNegotiatedSecurityMechanism:		
		case KSIPContactHeaderParams:
		case KSIPRegistrar:
		case KSIPOutboundProxy:
		case KSIPDigestUserName:
		case KSIPDigestRealm:
		case KSIPServerAddress:
		case KSIPPrivateIdentity:
		case KSIPDigestPassword:
		case KSIPHeaders:
		case KSIPContactHeaderUser:
		case KSIPSoIpTOS:
			User::Leave(KErrNotFound);			
		default:
			break;
		}
	}		


// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
TInt CSIPManagedProfile::SetParameter(TUint32 aServerType, TUint32 aParam, 
										const TDesC8& aVal)
	{
	switch (aParam)
		{
		case KSIPDigestPassword:
			if (aServerType == KSIPOutboundProxy)
				{
				iProxyPassword = aVal;					
				}
			else  // KSIPRegistrar
				{
				iRegistrarPassword = aVal;
				}
			break;

		default:
			break;
		}		
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfileRegistry
// -----------------------------------------------------------------------------
//	
CSIPManagedProfileRegistry* CSIPManagedProfileRegistry::NewL(
											MSIPProfileRegistryObserver& aObserver)
	{
    CSIPManagedProfileRegistry* self = CSIPManagedProfileRegistry::NewLC (aObserver);
    CleanupStack::Pop(self);
    return self;
	}


CSIPManagedProfileRegistry* CSIPManagedProfileRegistry::NewLC(
											MSIPProfileRegistryObserver& aObserver)
	{
	CSIPManagedProfileRegistry* self = new(ELeave)CSIPManagedProfileRegistry(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}


CSIPManagedProfileRegistry::~CSIPManagedProfileRegistry()
	{
	}

void CSIPManagedProfileRegistry::ConstructL()
	{
	}

CSIPManagedProfileRegistry::CSIPManagedProfileRegistry(
	MSIPProfileRegistryObserver& aObserver):
	CSIPProfileRegistryBase(aObserver)
	{
	}

CSIPProfile* CSIPManagedProfileRegistry::NewInstanceL()
	{
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase
// -----------------------------------------------------------------------------
//	
CSIPProfileRegistryBase::CSIPProfileRegistryBase(
	MSIPProfileRegistryObserver& aObserver):
	iObserver(aObserver)
	{

	}
	
CSIPProfileRegistryBase::~CSIPProfileRegistryBase()
	{

	}	
	
void CSIPProfileRegistryBase::ErrorOccurredL(TUint32 /*aProfileId*/,
										   TInt /*aStatus*/,
										   TInt /*aError*/)
	{
	}

void CSIPProfileRegistryBase::UpdatedL(TUint32 /*aProfileId*/, TUint /*aSize*/)
	{
	}

void CSIPProfileRegistryBase::AddedL(TUint32 /*aProfileId*/) 
	{
	}

void CSIPProfileRegistryBase::RemovedL(TUint32 /*aProfileId*/)
	{
		
	}
	
void CSIPProfileRegistryBase::RegistrationStatusChangedL(TUint32 /*aProfileId*/,
							  TInt /*aStatus*/,
							  TUint32 /*aStatusId*/)
	{
		
	}

// -----------------------------------------------------------------------------
// CSIPStrings
// -----------------------------------------------------------------------------
//	
	
void SIPStrings::OpenL()
	{
		
	}

void SIPStrings::Close()
	{
		
	}