/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipinternalerr.h
* Part of       : SIP Common
* Error codes visible only internally to SIP core stack
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPINTERNALERR_H
#define SIPINTERNALERR_H

#include <e32std.h>

// Part of unique SIP error codes from Symbian is reserved for internal use.
// This range is from -17749 to -17745 and it is between the error codes
// reserved for SIP Client API and SIP Codec API.

// INVITE transaction has ended
const TInt KInviteTransactionCompleted = -17745;

// A CANCEL has been received for a previously received INVITE.
const TInt KInviteTransactionCancelled = -17746;

// A HTTP digest challenge has been received
const TInt KChallengeReceived = -17747;

#endif // SIPINTERNALERR_H

// End of File
