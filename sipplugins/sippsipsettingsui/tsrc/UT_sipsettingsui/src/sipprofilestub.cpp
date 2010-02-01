/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "sipprofile.h"
//#include "teststubs.h"
#include <sipprofileregistrybase.h>
#include <sipprofileregistry.h>
#include <sipmanagedprofile.h>
#include <sipstrings.h>

// -----------------------------------------------------------------------------
// CSIPProfile
// -----------------------------------------------------------------------------
//	
CSIPProfile* CSIPProfile::NewL(CSIPProfileRegistry* aSIPRegistry)
	{
    CSIPProfile* self = CSIPProfile::NewLC (aSIPRegistry);
    CleanupStack::Pop(self);
    return self;
	}

CSIPProfile* CSIPProfile::NewLC(CSIPProfileRegistry* aSIPRegistry)
	{
	CSIPProfile* self = new(ELeave)CSIPProfile(aSIPRegistry);
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}

CSIPProfile::CSIPProfile(CSIPProfileRegistryBase* /*aRegistry*/)
	 : 	iEnabled(EFalse),
	   	iProviderNameValue(KProviderNameValue),
		iUserAorValue(KUserAorValue),
		iAccessPointIdValue(KAccessPointIdValue),
		iSnapIdValue(KSnapIdValue),
		iSigCompValue(KSigCompValue),
		iAutoRegistrationValue(KAutoRegistrationValue),
		iSecurityNegotiationValue(KSecurityNegotiationValue),
		iAllowAutoRegistrationOnlyOnHomeNetworkValue
					(KAllowAutoRegistrationOnlyOnHomeNetworkValue)

	{
	}

void CSIPProfile::ConstructL()
	{
	TSIPProfileTypeInfo* typeInfo = new (ELeave)TSIPProfileTypeInfo;
	typeInfo->iSIPProfileName = KProviderNameValue;
	typeInfo->iSIPProfileClass = TSIPProfileTypeInfo::EInternet;

	iSIPProfile = reinterpret_cast<CSIPConcreteProfile*>(typeInfo);
	}

	
CSIPProfile::~CSIPProfile()
	{
	TSIPProfileTypeInfo* typeInfo = 
		reinterpret_cast<TSIPProfileTypeInfo*>(iSIPProfile); 
	delete typeInfo;	
	}

const TSIPProfileTypeInfo& CSIPProfile::Type() const
	{
	return *reinterpret_cast<TSIPProfileTypeInfo*>(iSIPProfile);
	}	

TInt CSIPProfile::GetParameter(TUint32 aParam, TDesC8 const *& aVal) const
	{
	switch (aParam)
		{
		case KSIPProviderName:
			{
			aVal = &iProviderNameValue;
			break;
			}
		case KSIPUserAor:
			{
			aVal = &iUserAorValue; 
			break;
			}
		default:
			{
			break;
			}
		}
	return KErrNone;		
	}

TInt CSIPProfile::GetParameter(TUint32 aParam, TUint32& aVal) const
	{
	switch (aParam)
		{
		case KSIPProfileId:
			aVal = iAccessPointIdValue; 
			break;
		case KSIPSnapId:
			if ( !iEnabled ) // If SNAP is disabled, return Not Found
				{
				return KErrNotFound;
				}
			aVal = iSnapIdValue; 
			break;
		case KSIPAccessPointId:
			aVal = iAccessPointIdValue;
			break;		
		default:
			break;
		}
	return KErrNone;		
	}

TInt CSIPProfile::GetParameter(TUint32 aParam, TBool& aVal) const
	{
	switch (aParam)
		{
		case KSIPSigComp:
			aVal = iSigCompValue;
			break;
		case KSIPAutoRegistration:
			aVal = iAutoRegistrationValue;
			break;
		case KSIPSecurityNegotiation:
			aVal = iSecurityNegotiationValue;
			break;
		case KSIPProfileRegistered:
			aVal = iAutoRegistrationValue;
			break;
		case KSIPAllowAutoRegistrationOnlyOnHomeNetwork:
			aVal = iAllowAutoRegistrationOnlyOnHomeNetworkValue;
			break;
		default:			
			break;
		}
	return KErrNone;
		
	}
	
TInt CSIPProfile::GetParameter(TUint32 aServerType, TUint32 aParam, TDesC8 const *& aVal ) const
	{
		switch ( aParam )
		{
			case KSIPDigestPassword:
				if (aServerType == KSIPOutboundProxy)
					{
					aVal = &iProxyPassword;					
					}
				else  // KSIPRegistrar
					{
					aVal = &iRegistrarPassword;
					}
				break;	
		}
	return KErrNone;
	}
	
TUint32 CSIPProfile::ContextId() const
	{
	return 32;
	}
	
TBool CSIPProfile::IsContextActive() const
	{
	return EFalse;
	}

void CSIPProfile::SetEnabled(TBool aEnabled)
	{
	iEnabled = aEnabled;
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
