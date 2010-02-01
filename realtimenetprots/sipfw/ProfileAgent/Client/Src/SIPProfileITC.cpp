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
// Name        : sipprofileitc
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "SIPProfileITC.h"
#include "SIPRemoteProfile.h"
#include "sipconcreteprofile.h"
#include "sipconcreteprofileholder.h"
#include "sipprofileslots.h"
#include "sipprofileplugins.h"

const TInt  KBufInitSize = 100;

// -----------------------------------------------------------------------------
// template::InternalizeL
// -----------------------------------------------------------------------------
//
template<class T> T* InternalizeL(const TDesC8& aBuf)
	{
	CBufFlat* internalizeBuf = CBufFlat::NewL(KBufInitSize);
	CleanupStack::PushL(internalizeBuf);
	internalizeBuf->InsertL(0,aBuf);

	RBufReadStream readStream(*internalizeBuf,0);
	readStream.PushL();
	T* profile = static_cast<T*>(T::InternalizeL(readStream));
	readStream.Pop();
	readStream.Close();
	CleanupStack::PopAndDestroy(internalizeBuf);
	return profile;
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileITC::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileITC* CSIPProfileITC::NewL(RSIPProfile& aSipProfile)
	{
    CSIPProfileITC* self = CSIPProfileITC::NewLC (aSipProfile);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileITC* CSIPProfileITC::NewLC(RSIPProfile& aSipProfile)
	{
	CSIPProfileITC* self = new (ELeave) CSIPProfileITC(aSipProfile);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::CSIPProfileITC
// -----------------------------------------------------------------------------
//
CSIPProfileITC::CSIPProfileITC(RSIPProfile& aSipProfile) 
 : iHelper (aSipProfile)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::ConstructL()
	{
    iEmptyContent = HBufC8::NewL(0);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::~CSIPProfileITC
// -----------------------------------------------------------------------------
//
CSIPProfileITC::~CSIPProfileITC() 
	{
    delete iEmptyContent;
	iEmptyContent = 0;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::AddSIPProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::AddSIPProfileL(CSIPConcreteProfile& aProfile)
	{
	TSIPProfileSlots ids;
	iHelper.SendL(ids,ESipProfileItcOpAddProfile,aProfile);
	aProfile.SetId(ids.iProfileId);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::UpdateSIPProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::UpdateSIPProfileL(CSIPConcreteProfile& aProfile)
	{
	iHelper.SendL(ESipProfileItcOpUpdateProfile,aProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfileUsageL
// -----------------------------------------------------------------------------
//
TInt CSIPProfileITC::SIPProfileUsageL(TUint32 aProfileId) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;
	iHelper.SendL(ids,ESipProfileItcOpUsage);
	return ids.iSlot1;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::RemoveSIPProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::RemoveSIPProfileL(TUint32 aProfileId) 
	{
    iITCMsgArgs.Set (ESipProfileItcArgProfileId, aProfileId); 
	iHelper.SendL(iITCMsgArgs,ESipProfileItcOpRemoveProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::EnableSIPProfileL
// -----------------------------------------------------------------------------
//
TInt CSIPProfileITC::EnableSIPProfileL(TUint32 aProfileId) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;
	iHelper.SendL(ids,ESipProfileItcOpEnable);
	return ids.iSlot1;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::DisableSIPProfileL
// -----------------------------------------------------------------------------
//
TInt CSIPProfileITC::DisableSIPProfileL(TUint32 aProfileId) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;
	iHelper.SendL(ids,ESipProfileItcOpDisable);
	return ids.iSlot1;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::ForceDisableSIPProfileL
// -----------------------------------------------------------------------------
//
TInt CSIPProfileITC::ForceDisableSIPProfileL(TUint32 aProfileId) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;
	iHelper.SendL(ids,ESipProfileItcOpForceDisable);
	return ids.iSlot1;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileITC::SIPProfileL(TUint32 aProfileId, TUint aSize) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;

	if (aSize == 0) 
		{
		iHelper.SendL(ids,ESipProfileItcOpProfileSize);
		aSize = ids.iSlot1;
		if (aSize == 0) 
			{
			User::Leave(KErrNotFound);
			}
		}

	HBufC8* buf = HBufC8::NewLC(aSize);
	TPtr8 bufPtr = buf->Des();
    iITCMsgArgs.Set(ESipProfileItcArgProfileId, aProfileId);
    iITCMsgArgs.Set(ESipProfileItcArgProfile, &bufPtr);

	iHelper.SendL(iITCMsgArgs,ESipProfileItcOpProfile);

	CSIPConcreteProfile* profile = InternalizeL<CSIPConcreteProfile>(*buf);
	
	CleanupStack::PopAndDestroy(buf);

	profile->SetDefault(EFalse);
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfileRefreshL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileITC::SIPProfileRefreshL(
	TUint32 aProfileId, TUint aSize) 
	{
	__ASSERT_ALWAYS(aProfileId != 0, User::Leave(KErrArgument));
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;

	if (aSize == 0) 
		{
		iHelper.SendL(ids,ESipProfileItcOpProfileSize);
		aSize = ids.iSlot1;
		if (aSize == 0) 
			{
			User::Leave(KErrNotFound);
			}
		}

	HBufC8* buf = HBufC8::NewLC(aSize);
	TPtr8 bufPtr = buf->Des();
    iITCMsgArgs.Set(ESipProfileItcArgProfileId, aProfileId);
	iITCMsgArgs.Set(ESipProfileItcArgProfile, &bufPtr);
	
	iHelper.SendL(iITCMsgArgs,ESipProfileItcOpProfileRefresh);

	CSIPConcreteProfile* profile = InternalizeL<CSIPConcreteProfile>(*buf);
	
	CleanupStack::PopAndDestroy(buf);

	profile->SetDefault(EFalse);
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfileDefaultL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::SIPProfileDefaultL(TUint& aSize, TUint32& aProfileId) 
	{
	TSIPProfileSlots ids;
	iHelper.SendL(ids,ESipProfileItcOpProfileSizeDefault);
	aSize = ids.iSlot1;
	aProfileId = ids.iProfileId;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfilesByAORL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfileHolder* CSIPProfileITC::SIPProfilesByAORL(const TDesC8& aAOR) 
	{
	TSIPProfileSlots ids;

	HBufC8* aor = aAOR.AllocL();
	CleanupStack::PushL(aor);
	TPtr8 aorPtr = aor->Des();

    TPckgBuf<TSIPProfileSlots> sipIdsPckg(ids);
	
	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &aorPtr);
    iITCMsgArgs.Set(ESipProfileItcArgSlots, &sipIdsPckg);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpProfilesSizeByAOR);
	ids = sipIdsPckg();

	TUint size = ids.iSlot1;
	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();

	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &aorPtr);
    iITCMsgArgs.Set(ESipProfileItcArgProfiles, &bufPtr);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpProfilesByAOR);

	CSIPConcreteProfileHolder* holder = 
		InternalizeL<CSIPConcreteProfileHolder>(*buf);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(aor);
	return holder;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfileDefaultIdL
// -----------------------------------------------------------------------------
//
TUint CSIPProfileITC::SIPProfileDefaultIdL()
	{
	TSIPProfileSlots ids;
	iHelper.SendL(ids,ESipProfileItcOpProfileSizeDefault);
	return ids.iProfileId;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::DeleteSIPProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::DeleteSIPProfileL(TUint32 aProfileId) 
	{
	iITCMsgArgs.Set(ESipProfileItcArgProfileId, aProfileId);

	iHelper.SendL(iITCMsgArgs,ESipProfileItcOpDeleteProfile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfilePluginsL
// -----------------------------------------------------------------------------
//
CSIPProfilePlugins* CSIPProfileITC::SIPProfilePluginsL()
	{
	TSIPProfileSlots ids;
	iHelper.SendL(ids,ESipProfileItcOpPluginsSize);
	TUint size = ids.iSlot1;

	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();
	iITCMsgArgs.Set(ESipProfileItcArgPlugins, &bufPtr);
    
	iHelper.SendL(iITCMsgArgs,ESipProfileItcOpPlugins);

	CSIPProfilePlugins* plugins = InternalizeL<CSIPProfilePlugins>(*buf);	
	CleanupStack::PopAndDestroy(buf);

	return plugins;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfilesL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfileHolder* CSIPProfileITC::SIPProfilesL()
	{
	TSIPProfileSlots ids;

    TPckgBuf<TSIPProfileSlots> sipIdsPckg(ids);
	
    iITCMsgArgs.Set(ESipProfileItcArgSlots, &sipIdsPckg);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpProfilesSize);
	ids = sipIdsPckg();

	TUint size = ids.iSlot1;
	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();

    iITCMsgArgs.Set(ESipProfileItcArgProfiles, &bufPtr);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpProfiles);

	CSIPConcreteProfileHolder* holder = 
		InternalizeL<CSIPConcreteProfileHolder>(*buf);
	CleanupStack::PopAndDestroy(buf);
	
	return holder;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPProfilesByTypeL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfileHolder* CSIPProfileITC::SIPProfilesByTypeL(
	const TSIPProfileTypeInfo& aType) 
	{
	TSIPProfileSlots ids;

	HBufC8* type = aType.iSIPProfileName.AllocL();
	CleanupStack::PushL(type);
	TPtr8 typePtr = type->Des();

    TPckgBuf<TSIPProfileSlots> sipIdsPckg(ids);
	
	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &typePtr);
    iITCMsgArgs.Set(ESipProfileItcArgSlots, &sipIdsPckg);
    iITCMsgArgs.Set(ESipProfileItcArgType, aType.iSIPProfileClass);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpProfilesSizeByType);
	ids = sipIdsPckg();

	TUint size = ids.iSlot1;
	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();

	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &typePtr);
    iITCMsgArgs.Set(ESipProfileItcArgProfiles, &bufPtr);
    iITCMsgArgs.Set(ESipProfileItcArgType, aType.iSIPProfileClass);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpProfilesByType);

	CSIPConcreteProfileHolder* holder = 
		InternalizeL<CSIPConcreteProfileHolder>(*buf);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(type);
	return holder;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::SIPCreateProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileITC::SIPCreateProfileL(
	const TSIPProfileTypeInfo& aType) 
	{
	TSIPProfileSlots ids;

	HBufC8* type = aType.iSIPProfileName.AllocL();
	CleanupStack::PushL(type);
	TPtr8 typePtr = type->Des();

    TPckgBuf<TSIPProfileSlots> sipIdsPckg(ids);
	
	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &typePtr);
    iITCMsgArgs.Set(ESipProfileItcArgSlots, &sipIdsPckg);
    iITCMsgArgs.Set(ESipProfileItcArgType, aType.iSIPProfileClass);

	iHelper.SendL(iITCMsgArgs, ESipProfileItcOpCreateProfileSize);
	ids = sipIdsPckg();

	TUint size = ids.iSlot1;

	if (size == 0)
		{
		User::Leave(KErrNotFound);	
		}

	HBufC8* buf = HBufC8::NewLC(size);
	TPtr8 bufPtr = buf->Des();
	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &typePtr);
    iITCMsgArgs.Set(ESipProfileItcArgSlots, &sipIdsPckg);
    iITCMsgArgs.Set(ESipProfileItcArgType, aType.iSIPProfileClass);
    iITCMsgArgs.Set(ESipProfileItcArgProfile, &bufPtr);

	iHelper.SendL(iITCMsgArgs,ESipProfileItcOpCreateProfile);

	CSIPConcreteProfile* profile = InternalizeL<CSIPConcreteProfile>(*buf);
	
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(type);

	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::NegotiatedSecurityMechanismL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::NegotiatedSecurityMechanismL(CSIPConcreteProfile& aProfile)
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfile.Id();

	iHelper.SendL(ids,ESipProfileItcOpNegotiatedSecuritySize);
	TUint size = ids.iSlot1;
	HBufC8* buf = HBufC8::NewLC(size);
	if (size != 0) 
		{
		TPtr8 bufPtr = buf->Des();
		iITCMsgArgs.Set(ESipProfileItcArgProfileId, aProfile.Id());
		iITCMsgArgs.Set(ESipProfileItcArgNegotiated, &bufPtr);
		iHelper.SendL(iITCMsgArgs,ESipProfileItcOpNegotiatedSecurity);
		aProfile.SetNegotiatedSecurityMechanismL(*buf);
		}
	else
		{
		aProfile.SetNegotiatedSecurityMechanismL(*buf);
		}
	CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::RegisteredAORsL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::RegisteredAORsL(CSIPConcreteProfile& aProfile)
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfile.Id();

	iHelper.SendL(ids,ESipProfileItcOpRegisteredAORsSize);
	TUint size = ids.iSlot1;
	if (size != 0)
		{
		HBufC8* buf = HBufC8::NewLC(size);
		TPtr8 bufPtr = buf->Des();
		iITCMsgArgs.Set(ESipProfileItcArgProfileId, aProfile.Id());
		iITCMsgArgs.Set(ESipProfileItcArgAORs, &bufPtr);
		iHelper.SendL(iITCMsgArgs,ESipProfileItcOpRegisteredAORs);

		CBufFlat* internalizeBuf = CBufFlat::NewL(KBufInitSize);
		CleanupStack::PushL(internalizeBuf);
		internalizeBuf->InsertL(0, *buf);

		RBufReadStream readStream(*internalizeBuf,0);
		readStream.PushL();
		aProfile.InternalizeRegisteredAORsL(readStream);
		readStream.Pop();
		readStream.Close();
		CleanupStack::PopAndDestroy(internalizeBuf);
		CleanupStack::PopAndDestroy(buf);
		}
	else
		{
		CDesC8ArrayFlat* tmp = new (ELeave) CDesC8ArrayFlat(1);
		CleanupStack::PushL(tmp);
		aProfile.SetRegisteredAORsL(*tmp);
		CleanupStack::PopAndDestroy(tmp);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileITC::AllowMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::AllowMigrationL(TUint32 aProfileId, TUint32 aIapId) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;
	ids.iSlot1 = aIapId;
	iHelper.SendL(ids,ESipProfileItcOpAllowMigration);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileITC::DisallowMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::DisallowMigrationL(TUint32 aProfileId, TUint32 aIapId) 
	{
	TSIPProfileSlots ids;
	ids.iProfileId = aProfileId;
	ids.iSlot1 = aIapId;
	iHelper.SendL(ids,ESipProfileItcOpDisallowMigration);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileITC::AddALRObserverL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::AddALRObserverL() 
	{
	TSIPProfileSlots ids; // We won't need this at server side.
	iHelper.SendL(ids,ESipProfileItcOpAddALRObserver);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileITC::RemoveALRObserver
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::RemoveALRObserver()
	{
	TSIPProfileSlots ids; // We won't need this at server side.
	iHelper.Send(ids,ESipProfileItcOpRemoveALRObserver);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileITC::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//
void CSIPProfileITC::RefreshIapAvailabilityL(TUint32 aProfileId)
	{
	TSIPProfileSlots ids; 
	ids.iProfileId = aProfileId;
	iHelper.SendL(ids,ESipProfileItcOpRefreshIapAvailability);
	}


