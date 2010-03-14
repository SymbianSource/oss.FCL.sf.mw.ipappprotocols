/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sipprofile.h"
#include "sipmanagedprofile.h"
#include "sipprofileregistry.h"
#include "sipmanagedprofileregistry.h"

/*

//#include "sipconcreteprofile.h"
//#include "SIPProfileTypeInfo.h"
//_LIT8(KSIPProfileExtenstionParameter, "%u");
//const TUint KProfileBufferSize = 25;

_LIT8(KSIPProfileDefAOR, "user@aa");
        /** 
        * Sets profile parameter
        * @param aParam a parameter to set
        * @param aVal values to set; an empty array resets the value
        * @return KErrNotFound if parameter was not found, 
        *         KErrNoMemory if out of memory          
        *          KErrNone otherwise        
        */
EXPORT_C TInt CSIPManagedProfile::SetParameter( TUint32 /*aParam*/, 
                                         const MDesC8Array& /*aVal*/ )
    {
    return KErrNone;
    }

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSIPManagedProfile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPManagedProfile* CSIPManagedProfile::NewL(CSIPManagedProfileRegistry* aSIPRegistry)
	{
    CSIPManagedProfile* self = CSIPManagedProfile::NewLC (aSIPRegistry);
    CleanupStack::Pop();
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
/*
// -----------------------------------------------------------------------------
// CSIPManagedProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::GetParameter(TUint32 aParam, TDesC8 const *& aVal ) const
	{
	const TDesC8& val= KSIPProfileDefAOR;
    aVal = &val;
	return KErrNone;
	}	

// -----------------------------------------------------------------------------
// CSIPManagedProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::GetParameter(TUint32 aParam, TUint32& aVal) const
	{
	aVal = 1;
	return KErrNone;
	}	

// -----------------------------------------------------------------------------
// CSIPManagedProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPManagedProfile::GetParameter(TUint32 aParam, TBool& aVal) const
	{
	aVal = ETrue;
	return KErrNone;
	}
*/	
// -----------------------------------------------------------------------------
// CSIPManagedProfile::CSIPManagedProfile
// -----------------------------------------------------------------------------
//	
CSIPManagedProfile::CSIPManagedProfile(CSIPManagedProfileRegistry* aRegistry) 
    :  CSIPProfile( aRegistry )
	{
	}

/*
// -----------------------------------------------------------------------------
// CSIPManagedProfile::IsContextActive
// -----------------------------------------------------------------------------
//	
TBool CSIPManagedProfile::IsContextActive() const
	{
	return ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSIPManagedProfile::ContextId
// -----------------------------------------------------------------------------
//	
TUint32 CSIPManagedProfile::ContextId() const
	{
	return 1;
	}
*/

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
TInt CSIPManagedProfile::SetParameter(TUint32 aParam, const TDesC8& aVal)
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
			return KErrNotFound;
		case KSIPContactHeaderUser:
		    return KErrAccessDenied;
		default:
			break;
		}
	
	return KErrNone;
	
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
TInt CSIPManagedProfile::SetParameter(TUint32 aParam, TUint32 aVal)
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
			return KErrNotFound;
		default:
			break;
		}
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPManagedProfile::SetParameterL
// -----------------------------------------------------------------------------
//	
TInt CSIPManagedProfile::SetParameter(TUint32 aParam, TBool aVal)
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
			return KErrNotFound;			
		default:
			break;
		}
	
	return KErrNone;
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

void CSIPManagedProfile::SetType(const TSIPProfileTypeInfo& /*aSIPProfileTypeInfo*/)
	{
	
	}

CSIPManagedProfile* CSIPManagedProfile::CloneL() const
    {
    //const CSIPManagedProfile* temp = this;
    //return new (ELeave) CSIPManagedProfile(*this);
    //return this;
    return NULL;

    }
