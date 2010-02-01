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
// Name        : sipconcreteprofileholder.cpp
// Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
// Implementation
// Version     : %version: 2.1.1 %
//



//  INCLUDE FILES
#include "sipconcreteprofileholder.h"
#include "sipprofile.h"
#include "sipmanagedprofile.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfileHolder* CSIPConcreteProfileHolder::NewL()
	{
	CSIPConcreteProfileHolder* self = CSIPConcreteProfileHolder::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfileHolder* CSIPConcreteProfileHolder::NewLC()
	{
	CSIPConcreteProfileHolder* self = new (ELeave) CSIPConcreteProfileHolder();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
		
// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPConcreteProfileHolder::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::CSIPConcreteProfileHolder
// -----------------------------------------------------------------------------
//
CSIPConcreteProfileHolder::CSIPConcreteProfileHolder()
	{
	iInternalized = EFalse;
	}
		
// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::~CSIPConcreteProfileHolder
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfileHolder::~CSIPConcreteProfileHolder()
	{
	if (iInternalized)
		{
		iSIPProfiles.ResetAndDestroy();
		}
	else
		{
		iSIPProfiles.Reset();
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::SIPProfiles
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPConcreteProfile>& 
CSIPConcreteProfileHolder::SIPProfiles()
	{
	return iSIPProfiles;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfileHolder* CSIPConcreteProfileHolder::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPConcreteProfileHolder* self = CSIPConcreteProfileHolder::NewLC();
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPConcreteProfileHolder::ExternalizeL(
    RWriteStream& aWriteStream) const
	{
	TUint count = iSIPProfiles.Count();
	aWriteStream.WriteUint32L(count);
	for (TUint i = 0; i < count; i++) 
		{
		CSIPConcreteProfile* profile = CloneProfileWithoutPasswordsLC( 
			(CSIPConcreteProfile*) iSIPProfiles[i] );

		profile->ExternalizeL(aWriteStream);
		CleanupStack::PopAndDestroy(profile);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::ExternalizedSizeL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPConcreteProfileHolder::ExternalizedSizeL() const
	{
	TUint size = 4;
	for (TInt i = 0; i < iSIPProfiles.Count(); i++) 
		{
		CSIPConcreteProfile* profile = CloneProfileWithoutPasswordsLC( 
			(CSIPConcreteProfile*) iSIPProfiles[i] );

		size = size + profile->ExternalizedSizeL();
		CleanupStack::PopAndDestroy(profile);
		}
	return size;
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CSIPConcreteProfileHolder::DoInternalizeL(RReadStream& aReadStream)
	{
	iInternalized = ETrue;
	TUint count = aReadStream.ReadUint32L();
	for (TUint i = 0; i < count; i++) 
		{
		CSIPConcreteProfile* profile = 
		    CSIPConcreteProfile::InternalizeL(aReadStream);
		CleanupStack::PushL(profile);
		iSIPProfiles.AppendL(profile);
		CleanupStack::Pop(profile);
		}
	}

// -----------------------------------------------------------------------------
// CSIPConcreteProfileHolder::CloneProfileWithoutPasswordsLC
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPConcreteProfileHolder::CloneProfileWithoutPasswordsLC(
		const CSIPConcreteProfile* aProfile)
	{
	CSIPConcreteProfile* clonedProfile = 
		aProfile->CloneWithDynamicValuesL();
	
	// Remove passwords from copied profile
	CleanupStack::PushL(clonedProfile);
	clonedProfile->SetServerParameterL(
		KSIPOutboundProxy, KSIPDigestPassword, KNullDesC8 );
	clonedProfile->SetServerParameterL( 
		KSIPRegistrar, KSIPDigestPassword, KNullDesC8 );	

	return clonedProfile;
	}
