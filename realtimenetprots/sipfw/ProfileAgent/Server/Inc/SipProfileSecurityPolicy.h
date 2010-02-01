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
* Name        : SipProfileSecurityPolicy.h
* Part of     : SIP Profile Server
* Interface   : private
* The security policy for Profile Agent Server
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPPROFILESECURITYPOLICY_H
#define SIPPROFILESECURITYPOLICY_H


/**
 * Number of different IPC ranges 
 */
const TUint KSipProfilePlatSecRangeCount = 6;


/**
 * IPC ranges segmented into groups based on their capability requirements
 */
const TInt TSipProfilePlatSecRanges[KSipProfilePlatSecRangeCount] =
	{
	0,  // IPC 0-15  No capabilities required
    16, // IPC 16-22 ReadUserData required
    23, // IPC 23-28 WriteDeviceData and NetworkServices
	29, // IPC 29-31 NetworkServices
	32, // IPC 32 Network Control 
	33  // Not supported 33 ->	
	};


/**
 * Capability sets and the action performed in case of failure
 */	
const CPolicyServer::TPolicyElement TSipProfilePlatSecElements[4] =
	{
	
	{ // 0 
	_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData),
	CPolicyServer::EFailClient
	},

	{ // 1
	_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices), 
	CPolicyServer::EFailClient
    },

	{ // 2
	_INIT_SECURITY_POLICY_C2(ECapabilityWriteDeviceData,
	                         ECapabilityNetworkServices),
	CPolicyServer::EFailClient
    },
    
    { // 3
    _INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl),
    CPolicyServer::EFailClient    		
    }
    
	};


/**
 * IPC segments and their capability requirements
 */
const TUint8 TSipProfilePlatSecElementsIndex[KSipProfilePlatSecRangeCount] =
	{
    CPolicyServer::EAlwaysPass, // IPC 0-15
    0, // IPC 16-22
    2, // IPC 23-28
    1, // IPC 29-31
    3, // IPC 32
    CPolicyServer::ENotSupported // IPC 33 ->
	};
	
	
/**
 * Platform security policy that Profile Agent uses. 
 */	
const CPolicyServer::TPolicy TSipProfilePlatSecPolicy =
	{
	CPolicyServer::EAlwaysPass, 	// Specifies all connect attempts should pass
	KSipProfilePlatSecRangeCount,	// Number of IPC ranges
	TSipProfilePlatSecRanges,		// IPC Ranges
	TSipProfilePlatSecElementsIndex,// Policy to IPC mapping array
	TSipProfilePlatSecElements		// Policies used in Profile agent
	};
						
#endif // SIPPROFILESECURITYPOLICY_H		
		