// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipActiveScheduler.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//



#include "CSipActiveScheduler.h"
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CSipActiveScheduler::CSipActiveScheduler
// -----------------------------------------------------------------------------
//
CSipActiveScheduler::CSipActiveScheduler ()
	{
	}

// -----------------------------------------------------------------------------
// CSipActiveScheduler::~CSipActiveScheduler
// -----------------------------------------------------------------------------
//
CSipActiveScheduler::~CSipActiveScheduler ()
	{
	}

// -----------------------------------------------------------------------------
// CSipActiveScheduler::Error
// -----------------------------------------------------------------------------
//
void CSipActiveScheduler::Error (TInt aError) const
	{
    // This function will be entered only if a RunL-function of 
    // a non-C/S-session related active object leaves.
    // The SIP C/S-server has to be kept running under any conditions.
    __SIP_INT_LOG1("CSipActiveScheduler::Error", aError)
    
    // Avoid compiler warning in UREL
    (void)aError;
	}
