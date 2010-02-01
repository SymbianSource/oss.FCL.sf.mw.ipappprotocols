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
// Name        : SIPProfileStorage.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SIPProfileStorage.h"
#include "SIPProfileStorageIndex.h"
#include "sipconcreteprofile.h"
#include <centralrepository.h>
#include <sipprofile.h>
#include <sipmanagedprofile.h>
#include "SIPProfileStoragePrivateCRKeys.h"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileStorage* CSIPProfileStorage::NewL(RFs& aFs)
    {
    CSIPProfileStorage* self = new (ELeave) CSIPProfileStorage(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorage::CSIPProfileStorage
// -----------------------------------------------------------------------------
//
CSIPProfileStorage::CSIPProfileStorage(RFs& aFs)
 : CSIPProfileStorageBase(aFs)
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorage::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorage::ConstructL()
    {
	BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorage::~CSIPProfileStorage
// -----------------------------------------------------------------------------
//
CSIPProfileStorage::~CSIPProfileStorage()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorage::AddSIPProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorage::AddSIPProfileL(CSIPConcreteProfile& aProfile)
    {
	
	__ASSERT_ALWAYS(aProfile.StorageId() == 0, User::Leave (KErrArgument));
    
    TInt newId = iProfileIndex->GetNextProfileIdL();
    aProfile.SetId(newId);
    aProfile.SetStorageId(newId);
    User::LeaveIfError(iSipCenRep->StartTransaction(
    	CRepository::EReadWriteTransaction));
	iSipCenRep->CleanupCancelTransactionPushL();    
	StoreProfileL(aProfile);
	iProfileIndex->AddProfileL(newId, aProfile.IsDefault());	
	TUint32 rowsInserted(0);
	TInt err = iSipCenRep->CommitTransaction(rowsInserted);
	CleanupStack::PopAndDestroy();		
	if(KErrNone != err)
		{
		TRAP_IGNORE(iProfileIndex->RemoveProfileL(newId));
		User::Leave(err);
		}
    
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorage::UpdateSIPProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorage::UpdateSIPProfileL(CSIPConcreteProfile& aProfile)
    {
	
    __ASSERT_ALWAYS(iProfileIndex->ProfileExists(aProfile.StorageId()),
	                User::Leave (KErrNotFound));    

	User::LeaveIfError(iSipCenRep->StartTransaction(
		CRepository::EReadWriteTransaction));
	iSipCenRep->CleanupCancelTransactionPushL(); 
	StoreProfileL(aProfile);   
	TUint32 rowsAffected(0);
    User::LeaveIfError(iSipCenRep->CommitTransaction(rowsAffected));
	CleanupStack::PopAndDestroy();
	
	if (aProfile.IsDefault())
		{		
		UpdateDefaultSIPProfileL(aProfile.Id());
		}     	
	
    }
    

// -----------------------------------------------------------------------------
// CSIPProfileStorage::StoreKeyL
// -----------------------------------------------------------------------------
//
template<typename T>
void CSIPProfileStorage::StoreKeyL(TUint32 aKey, const T& aVal)
	{
	
	TUint32 metaData;
	if (KErrNotFound == iSipCenRep->GetMeta(aKey, metaData))
		{
		
		User::LeaveIfError(iSipCenRep->Create(aKey, aVal));
		}
	else
		{
		
		User::LeaveIfError(iSipCenRep->Set(aKey, aVal));		
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorage::StoreProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorage::StoreProfileL(CSIPConcreteProfile& aProfile)
	{
	
	TInt key = aProfile.Id() << KProfileIdShift;

	StoreKeyL(key | KSIPProfileId << KVariableIdShift , (TInt)aProfile.Id());
	StoreKeyL(
		key | KSIPAccessPointId << KVariableIdShift,
		(TInt)aProfile.IapId());
	StoreKeyL(
		key | KSIPProfileName << KVariableIdShift,
		aProfile.ProfileType().iSIPProfileName);
	StoreKeyL(
		key | KSIPProfileClass << KVariableIdShift,
		aProfile.ProfileType().iSIPProfileClass);
	StoreKeyL(
		key | KSIPProviderName  << KVariableIdShift,
		aProfile.ProviderName());
	StoreKeyL(
		key | KSIPPrivateIdentity   << KVariableIdShift,
		aProfile.PrivateIdentity());
	StoreKeyL(key | KSIPUserAor << KVariableIdShift, aProfile.AOR());
	StoreKeyL(
		key | KSIPSigComp  << KVariableIdShift,
		aProfile.IsSigCompEnabled());
	StoreKeyL(
		key | KSIPSecurityNegotiation   << KVariableIdShift,
		aProfile.IsSecurityNegotiationEnabled());
	StoreKeyL(
		key | KSIPAutoRegistration   << KVariableIdShift,
		aProfile.IsAutoRegistrationEnabled());
	StoreKeyL(key | KSIPRegistrar << KVariableIdShift,
		aProfile.Server(KSIPRegistrar));
	StoreKeyL(key | KSIPOutboundProxy << KVariableIdShift,
		aProfile.Server(KSIPOutboundProxy));
    StoreKeyL(key | KSIPNegotiatedSecurityMechanism << KVariableIdShift,
        aProfile.NegotiatedSecurityMechanism());
    StoreKeyL(key | KSIPDynamicProxy << KVariableIdShift,
        aProfile.DynamicProxy());
    StoreKeyL(key | KSIPContextId << KVariableIdShift,
        (TInt)aProfile.ContextId());
    StoreKeyL(key | KSIPIsEnabled << KVariableIdShift,
        aProfile.IsEnabled());
	
	StoreArraysL(aProfile);
	
	}

// -----------------------------------------------------------------------------
// CSIPProfileStorage::StoreArraysL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorage::StoreArraysL(CSIPConcreteProfile& aProfile)
	{
	
	TInt count;
	TUint32 intVal;
	TBool boolVal;
	const TDesC8* desVal;
	const MDesC8Array* desArrayVal;
	const TDesC8* desKey;
	TInt i,pos;
	TUint id;
	
	TInt key = aProfile.Id() << KProfileIdShift;
	
	// Server Parameters - Registrar:
	count = aProfile.ServerParameterCount(KSIPRegistrar);
	for (i = 0; i < aProfile.ServerParameterCount(KSIPRegistrar); ++i)
		{
		User::LeaveIfError(aProfile.ServerParameter(
			KSIPRegistrar,
			i,
			id,
			desVal));
		
		switch (id)
			{
			case KSIPDigestUserName:
				StoreKeyL(
					key | KSIPRegistrarDigestUserName << KVariableIdShift,
					*desVal);
				break;
			
			case KSIPDigestRealm:
				StoreKeyL(
					key | KSIPRegistrarDigestRealm << KVariableIdShift,
					*desVal); 
				break;

			case KSIPDigestPassword:
				StoreKeyL(
					key | KSIPRegistrarDigestPassword << KVariableIdShift,
					*desVal);
				break;
			
			default:
				User::Leave(KErrArgument);
			}
		}

	// Server Parameters - Outbound Proxy:
	count = aProfile.ServerParameterCount(KSIPOutboundProxy);
	for (i = 0; i < count; ++i)
		{
		User::LeaveIfError(aProfile.ServerParameter(
			KSIPOutboundProxy,
			i,
			id,
			desVal));
		
		switch (id)
			{
			case KSIPDigestUserName:
				StoreKeyL(
					key | KSIPProxyDigestUserName << KVariableIdShift,
					*desVal);
				break;
			
			case KSIPDigestRealm:
				StoreKeyL(
					key | KSIPProxyDigestRealm << KVariableIdShift,
					*desVal); 
				break;

			case KSIPDigestPassword:
				StoreKeyL(
					key | KSIPProxyDigestPassword << KVariableIdShift,
					*desVal);
				break;
			
			default:
				User::Leave(KErrArgument);
			}
		}

	// Server Extension Parameters - Registrar:
	DeleteArrayL(aProfile.StorageId(), KSIPRegistrarExtParam); 
	count = aProfile.ServerExtensionParameterCount(KSIPRegistrar);
	StoreKeyL(key| KSIPRegistrarExtParam << KVariableIdShift, count);
	for (i = 0, pos = 0; i < count; ++i)
		{
		User::LeaveIfError(
			aProfile.ServerExtensionParameter(
				KSIPRegistrar,
				i,
				desKey,
				desVal));
		StoreKeyL(
			key | KSIPRegistrarExtParam << KVariableIdShift | ++pos,
			*desKey);
		StoreKeyL(
			key | KSIPRegistrarExtParam << KVariableIdShift | ++pos,
			*desVal);
		}
	
	// Server Extension Parameters - Outbound Proxy:
	DeleteArrayL(aProfile.StorageId(), KSIPExtProxyParam); 
	count = aProfile.ServerExtensionParameterCount(KSIPOutboundProxy);
	StoreKeyL(key| KSIPExtProxyParam << KVariableIdShift , count);
	for (i = 0, pos = 0; i < count; ++i)
		{
		User::LeaveIfError(
			aProfile.ServerExtensionParameter(
				KSIPOutboundProxy,
				i,
				desKey,
				desVal));
		StoreKeyL(
			key | KSIPExtProxyParam << KVariableIdShift | ++pos,
			*desKey);
		StoreKeyL(
			key | KSIPExtProxyParam << KVariableIdShift | ++pos,
			*desVal);
		}
	
	// Contact Headers:
	DeleteArrayL(aProfile.StorageId(), KSIPContactHeaderParams); 
	const MDesC8Array& contactHeaders = aProfile.ContactHeaderParams();
	count = contactHeaders.MdcaCount();
	StoreKeyL(key | KSIPContactHeaderParams << KVariableIdShift , count);
	for (i = 0; i < count; ++i)
		{
		StoreKeyL(
			key | KSIPContactHeaderParams << KVariableIdShift | i + 1,
			contactHeaders.MdcaPoint(i));
		}
	
	// SIP Headers:
	DeleteArrayL(aProfile.StorageId(), KSIPHeaders); 
	const MDesC8Array& sipHeaders = aProfile.SIPHeaders();
	count = sipHeaders.MdcaCount();
	StoreKeyL(key| KSIPHeaders << KVariableIdShift , count);
	for (i = 0; i < count; ++i)
		{
		StoreKeyL(
			key | KSIPHeaders  << KVariableIdShift | i + 1,
			sipHeaders.MdcaPoint(i));
		}
	
	// Extension Bool Parameters
	DeleteArrayL(aProfile.StorageId(), KSipExtensionBoolParam); 
	count = aProfile.ExtensionBoolParameterCount();
	StoreKeyL(key | KSipExtensionBoolParam << KVariableIdShift , count);
	for (i = 0, pos = 0; i < count; ++i)
		{
		User::LeaveIfError(aProfile.ExtensionParameter(i, id, boolVal));
		StoreKeyL(
			key | KSipExtensionBoolParam << KVariableIdShift | ++pos, 
			(TInt &)id);
		StoreKeyL(
			key | KSipExtensionBoolParam << KVariableIdShift | ++pos,
			boolVal);
		}
	
	// Extension Int Parameters:
	DeleteArrayL(aProfile.StorageId(), KSIPExtensionIntParam); 
	count = aProfile.ExtensionIntParameterCount();
	StoreKeyL(key | KSIPExtensionIntParam << KVariableIdShift , count);
	for (i = 0, pos = 0; i < count; ++i)
		{
		User::LeaveIfError(aProfile.ExtensionParameter(i, id, intVal));
		StoreKeyL(
			key | KSIPExtensionIntParam << KVariableIdShift | ++pos, 
			(TInt &)id);
		StoreKeyL(
			key | KSIPExtensionIntParam << KVariableIdShift | ++pos,
			(TInt)intVal);
		}
	
	// Extension Des Parameters:
	DeleteArrayL(aProfile.StorageId(), KSIPExtensionDescParam); 
	count = aProfile.ExtensionDesParameterCount();
	StoreKeyL(key | KSIPExtensionDescParam << KVariableIdShift , count);
	for (i = 0, pos = 0; i < count; ++i)
		{
		User::LeaveIfError(aProfile.ExtensionParameter(i, id, desVal));
		StoreKeyL(
			key | KSIPExtensionDescParam << KVariableIdShift | ++pos,
			(TInt &)id);
		StoreKeyL(
			key | KSIPExtensionDescParam << KVariableIdShift | ++pos,
			*desVal);
		}
	
	// Extension Des Array Parameters:
	DeleteArrayL(aProfile.StorageId(), KSIPExtensionDescArrayParam);
	count = aProfile.ExtensionDesArrayParameterCount();
	TInt totalcount(0);
	for (i = 0; i < count; i++)
    	{
    	User::LeaveIfError(aProfile.ExtensionParameter(i, id, desArrayVal));
    	totalcount += desArrayVal->MdcaCount();
    	totalcount += 2; // sub array length  + id 
    	}
	StoreKeyL(
		key | KSIPExtensionDescArrayParam << KVariableIdShift, 
		totalcount);
	
	for (i = 0, pos = 0; i < count; ++i)
		{
		User::LeaveIfError(aProfile.ExtensionParameter(i, id, desArrayVal));
		StoreKeyL(key | KSIPExtensionDescArrayParam << KVariableIdShift | ++pos ,
			desArrayVal->MdcaCount());
		StoreKeyL(key | KSIPExtensionDescArrayParam << KVariableIdShift | ++pos,
			(TInt &)id);
		for(TInt k = 0; k < desArrayVal->MdcaCount(); ++k)
			{
			StoreKeyL(
				key | KSIPExtensionDescArrayParam << KVariableIdShift | ++pos,
				desArrayVal->MdcaPoint(k));	
			}
		}
	
	}


// -----------------------------------------------------------------------------
// CSIPProfileStorage::UpdateDefaultSIPProfileL
// -----------------------------------------------------------------------------
//

void CSIPProfileStorage::UpdateDefaultSIPProfileL(TUint32 aStorageId)
    {	
	
	iProfileIndex->SetDefaultProfileL(aStorageId); 
	
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorage::DeleteSIPProfileL
// -----------------------------------------------------------------------------
//

void CSIPProfileStorage::DeleteSIPProfileL(TUint32 aStorageId) 
    {
    PROFILE_DEBUG3("CSIPProfileStorage::DeleteSIPProfileL : Storage Id :",aStorageId)
	__ASSERT_ALWAYS(iProfileIndex->ProfileExists(aStorageId), 
	                User::Leave(KErrNotFound));
	
	User::LeaveIfError(iSipCenRep->StartTransaction(
		CRepository::EReadWriteTransaction));
	iSipCenRep->CleanupCancelTransactionPushL(); 
	TUint32 errKey;
	iSipCenRep->Delete(aStorageId << KProfileIdShift, KProfileIdMask, errKey);	
	TUint32 rowsAffected;
	
	TInt err = iSipCenRep->CommitTransaction(rowsAffected);
	
	// On error the transaction will be rolled-out hence the profile will not be
	// removed from cenrep. Remove profileID only when the transaction is 
	if (err == KErrNone)
	{
		iProfileIndex->RemoveProfileL(aStorageId);
	}

    CleanupStack::PopAndDestroy();
	
    }


// -----------------------------------------------------------------------------
// CSIPProfileStorage::DeleteArrayL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorage::DeleteArrayL(TUint32 aStorageId, TInt aKey)
	{
	// deletes all the array elements which are corresponding to the Variable Id.
    TUint32 errorKey;
    TInt err = iSipCenRep->Delete(
    	((aStorageId << KProfileIdShift) |  (aKey << KVariableIdShift)),
		KProfileAndVarMask, 
		errorKey);
    if (KErrNone != err && KErrNotFound != err)
    	{
    	User::Leave(err);
    	}
	
	}
