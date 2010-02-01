// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofileitem
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "SipProfileItem.h"
#include "sipmanagedprofile.h"
#include "sipconcreteprofile.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileItem* CSIPProfileItem::NewL()
	{
    CSIPProfileItem* self = CSIPProfileItem::NewLC();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileItem* CSIPProfileItem::NewLC()
	{
	CSIPProfileItem* self = new (ELeave) CSIPProfileItem();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::CSIPProfileItem
// -----------------------------------------------------------------------------
//
CSIPProfileItem::CSIPProfileItem()
:iProfileId(0), iEnabled(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::~CSIPProfileItem
// -----------------------------------------------------------------------------
//
CSIPProfileItem::~CSIPProfileItem()
	{
	iProfiles.Reset();
	iProfiles.Close();
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::SetProfileId
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::SetProfileId(TUint aProfileId)
	{
	iProfileId = aProfileId;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::SetProfile
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::SetProfile(CSIPProfile& aProfile)
	{
	iConcreteProfile = &(aProfile.ConcreteProfile());
	iProfileId = iConcreteProfile->Id();
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::Profile
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPProfileItem::Profile() const
	{
	ASSERT(iProfiles.Count() > 0);
	return static_cast<CSIPProfile*>(iProfiles[0]);
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::SetConcreteProfile
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::SetConcreteProfile(CSIPConcreteProfile* aProfile)
	{

	for (int i = 0; i < iProfiles.Count(); i++)
		{
		iProfiles[i]->ClearConcreteProfile();
		iProfiles[i]->SetConcreteProfile(aProfile);
		}
	delete iConcreteProfile;
	iConcreteProfile = aProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::ConcreteProfile
// -----------------------------------------------------------------------------
//
const CSIPConcreteProfile& CSIPProfileItem::ConcreteProfile() const
	{
	return *iConcreteProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::ConcreteProfile
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileItem::ConcreteProfile()
	{
	return *iConcreteProfile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::IncrementReferenceCountL
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::IncrementReferenceCountL(CSIPProfile& aProfile)
	{
	User::LeaveIfError(iProfiles.Append(&aProfile));
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::DecrementReferenceCount
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::DecrementReferenceCount(CSIPProfile& aProfile)
	{
	TBool found = EFalse;
	for (int i = 0; i < iProfiles.Count() && !found; i++)
		{
		if(iProfiles[i] == &aProfile)
			{
			iProfiles.Remove(i);
			found = ETrue;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::ReferenceCount
// -----------------------------------------------------------------------------
//
TUint CSIPProfileItem::ReferenceCount() const
	{
	return iProfiles.Count();
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::SetEnabled
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::SetEnabled(TBool aEnabled)
	{
	for (int i = 0; i < iProfiles.Count(); i++)
		{
		(iProfiles[i])->SetEnabled(aEnabled);
		}
	iEnabled = aEnabled;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::IsEnabled
// -----------------------------------------------------------------------------
//
TBool CSIPProfileItem::IsEnabled() const
	{
	return iEnabled;
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::ErrorOccurred
// -----------------------------------------------------------------------------
//
void CSIPProfileItem::ErrorOccurred(CSIPConcreteProfile::TStatus aStatus, 
									TInt aError)
	{
	iConcreteProfile->SetStatus(CSIPConcreteProfile::EUnregistered);
	if (aStatus == CSIPConcreteProfile::ERegistrationInProgress)
		{
		iConcreteProfile->SetLastRegistrationError(aError);
		}
	SetEnabled(EFalse);
	}

// -----------------------------------------------------------------------------
// CSIPProfileItem::Compare
// -----------------------------------------------------------------------------
//
TBool CSIPProfileItem::Compare(const CSIPProfileItem& aItem,
				const CSIPProfileItem& aItem2) 
	{
	return (aItem.iProfileId == aItem2.iProfileId);
	}

