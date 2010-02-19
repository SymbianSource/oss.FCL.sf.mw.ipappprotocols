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
// Name        : sipprofileservercore.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileServerCore.h"
#include "SipProfileCSServer.h"
#include "sipextendedconcreteprofileobserver.h"
#include "SIPProfileStorage.h"
#include "SipProfileStorageSecureBackup.h"
#include "SipProfileCacheItem.h"
#include "SipProfileCacheCleanupItem.h"
#include "sipconcreteprofile.h"
#include "sipprofileplugins.h"
#include "sipplugindirector.h"
#include "SipProfileStateUnregistered.h"
#include "SipProfileStateRegistered.h"
#include "SipProfileStateRegInProg.h"
#include "SipProfileStateUnregInProg.h"
#include "sipprofilestatewaitforiap.h"
#include "sipprofilestatewaitforpermission.h"
#include "sipprofilestatemigratingtonewiap.h"
#include "sipprofilestateunregisteringoldiap.h"
#include "SipProfileLog.h"
#include "sipalrhandler.h"
#include "sipalrmigrationcontroller.h"
#include "sipmanagedprofile.h"
#include "sipprofile.h"
#include "sipprofilecs.h"
#include "SIPProfileStorageIndex.h"
#include <siperr.h>
#include <sipsystemstatemonitor.h>
#include <random.h>

