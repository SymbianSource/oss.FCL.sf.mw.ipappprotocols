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
//

#ifndef __SIPCLIENT_PAN__
#define __SIPCLIENT_PAN__

#include <e32std.h>

/**
 * SIP Client panics
 */

/**
 * Constant literal description
 */

_LIT(KSipClient, "SipClient");

/** SIP application panic codes */
enum TSipClientPanics 
    {
	/** Function preconditions was not met */
    ESipClientPreCondNotMet,
	/** Function postconditions was not met */
    ESipClientPostCondNotMet,
	/** The user has not successfully connected to sip C/S-server */
    ESipClientNotConnected,
	/** The user has not cancelled an outstanding receive before closing */
    ESipReceiveNotCancelled
    // add further panics here
    };

#endif // SIPCLIENT_PAN
