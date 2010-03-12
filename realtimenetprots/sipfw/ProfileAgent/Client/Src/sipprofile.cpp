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
// Name        : sipprofile
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "sipprofile.h"
#include "sipmanagedprofile.h"
#include "sipprofileregistry.h"
#include "sipconcreteprofile.h"
#include "sipprofiletypeinfo.h"
#include <e32const.h>
#include <cmmanager.h>


_LIT8(KSIPProfileExtenstionParameter, "%u");
const TUint KProfileBufferSize = 25;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPProfile::NewL(CSIPProfileRegistry* aSIPRegistry)
	{
    CSIPProfile* self = CSIPProfile::NewLC (aSIPRegistry);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPProfile::NewLC(CSIPProfileRegistry* aSIPRegistry)
	{
	CSIPProfile* self = new(ELeave)CSIPProfile(aSIPRegistry);
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::~CSIPProfile
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfile::~CSIPProfile()
	{
	if(iSIPProfileRegistry)
		{
		iSIPProfileRegistry->ProfileDeleted(*this);	
		}

	delete iSIPProfile;
	iSIPProfile = 0;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::Type
// -----------------------------------------------------------------------------
//	
EXPORT_C const TSIPProfileTypeInfo& CSIPProfile::Type() const
	{
	return iSIPProfile->ProfileType();
	}	

// -----------------------------------------------------------------------------
// CSIPProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPProfile::GetParameter(TUint32 aParam, TDesC8 const *& aVal) const
	{
	switch (aParam)
		{
		case KSIPProviderName:
			{
			const TDesC8& val= iSIPProfile->ProviderName();
			aVal = &val;
			break;
			}
		case KSIPUserAor:
			{
			const TDesC8& val = iSIPProfile->AOR();
			aVal = &val;
			break;
			}
		case KSIPPrivateIdentity:
			{
			const TDesC8& val = iSIPProfile->PrivateIdentity();
			aVal = &val;
			break;
			}
		case KSIPNegotiatedSecurityMechanism:
			{
			if(iSIPProfileRegistry)
				{
				TRAPD(err,iSIPProfileRegistry->NegotiatedSecurityMechanismL(*iSIPProfile));
				if (err != KErrNone)
					{
					return err;
					}
				}
			const TDesC8& val = iSIPProfile->NegotiatedSecurityMechanism();
			aVal = &val;
			break;
			}
		case KSIPProfileId:
		case KSIPAccessPointId:
		case KSIPSigComp:
		case KSIPSecurityNegotiation:
		case KSIPAutoRegistration:
		case KSIPProfileRegistered:
		case KSIPRegisteredAors:
		case KSIPDefaultProfile:
		case KSIPContactHeaderParams:
		case KSIPRegistrar:
		case KSIPOutboundProxy:
		case KSIPDigestUserName:
		case KSIPDigestRealm:
		case KSIPServerAddress:
		case KSIPDigestPassword:
			{
			return KErrNotFound;
			}
		default:
			{
			return iSIPProfile->ExtensionParameter(aParam, aVal);
			}
		}
	return KErrNone;
	}	

// -----------------------------------------------------------------------------
// CSIPProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPProfile::GetParameter(TUint32 aParam, TUint32& aVal) const
	{
	TInt res(KErrNone);
	switch (aParam)
		{
		case KSIPProfileId:
			aVal = ((iSIPProfile != 0)?iSIPProfile->Id():0);
			break;
		case KSIPAccessPointId:
			aVal = iSIPProfile->IapId();
			break;
		case KSIPSnapId:
			// Explicit check for SNAP ID inorder to return default SNAP in case of KMaxTUint32.
			{
			res = iSIPProfile->ExtensionParameter(aParam, aVal);
			if(aVal == KMaxTUint32)
				{ 
				TRAPD(err,DefaultSNAPL(aVal));
				if (err)
				    {
				    return err;
				    }	
				}
				break;
			}	
					
		default:
			//If aParam is one of the predefined values (e.g. KSIPSigComp),
			//it won't be found and ExtensionParameter returns KErrNotFound.
			return iSIPProfile->ExtensionParameter(aParam, aVal);
		}
	return res;
	}	

// -----------------------------------------------------------------------------
// CSIPProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPProfile::GetParameter(TUint32 aParam, TBool& aVal) const
	{
	switch (aParam)
		{
		case KSIPSigComp:
			aVal = iSIPProfile->IsSigCompEnabled();
			break;
		case KSIPAutoRegistration:
			aVal = iSIPProfile->IsAutoRegistrationEnabled();
			break;
		case KSIPSecurityNegotiation:
			aVal = iSIPProfile->IsSecurityNegotiationEnabled();
			break;
		case KSIPProfileRegistered:
			aVal = (iSIPProfile->Status() == CSIPConcreteProfile::ERegistered);
			break;
		case KSIPDefaultProfile:
		    if(iSIPProfileRegistry)
		        {
			    TRAPD(err, aVal = iSIPProfileRegistry->IsDefaultProfileL(*this));
			    if (err)
				    {
				    return err;
				    }		        
		        }

			break;
		default:			
			//If aParam is one of the predefined values it won't be found and
			//ExtensionParameter returns KErrNotFound.
			return iSIPProfile->ExtensionParameter(aParam, aVal);
		}
	return KErrNone;
	}	

// -----------------------------------------------------------------------------
// CSIPProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPProfile::GetParameter(TUint32 aParam, 
	MDesC8Array const *& aVal) const
	{
	switch (aParam)
		{
		case KSIPRegisteredAors:
			{
			if(iSIPProfileRegistry)
				{
				TRAPD(err, iSIPProfileRegistry->RegisteredAORsL(*iSIPProfile));
				if (err != KErrNone)
					{
					return err;
					}
				}
			const MDesC8Array& array = iSIPProfile->RegisteredAORs();
			aVal = &array;
			}
			break;
		case KSIPContactHeaderParams:
			{
			const MDesC8Array& array = iSIPProfile->ContactHeaderParams();
			aVal = &array;
			}
			break;
		case KSIPHeaders:
			{
			const MDesC8Array& array = iSIPProfile->SIPHeaders();
			aVal = &array;
			}
			break;			
		default:
			//If aParam is one of the predefined values (e.g. KSIPRegistrar),
			//it won't be found and ExtensionParameter returns KErrNotFound.
			return iSIPProfile->ExtensionParameter(aParam, aVal);
		}
	return KErrNone;
	}	

// -----------------------------------------------------------------------------
// CSIPProfile::GetParameter
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CSIPProfile::GetParameter(TUint32 aServerType,
	TUint32 aParam, TDesC8 const *& aVal) const
	{
	switch (aParam)
		{
		case KSIPDigestRealm:
		case KSIPDigestUserName:
			{
			const TDesC8& val = iSIPProfile->ServerParameter(aServerType, aParam);
			aVal = &val;
			break;
			}
		case KSIPServerAddress:
			{
			const TDesC8& val = iSIPProfile->Server(aServerType);
			aVal = &val;
			break;
			}

		case KSIPDigestPassword:
			{
			return KErrPermissionDenied;
			}

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
		case KSIPSoIpTOS:
		case KPrimaryAPN:
		case KSecondaryAPN:
			{
			return KErrNotFound;
			}
		default:
			{
			TBuf8<KProfileBufferSize> buf;
			buf.Format(KSIPProfileExtenstionParameter, aParam);
			const TDesC8& val = iSIPProfile->ServerExtensionParameter(
				aServerType, buf);
			aVal = &val;
			break;
			}
		}
	return KErrNone;
	}	

// -----------------------------------------------------------------------------
// CSIPProfile::CSIPProfile
// -----------------------------------------------------------------------------
//	
CSIPProfile::CSIPProfile(CSIPProfileRegistryBase* aRegistry):
	iSIPProfileRegistry(aRegistry), iEnabled(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfile::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPProfile::ConstructL()
	{
	iSIPProfile = CSIPConcreteProfile::NewL();
	}

// -----------------------------------------------------------------------------
// CSIPProfile::SetEnabled
// -----------------------------------------------------------------------------
//	
void CSIPProfile::SetEnabled(TBool aEnabled)
	{
	iEnabled = aEnabled;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::SetConcreteProfile
// -----------------------------------------------------------------------------
//	
void CSIPProfile::SetConcreteProfile(CSIPConcreteProfile* aProfile)
	{
	delete iSIPProfile;
	iSIPProfile = aProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::ClearConcreteProfile
// -----------------------------------------------------------------------------
//	
void CSIPProfile::ClearConcreteProfile()
	{
	iSIPProfile = 0;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::ConcreteProfile
// -----------------------------------------------------------------------------
//	
CSIPConcreteProfile& CSIPProfile::ConcreteProfile()
	{
	return *iSIPProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::ConcreteProfile
// -----------------------------------------------------------------------------
//	
const CSIPConcreteProfile& CSIPProfile::ConcreteProfile() const
	{
	return *iSIPProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::IsEnabled
// -----------------------------------------------------------------------------
//	
TBool CSIPProfile::IsEnabled() const
	{
	return iEnabled;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::LastRegistrationError
// -----------------------------------------------------------------------------
//	
TInt CSIPProfile::LastRegistrationError() const
	{
	return iSIPProfile->LastRegistrationError();
	}

// -----------------------------------------------------------------------------
// CSIPProfile::SetRegistry
// -----------------------------------------------------------------------------
//	
void CSIPProfile::SetRegistry(CSIPProfileRegistryBase* aRegistry)
	{
	iSIPProfileRegistry = aRegistry;
	}

// -----------------------------------------------------------------------------
// CSIPProfile::ClearRegistry
// -----------------------------------------------------------------------------
//	
void CSIPProfile::ClearRegistry()
	{
	iSIPProfileRegistry = 0;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfile::IsContextActive
// -----------------------------------------------------------------------------
//	
TBool CSIPProfile::IsContextActive() const
	{
	return (iEnabled && iSIPProfile->Status() == CSIPConcreteProfile::ERegistered);
	}

// -----------------------------------------------------------------------------
// CSIPProfile::ContextId
// -----------------------------------------------------------------------------
//	
TUint32 CSIPProfile::ContextId() const
	{
	return iSIPProfile->ContextId();
	}

// -----------------------------------------------------------------------------
// CSIPProfile::DefaultSNAPL
// -----------------------------------------------------------------------------
//		
void CSIPProfile::DefaultSNAPL(TUint32& aSnapId) const
	
		{
			RCmManager cmManager;
			cmManager.OpenL();
			CleanupClosePushL(cmManager);
			TCmDefConnValue defConn;
			cmManager.ReadDefConnL( defConn );
			aSnapId = defConn.iId;	
			CleanupStack::PopAndDestroy();
		}