#include <CommsDatTypesV1_1.h>
#include <MetaDatabase.h> 
#include <commsdattypeinfov1_1_internal.h>
using namespace CommsDat;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileServerCore* CSIPProfileServerCore::NewL()
    {
    CSIPProfileServerCore* self = CSIPProfileServerCore::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileServerCore* CSIPProfileServerCore::NewLC()
    {
    CSIPProfileServerCore* self = new(ELeave)CSIPProfileServerCore();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::CSIPProfileServerCore
// -----------------------------------------------------------------------------
//
CSIPProfileServerCore::CSIPProfileServerCore() :
	iBackupInProgress(EFalse)
#ifdef CPPUNIT_TEST
    // Set the array granularity to 1, so they allocate memory for every append    
    , iProfileCache(1),
    iObservers(1),
    iMigrationControllers(1)
#endif
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());

    iFindEntry = CSIPProfileCacheItem::NewL(*this, iUnregistered);

    iPluginDirector = CSIPPluginDirector::NewL(*this);

    iUnregistered = CSIPProfileStateUnregistered::NewL(*iPluginDirector);
    iRegistered = CSIPProfileStateRegistered::NewL(*iPluginDirector);
    iUnregInProg = CSIPProfileStateUnregInProg::NewL(*iPluginDirector);
    iRegInProg = CSIPProfileStateRegInProg::NewL(*iPluginDirector);
    iWaitForIAP = CSIPProfileStateWaitForIAP::NewL(*iPluginDirector);
    iWaitForPermission =
    	CSIPProfileStateWaitForPermission::NewL(*iPluginDirector);
    iMigratingToNewIAP =
    	CSIPProfileStateMigratingToNewIAP::NewL(*iPluginDirector);
    iUnregisteringOldIAP =
    	CSIPProfileStateUnregisteringOldIAP::NewL(*iPluginDirector);

    iUnregistered->SetNeighbourStates(*iRegInProg, *iWaitForIAP);
    iRegistered->SetNeighbourStates(*iUnregInProg,
    								*iUnregistered,
    								*iRegInProg,
    								*iWaitForPermission);
    iUnregInProg->SetNeighbourStates(*iUnregistered, *iRegistered);
    iRegInProg->SetNeighbourStates(*iRegistered,
    							   *iUnregistered,
    							   *iUnregInProg,
    							   *iWaitForIAP,
    							   *iWaitForPermission);
    iWaitForIAP->SetNeighbourStates(*iWaitForPermission,
    								*iRegInProg,
    								*iUnregInProg,
    								*iUnregistered);
    iWaitForPermission->SetNeighbourStates(*iWaitForIAP,
    	*iMigratingToNewIAP,
    	*iRegInProg,
    	*iRegistered,
    	*iUnregInProg,
    	*iUnregistered);
    iMigratingToNewIAP->SetNeighbourStates(*iUnregisteringOldIAP,
    									   *iWaitForIAP,
    									   *iRegistered,
    									   *iUnregInProg,
    									   *iUnregistered);
	iUnregisteringOldIAP->SetNeighbourStates(*iRegistered, *iUnregInProg);
	
	LoadSystemStateMonitorL();
	
	iAlrHandler = CSipAlrHandler::NewL(*this,iSystemStateMonitor);

    LoadProfilesL(EFalse);

    iNotify = CSIPProfileStorageSecureBackup::NewL(this);
    
    iServer = CSIPProfileCSServer::NewL(*this);
	
	iOfflineEventReceived = EFalse;
    
    PROFILE_DEBUG1("ProfileServer started")
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::~CSIPProfileServerCore
// -----------------------------------------------------------------------------
//
CSIPProfileServerCore::~CSIPProfileServerCore()
    {
    // If the monitor plug-in is deleted later in destructor, leaks memory.
    if (iSystemStateMonitor)
    	{
    	iSystemStateMonitor->StopMonitoring( 
    	    CSipSystemStateMonitor::ESystemState, 0, *this);
    	delete iSystemStateMonitor;
    	}
    
    delete iServer;    
    
    iObservers.Reset();

	// Profile cache entries use migration controllers, so delete entries first
    iProfileCache.ResetAndDestroy();

	// Migration controllers use ALR handler, so delete controllers first
	iMigrationControllers.ResetAndDestroy();
	delete iAlrHandler;

    delete iProfileStorage;
    delete iPluginDirector;
    delete iProfilePlugins;
    delete iFindEntry;

    delete iUnregistered;
    delete iRegistered;
    delete iUnregInProg;
    delete iRegInProg;
    delete iWaitForIAP;
    delete iWaitForPermission;
    delete iMigratingToNewIAP;
    delete iUnregisteringOldIAP;

    delete iNotify;

    iFs.Close();

    PROFILE_DEBUG1("ProfileServer stopped")
    
#ifdef _BullseyeCoverage
    // Store coverage measurement
    cov_write();
#endif
    }
    
// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SIPProfileStatusEvent
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SIPProfileStatusEvent(CSIPConcreteProfile& aProfile,
												  TUint32 aContextId)
    {
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfile.Id());
    TRAPD(err, SIPProfileStatusEventL(aProfile.Id(), aContextId));

    if (err != KErrNone)
        {
		if (item)
        	{
        	HandleAsyncError(*item,
        					 CSIPConcreteProfile::ERegistrationInProgress,
        					 err);
        	}
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SIPProfileStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SIPProfileStatusEventL(TUint32 aProfileId,
    											   TUint32 aContextId)
    {
    PROFILE_DEBUG4("ProfileServerCore::SIPProfileStatusEventL id,ctxId",
    			   aProfileId, aContextId)

    CSIPProfileCacheItem* item = ProfileCacheItem(aProfileId);
    if (item)
    	{
		// In MigratingToNewIAP state, only profile in use is new IAP's register
    	CSIPConcreteProfile::TStatus status;
        iPluginDirector->State(status, item->UsedProfile());

        item->RegistrationStatusEventL(status, aContextId);

        if (!item->IsActiveState() )
        	{
        	if (status == CSIPConcreteProfile::ERegistered)
        		item->RemoveL();
        	else
        	if (item->CanBePermanentlyRemoved())
        		{
        		RemoveProfileItem(aProfileId);
            	delete item;
				item = NULL;
            	PROFILE_DEBUG3(
            	"ProfileServerCore::SIPProfileStatusEventL(removed) id",
                	aProfileId)
            	}
        		
        	}
        if (item && item->IsRfsInprogress())
            {
            CSIPConcreteProfile::TStatus status;
            TInt count = iProfileCache.Count();
            for (TInt i = 0; i < iProfileCache.Count(); i++)
                {
                iPluginDirector->State(status, iProfileCache[i]->UsedProfile());
                if (status == CSIPConcreteProfile::EUnregistered)
                    count--;
                }
            if (!count)
                {
                iSystemStateMonitor->EventProcessingCompleted(
								CSipSystemStateMonitor::ERfsState, 0, *this);
                }
            }
        }
		
		if (iOfflineEventReceived)
        {
        CSIPConcreteProfile::TStatus status;
        TInt count = iProfileCache.Count();
        for (TInt i = 0; i < iProfileCache.Count(); i++)
            {
            iPluginDirector->State(status, iProfileCache[i]->UsedProfile());
            if (status == CSIPConcreteProfile::EUnregistered)
                count--;
            }
        if (!count)
            {
            iSystemStateMonitor->EventProcessingCompleted(CSipSystemStateMonitor::ESystemState, 0, *this);
            }
        }
		
    CheckServerStatus();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SIPProfileErrorEvent
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SIPProfileErrorEvent(CSIPConcreteProfile& aProfile,
    											 TInt aError)
    {
    PROFILE_DEBUG4("ProfileServerCore::SIPProfileErrorEvent id,err", 
                   aProfile.Id(), aError)

    CSIPProfileCacheItem* item = ProfileCacheItem(aProfile.Id());
    if (item)
    	{
    	CSIPConcreteProfile::TStatus status = ValidateErrorStatus(*item);
        HandleAsyncError(*item, status, aError);
        iPluginDirector->StartTimerForCleanup();
    	}

    CheckServerStatus();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProceedRegistration
// -----------------------------------------------------------------------------
//
TBool CSIPProfileServerCore::ProceedRegistration
		(CSIPConcreteProfile& aProfile, TInt aError)
    {
    PROFILE_DEBUG4("ProfileServerCore::ProceedRegistration id, err", aProfile.Id(), aError)
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfile.Id());
    if (!item)
    	{
    	return EFalse;
    	}

    if ( ShouldChangeIap(item->UsedProfile(), aError) && 
    	 !item->SnapRetryCountReached() )
        {
        PROFILE_DEBUG1("CSIPProfileServerCore::ProceedRegistration, IAP should be changed")
        // If IAP should be changed, handle this like an error, so the profile
        // can ask for a new IAP.
        SIPProfileErrorEvent(item->UsedProfile(), aError);
        
        // Return false so that the profile agent does not retry registration
        // with old IAP.
        return EFalse;
        }
	PROFILE_DEBUG1("CSIPProfileServerCore::ProceedRegistration, IAP should NOT be changed")        
        
    if (!item->IsActiveState())
        {
        PROFILE_DEBUG1("ProceedRegistration, profile removed")
        return EFalse;
        }
    if (item->ReferenceCount() == 0)
    	{
    	if (!item->LatestProfile().IsAutoRegistrationEnabled())
            {
            PROFILE_DEBUG1("ProceedRegistration, profile not used")
            return EFalse;
            }
    	if (item->HasProfileUpdate())
            {
            PROFILE_DEBUG1("ProceedRegistration, profile updated, not used")
            return EFalse;
            }
    	}

    PROFILE_DEBUG1("ProceedRegistration, profile untouched")
    return ETrue;    
    }    

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::GetFailedProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::GetFailedProfilesL(const TSIPProfileTypeInfo& aType,
    RPointerArray<CSIPConcreteProfile>& aFailedProfiles) const
    {
    for (TInt i = 0; i < iProfileCache.Count(); i++)
        {
        const CSIPConcreteProfile& profile = iProfileCache[i]->Profile();

        if (profile.ProfileType().iSIPProfileClass == aType.iSIPProfileClass &&
        	profile.ProfileType().iSIPProfileName  == aType.iSIPProfileName &&
            (profile.IsEnabled() || profile.IsAutoRegistrationEnabled()) &&
             profile.Status() == CSIPConcreteProfile::EUnregistered &&
             profile.LastRegistrationError() != KErrNone)
        	{
        	aFailedProfiles.AppendL(&profile);
        	}
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::HandleProfileError
// -----------------------------------------------------------------------------
//
TBool CSIPProfileServerCore::HandleProfileError(CSIPProfileCacheItem& aItem,
											    TInt aOwnError)
	{
    iPluginDirector->TerminateHandling(aItem.UsedProfile());
    return aItem.HandleError(aOwnError);
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SystemVariableUpdated
// If a system shutdown event is received, initiate de-registering each profile
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SystemVariableUpdated( 
    CSipSystemStateMonitor::TSystemVariable aVariable,
    TInt /*aObjectId*/,
    TInt aValue )
    {
    PROFILE_DEBUG3("CSIPProfileServerCore::SystemVariableUpdated System State changed to value", aValue)
    
    //If the SystemState is Offline, turn the boolean variable to true so that Profile-Server
    //can indicate System State monitor that event processing is completed, provided all the 
    //profiles have got deregistered
    if (CSipSystemStateMonitor::ESystemOffline == aValue)
        iOfflineEventReceived = ETrue;
    
	if ( aVariable == CSipSystemStateMonitor::ESystemState &&
	     (aValue == CSipSystemStateMonitor::ESystemShuttingDown || 
	     aValue == CSipSystemStateMonitor::ESystemOffline
			 ))
	    {    
        for (TInt i = 0; i < iProfileCache.Count(); i++)
            {
            iProfileCache[i]->ShutdownInitiated();
            }	    
	    }
	//If the System State is Online, register all the profiles in always on mode
	else if(aVariable == CSipSystemStateMonitor::ESystemState && 
		aValue == CSipSystemStateMonitor::ESystemOnline)
		{
		iOfflineEventReceived = EFalse;
		for (TInt i = 0; i < iProfileCache.Count(); i++)
		    {
		    iProfileCache[i]->ResetShutdownvariable();
		    CSIPProfileCacheItem* item = iProfileCache[i];
		    if (item->Profile().IsAutoRegistrationEnabled())
		        {
                TRAPD(err, item->StartRegisterL(*iWaitForIAP, *iRegInProg, ETrue));
		        if (err != KErrNone)
		            {
		            HandleAsyncError(*item,CSIPConcreteProfile::ERegistrationInProgress,err);
		            }
				}
			}
		}
	else if(aVariable == CSipSystemStateMonitor::ERfsState)
	    {
	    if(aValue == CSipSystemStateMonitor::ERfsStarted)
	        {
	        PROFILE_DEBUG1("RFS Started, de-registering the profiles")
	        for (TInt i = 0; i < iProfileCache.Count(); i++)
	            {
	            iProfileCache[i]->RfsInprogress(ETrue);
	            }
	        }
	    else if(aValue == CSipSystemStateMonitor::ERfsFailed)
	        {
	        PROFILE_DEBUG1("RFS Failed, re-registering the profiles")
	        for (TInt i = 0; i < iProfileCache.Count(); i++)
	            {
	            iProfileCache[i]->RfsInprogress(EFalse);
	            if (iProfileCache[i]->IsReferred())
	                {
	                TRAPD(err, iProfileCache[i]->StartRegisterL(*iWaitForIAP, *iRegInProg, ETrue));
	                if (err != KErrNone)
	                    {
	                    HandleAsyncError(*iProfileCache[i],
	                            CSIPConcreteProfile::ERegistrationInProgress,
	                            err);	                            
	                    }
	                }
	            }
	        }
	    else if(aValue == CSipSystemStateMonitor::ERfsCompleted)
	        {
	        PROFILE_DEBUG1("RFS Completed")
	        for (TInt i = 0; i < iProfileCache.Count(); i++)
	            {
	            iProfileCache[i]->RfsInprogress(EFalse);
	            }
	        }
	    }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SessionRegisterL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SessionRegisterL(
	const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    iObservers.AppendL(&aObserver);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SessionCleanup
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SessionCleanup(	
	const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    TRAP_IGNORE(SessionCleanupL(aObserver))
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfilePluginsL
// -----------------------------------------------------------------------------
//
const CSIPProfilePlugins& CSIPProfileServerCore::ProfilePluginsL()
    {
    delete iProfilePlugins;
    iProfilePlugins = NULL;
    iProfilePlugins = CSIPProfilePlugins::NewL();
    RArray<TSIPProfileTypeInfo> array;
    CleanupClosePushL(array);

    RImplInfoPtrArray infoArray;
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyInfo, &infoArray));

    TRAPD(err, iPluginDirector->ListImplementationsL(infoArray));
    if (err == KErrNone) 
        {
        for (TInt i = 0; i < infoArray.Count(); i ++)
            {
            CImplementationInformation* ecomInfo = infoArray[i];
            TSIPProfileTypeInfo info;
            ConvertTypeInfoL(*ecomInfo, info);
            array.AppendL(info);
            }
        }
    CleanupStack::Pop(&infoArray);
    infoArray.ResetAndDestroy();
    infoArray.Close();

    iProfilePlugins->SetPluginsL(array);
    CleanupStack::Pop(&array);
    array.Close();
    return *iProfilePlugins;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::AddProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::AddProfileL(CSIPConcreteProfile* aProfile,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    __ASSERT_ALWAYS(iProfileStorage != NULL, User::Leave(KErrLocked));

    CSIPProfileCacheCleanupItem* cleanup =
    	new (ELeave) CSIPProfileCacheCleanupItem;
	CleanupStack::PushL(cleanup);
    CleanupStack::PushL(TCleanupItem(CrashRevert, cleanup));

    CSIPProfileCacheItem* item = CSIPProfileCacheItem::NewL(*this,
    														iUnregistered);
    cleanup->iCacheItem = item;

    item->AddObserverL(aObserver);
    iProfileCache.AppendL(item);
    cleanup->iCache = &iProfileCache;

	// AddSIPProfileL sets stream id to aProfile's profile id
    iProfileStorage->AddSIPProfileL(*aProfile);

    CleanupStack::Pop(); // TCleanupItem
    CleanupStack::PopAndDestroy(cleanup);
	
	CSIPConcreteProfile* defaultProfile = FindDefaultProfile();

	if ( !defaultProfile )
		{
		aProfile->SetDefault( ETrue );
		}

	if ( defaultProfile && aProfile->IsDefault() && defaultProfile != aProfile )
		{
		defaultProfile->SetDefault( EFalse );
		}

    item->SetProfile(aProfile);

    SendProfileAddedEvent(*aProfile);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::RemoveProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::RemoveProfileL(TUint32 aProfileId)

    {
    __ASSERT_ALWAYS(iProfileStorage != NULL, User::Leave(KErrLocked));

    CSIPProfileCacheItem* item = ProfileCacheItemL(aProfileId);
    __ASSERT_ALWAYS(item->ReferenceCount() == 0, User::Leave(KErrInUse));

    iProfileStorage->DeleteSIPProfileL(item->Profile().StorageId());

	CSIPConcreteProfile* defaultProfile = FindDefaultProfile();
	
	if ( defaultProfile && defaultProfile->Id() == aProfileId )
        {
        TUint32 storageId = iProfileStorage->DefaultProfile();

        if ( storageId )
            {
            CSIPConcreteProfile* newDefault = &( ProfileCacheItemByStorageIdL( storageId ).Profile() );
            newDefault->SetDefault( ETrue );
            }
        }

    SendProfileRemovedEventL(*item);

    item->RemoveL();

    if (item->CanBePermanentlyRemoved())
        {
        RemoveProfileItem(aProfileId);
        delete item;
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::UpdateProfileToStoreL
// aProfile has the correct profile id.
// -----------------------------------------------------------------------------
//
TBool CSIPProfileServerCore::UpdateProfileToStoreL(
    CSIPConcreteProfile *aProfile,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    __ASSERT_ALWAYS(iProfileStorage != NULL, User::Leave(KErrLocked));

    CSIPProfileCacheItem* item = ProfileCacheItemL(aProfile->Id());    

	if (aProfile->ServerParameter(
			KSIPOutboundProxy, KSIPDigestPassword) == KNullDesC8)
		{
		const TDesC8& oldPassword( 
				item->Profile().ServerParameter(
						KSIPOutboundProxy, KSIPDigestPassword) );

		aProfile->SetServerParameterL( 
			KSIPOutboundProxy, KSIPDigestPassword, oldPassword);	
		}

	if (aProfile->ServerParameter(
			KSIPRegistrar, KSIPDigestPassword) == KNullDesC8)
		{
		const TDesC8& oldPassword( 
				item->Profile().ServerParameter(
						KSIPRegistrar, KSIPDigestPassword) );

		aProfile->SetServerParameterL( 
			KSIPRegistrar, KSIPDigestPassword, oldPassword);	
		}

	aProfile->SetStorageId(item->Profile().StorageId());
    iProfileStorage->UpdateSIPProfileL(*aProfile);

	if ( aProfile->IsDefault() )
        {
        CSIPConcreteProfile* defaultProfile = FindDefaultProfile();
        
        if ( defaultProfile && defaultProfile != aProfile )
        	{
        	defaultProfile->SetDefault( EFalse );
        	}
        }
	
    SendProfileUpdatedEventL(*item, *aProfile);
    
    return item->CacheOldProfile(aProfile, aObserver);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::Profile
// -----------------------------------------------------------------------------
//
const CSIPConcreteProfile*
CSIPProfileServerCore::Profile(TUint32 aProfileId) const
    {
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfileId);
    if (item)
        {
        return &item->Profile();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileDefaultL
// -----------------------------------------------------------------------------
//
const CSIPConcreteProfile* CSIPProfileServerCore::ProfileDefaultL()
    {
    __ASSERT_ALWAYS(iProfileStorage != NULL, User::Leave(KErrLocked));

	CSIPProfileCacheItem* item =
		ProfileCacheItemByStorageId(iProfileStorage->DefaultProfile());
	if (item)
        {
        return &item->Profile();
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileServerCore::ProfileL(TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& aObserver) const
    {
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfileId);
	__ASSERT_ALWAYS(item != NULL && item->IsActiveState(),
					User::Leave(KErrNotFound));
    __ASSERT_ALWAYS(!item->IsObserverOrUser(aObserver), User::Leave(KErrInUse));

    item->AddObserverL(aObserver);
	return item->LatestProfile();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileRefreshL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPProfileServerCore::ProfileRefreshL(TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& aObserver) const
    {
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfileId);
	__ASSERT_ALWAYS(item != NULL && item->IsActiveState(),
					User::Leave(KErrNotFound));
    __ASSERT_ALWAYS(item->IsObserverOrUser(aObserver),
    				User::Leave(KErrArgument));
	return item->LatestProfile();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::CreateProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile*
CSIPProfileServerCore::CreateProfileL(const TSIPProfileTypeInfo& aType)
    {
    PROFILE_DEBUG1("CreateProfileL")

    return iPluginDirector->CreateProfileL(aType);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfilesByAORL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ProfilesByAORL(const TDesC8& aAOR,
	const MSIPExtendedConcreteProfileObserver* aObserver,
	RPointerArray<CSIPConcreteProfile>& aArray) const
    {
    for (TInt i = 0; i < iProfileCache.Count(); i++)
        {
        if (iProfileCache[i]->HasAorL(aAOR) &&
        	iProfileCache[i]->IsActiveState())
            {
            HandleMatchingProfileL(aObserver, aArray, *iProfileCache[i]);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::DeleteProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::DeleteProfileL(
    TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfileId);
	if (item)
		{
		if (item->IsUser(aObserver))
            {
            DisableProfileL(aProfileId, aObserver);
            }
        item->RemoveObserver(aObserver);
		}
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::CanServerStop
// -----------------------------------------------------------------------------
//
TBool CSIPProfileServerCore::CanServerStop() const
    {
    if (iBackupInProgress || iObservers.Count() > 0)
        {
        return EFalse;
        }

    for (TInt i = 0; i < iProfileCache.Count(); i++)
        {
        const CSIPConcreteProfile& profile = iProfileCache[i]->Profile();
        if ((profile.IsAutoRegistrationEnabled() &&
             profile.LastRegistrationError() == KErrNone)
             ||
             profile.Status() != CSIPConcreteProfile::EUnregistered)
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::BackupInProgress
// -----------------------------------------------------------------------------
//
TBool CSIPProfileServerCore::BackupInProgress() const
    {
    return iBackupInProgress;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ProfilesL(
	const MSIPExtendedConcreteProfileObserver* aObserver,
	RPointerArray<CSIPConcreteProfile>& aArray) const
    {
    for (TInt i = 0; i < iProfileCache.Count(); i++)
        {
		HandleMatchingProfileL(aObserver, aArray, *iProfileCache[i]);
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfilesByTypeL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ProfilesByTypeL(
	const TSIPProfileTypeInfo& aType,                                   
	const MSIPExtendedConcreteProfileObserver* aObserver,
	RPointerArray<CSIPConcreteProfile>& aArray) const
    {
    for (TInt i = 0; i < iProfileCache.Count(); i++)
        {
		const TSIPProfileTypeInfo& type =
        	iProfileCache[i]->LatestProfile().ProfileType();

        if (aType.iSIPProfileName.Compare(type.iSIPProfileName) == 0 &&
            aType.iSIPProfileClass == type.iSIPProfileClass &&
            iProfileCache[i]->IsActiveState())
            {
			HandleMatchingProfileL(aObserver, aArray, *iProfileCache[i]);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::HandleMatchingProfileL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::HandleMatchingProfileL(
	const MSIPExtendedConcreteProfileObserver* aObserver,
	RPointerArray<CSIPConcreteProfile>& aArray,
	CSIPProfileCacheItem& aItem) const
	{
    // Append only if the profile state is Active i.e if it not 
    // requested for the removal
    if (aItem.IsActiveState())
    	{
    	aArray.AppendL(&(aItem.LatestProfile()));        	
    	}    

    if (aObserver && !aItem.IsObserverOrUser(*aObserver))
        {
        aItem.AddObserverL(*aObserver);
        }
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::UpdateRegistrationL
// Second phase of profile update.
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::UpdateRegistrationL(TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    CSIPProfileCacheItem* item = ProfileCacheItemL(aProfileId);
    CSIPConcreteProfile::TStatus
    	status(CSIPConcreteProfile::ERegistrationInProgress);
    if (item->Profile().Status() == CSIPConcreteProfile::ERegistered)
        {
        status = CSIPConcreteProfile::EUnregistrationInProgress;
        }

    TRAPD(err, item->UpdateRegistrationL(aObserver));
    if (err != KErrNone)
        {
        HandleAsyncError(*item, status, err);
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::EnableProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileServerCore::EnableProfileL(
    TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
	CSIPProfileCacheItem* item = ProfileCacheItemL(aProfileId);
    iAlrHandler->EnableProfileL(*item, aObserver);
    return item->Profile().Status();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::DisableProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileServerCore::DisableProfileL(
    TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    CSIPProfileCacheItem* item = ProfileCacheItemL(aProfileId);
    iAlrHandler->DisableProfileL(*item, aObserver);
    return item->Profile().Status();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ForceDisableProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus CSIPProfileServerCore::ForceDisableProfileL(
    TUint32 aProfileId,
    const MSIPExtendedConcreteProfileObserver& /* aObserver */)
    {
    CSIPProfileCacheItem* item = ProfileCacheItemL(aProfileId);
    
    //When profile state is not unregistered, 
    //perform cleanup and send event notification
    
    if(CSIPConcreteProfile::EUnregistered != item->UsedProfile().Status())
    	{
    	PROFILE_DEBUG3("CSIPProfileServerCore::ForceDisableProfileL, Forcibly disabling profile", aProfileId);
    	item->SwitchToUnregisteredState();
    	iPluginDirector->TerminateHandling(item->UsedProfile());
    	item->ClearMigrationProfiles();
    	iPluginDirector->StartTimerForCleanup();
    	item->ClearOldProfile();
    	SendProfileForciblyDisabledEvent(*item);
    	}
    return item->Profile().Status();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileUsageL
// -----------------------------------------------------------------------------
//
TInt CSIPProfileServerCore::ProfileUsageL(TUint32 aProfileId) const
    {
    return ProfileCacheItemL(aProfileId)->ReferenceCount();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::RegisterProfiles
// If always-on profile has SNAP, it must obtain an IAP before it can register.
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::RegisterProfiles()
    {
    for (TInt i = 0; i < iProfileCache.Count(); i++)
        {
        CSIPProfileCacheItem* item = iProfileCache[i];
        if (item->Profile().IsAutoRegistrationEnabled())
            {
			TRAPD(err, item->StartRegisterL(*iWaitForIAP, *iRegInProg, ETrue));
            if (err != KErrNone)
                {
                HandleAsyncError(*item,
                    			 CSIPConcreteProfile::ERegistrationInProgress,
                    			 err);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::LoadProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::LoadProfilesL(TBool aNotifyProfileCreation)
    {
    // Read profiles from cenrep
    TRAPD(err, iProfileStorage = CSIPProfileStorage::NewL(iFs));
    HandleProfileStorageErrorL(err);

    RPointerArray<CSIPConcreteProfile>* profiles(NULL);
    TRAP(err, profiles = iProfileStorage->SIPProfilesL());
    HandleProfileStorageErrorL(err);
    if (err != KErrNone)
        {
        profiles = iProfileStorage->SIPProfilesL();
        }

    CleanupStack::PushL(TCleanupItem(ResetAndDestroy, profiles));
    AddProfilesInCacheL(*profiles, aNotifyProfileCreation);    
    
    CleanupStack::Pop(); // TCleanupItem
    profiles->ResetAndDestroy();
    delete profiles;
    
    // Read profiles from profiles.dat file
    TRAP(err, profiles = iProfileStorage->SIPProfilesL(ETrue));
    HandleProfileStorageErrorL(err, ETrue);
    
    if (err != KErrNone)
        {
        profiles = iProfileStorage->SIPProfilesL(ETrue);
        }    

    CleanupStack::PushL(TCleanupItem(ResetAndDestroy, profiles));
    AddProfilesInCacheL(*profiles, aNotifyProfileCreation);
    
    
    CleanupStack::Pop(); // TCleanupItem
    profiles->ResetAndDestroy();
    delete profiles;
    

    RegisterProfiles();    
    iProfileStorage->GetProfileStorageIndexObject()->SetProfileServerCoreObject(this);
    }
    
// -----------------------------------------------------------------------------
// CSIPProfileServerCore::AddProfilesInCacheL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::AddProfilesInCacheL(RPointerArray<CSIPConcreteProfile>& profiles,TBool aNotifyProfileCreation)
	{
	TInt count = profiles.Count();
    for (TInt i = 0; i < count; i++) 
        {
        CSIPConcreteProfile* profile =
        	static_cast<CSIPConcreteProfile*>(profiles[0]);
        CSIPProfileCacheItem* item =
        	CSIPProfileCacheItem::NewLC(*this, iUnregistered);
        profiles.Remove(0);
        item->SetProfile(profile);
        iProfileCache.AppendL(item);
        CleanupStack::Pop(item);
        if (aNotifyProfileCreation)
            {
            SendProfileAddedEvent(*profile);
            }
        }
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::TerminateProfilesL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::TerminateProfilesL()
    {
    TInt count = iProfileCache.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iPluginDirector->TerminateHandling(iProfileCache[i]->UsedProfile());
        SendProfileRemovedEventL(*iProfileCache[i]);
        }
    iProfileCache.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileCacheItemL
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem*
CSIPProfileServerCore::ProfileCacheItemL(TUint32 aProfileId) const
    {
    CSIPProfileCacheItem* item = ProfileCacheItem(aProfileId);
    __ASSERT_ALWAYS(item != NULL, User::Leave(KErrNotFound));
    return item;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileCacheItem
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem*
CSIPProfileServerCore::ProfileCacheItem(TUint32 aProfileId) const
    {
    return FindProfileCacheItem(ProfileCached(aProfileId));
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileCached
// -----------------------------------------------------------------------------
//
TInt CSIPProfileServerCore::ProfileCached(TUint32 aProfileId) const
    {
    TIdentityRelation<CSIPProfileCacheItem>
    	compareId(CSIPProfileCacheItem::Compare);
    iFindEntry->SetProfileId(aProfileId);
    return iProfileCache.Find(iFindEntry, compareId);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::FindProfileCacheItem
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem*
CSIPProfileServerCore::FindProfileCacheItem(TInt aIndex) const
    {
    if (aIndex != KErrNotFound)
        {
        return static_cast<CSIPProfileCacheItem*>(iProfileCache[aIndex]);
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileCacheItemByStorageIdL
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem&
CSIPProfileServerCore::ProfileCacheItemByStorageIdL(TUint32 aStorageId) const
    {
    CSIPProfileCacheItem* item = ProfileCacheItemByStorageId(aStorageId);
    __ASSERT_ALWAYS(item != NULL, User::Leave(KErrNotFound));    
    return *item;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ProfileCacheItemByStorageId
// -----------------------------------------------------------------------------
//
CSIPProfileCacheItem*
CSIPProfileServerCore::ProfileCacheItemByStorageId(TUint32 aStorageId) const
    {
    for (TInt i = 0; i < iProfileCache.Count(); ++i)
        {
        if (iProfileCache[i]->Profile().StorageId() == aStorageId)
        	{
        	return iProfileCache[i];
        	}
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendRegistrationStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SendRegistrationStatusEventL(
    CSIPProfileCacheItem& aItem) const
    {        
    SendStatusEventL(aItem, aItem.Profile().Status());
    iPluginDirector->StartTimerForCleanup();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendUnregisteredStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SendUnregisteredStatusEventL(
    CSIPProfileCacheItem& aItem) const
    {
    SendStatusEventL(aItem, CSIPConcreteProfile::EUnregistered);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendStatusEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SendStatusEventL(CSIPProfileCacheItem& aItem, 
	CSIPConcreteProfile::TStatus aStatus) const
    {
    CSIPConcreteProfile &profile = aItem.Profile();
    const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	aItem.Observers();

    for (TInt i = 0; i < obs.Count(); i++)
        {
		if(aStatus == CSIPConcreteProfile::EUnregistered)	
	        obs[i]->RegistrationStatusChangedL(aItem.ProfileId(),
        								   aStatus,
                                     	   0);
		else
			obs[i]->RegistrationStatusChangedL(aItem.ProfileId(),
        									   aStatus,
                	                    	   profile.ContextId());
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendErrorEvent
// -----------------------------------------------------------------------------
//
TInt CSIPProfileServerCore::SendErrorEvent(CSIPProfileCacheItem& item,
                                           CSIPConcreteProfile::TStatus aStatus,
                                           TInt aError) const
    {
    TInt err(KErrNone);
    
    const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	item.Observers();

    for (TInt i = 0; i < obs.Count() && !err; i++)
        {
        TRAP(err, obs[i]->ErrorOccurredL(item.ProfileId(), aStatus, aError));
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendProfileAddedEvent
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SendProfileAddedEvent(
    const CSIPConcreteProfile& aProfile) const
    {
    for (TInt i = 0; i < iObservers.Count(); i++)
        {
        TRAP_IGNORE(iObservers[i]->AddedL(aProfile.Id()))
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendProfileUpdatedEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SendProfileUpdatedEventL(
    const CSIPProfileCacheItem& aItem,
    const CSIPConcreteProfile& aProfile) const
    {
    const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	aItem.Observers();

    for (TInt i = 0; i < obs.Count(); i++)
        {
		obs[i]->UpdatedL(aItem.ProfileId(), aProfile.ExternalizedSizeL());
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SendProfileRemovedEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SendProfileRemovedEventL(
    CSIPProfileCacheItem& aItem) const
    {
    const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	aItem.Observers();

    for (TInt i = 0; i < obs.Count(); i++)
        {
        obs[i]->RemovedL(aItem.ProfileId());
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::SessionCleanupL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::SessionCleanupL(
    const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    TInt index = iObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }

    for (TInt i = 0; i < iProfileCache.Count(); i ++)
        {
        CSIPProfileCacheItem* item = iProfileCache[i];
        if (item->IsUser(aObserver))
            {
            DisableProfileL(item->ProfileId(), aObserver);
            }        
        item->RemoveObserver(aObserver);
        }
    iPluginDirector->Cleanup();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ConvertTypeInfoL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ConvertTypeInfoL(
    const CImplementationInformation& aEcomInfo,
    TSIPProfileTypeInfo& aInfo) const
    {
    TLex8 lex(aEcomInfo.DataType());
    if (!lex.Peek().IsDigit())
        {
        User::Leave(KErrArgument);
        }
    TInt id(0);
    User::LeaveIfError(lex.Val(id));
    lex.Inc();
    aInfo.iSIPProfileName = lex.Remainder();
    aInfo.iSIPProfileClass = (enum TSIPProfileTypeInfo::TSIPProfileClass) id;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::CheckServerStatus
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::CheckServerStatus() const
    {
    if (CanServerStop())
        {
        iServer->ServerCanStop();
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::HandleProfileStorageErrorL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::HandleProfileStorageErrorL(TInt aErr, TBool fileStore)
    {
    switch (aErr)
    	{
    	case KErrNone:
    		break;

    	case KErrNoMemory:
    	case KErrInUse: 
    		User::Leave(aErr);
    		break;

    	default: 	        	        
	        if (!fileStore)
	        	{
	        	delete iProfileStorage;
	        	iProfileStorage = NULL;
	        	CSIPProfileStorage::DeleteAllProfilesL();
	        	iProfileStorage = CSIPProfileStorage::NewL(iFs);
	        	}
	        	else
	        	{
	        	// Corrupted sipprofiles.dat file
	        	iProfileStorage->DeleteAllStreamProfilesL();	        		
	        	}	        
    	}
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ValidateErrorStatus
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile::TStatus
CSIPProfileServerCore::ValidateErrorStatus(CSIPProfileCacheItem& aItem) const
    {
    CSIPConcreteProfile::TStatus status = aItem.Profile().Status();
    if (status == CSIPConcreteProfile::EUnregistrationInProgress)
        {
        return status;
        }
    return CSIPConcreteProfile::ERegistrationInProgress;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::HandleAsyncError
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::HandleAsyncError(CSIPProfileCacheItem& aItem, 
	CSIPConcreteProfile::TStatus aStatus,
    TInt aError)
    {
    TInt err(aError);
    if (aStatus == CSIPConcreteProfile::EUnregistrationInProgress)
        {
        err = KErrNone;
        }

    if (HandleProfileError(aItem, err))
    	{
	    if (err == KErrTotalLossOfPrecision)
	        {
	        TRAP_IGNORE(SendUnregisteredStatusEventL(aItem))
	        }
	    else
	        {
	        SendErrorEvent(aItem, aStatus, aError);
	        }
    	}
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ReleaseStorage
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ReleaseStorage(TBool aRestoreOngoing)
    {
    PROFILE_DEBUG1("ProfileServerCore::ReleaseStorage")

    iBackupInProgress = ETrue;
    TRAPD(err, ReleaseStorageL(aRestoreOngoing));

    if (err != KErrNone)
        {
        PROFILE_DEBUG1("ProfileServerCore::ReleaseStorage => failed")
#ifndef CPPUNIT_TEST
        User::Panic(KSipProfileCSServerPanic, ERestoreFailure);
#endif
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ReleaseStorageL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ReleaseStorageL(TBool aRestoreOngoing)
    {
    __ASSERT_ALWAYS(iProfileStorage != NULL,
    				User::Leave(KErrTotalLossOfPrecision));

	if (aRestoreOngoing)
		{
		TerminateProfilesL();	
		}

    delete iProfileStorage;
    iProfileStorage = NULL;
    PROFILE_DEBUG1("ProfileServerCore::ReleaseStorageL, storage released")
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ReserveStorage
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ReserveStorage(TBool aRestoreOngoing)
    {
    PROFILE_DEBUG1("ProfileServerCore::ReserveStorage")

    iBackupInProgress = EFalse;
    TRAPD(err, ReserveStorageL(aRestoreOngoing));

    if (err != KErrNone)
        {
        PROFILE_DEBUG1("ProfileServerCore::ReserveStorage => failed")
#ifndef CPPUNIT_TEST
        User::Panic(KSipProfileCSServerPanic, ERestoreFailure);
#endif
        }
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ReserveStorageL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ReserveStorageL(TBool aRestoreOngoing)
    {
	if (!iProfileStorage)
		{
		if (aRestoreOngoing)
			{
			LoadProfilesL(ETrue);
    		CheckServerStatus();
			}
		else
			{
			// Backup ends. Do not read profiles, as they are already in cache.
			iProfileStorage = CSIPProfileStorage::NewL(iFs);
			}
		PROFILE_DEBUG1("ProfileServerCore::ReserveStorageL, storage reserved")
		}
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ResetAndDestroy
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ResetAndDestroy(TAny* aArray)
    {
    RPointerArray<CSIPConcreteProfile>* array =
    	static_cast<RPointerArray<CSIPConcreteProfile>*> (aArray);
    array->ResetAndDestroy();
    delete array;
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ResetAndDestroyInfo
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::ResetAndDestroyInfo(TAny* aArray)
    {
    RPointerArray<CImplementationInformation>* array =
    	static_cast<RPointerArray<CImplementationInformation>*> (aArray);
    array->ResetAndDestroy();
    array->Close();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::CrashRevert
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::CrashRevert(TAny* aItem)
    {
    CSIPProfileCacheCleanupItem* item =
    	static_cast<CSIPProfileCacheCleanupItem*> (aItem);
    item->CrashRevert();
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::AllowMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::AllowMigrationL(TUint32 aProfileId,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    iAlrHandler->AllowMigrationL(*ProfileCacheItemL(aProfileId),
    							 aIapId,
    							 aObserver);
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::DisallowMigrationL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::DisallowMigrationL(TUint32 aProfileId,
	TUint32 aIapId,
	const MSIPExtendedConcreteProfileObserver& aObserver)
    {
    iAlrHandler->DisallowMigrationL(*ProfileCacheItemL(aProfileId),
    								aIapId,
    								aObserver);
    }
    
// -----------------------------------------------------------------------------
// CSIPProfileServerCore::RefreshIapAvailabilityL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::RefreshIapAvailabilityL(TUint32 aProfileId)
    {
    iAlrHandler->RefreshIapAvailabilityL(*ProfileCacheItemL(aProfileId));
    }

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::MigrationStartedL
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::MigrationStartedL(const CSIPProfileCacheItem& aItem,
											  TUint32 aSnapId,
											  TUint32 aIapId) const
	{
	const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	aItem.Observers();
    for (TInt i = 0; i < obs.Count(); ++i)
        {
        obs[i]->MigrationStartedL(aItem.ProfileId(), aSnapId, aIapId);
        }
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::MigrationCompletedL
// -----------------------------------------------------------------------------
//
void
CSIPProfileServerCore::MigrationCompletedL(const CSIPProfileCacheItem& aItem,
										   TUint32 aSnapId,
										   TUint32 aIapId) const
	{
	const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	aItem.Observers();
    for (TInt i = 0; i < obs.Count(); ++i)
        {
        obs[i]->MigrationCompletedL(aItem.ProfileId(), aSnapId, aIapId);
        }
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::PassAlrErrorToClientL
// -----------------------------------------------------------------------------
//
TInt
CSIPProfileServerCore::PassAlrErrorToClient(const CSIPProfileCacheItem& aItem,
											TInt aError,
											TUint32 aSnapId,
											TUint32 aIapId) const
	{
	TInt err(KErrNone);
	const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
    	aItem.Observers();
    for (TInt i = 0; i < obs.Count() && !err; ++i)
        {
        err = obs[i]->AlrError(aError, aItem.ProfileId(), aSnapId, aIapId);
        }
    return err;
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::MigrationControllerL
// -----------------------------------------------------------------------------
//
CSipAlrMigrationController&
CSIPProfileServerCore::MigrationControllerL(TUint32 aSnapId)
	{
	RemoveUnusedMigrationControllers(aSnapId);

	for (TInt i = 0; i < iMigrationControllers.Count(); ++i)
		{
		if (iMigrationControllers[i]->SnapId() == aSnapId)
			{
			return *iMigrationControllers[i];
			}
		}

	CSipAlrMigrationController* ctrl =
		CSipAlrMigrationController::NewLC(iAlrHandler->AlrMonitorL(), aSnapId);
	iMigrationControllers.AppendL(ctrl);
	CleanupStack::Pop(ctrl);
	return *ctrl;
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::UnregisteredState
// -----------------------------------------------------------------------------
//
CSIPProfileState& CSIPProfileServerCore::UnregisteredState()
	{
	return *iUnregistered;
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::RemoveProfileItem
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::RemoveProfileItem(TUint32 aProfileId)
	{
    TInt index = ProfileCached(aProfileId);
    if (index >= 0)
    	{
    	iProfileCache.Remove(index);
    	}
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::RemoveUnusedMigrationControllers
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::RemoveUnusedMigrationControllers(TUint32 aSnapId)
	{	
	for (TInt i = iMigrationControllers.Count() - 1; i >= 0; --i)
		{
		if (!iMigrationControllers[i]->IsUsed() &&
			iMigrationControllers[i]->SnapId() != aSnapId)
			{
			CSipAlrMigrationController* unused = iMigrationControllers[i];
			iMigrationControllers.Remove(i);
			delete unused;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::GenerateProfileIdL
// Generate a random profile id (don't use value 0) and check it is not in use.
// -----------------------------------------------------------------------------
//
TUint32 CSIPProfileServerCore::GenerateProfileIdL()
	{
	const TInt KProfileIdLength = sizeof(TUint32);
	HBufC8* buf = HBufC8::NewLC(KProfileIdLength);
    TPtr8 dataPtr(buf->Des());
    dataPtr.FillZ(dataPtr.MaxLength());
	CSystemRandom* random = CSystemRandom::NewLC();

	const TUint32 KEmptyProfileId = 0;
	TUint32 id(KEmptyProfileId);
	TBool alreadyUsed(EFalse);
	do
		{
		random->GenerateBytesL(dataPtr);

		const TInt KHighestBytePos 		 = 24;
		const TInt KSecondHighestBytePos = 16;
		const TInt KLowestBytePos 		 = 8;
		id = dataPtr[0] << KHighestBytePos |
			 dataPtr[1] << KSecondHighestBytePos |
			 dataPtr[2] << KLowestBytePos |
			 dataPtr[3];
		alreadyUsed = EFalse;

		for (TInt i = 0; i < iProfileCache.Count() && !alreadyUsed; ++i)
	        {
	        alreadyUsed = (iProfileCache[i]->Profile().Id() == id);
	        }
		} while (alreadyUsed || id == KEmptyProfileId);

	CleanupStack::PopAndDestroy(random);
	CleanupStack::PopAndDestroy(buf);
	return id;
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::LoadSystemStateMonitorL
// Load first matching plugin. There should be just one.
// -----------------------------------------------------------------------------
//
void CSIPProfileServerCore::LoadSystemStateMonitorL()
	{
	__ASSERT_ALWAYS(!iSystemStateMonitor, User::Leave(KErrAlreadyExists));

	RImplInfoPtrArray infoArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInfo, &infoArray));
	REComSession::ListImplementationsL(KSipSystemStateMonitorIfUid, infoArray);    

	if (infoArray.Count() > 0 && infoArray[0])
	    {
		// Create a specific plugin with its UID
		iSystemStateMonitor = reinterpret_cast<CSipSystemStateMonitor*>(
			REComSession::CreateImplementationL(
				infoArray[0]->ImplementationUid(),
				_FOFF(CSipSystemStateMonitor, iInstanceKey)));
        PROFILE_DEBUG1("System state monitor loaded")
  		iSystemStateMonitor->StartMonitoringL(
  		    CSipSystemStateMonitor::ESystemState, 0, *this);
		iSystemStateMonitor->StartMonitoringL(
			CSipSystemStateMonitor::ERfsState, 0, *this);
	    }
	CleanupStack::Pop(); // TCleanupItem
	infoArray.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::FindDefaultProfile
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPProfileServerCore::FindDefaultProfile() const
	{
	const TInt count = iProfileCache.Count();
	
	for ( TInt i=0 ; i<count ; ++i )
		{
		if ( iProfileCache[i]->ProfileId() && iProfileCache[i]->IsActiveState() )
			{
			CSIPConcreteProfile* ptr = &( iProfileCache[i]->LatestProfile() );
			
			if ( ptr && ptr->IsDefault() )
				{
				return ptr;
				}
			}
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPProfileServerCore::ShouldChangeIap
// -----------------------------------------------------------------------------
// 
TBool CSIPProfileServerCore::ShouldChangeIap(CSIPConcreteProfile& aProfile, TInt aError) const
	{
	PROFILE_DEBUG3("CSIPProfileServerCore::ShouldChangeIap, error", aError)
	TUint32 snapId(0);	
	if ( aProfile.ExtensionParameter(KSIPSnapId, snapId) == KErrNone 
	        && !AnyRegisteredProfileUsesIap(aProfile.IapId()) )
	    {
		PROFILE_DEBUG1("CSIPProfileServerCore::ShouldChangeIap, snap is in use")

	    // This profile uses a SNAP.
	    // There are no registered profiles using the same IAP as this profile.
		
		TUint iapCount(0);
		TRAPD(err, iapCount = IAPCountL(snapId));
		if(KErrNone == err)
			{			
			if ( (aError == KErrTimedOut ||
				 aError == KErrSIPResolvingFailure) 
				 && iapCount > 1)
				{
				PROFILE_DEBUG1("CSIPProfileServerCore::ShouldChangeIap returns True")
				return ETrue;
				}
			}
			
		}
	PROFILE_DEBUG1("CSIPProfileServerCore::ShouldChangeIap returns false")
    return EFalse;
	}
 
TBool CSIPProfileServerCore::AnyRegisteredProfileUsesIap(TUint aIap) const
    {
    
    for ( TInt i = 0; i < iProfileCache.Count(); i++ )
        {
        PROFILE_DEBUG3("CSIPProfileServerCore::AnyRegisteredProfileUsesIap, i", i)
        // Iterate all CSIPProfileCacheItems.
        if ( iProfileCache[i]->IsActiveState() )
            {
            // Cache item is valid.
            CSIPConcreteProfile& profile = iProfileCache[i]->Profile();
			PROFILE_DEBUG3("CSIPProfileServerCore::AnyRegisteredProfileUsesIap, profile status", profile.Status())
			PROFILE_DEBUG4("CSIPProfileServerCore::AnyRegisteredProfileUsesIap, aIap, profile.IapId", aIap, profile.IapId())

            if ( profile.Status() == CSIPConcreteProfile::ERegistered && 
                 profile.IapId() == aIap )
                {
                    // Profile is registered and is using the specified IAP.
					PROFILE_DEBUG1("CSIPProfileServerCore::AnyRegisteredProfileUsesIap returns True")

                    return ETrue;
                }
            }
        }

	PROFILE_DEBUG1("CSIPProfileServerCore::AnyRegisteredProfileUsesIap returns false")
    return EFalse;
    }

// ----------------------------------------------------------------------------
// Function is used to send ForciblyDisabled event on the profile to the
// Profile Agent Client.
// ----------------------------------------------------------------------------
void CSIPProfileServerCore::SendProfileForciblyDisabledEvent(
	const CSIPProfileCacheItem& aItem) const
    {
    	//Get the list of observers for a given Profile Item
    	const RPointerArray<MSIPExtendedConcreteProfileObserver>& obs =
        	aItem.Observers();

    	//Iterate through the list of observers for that profile item
    	//and send event to each of them.
        for (TInt i = 0; i < obs.Count(); i++)
        	{
			TRAP_IGNORE(obs[i]->RegistrationStatusChangedL(aItem.ProfileId(),
						aItem.Profile().Status(), ESipProfileItcOpProfileForciblyDisabled));
            }
    }
	
	
// -----------------------------------------------------------------------------
// CSIPProfileServerCore::IAPCount
// -----------------------------------------------------------------------------
//
TInt CSIPProfileServerCore::IAPCountL(TUint32 aSnapId) const
    {
    const TUint KRecordId = aSnapId;
	TUint32 count(0);
	CMDBSession* db = CMDBSession::NewLC( KCDVersion1_2);
	db->SetAttributeMask( ECDHidden );	
	
    //Load the Selection Policy record
	CCDIAPPrioritySelectionPolicyRecord *selPolRecord = (CCDIAPPrioritySelectionPolicyRecord *)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
	
	CleanupStack::PushL(selPolRecord);    
    selPolRecord->SetRecordId(KRecordId);
	
    selPolRecord->LoadL( *db ); 
	count = selPolRecord->iIapCount;
	PROFILE_DEBUG3("CSIPProfileServerCore::IAPCount, IAP Count", count)
	      
    CleanupStack::PopAndDestroy(selPolRecord);
	CleanupStack::PopAndDestroy( db );
    return count;
    }
	
