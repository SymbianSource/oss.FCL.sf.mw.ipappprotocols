// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofileactivescheduler
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "SipProfileActiveScheduler.h"
#include "SipProfileCSServer.pan"
#include "SipProfileLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileActiveScheduler::CSIPProfileActiveScheduler
// -----------------------------------------------------------------------------
//
CSIPProfileActiveScheduler::CSIPProfileActiveScheduler()
	{
	}


// -----------------------------------------------------------------------------
// CSIPProfileActiveScheduler::~CSIPProfileActiveScheduler
// -----------------------------------------------------------------------------
//
CSIPProfileActiveScheduler::~CSIPProfileActiveScheduler()
	{
	}


// -----------------------------------------------------------------------------
// CSIPProfileActiveScheduler::Error
// -----------------------------------------------------------------------------
//
void CSIPProfileActiveScheduler::Error(TInt aError) const
	{
    // This function will be entered only if a RunL-function of 
    // a non-C/S-session related active object leaves.
    // The SIP C/S-server has to be kept running under any conditions,
	// except if SIP server is terminated.

	if (aError == KErrServerTerminated)
		{
		PROFILE_DEBUG1("ProfileServer shutdown because SIP server terminated")
		User::Panic(KSipProfileCSServerPanic, EBadSipSessionHandle);
		}
	}
