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
// Name        : sipprofileregistrybase
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "sip.h"
#include "sipprofileregistrybase.h"
#include "sipprofileregistryobserver.h"
#include "sipprofileregistry.h"
#include "sipprofileregistryobserver.h"
#include "sipmanagedprofile.h"
#include "SipProfileItem.h"
#include "sipprofileplugins.h"
#include "sipconcreteprofileholder.h"
#include "SipProfileCleanupItem.h"
#include "SIPProfileITC.h"
#include "SIPProfileITCReceiver.h"
#include "sipconnectionobserver.h"
#include "SIPRemoteProfile.h"
#include "sipprofilecs.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::~CSIPProfileRegistryBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileRegistryBase::~CSIPProfileRegistryBase()
	{
	delete iProfilePlugins;
	iProfilePlugins = 0;

	delete iFindEntry;
	iFindEntry = 0;

	delete iITCReceiver;
	iITCReceiver =  0;

	delete iITC;
	iITC = 0;

	for (TInt i = 0; i < iProfiles.Count(); i++)
		{
		CSIPProfileItem* item = static_cast<CSIPProfileItem*>(iProfiles[i]);
		if (item->Profile())
			{
			item->Profile()->ClearRegistry();
			item->SetConcreteProfile(0);
			}
		}
		
	iProfiles.ResetAndDestroy();
	iProfiles.Close();

	if (iSipClient)
		{
		iSipClient->Close();
		delete iSipClient;
		iSipClient = 0;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfilesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileRegistryBase::ProfilesL(
			 RPointerArray<CSIPProfile>& aProfiles)
	{
	CSIPConcreteProfileHolder* holder = iITC->SIPProfilesL();
	
	ProfilesCommonL(aProfiles, holder);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfilesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileRegistryBase::ProfilesL(
			 const TSIPProfileTypeInfo& aType,
			 RPointerArray<CSIPProfile>& aProfiles)
	{
	CSIPConcreteProfileHolder* holder = iITC->SIPProfilesByTypeL(aType);
	
	ProfilesCommonL(aProfiles, holder);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfilesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileRegistryBase::ProfilesL(
			 const TDesC8& aAOR,
			 RPointerArray<CSIPProfile>& aProfiles)
	{
	CSIPConcreteProfileHolder* holder = iITC->SIPProfilesByAORL(aAOR);
	
	ProfilesCommonL(aProfiles, holder);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfilesCommonL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::ProfilesCommonL(
			 				RPointerArray<CSIPProfile>& aProfiles,
			 				CSIPConcreteProfileHolder* aHolder)
	{
	RPointerArray<CSIPConcreteProfile>& profiles = aHolder->SIPProfiles();

	CleanupStack::PushL(aHolder);

	StoreArrayL(profiles, aProfiles);

	CleanupStack::Pop(aHolder);
	delete aHolder;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfileL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfile* CSIPProfileRegistryBase::ProfileL(TUint32 aId)
	{
	return SIPProfileL(aId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::DefaultProfileL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfile* CSIPProfileRegistryBase::DefaultProfileL()
	{
	TUint32 profileId(0);
	TUint size(0);
	iITC->SIPProfileDefaultL(size, profileId);

	if (size == 0)
		{
		User::Leave(KErrNotFound);
		}

	return SIPProfileL(profileId, size);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::SupportedProfileTypesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileRegistryBase::SupportedProfileTypesL(
					RArray<TSIPProfileTypeInfo>& aSupportedProfileTypes)
	{
	delete iProfilePlugins;
	iProfilePlugins = NULL;
	iProfilePlugins = iITC->SIPProfilePluginsL();
	
	const RArray<TSIPProfileTypeInfo>& array = iProfilePlugins->Plugins();
	for (TInt i = 0; i < array.Count(); i++)
		{
		User::LeaveIfError(aSupportedProfileTypes.Append(array[i]));
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::LastRegistrationError
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPProfileRegistryBase::LastRegistrationError(
			 const CSIPProfile& aProfile) const
	{
	return aProfile.LastRegistrationError();
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::CSIPProfileRegistryBase
// -----------------------------------------------------------------------------
//
CSIPProfileRegistryBase::CSIPProfileRegistryBase(
	MSIPProfileRegistryObserver& aObserver):
	iObserver(aObserver)
	{

	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfileDeleted
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::ProfileDeleted(CSIPProfile& aProfile)
	{
	TUint32 profileId = 0;
	aProfile.GetParameter(KSIPProfileId, profileId);
	TInt index = ProfileIndex(profileId);
	if(index != KErrNotFound) 
		{
		CSIPProfileItem* item = ProfileItemByIndex(index);
		item->DecrementReferenceCount((CSIPProfile&) aProfile);
		if (item->ReferenceCount() == 0) 
			{
			iProfiles.Remove(index);
			delete item;
			// ignore any error
			TRAPD(err, iITC->DeleteSIPProfileL(aProfile.ConcreteProfile().Id()));
			//for get rid of armv5 warning
			err++;
			}
		else
			{
			aProfile.ClearConcreteProfile();
			}
		} 
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::EnableProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::EnableProfileL(
	CSIPProfile& aProfile)
	{
	CSIPProfileItem* item = ProfileItemL(aProfile.ConcreteProfile().Id());
	TInt status = 0;
	TRAPD(err, status = iITC->EnableSIPProfileL(aProfile.ConcreteProfile().Id()));
	aProfile.ConcreteProfile().SetLastRegistrationError(err);
	User::LeaveIfError(err);
	item->SetEnabled(ETrue);
	aProfile.ConcreteProfile().SetStatus((enum CSIPConcreteProfile::TStatus) status);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::DisableProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::DisableProfileL(CSIPProfile& aProfile)
	{
	CSIPProfileItem* item = ProfileItemL(aProfile.ConcreteProfile().Id());
	item->SetEnabled(EFalse);
	TInt status = iITC->DisableSIPProfileL(aProfile.ConcreteProfile().Id());
	aProfile.ConcreteProfile().SetStatus((enum CSIPConcreteProfile::TStatus) status);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ForceDisableProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::ForceDisableProfileL(
	CSIPProfile& aProfile)
	{
	CSIPProfileItem* item = ProfileItemL(aProfile.ConcreteProfile().Id());
	item->SetEnabled(EFalse);
	TInt status = iITC->ForceDisableSIPProfileL(aProfile.ConcreteProfile().Id());
	aProfile.ConcreteProfile().SetStatus((enum CSIPConcreteProfile::TStatus) status);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::UsageL
// -----------------------------------------------------------------------------
//
TInt CSIPProfileRegistryBase::UsageL(const CSIPProfile& aProfile) const
	{
	return iITC->SIPProfileUsageL(aProfile.ConcreteProfile().Id());
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::AddProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::AddProfileL(CSIPProfile& aProfile)
	{
	CSIPProfileItem* item = CSIPProfileItem::NewL();
	CleanupStack::PushL(item);
	item->IncrementReferenceCountL(aProfile);
	User::LeaveIfError(iProfiles.Append(item));
	CleanupStack::Pop(item);

	TRAPD(err, iITC->AddSIPProfileL(aProfile.ConcreteProfile()));
	if (aProfile.ConcreteProfile().Id() != 0)
		{
		item->SetProfile(aProfile);
		}
	else
		{
		TBool found = EFalse;
		for (TInt i = 0; i < iProfiles.Count() && !found; i++)
			{
			if (iProfiles[i] == item)
				{
				iProfiles.Remove(i);
				delete item;
				found = ETrue;
				}
			}
		}
	User::LeaveIfError(err);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::UpdateProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::UpdateProfileL(CSIPProfile& aProfile)
	{	
	iITC->UpdateSIPProfileL(aProfile.ConcreteProfile());
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::RemoveProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::RemoveProfileL(CSIPProfile& aProfile)
	{
	iITC->RemoveSIPProfileL(aProfile.ConcreteProfile().Id());
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::IsDefaultProfileL
// -----------------------------------------------------------------------------
//
TBool CSIPProfileRegistryBase::IsDefaultProfileL(const CSIPProfile& aProfile)
	{
	TUint32 profileId = iITC->SIPProfileDefaultIdL();
	return (aProfile.ConcreteProfile().Id() == profileId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::RegistrationStatusChangedL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::NegotiatedSecurityMechanismL(
	CSIPConcreteProfile& aProfile)
	{
	iITC->NegotiatedSecurityMechanismL(aProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::RegisteredAORsL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::RegisteredAORsL(CSIPConcreteProfile& aProfile)
	{
	iITC->RegisteredAORsL(aProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::SIPProfileL
// -----------------------------------------------------------------------------
//
CSIPProfile* CSIPProfileRegistryBase::SIPProfileL(TUint32 aId, TInt aSize)
	{
	__ASSERT_ALWAYS (aId > 0, User::Leave (KErrArgument));
	TInt index = ProfileIndex(aId);
	if(index == KErrNotFound) 
		{
		CSIPProfileCleanupItem* cleanupItem = new (ELeave) CSIPProfileCleanupItem(iITC);
		CleanupStack::PushL(cleanupItem);
		CleanupStack::PushL(TCleanupItem(CrashRevert, cleanupItem));
		
		cleanupItem->iProfileId = aId;
		cleanupItem->iConcreteProfile = iITC->SIPProfileL(aId, aSize);

		CSIPProfile* managed = 0;
		if (cleanupItem->iConcreteProfile != 0)
			{
			StoreProfileL(cleanupItem);
			managed = cleanupItem->iManagedProfile;
			}

		CleanupStack::Pop();
		CleanupStack::PopAndDestroy(cleanupItem);
		return managed;
		}
	else
		{
		CSIPProfileItem* item = ProfileItemL(aId);
		CSIPProfile* managed = NewInstanceL();
		CleanupStack::PushL(managed);
		managed->SetConcreteProfile(&(item->ConcreteProfile()));
		managed->SetEnabled(item->IsEnabled());
		item->IncrementReferenceCountL(*managed);
		CleanupStack::Pop(managed);
		return managed;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::StoreProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::StoreProfileL(CSIPProfileCleanupItem* aCleanupItem)
	{
	CSIPProfile* managed = NewInstanceL();
	aCleanupItem->iManagedProfile = managed;
	managed->SetConcreteProfile(aCleanupItem->iConcreteProfile);

	CSIPProfileItem* item = CSIPProfileItem::NewL();
	item->SetProfile(*managed);

	CleanupStack::PushL(item);
	User::LeaveIfError(iProfiles.Append(item));
	CleanupStack::Pop(item);

	item->IncrementReferenceCountL(*managed);
	aCleanupItem->iStored = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::StoreProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::StoreArrayL(
	RPointerArray<CSIPConcreteProfile>& aProfiles,
	RPointerArray<CSIPProfile>& aRetProfiles)
	{
	int count = aProfiles.Count();

	for (TInt i = 0; i < count; i++)
		{
		TUint profileId = (aProfiles[0])->Id();
		TInt index = ProfileIndex(profileId);
		
		CSIPProfileCleanupItem* cleanupItem = new (ELeave) CSIPProfileCleanupItem(iITC);
		CleanupStack::PushL(cleanupItem);

		if(index == KErrNotFound) 
			{
			CleanupStack::PushL(TCleanupItem(CrashRevert, cleanupItem));

			cleanupItem->iProfileId = (aProfiles[0])->Id();
			cleanupItem->iConcreteProfile = aProfiles[0];
			aProfiles.Remove(0);

			StoreProfileL(cleanupItem);
			User::LeaveIfError(aRetProfiles.Append(cleanupItem->iManagedProfile));
			}
		else
			{
			CleanupStack::PushL(TCleanupItem(LocalCrashRevert, cleanupItem));

			CSIPProfileItem* item = ProfileItemL(profileId);
			CSIPProfile* managed = NewInstanceL();
			cleanupItem->iManagedProfile = managed;
			managed->SetConcreteProfile(&(item->ConcreteProfile()));
			managed->SetEnabled(item->IsEnabled());
			item->IncrementReferenceCountL(*managed);
			User::LeaveIfError(aRetProfiles.Append(managed));
			CSIPConcreteProfile* profileNotUsed = aProfiles[0];
			aProfiles.Remove(0);
			delete profileNotUsed;
			}
		}
	aProfiles.Reset();

	for (TInt j = 0; j < count; j++)
		{
		CleanupStack::Pop(); //TCleanupItem
		CleanupStack::PopAndDestroy(); //cleanupItem
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfileItemL
// -----------------------------------------------------------------------------
//
CSIPProfileItem* CSIPProfileRegistryBase::ProfileItemL(TUint aProfileId)
	{
	TInt index = ProfileIndex (aProfileId);
	__ASSERT_ALWAYS (index != KErrNotFound, User::Leave (KErrNotFound));
	return ProfileItemByIndex(index); 
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfileIndex
// -----------------------------------------------------------------------------
//
TInt CSIPProfileRegistryBase::ProfileIndex (TUint aProfileId)
	{
	TIdentityRelation<CSIPProfileItem> compareId(CSIPProfileItem::Compare);
	iFindEntry->SetProfileId(aProfileId);
	TInt index = iProfiles.Find(iFindEntry, compareId);
	return index;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ProfileItemByIndex
// -----------------------------------------------------------------------------
//
CSIPProfileItem* CSIPProfileRegistryBase::ProfileItemByIndex(TUint aIndex)
	{
	return static_cast<CSIPProfileItem*>(iProfiles[aIndex]);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::RegistrationStatusChangedL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::RegistrationStatusChangedL(TUint32 aProfileId,
							  TInt aStatus,
							  TUint32 aStatusId)
	{
	TInt index = ProfileIndex(aProfileId);
	if(index != KErrNotFound) 
		{
		CSIPProfileItem* item = ProfileItemByIndex(index);
		// The concrete profile has to be fetched to get the registered Contact
		CSIPConcreteProfile* updatedProfile = 
		    iITC->SIPProfileRefreshL(aProfileId);
		updatedProfile->SetStatus((CSIPConcreteProfile::TStatus) aStatus);
		updatedProfile->SetContextId(aStatusId);
		item->SetConcreteProfile(updatedProfile);
		if (aStatus == CSIPConcreteProfile::ERegistered)
			{
			iObserver.ProfileRegistryEventOccurred(aProfileId, 
				MSIPProfileRegistryObserver::EProfileRegistered);
			}
		else
			{
			if (aStatusId == ESipProfileItcOpProfileForciblyDisabled)
				iObserver.ProfileRegistryEventOccurred(aProfileId,
					MSIPProfileRegistryObserver::EProfileForciblyDisabled);	
			else
				iObserver.ProfileRegistryEventOccurred(aProfileId,
					MSIPProfileRegistryObserver::EProfileDeregistered);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::UpdatedL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::UpdatedL(TUint32 aProfileId, TUint aSize)
	{
	TInt index = ProfileIndex(aProfileId);
	if(index != KErrNotFound) 
		{	
		CSIPConcreteProfile* concrete = 0;
		CSIPProfileItem* item = ProfileItemByIndex(index);
		TRAPD(err, concrete = iITC->SIPProfileRefreshL(aProfileId, aSize));
		if (err == KErrNone)
			{
			item->SetConcreteProfile(concrete); 
			iObserver.ProfileRegistryEventOccurred(aProfileId,
				MSIPProfileRegistryObserver::EProfileUpdated);
			}
#ifdef CPPUNIT_TEST
		if (err == KErrNoMemory) User::Leave(KErrNoMemory);
#endif
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::AddedL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::AddedL(TUint32 aProfileId) 
	{
	iObserver.ProfileRegistryEventOccurred(aProfileId,
		MSIPProfileRegistryObserver::EProfileCreated);
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::RemovedL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::RemovedL(TUint32 aProfileId)
	{
	TInt index = ProfileIndex(aProfileId);
	if(index != KErrNotFound) 
		{
		iObserver.ProfileRegistryEventOccurred(aProfileId,
			MSIPProfileRegistryObserver::EProfileDestroyed);
		}
	}	

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ErrorOccurredL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::ErrorOccurredL(TUint32 aProfileId,
										   TInt aStatus,
										   TInt aError)
	{
	TInt index = ProfileIndex(aProfileId);
	if(index != KErrNotFound) 
		{
		CSIPProfileItem* item = ProfileItemByIndex(index);
		item->ErrorOccurred((CSIPConcreteProfile::TStatus) aStatus, aError);
		iObserver.ProfileRegistryErrorOccurred(aProfileId, aError);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::BaseConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::BaseConstructL()
	{
	iFindEntry = CSIPProfileItem::NewL();

	iSipClient = new (ELeave) RSIPProfile;
	User::LeaveIfError(iSipClient->Connect());

	iITCReceiver = CSIPProfileITCReceiver::NewL (*iSipClient, *this);
	iITC = CSIPProfileITC::NewL (*iSipClient);
	
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::CrashRevert
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::CrashRevert (TAny* aItem)
	{
	CSIPProfileCleanupItem* cleanupItem = static_cast<CSIPProfileCleanupItem*>(aItem);
	TRAPD(err, cleanupItem->CrashRevertL());
	//for get rid of armv5 warning
	err++;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::LocalCrashRevert
// -----------------------------------------------------------------------------
//
void CSIPProfileRegistryBase::LocalCrashRevert (TAny* aItem)
	{
	CSIPProfileCleanupItem* cleanupItem = static_cast<CSIPProfileCleanupItem*>(aItem);

	delete cleanupItem->iManagedProfile;
	cleanupItem->iManagedProfile = 0;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::Itc
// -----------------------------------------------------------------------------
//
CSIPProfileITC& CSIPProfileRegistryBase::Itc()
	{
	return *iITC;
	}

// -----------------------------------------------------------------------------
// CSIPProfileRegistryBase::ItcReceiver
// -----------------------------------------------------------------------------
//
CSIPProfileITCReceiver& CSIPProfileRegistryBase::ItcReceiver()
	{
	return *iITCReceiver;
	}

