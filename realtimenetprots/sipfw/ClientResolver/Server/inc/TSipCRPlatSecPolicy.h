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
* Name        : TSipCRPlatSecPolicy.h
* Part of     : SIP Client Resolver
* The security policy for Sip Client resolver server
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPCRPLATSECPOLICY_H
#define TSIPCRPLATSECPOLICY_H


/**
 * Number of different IPC ranges 
 */
const TUint KSipCRPlatSecRangeCount = 4;


/**
 * IPC ranges segmented into groups based on their capability requirements
 */
const TInt TSipCRPlatSecRanges[KSipCRPlatSecRangeCount] =
    {
    0,  // IPC message 0 - 3
    4,	// IPC message 4 - 6
    7,	// IPC message 7 - 9
    10	// Not Supported 10 ->
    };

/**
 * IPC segments and their capability requirements
 */
const TUint8 TSipCRPlatSecElementsIndex[KSipCRPlatSecRangeCount] =
    {
	CPolicyServer::EAlwaysPass, // Security policy for ITC 0-3
    0, // Security policy for IPC 0-2 (WriteDeviceData)
    1, // Security policy for ITC 3-9 (ReadDeviceData)
    CPolicyServer::ENotSupported // Not Supported 10 ->
    };
	
/**
 * Capability sets and the action performed in case of failure
 */	
const CPolicyServer::TPolicyElement TSipCRPlatSecElements[2] =
    {	
    // Requires the user to have WriteDeviceData capability, otherwise the message is 
    // completed with KErrPermissionDenied
    { _INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient },
	
	// Requires the user to have ReadDeviceData capability, otherwise the message is 
	// completed with KErrPermissionDenied
	{ _INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient },
	
    };
	
/**
 * Platform security policy that SIP Server uses. 
 */	
const CPolicyServer::TPolicy TSIPCRPlatSecPolicy =
    {
    CPolicyServer::EAlwaysPass, 	// Specifies all connect attempts should pass
    KSipCRPlatSecRangeCount,		// Number of ITC ranges
    TSipCRPlatSecRanges,			// ITC Ranges
    TSipCRPlatSecElementsIndex,		// Policy to ITC mapping array
    TSipCRPlatSecElements			// Policies used in SIP Server
    };
						
#endif			
		