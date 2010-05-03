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
// Name        : sipprofilecssession
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileCSServer.h"
#include "SipProfileCSSession.h"
#include "sipconcreteprofile.h"
#include "sipconcreteprofileholder.h"
#include "sipprofileslots.h"
#include "sipprofileplugins.h"
#include "SipProfileLog.h"
#include "sipprofile.h"
#include "sipmanagedprofile.h"
#include <s32mem.h>

const TUint KBufferSize = 100;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileCSSession* CSIPProfileCSSession::NewL(CSIPProfileCSServer& aServer,
												 CSIPProfileServerCore& aCore)
	{
    CSIPProfileCSSession* self = CSIPProfileCSSession::NewLC(aServer, aCore);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::NewLC
// -----------------------------------------------------------------------------
//
CSIPProfileCSSession* CSIPProfileCSSession::NewLC(CSIPProfileCSServer& aServer,
									 			  CSIPProfileServerCore& aCore)
	{
    CSIPProfileCSSession* self =
    	new (ELeave) CSIPProfileCSSession(aServer, aCore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::CSIPProfileCSSession
// -----------------------------------------------------------------------------
//
 CSIPProfileCSSession::CSIPProfileCSSession(CSIPProfileCSServer& aServer,
							  				CSIPProfileServerCore& aCore) :
	iProfileServer(aServer),
  	iCore(aCore),
  	iClientReadyToReceive(EFalse),
  	iSessionTerminated(EFalse),
  	iALRObserverExists(EFalse)
	{
    iProfileServer.IncrementSessions();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ConstructL()
	{
	iCore.SessionRegisterL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::~CSIPProfileCSSession
// -----------------------------------------------------------------------------
//
CSIPProfileCSSession::~CSIPProfileCSSession()
	{
	iCore.SessionCleanup(*this);
	iEventQueue.Close();
    iProfileServer.DecrementSessions();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::PanicClient
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::PanicClient(TInt aError)
	{
    if (iClientReadyToReceive)
        {
		iEventMessage.Panic(KSipProfileServerName, aError);
		}
	iSessionTerminated = ETrue;	
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ServiceL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ServiceL(const RMessage2& aMessage)
	{
	TInt function = iHelper.ServiceFunction(aMessage);
    __ASSERT_ALWAYS (!(iSessionTerminated &&
		function != ESipProfileItcOpCancelReceive), 
		aMessage.Panic (KSipProfileServerName, EBadSipSessionHandle));
		
#ifndef PLAT_SEC_TEST
	PROFILE_DEBUG3("ProfileCSSession::ServiceL", function)
	
    switch (function)
		{
        case ESipProfileItcOpPluginsSize: ProfilePluginsSizeL(aMessage); break;

        case ESipProfileItcOpPlugins: ProfilePluginsL(aMessage); break;

        case ESipProfileItcOpAddProfile: AddProfileL(aMessage); break;

        case ESipProfileItcOpUpdateProfile: UpdateProfileL(aMessage); break;

        case ESipProfileItcOpRemoveProfile: RemoveProfileL(aMessage); break;

        case ESipProfileItcOpEnable: EnableL(aMessage); break;

        case ESipProfileItcOpDisable: DisableL(aMessage); break;

        case ESipProfileItcOpUsage: UsageL(aMessage); break;

        case ESipProfileItcOpProfileSize: SipProfileSizeL (aMessage); break;

        case ESipProfileItcOpProfileSizeDefault: SipProfileSizeDefaultL (aMessage); break;

        case ESipProfileItcOpProfile: SipProfileL (aMessage); break;

        case ESipProfileItcOpProfileRefresh: SipProfileRefreshL (aMessage); break;

        case ESipProfileItcOpProfilesSize: SipProfilesSizeL (aMessage); break;

        case ESipProfileItcOpProfiles: SipProfilesL (aMessage); break;

        case ESipProfileItcOpProfilesSizeByAOR: SipProfilesSizeByAORL (aMessage); break;

        case ESipProfileItcOpProfilesByAOR: SipProfilesByAORL (aMessage); break;

        case ESipProfileItcOpProfilesSizeByType: SipProfilesSizeByTypeL (aMessage); break;

        case ESipProfileItcOpProfilesByType: SipProfilesByTypeL (aMessage); break;

        case ESipProfileItcOpDeleteProfile: DeleteSipProfileL (aMessage); break;

        case ESipProfileItcOpCancelReceive: CancelClientReceiveL (); break;

        case ESipProfileItcOpCreateProfileSize: CreateProfileSizeL (aMessage); break;

        case ESipProfileItcOpCreateProfile: CreateProfileL (aMessage); break;
 
        case ESipProfileItcOpRegisteredAORs: RegisteredAORsL (aMessage); break;

        case ESipProfileItcOpRegisteredAORsSize: RegisteredAORsSizeL (aMessage); break;

        case ESipProfileItcOpNegotiatedSecurity: NegotiatedSecurityL (aMessage); break;

        case ESipProfileItcOpNegotiatedSecuritySize: NegotiatedSecuritySizeL (aMessage); break;

		case ESipProfileItcOpAllowMigration: AllowMigrationL (aMessage); break;
		
		case ESipProfileItcOpDisallowMigration: DisallowMigrationL (aMessage); break;
		
		case ESipProfileItcOpAddALRObserver: iALRObserverExists = ETrue; break;
		
		case ESipProfileItcOpRemoveALRObserver: iALRObserverExists = EFalse; break;
		
		case ESipProfileItcOpRefreshIapAvailability: RefreshIapAvailabilityL (aMessage); break;

		case ESipProfileItcOpForceDisable: ForceDisableL(aMessage); break;

        case ESipProfileItcOpReadyToReceive: // Asynchronous. Do not complete yet!
            ClientReadyToReceiveL (aMessage); return;


        default: aMessage.Panic (KSipProfileServerName, EBadRequest); break;
		}
#endif	
	iHelper.CompleteService(aMessage, KErrNone);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ProfilePluginsSizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ProfilePluginsSizeL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	const CSIPProfilePlugins& plugins = iCore.ProfilePluginsL();
	ids.iSlot1 = plugins.ExternalizedSize(); 
	iHelper.WriteL(aMessage, ids);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ProfilePluginsL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ProfilePluginsL(const RMessage2& aMessage)
    {
	const CSIPProfilePlugins& plugins = iCore.ProfilePluginsL();
	CBufFlat* buf = ExternalizeLC (plugins);
	TPtr8 externalized = buf->Ptr(0);
	iHelper.WriteL(ESipProfileItcArgPlugins, aMessage, externalized);
	CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::AddProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::AddProfileL(const RMessage2& aMessage)
    {
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	HBufC8* profileBuf = iHelper.ReadLC(ESipProfileItcArgProfile,aMessage);
	CSIPConcreteProfile* profile = InternalizeProfileLC(*profileBuf);
	iCore.AddProfileL(profile, *this);
	CleanupStack::Pop(profile);
	CleanupStack::PopAndDestroy(profileBuf);
	ids.iProfileId = profile->Id(); 
	iHelper.WriteL(aMessage, ids);
	iCore.UpdateRegistrationL(profile->Id(), *this);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::UpdateProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::UpdateProfileL(const RMessage2& aMessage)
    {
	HBufC8* profileBuf = iHelper.ReadLC(ESipProfileItcArgProfile,aMessage);
	CSIPConcreteProfile* profile = InternalizeProfileLC(*profileBuf);
	TBool updateAllowed = iCore.IsUpdateAllowed(profile);
	
	//Save the dynamic Data before UpdateProfile happens
	TBool isProfile = EFalse;
	const CSIPConcreteProfile* aProfile = iCore.Profile(profile->Id());
	CDesC8ArrayFlat* aRegisteredAORs = NULL;
	HBufC8* aNegotiatedSecurityMechanism = NULL;
	CSIPConcreteProfile::TStatus status = CSIPConcreteProfile::EUnregistered;
	if(aProfile)
	    {
        isProfile = ETrue;
        status = aProfile->Status();
        //Copy registeredAors
        const MDesC8Array& aArray = aProfile->RegisteredAORs();
        aRegisteredAORs = new (ELeave) CDesC8ArrayFlat(1);
        CleanupStack::PushL(aRegisteredAORs);
        for (TInt i = 0; i < aArray.MdcaCount(); i++)
            {
            aRegisteredAORs->AppendL(aArray.MdcaPoint(i));
            }    
        //Copy NegotiatedSecurityMechanism
        aNegotiatedSecurityMechanism = aProfile->NegotiatedSecurityMechanism().AllocL();    
        CleanupStack::PushL(aNegotiatedSecurityMechanism);
	    }
	
	if(updateAllowed)
	    {
        TBool canProceed = iCore.UpdateProfileToStoreL(profile, *this);        
        if (canProceed)
            {
            iCore.UpdateRegistrationL(profile->Id(), *this);
            }
	    }
	else
	    {
	    User::Leave(KErrNotSupported);
	    }

    //If UpdateProfile has not resulted into deregistration and reregistration
	//copy the dynamic data extraced from previous copy of concrete profile to new copy
    //of concrete profile so that dynamic information is not lost.
	CSIPConcreteProfile* aUpdatedProfile = iCore.Profile(profile->Id());    
    if(aUpdatedProfile && (status == aUpdatedProfile->Status()))
       {
       if(0 == aUpdatedProfile->RegisteredAORs().MdcaCount() && aRegisteredAORs)           
           aUpdatedProfile->SetRegisteredAORsL(*aRegisteredAORs);
       if(0 == aUpdatedProfile->NegotiatedSecurityMechanism().Length() && aNegotiatedSecurityMechanism)
           aUpdatedProfile->SetNegotiatedSecurityMechanismL(*aNegotiatedSecurityMechanism);
       }
    
    if(isProfile)
        {
        CleanupStack::PopAndDestroy(aNegotiatedSecurityMechanism);
        CleanupStack::PopAndDestroy(aRegisteredAORs);
        }
    
    CleanupStack::Pop(profile);
    CleanupStack::PopAndDestroy(profileBuf);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::RemoveProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::RemoveProfileL(const RMessage2& aMessage)
    { 
	TUint id = iHelper.IPCArgTUintL(ESipProfileItcArgProfileId, aMessage);
	iCore.RemoveProfileL(id);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::EnableL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::EnableL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	ids.iSlot1 = iCore.EnableProfileL(ids.iProfileId, *this);
	iHelper.WriteL(aMessage, ids);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::DisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::DisableL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	ids.iSlot1 = iCore.DisableProfileL(ids.iProfileId, *this);
	iHelper.WriteL(aMessage, ids);	
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ForceDisableL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ForceDisableL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	ids.iSlot1 = iCore.ForceDisableProfileL(ids.iProfileId, *this);
	iHelper.WriteL(aMessage, ids);	
	}

// CSIPProfileCSSession::UsageL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::UsageL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	ids.iSlot1 = iCore.ProfileUsageL(ids.iProfileId);
	iHelper.WriteL(aMessage, ids);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfileSizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfileSizeL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	const CSIPConcreteProfile* profile = iCore.Profile(ids.iProfileId);
	if (profile) 
		{
		CSIPConcreteProfile* clonedProfile = 
			CloneProfileWithoutPasswordsLC(profile);
		ids.iSlot1 = clonedProfile->ExternalizedSizeL(); 
		CleanupStack::PopAndDestroy(clonedProfile);
		
		iHelper.WriteL(aMessage, ids);
		
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfileSizeDefaultL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfileSizeDefaultL(const RMessage2& aMessage)
	{
	const CSIPConcreteProfile* profile = iCore.ProfileDefaultL();
	if (profile)
		{
		TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);

		CSIPConcreteProfile* clonedProfile = 
			CloneProfileWithoutPasswordsLC(profile);
		ids.iProfileId = clonedProfile->Id();
		ids.iSlot1 = clonedProfile->ExternalizedSizeL(); 
		CleanupStack::PopAndDestroy(clonedProfile);

		iHelper.WriteL(aMessage, ids);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfileL(const RMessage2& aMessage)
	{
	TUint id = iHelper.IPCArgTUintL(ESipProfileItcArgProfileId, aMessage);
	CSIPConcreteProfile* profile = 
		CloneProfileWithoutPasswordsLC(&iCore.ProfileL(id, *this));
	WriteProfileL( aMessage, profile ) ;
	CleanupStack::PopAndDestroy (profile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfileRefreshL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfileRefreshL(const RMessage2& aMessage)
	{
	TUint id = iHelper.IPCArgTUintL(ESipProfileItcArgProfileId, aMessage);
	CSIPConcreteProfile* profile = 
		CloneProfileWithoutPasswordsLC( &iCore.ProfileRefreshL(id, *this) );

	WriteProfileL( aMessage, profile );

	CleanupStack::PopAndDestroy (profile);	
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfilesSizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfilesSizeL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	CSIPConcreteProfileHolder* holder = CSIPConcreteProfileHolder::NewLC();
	RPointerArray<CSIPConcreteProfile>& array = holder->SIPProfiles();
	iCore.ProfilesL(NULL, array);
	ids.iSlot1 = holder->ExternalizedSizeL();
	array.Reset();
	CleanupStack::PopAndDestroy(holder);
	iHelper.WriteL(aMessage, ids);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfilesL(const RMessage2& aMessage)
	{
	CSIPConcreteProfileHolder* holder = CSIPConcreteProfileHolder::NewLC();
	RPointerArray<CSIPConcreteProfile>& array = holder->SIPProfiles();
	iCore.ProfilesL(this, array);
	CBufFlat* buf = ExternalizeLC (*holder);
	TPtr8 externalized = buf->Ptr(0);
	iHelper.WriteL(ESipProfileItcArgProfiles, aMessage, externalized);
	array.Reset();
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(holder);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfilesSizeByAORL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfilesSizeByAORL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	HBufC8* aor = iHelper.ReadLC(ESipProfileItcArgNarrator,aMessage);
	CSIPConcreteProfileHolder* holder = CSIPConcreteProfileHolder::NewLC();
	RPointerArray<CSIPConcreteProfile>& array = holder->SIPProfiles();

	iCore.ProfilesByAORL(*aor, NULL, array);

	ids.iSlot1 = holder->ExternalizedSizeL();
	array.Reset();
	CleanupStack::PopAndDestroy(holder);
	CleanupStack::PopAndDestroy(aor);
	iHelper.WriteL(aMessage, ids);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfilesByAORL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfilesByAORL(const RMessage2& aMessage)
	{
	HBufC8* aor = iHelper.ReadLC(ESipProfileItcArgNarrator,aMessage);
	CSIPConcreteProfileHolder* holder = CSIPConcreteProfileHolder::NewLC();
	RPointerArray<CSIPConcreteProfile>& array = holder->SIPProfiles();

	iCore.ProfilesByAORL(*aor, this, array);

	CBufFlat* buf = ExternalizeLC(*holder);
	TPtr8 externalized = buf->Ptr(0);
	iHelper.WriteL(ESipProfileItcArgProfiles, aMessage, externalized);
	array.Reset();
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(holder);
	CleanupStack::PopAndDestroy(aor);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfilesSizeByTypeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfilesSizeByTypeL(const RMessage2& aMessage)
	{
	TSIPProfileTypeInfo info;
	iHelper.ReadSIPProfileTypeInfoL(aMessage,info);
	
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	
	CSIPConcreteProfileHolder* holder = CSIPConcreteProfileHolder::NewLC();
	RPointerArray<CSIPConcreteProfile>& array = holder->SIPProfiles();

	iCore.ProfilesByTypeL(info, NULL, array);

	ids.iSlot1 = holder->ExternalizedSizeL();
	array.Reset();
	CleanupStack::PopAndDestroy(holder);
	iHelper.WriteL(aMessage, ids);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SipProfilesByTypeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SipProfilesByTypeL(const RMessage2& aMessage)
	{	
	TSIPProfileTypeInfo info;
	iHelper.ReadSIPProfileTypeInfoL(aMessage,info);
	
	CSIPConcreteProfileHolder* holder = CSIPConcreteProfileHolder::NewLC();
	RPointerArray<CSIPConcreteProfile>& array = holder->SIPProfiles();

	iCore.ProfilesByTypeL(info, this, array);

	CBufFlat* buf = ExternalizeLC(*holder);
	TPtr8 externalized = buf->Ptr(0);
	iHelper.WriteL(ESipProfileItcArgProfiles, aMessage, externalized);
	array.Reset();
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(holder);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::DeleteSipProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::DeleteSipProfileL(const RMessage2& aMessage)
	{
	TUint id = iHelper.IPCArgTUintL(ESipProfileItcArgProfileId, aMessage);

	iCore.DeleteProfileL(id, *this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::CancelClientReceiveL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::CancelClientReceiveL()
	{
    __ASSERT_ALWAYS (iClientReadyToReceive, User::Leave (KErrNotFound));

    iClientReadyToReceive = EFalse;
    iHelper.CompleteService(iEventMessage, KErrCancel);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::CreateProfileSizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::CreateProfileSizeL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);

	TSIPProfileTypeInfo info;
	iHelper.ReadSIPProfileTypeInfoL(aMessage,info);

	CSIPConcreteProfile* profile = iCore.CreateProfileL(info);
	CleanupStack::PushL(profile);
	ids.iSlot1 = profile->ExternalizedSizeL(); 
	iHelper.WriteL(aMessage, ids);
	CleanupStack::PopAndDestroy(profile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::NegotiatedSecuritySizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::NegotiatedSecuritySizeL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	const CSIPConcreteProfile* profile = iCore.Profile(ids.iProfileId);
	if (profile)
		{
		ids.iSlot1 = profile->NegotiatedSecurityMechanism().Length(); 
		iHelper.WriteL(aMessage, ids);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::NegotiatedSecurityL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::NegotiatedSecurityL(const RMessage2& aMessage)
	{
	TUint id = iHelper.IPCArgTUintL(ESipProfileItcArgProfileId, aMessage);
	const CSIPConcreteProfile* profile = iCore.Profile(id);
	if (profile)
		{
		iHelper.WriteL(ESipProfileItcArgNegotiated, aMessage,
			profile->NegotiatedSecurityMechanism());
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::RegisteredAORsSizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::RegisteredAORsSizeL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	const CSIPConcreteProfile* profile = iCore.Profile(ids.iProfileId);
	if (profile)
		{
		ids.iSlot1 = profile->ExternalizedRegisteredAORsSizeL(); 
		iHelper.WriteL(aMessage, ids);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::RegisteredAORsL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::RegisteredAORsL(const RMessage2& aMessage)
	{
	TUint id = iHelper.IPCArgTUintL(ESipProfileItcArgProfileId, aMessage);
	const CSIPConcreteProfile* profile = iCore.Profile(id);
	if (profile)
		{
		CBufFlat* buf = CBufFlat::NewL(KBufferSize);
		CleanupStack::PushL(buf);
		RBufWriteStream writeStream(*buf, 0);
		writeStream.PushL();
		profile->ExternalizeRegisteredAORsL(writeStream);
		CleanupStack::Pop(1); // writeStream
		writeStream.Close();

		TPtr8 externalized = buf->Ptr(0);
		iHelper.WriteL(ESipProfileItcArgAORs, aMessage, externalized);
		CleanupStack::PopAndDestroy(buf);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::CreateProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::CreateProfileL(const RMessage2& aMessage)
	{
	TSIPProfileTypeInfo info;
	iHelper.ReadSIPProfileTypeInfoL(aMessage,info);

	CSIPConcreteProfile* profile = iCore.CreateProfileL(info);
	CleanupStack::PushL(profile);
	WriteProfileL(aMessage, profile);
	CleanupStack::PopAndDestroy(profile);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::InternalizeProfileLC
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile*
CSIPProfileCSSession::InternalizeProfileLC(const TDesC8& aProfile)
	{
	RDesReadStream readStream(aProfile);
	readStream.PushL();
	CSIPConcreteProfile* profile =
		static_cast<CSIPConcreteProfile*>(
			CSIPConcreteProfile::InternalizeL(readStream));
	readStream.Pop();
	readStream.Close();
	CleanupStack::PushL(profile);
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::RegistrationStatusChangedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::RegistrationStatusChangedL(TUint32 aProfileId, 
													  TInt aStatus,
													  TUint32 aStatusId)
	{
	switch (aStatus)
		{
	case CSIPConcreteProfile::ERegistered:
		SendEvent(aProfileId, ESipProfileItcOpRegistered, aStatusId);
		break;
	case CSIPConcreteProfile::EUnregistered:
		{
			if(ESipProfileItcOpProfileForciblyDisabled == aStatusId)
				SendEvent(aProfileId, ESipProfileItcOpProfileForciblyDisabled, 0);
            else
				SendEvent(aProfileId, ESipProfileItcOpDeregistered, 0);
		}
		break;
	default:
		break;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::AddedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::AddedL(TUint32 aProfileId)
	{
	SendEventL(aProfileId, ESipProfileItcOpProfileAdded);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::RemovedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::RemovedL(TUint32 aProfileId)
	{
	SendEvent(aProfileId, ESipProfileItcOpProfileRemoved);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::UpdatedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::UpdatedL(TUint32 aProfileId, TUint aSize)
	{
	SendEventL(aProfileId, ESipProfileItcOpProfileUpdated, aSize);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ErrorOccurredL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ErrorOccurredL(TUint32 aProfileId,
										  TInt aStatus,
										  TInt aError)
	{
	TInt event(ESipProfileItcOpUnregistrationError);
	if (aStatus == CSIPConcreteProfile::ERegistrationInProgress)
		{
		event = ESipProfileItcOpRegistrationError;
		}

    SendEventL(aProfileId, event, aError);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::IapAvailableL
// -----------------------------------------------------------------------------
//
TBool CSIPProfileCSSession::IapAvailableL(TUint32 aProfileId,
										  TUint32 aSnapId,
										  TUint32 aIapId)
	{
	if (iALRObserverExists)
		{
		SendEventL(aProfileId, ESipProfileItcOpIapAvailable, aSnapId, aIapId);
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::MigrationStartedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::MigrationStartedL(TUint32 aProfileId,
										   	 TUint32 aSnapId,
										   	 TUint32 aIapId)
	{
	if (iALRObserverExists)
		{
		SendEventL(aProfileId,
				   ESipProfileItcOpMigrationStarted,
				   aSnapId,
				   aIapId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::MigrationCompletedL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::MigrationCompletedL(TUint32 aProfileId,
										   	   TUint32 aSnapId,
										   	   TUint32 aIapId)
	{
	if (iALRObserverExists)
		{
		SendEventL(aProfileId,
				   ESipProfileItcOpMigrationCompleted,
				   aSnapId,
				   aIapId);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::AlrError
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCSSession::AlrError(TInt aError,
									TUint32 aProfileId,
									TUint32 aSnapId,
									TUint32 aIapId)
	{
	TInt err(KErrNone);
	if (iALRObserverExists)
		{
		err = SendEvent(aProfileId,
				        ESipProfileItcOpALRError,
				        aSnapId,
				        aIapId,
				        aError);
		}
    return err;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SendEvent
// -----------------------------------------------------------------------------
//
TInt CSIPProfileCSSession::SendEvent(TUint aProfileId,
									 TUint aEventId,
									 TInt aSlot,
									 TInt aSlot2,
									 TInt aError)
	{
	TRAPD(err, SendEventL(aProfileId, aEventId, aSlot, aSlot2, aError));
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SendEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SendEventL(TUint aProfileId,
									  TUint aEventId,
									  TInt aSlot,
									  TInt aSlot2,
									  TInt aError)
	{
    if (iClientReadyToReceive)
        {
		TSIPProfileSlots ids = iHelper.ReadSipIdsL(iEventMessage);
		SetProfileSlots(ids, aProfileId, aEventId, aSlot, aSlot2, aError);

		iHelper.WriteL(iEventMessage, ids);
		iHelper.CompleteService(iEventMessage, KErrNone);
        iClientReadyToReceive = EFalse;
		}
	else
		{
		TSIPProfileSlots ids;
		SetProfileSlots(ids, aProfileId, aEventId, aSlot, aSlot2, aError);

		iEventQueue.AppendL(ids);
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ClientReadyToReceiveL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::ClientReadyToReceiveL(const RMessage2& aMessage)
	{
    __ASSERT_ALWAYS(!iClientReadyToReceive, User::Leave(KErrNotFound));

    iEventMessage = aMessage;
    if (iEventQueue.Count() == 0)
		{
        iClientReadyToReceive = ETrue;
		}
    else
		{
		const TSIPProfileSlots& stored = iEventQueue[0];
		TSIPProfileSlots ids = iHelper.ReadSipIdsL(iEventMessage);
		ids.iProfileId = stored.iProfileId; 
		ids.iEventId = stored.iEventId; 
		ids.iSlot1 = stored.iSlot1; 
		iHelper.WriteL(iEventMessage, ids);
		iHelper.CompleteService(iEventMessage, KErrNone);
		iEventQueue.Remove(0);
        iClientReadyToReceive = EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::WriteProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::WriteProfileL(const RMessage2& aMessage, 
	CSIPConcreteProfile* aProfile)
	{
	CBufFlat* buf = ExternalizeLC (*aProfile);
	TPtr8 externalized = buf->Ptr(0);
	iHelper.WriteL(ESipProfileItcArgProfile, aMessage, externalized);
	CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::ExternalizeLC
// -----------------------------------------------------------------------------
//
template<class T> CBufFlat*
CSIPProfileCSSession::ExternalizeLC(const T& aElements)
	{
	CBufFlat* buf = CBufFlat::NewL(KBufferSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf, 0);
	writeStream.PushL();
	aElements.ExternalizeL(writeStream);
	CleanupStack::Pop(1); // writeStream
	writeStream.Close();
    return buf;
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileCSSession::AllowMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::AllowMigrationL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	iCore.AllowMigrationL(ids.iProfileId, ids.iSlot1, *this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::DisallowMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::DisallowMigrationL(const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	iCore.DisallowMigrationL(ids.iProfileId, ids.iSlot1, *this);		
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileCSSession::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::RefreshIapAvailabilityL (const RMessage2& aMessage)
	{
	TSIPProfileSlots ids = iHelper.ReadSipIdsL(aMessage);
	iCore.RefreshIapAvailabilityL(ids.iProfileId);		
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::SetProfileSlots
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSession::SetProfileSlots(TSIPProfileSlots& aSlots,
										   TUint aProfileId,
										   TUint aEventId,
										   TInt aSlot,
										   TInt aSlot2,
										   TInt aError) const
	{
	aSlots.iProfileId = aProfileId; 
	aSlots.iEventId = aEventId; 
	aSlots.iSlot1 = aSlot;
	aSlots.iSlot2 = aSlot2;
	aSlots.iError = aError;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSession::CloneProfileWithoutPasswordsLC
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileCSSession::CloneProfileWithoutPasswordsLC(
		const CSIPConcreteProfile* aProfile)
	{
	CSIPConcreteProfile* clonedProfile = 
		aProfile->CloneWithDynamicValuesL();

	// Remove password from cloned profile
	CleanupStack::PushL(clonedProfile);
	clonedProfile->SetServerParameterL(
		KSIPOutboundProxy, KSIPDigestPassword, KNullDesC8 );
	clonedProfile->SetServerParameterL( 
		KSIPRegistrar, KSIPDigestPassword, KNullDesC8 );	

	return clonedProfile;
	}
