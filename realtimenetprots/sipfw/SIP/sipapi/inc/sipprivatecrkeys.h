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
* Name          : sipprivatecrkeys.h
* Part of       : SIP API
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPPRIVATECRKEYS_H
#define SIPPRIVATECRKEYS_H

// INCLUDES
#include <e32def.h>

#if !defined(SIPCRKEYS_H)
const TUid KCRUidSIP = { 0x101FED88 };
#endif

/// The number of "UDVM cycles" available to decompress each 
/// bit in a SigComp message.
const TUint32 KSIPSigCompCPB = 0x04;

/// The number of bytes offered to a particular compartment 
/// for the creation of state.
const TUint32 KSIPSigCompSMS = 0x05;

/// Amount of memory available to decompress SigComp message
const TUint32 KSIPSigCompDMS = 0x06;

/// The used SigComp algorithm
const TUint32 KSIPSigCompAlgorithm = 0x07;

/// Maximum number of compartments
const TUint32 KSIPSigCompMaxCompartments = 0x08;

/// If this parameter has a non-zero value, 
/// an empty value is used for the response-parameter in Authorization-header
/// after a SQN synchronization in IMS authentication 
const TUint32 KSIPSendEmptyResponseParameterAfterSqnFailure = 0x09;

/// Maximum number of pending server transactions.
/// When this value is reached, SIP stops handling new incoming SIP requests
/// until the current server transactions start to complete.
const TUint32 KSIPMaxPendingServerTransactions = 0x0a;
#endif // SIPPRIVATECRKEYS_H

// End of File
