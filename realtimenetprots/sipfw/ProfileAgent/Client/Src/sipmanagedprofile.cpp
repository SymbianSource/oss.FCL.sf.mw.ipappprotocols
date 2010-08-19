// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipmanagedprofile
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "sipmanagedprofile.h"
#include "sipconcreteprofile.h"
#include "sipprofileregistry.h"
#include "sipmanagedprofileregistry.h"

_LIT8(KSIPProfileExtenstionParameter, "%u");
const TUint KParameterbufLength = 25;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPManagedProfile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPManagedProfile* CSIPManagedProfile::NewL(CSIPManagedProfileRegistry* aSIPRegistry)
	{
    CSIPManagedProfile* self = CSIPManagedProfile::NewLC (aSIPRegistry);
    CleanupStack::Pop (self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPManagedProfile* CSIPManagedProfile::NewLC(CSIPManagedProfileRegistry* aSIPRegistry)
	{
	CSIPManagedProfile* self = new(ELeave)CSIPManagedProfile(aSIPRegistry);
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}
				
// -----------------------------------------------------------------------------
// CSIPManagedProfile::~CSIPManagedProfile
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPManagedProfile::~CSIPManagedProfile()
	{
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetType
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPManagedProfile::SetType(
	const TSIPProfileTypeInfo& aSIPProfileTypeInfo)
	{
	iSIPProfile->SetProfileType(aSIPProfileTypeInfo);
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::SetParameter(TUint32 aParam, const TDesC8& aVal)
	{
	TRAPD(err, SetParameterL(aParam,aVal));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::SetParameter(TUint32 aParam, TUint32 aVal)
	{
	TRAPD(err, SetParameterL(aParam,aVal));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::SetParameter(TUint32 aParam, TBool aVal)
	{
	TRAPD(err, SetParameterL(aParam,aVal));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::SetParameter(TUint32 aServerType,
	TUint32 aParam, const TDesC8& aVal)
	{
	TRAPD(err, SetParameterL(aServerType,aParam,aVal));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::SetParameter(TUint32 aParam,
	const MDesC8Array& aVal)
	{
	TRAPD(err, SetParameterL(aParam,aVal));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::CloneL
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPManagedProfile* CSIPManagedProfile::CloneL() const
	{
	__ASSERT_ALWAYS(iSIPProfileRegistry, User::Leave(KErrBadHandle));
	CSIPManagedProfile* newCopy = CSIPManagedProfile::NewLC(
		static_cast<CSIPManagedProfileRegistry*>(iSIPProfileRegistry));

	newCopy->SetConcreteProfile(iSIPProfile->CloneL());

	CleanupStack::Pop(newCopy);
	return newCopy;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::CSIPManagedProfile
// -----------------------------------------------------------------------------
//	
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
			iSIPProfile->SetProviderNameL(aVal);
			break;
		case KSIPUserAor:
			iSIPProfile->SetAORL(aVal);
			break;
		case KSIPPrivateIdentity:
			iSIPProfile->SetPrivateIdentityL(aVal);
			break;
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
		case KBearerType:
			User::Leave(KErrNotFound);
		case KSIPContactHeaderUser:
		    User::Leave(KErrAccessDenied);
		default:
			iSIPProfile->SetExtensionParameterL(aParam, aVal);
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
			iSIPProfile->SetIapId(aVal);
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
        case KPrimaryAPN:
        case KSecondaryAPN:
			User::Leave(KErrNotFound);
		case KSIPSnapId:
			__ASSERT_ALWAYS((aVal>0),User::Leave(KErrArgument));
		default:
			iSIPProfile->SetExtensionParameterL(aParam, aVal);
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
		case KSIPDefaultProfile:
			__ASSERT_ALWAYS(aVal, User::Leave(KErrArgument));
			iSIPProfile->SetDefault(aVal);
			break;
		case KSIPSigComp:
			iSIPProfile->EnableSigComp(aVal);
			break;
		case KSIPAutoRegistration:
			iSIPProfile->SetAutoRegistrationEnabled(aVal);
			break;
		case KSIPSecurityNegotiation:
			iSIPProfile->EnableSecurityNegotiation(aVal);
			break;		
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
		case KSIPSnapId:
        case KPrimaryAPN:
        case KSecondaryAPN:
        case KBearerType:
			User::Leave(KErrNotFound);			
		default:
			iSIPProfile->SetExtensionParameterL(aParam, aVal);
		}
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
void CSIPManagedProfile::SetParameterL(TUint32 aServerType,
	TUint32 aParam, const TDesC8& aVal)
	{
	switch (aParam)
		{
		case KSIPDigestRealm:
		case KSIPDigestUserName:
		case KSIPDigestPassword:
			iSIPProfile->SetServerParameterL(aServerType, aParam, aVal);
			break;
		case KSIPServerAddress:
			iSIPProfile->SetServerL(aServerType, aVal);
			break;
		case KSIPProfileId:
		case KSIPProviderName:
		case KSIPAccessPointId:
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
		case KSIPPrivateIdentity:
		case KSIPContactHeaderUser:
		case KSIPSoIpTOS:	
		case KSIPSnapId:
        case KPrimaryAPN:
        case KSecondaryAPN:
		User::Leave(KErrNotFound);
		default:
			TBuf8<KParameterbufLength> buf;
			buf.Format(KSIPProfileExtenstionParameter, aParam);
			iSIPProfile->SetServerExtensionParameterL(
				aServerType, buf, aVal);
		}
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
void CSIPManagedProfile::SetParameterL(TUint32 aParam,
	const MDesC8Array& aVal)
	{
	switch (aParam)
		{
		case KSIPContactHeaderParams:
			iSIPProfile->SetContactHeaderParamsL(aVal);
			break;
		case KSIPHeaders:
			iSIPProfile->SetSIPHeadersL(aVal);
			break;		
		case KSIPProfileId:
		case KSIPProviderName:
		case KSIPAccessPointId:
		case KSIPSigComp:
		case KSIPSecurityNegotiation:
		case KSIPAutoRegistration:
		case KSIPUserAor:
		case KSIPProfileRegistered:
		case KSIPRegisteredAors:
		case KSIPNegotiatedSecurityMechanism:
		case KSIPDefaultProfile:		
		case KSIPRegistrar:
		case KSIPOutboundProxy:
		case KSIPDigestUserName:
		case KSIPDigestRealm:
		case KSIPServerAddress:
		case KSIPPrivateIdentity:
		case KSIPDigestPassword:
		case KSIPContactHeaderUser:
		case KSIPSoIpTOS: 	
		case KSIPSnapId:
        case KPrimaryAPN:
        case KSecondaryAPN:
		User::Leave(KErrNotFound);
		default:
			iSIPProfile->SetExtensionParameterL(aParam, aVal);
		}
	}
