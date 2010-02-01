/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TSipCSPlatSecPolicy.h
* Part of     : SIPServerCore
* The security policy for Sip Server
* Version     : SIP/6.0
*
*/




/**
 @internalComponent
*/
#ifndef TSIPCSPLATSECPOLICY_H
#define TSIPCSPLATSECPOLICY_H


/**
 * Number of different IPC ranges 
 */
const TUint KSipCSPlatSecRangeCount = 5;


/**
 * IPC ranges segmented into groups based on their capability requirements
 */
const TInt TSipCSPlatSecRanges[KSipCSPlatSecRangeCount] =
	{
	0,  // IPC message 0 - 25
	26, // IPC message 26
	27, // IPC message 27 - 28
	29, // IPC message 29 - 53
	54	// Not Supported 54 ->
	};

/**
 * IPC segments and their capability requirements
 */
const TUint8 TSipCSPlatSecElementsIndex[KSipCSPlatSecRangeCount] =
	{
	CPolicyServer::EAlwaysPass, // Security policy for IPC 0-25
	0, // Security policy for IPC 26 (WriteDeviceData)
	CPolicyServer::ECustomCheck, // Security policy for IPC 27-28
    1, // Security policy for IPC 29-53 (NetworkServices)
    CPolicyServer::ENotSupported // Not Supported 54 ->
	};
	
/**
 * Capability sets and the action performed in case of failure
 */	
const CPolicyServer::TPolicyElement TSipCSPlatSecElements[2] =
	{
	// Requires the user to have WriteDeviceData capability, 
	// otherwise the message is completed with KErrPermissionDenied
	{ _INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData),
	  CPolicyServer::EFailClient },
	
	// Requires the user to have NetworkServices capability, 
	// otherwise the message is completed with KErrPermissionDenied
	{ _INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices),
	  CPolicyServer::EFailClient }
	};
	
/**
 * Platform security policy that SIP Server uses. 
 */	
const CPolicyServer::TPolicy TSipCSPlatSecPolicy =
	{
	CPolicyServer::EAlwaysPass, 	// Specifies all connect attempts should pass
	KSipCSPlatSecRangeCount,		// Number of ITC ranges
	TSipCSPlatSecRanges,			// ITC Ranges
	TSipCSPlatSecElementsIndex,		// Policy to ITC mapping array
	TSipCSPlatSecElements			// Policies used in SIP Server
	};
						
#endif // TSIPCSPLATSECPOLICY_H
