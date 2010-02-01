/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name        : sipprofilecs.h
* Part of     : SIP Profile
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPPROFILECS_H
#define SIPPROFILECS_H

#include <e32std.h>

_LIT(KSipProfileServerName,"SipProfileSrv");
_LIT(KSipProfileServerFilename, "SipProfileSrv");
_LIT(KSipProfileServerSemaphoreName, "SipProfileServerSemaphore");
_LIT(KSipProfileServerShutdownSemaphore, "SipProfileServerShutdownSemaphore");

const TUid KServerUid3 = {0x101F413C};

#if (defined (__WINS__) || defined(__WINSCW__))
static const TUint KServerMinHeapSize =  0x1000;  //   4K
static const TUint KServerMaxHeapSize = 0x80000;  // 512K 
#endif

// The server version.
// A version must be specifyed when creating a session with the server
const TUint KSipProfileServerMajorVersionNumber=0;
const TUint KSipProfileServerMinorVersionNumber=1;
const TUint KSipProfileServerBuildVersionNumber=1;


// Oper. codes used in passing IPC messages between client and server
enum TSipProfileItcFunctions
	{
    // No capabilities required
	ESipProfileItcOpCreateProfileSize = 0,
	ESipProfileItcOpCreateProfile,    
    ESipProfileItcOpCancelReceive,
    ESipProfileItcOpReadyToReceive,
	ESipProfileItcOpPluginsSize,
	ESipProfileItcOpPlugins,    
    ESipProfileItcOpProfileSize, 
    ESipProfileItcOpProfileSizeDefault,    
    ESipProfileItcOpProfilesSize,    
    ESipProfileItcOpProfilesSizeByAOR,    
    ESipProfileItcOpProfilesSizeByType,    
    ESipProfileItcOpRegisteredAORsSize,    
	ESipProfileItcOpNegotiatedSecurity,
    ESipProfileItcOpNegotiatedSecuritySize,	
    ESipProfileItcOpAddALRObserver,
    ESipProfileItcOpRemoveALRObserver,
	// ReadUserData required
	ESipProfileItcOpUsage,
    ESipProfileItcOpProfile,
    ESipProfileItcOpProfileRefresh, 
    ESipProfileItcOpProfiles, 
    ESipProfileItcOpProfilesByAOR, 
    ESipProfileItcOpProfilesByType,
	ESipProfileItcOpRegisteredAORs,
	// WriteDeviceData, NetworkServices
	ESipProfileItcOpAddProfile,
	ESipProfileItcOpUpdateProfile,
	ESipProfileItcOpRemoveProfile,
	ESipProfileItcOpAllowMigration,
	ESipProfileItcOpDisallowMigration,
	ESipProfileItcOpRefreshIapAvailability,
	// NetworkServices
	ESipProfileItcOpDeleteProfile,
	ESipProfileItcOpDisable,
	ESipProfileItcOpEnable,
	ESipProfileItcOpForceDisable
	};

enum TSipProfileItcEvents
	{
	ESipProfileItcOpRegistered = 0,
	ESipProfileItcOpDeregistered,
	ESipProfileItcOpRegistrationError,
	ESipProfileItcOpUnregistrationError,
	ESipProfileItcOpProfileAdded,
	ESipProfileItcOpProfileAddedDefault,
	ESipProfileItcOpProfileUpdated,
	ESipProfileItcOpProfileRemoved,
	ESipProfileItcOpIapAvailable,
	ESipProfileItcOpMigrationStarted,
	ESipProfileItcOpMigrationCompleted,
	ESipProfileItcOpALRError,
	ESipProfileItcOpProfileForciblyDisabled
	};	
	

// Indexes used in IPC parameter array passed between C/S-client and C/S-server
//
// NOTE: Remember to update the types in comments if the actual types change.
//

enum TSipProfileItcArguments
	{
    ESipProfileItcArgProfileId = 0,		// Type: TUint
    ESipProfileItcArgType = 0,			// Type: TUint
    ESipProfileItcArgNarrator = 1,      // Type: HBufC8
	ESipProfileItcArgSlots = 2,			// Type: TSIPProfileSlots
    ESipProfileItcArgProfile = 3,       // Type: CSIPConcreteProfile
    ESipProfileItcArgProfiles = 3,      // Type: CSIPConcreteProfileHolder
    ESipProfileItcArgPlugins = 3,       // Type: CSIPProfilePlugins
    ESipProfileItcArgNegotiated = 3,    // Type: HBufC8
    ESipProfileItcArgAORs = 3		    // Type: CDesC8ArrayFlat
	};

#endif // SIPPROFILECS_H
