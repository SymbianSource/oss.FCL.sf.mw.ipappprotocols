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
// Name        : sipprofilecsserverclosetimer.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include "SipProfileCSServerCloseTimer.h"

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::NewL
// -----------------------------------------------------------------------------
//
CSipProfileCSServerCloseTimer* CSipProfileCSServerCloseTimer::NewL()
    {
    CSipProfileCSServerCloseTimer* self =
    	CSipProfileCSServerCloseTimer::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::NewLC
// -----------------------------------------------------------------------------
//
CSipProfileCSServerCloseTimer* CSipProfileCSServerCloseTimer::NewLC ()
    {
    CSipProfileCSServerCloseTimer* self =
    	new (ELeave) CSipProfileCSServerCloseTimer;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CSipProfileCSServerCloseTimer::ConstructL()
    {
	User::LeaveIfError(iTimer.CreateLocal());
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::CSipProfileCSServerCloseTimer
// -----------------------------------------------------------------------------
//
CSipProfileCSServerCloseTimer::CSipProfileCSServerCloseTimer() :
	CActive(EPriorityStandard)
    {
	CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::~CSipProfileCSServerCloseTimer
// -----------------------------------------------------------------------------
//
CSipProfileCSServerCloseTimer::~CSipProfileCSServerCloseTimer()
    {
	CActive::Cancel();
	iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::StopActiveSchedulerAfter
// -----------------------------------------------------------------------------
//
void CSipProfileCSServerCloseTimer::StopActiveSchedulerAfter(TUint aSeconds)
    {
    if (!IsActive())
        {
        const TUint KMicrosecsInSecond = 1000000;
	    TTimeIntervalMicroSeconds32 timeInterval(KMicrosecsInSecond * aSeconds);
        iTimer.After(iStatus, timeInterval);
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::RunL
// -----------------------------------------------------------------------------
//
void CSipProfileCSServerCloseTimer::RunL()
    {
	CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CSipProfileCSServerCloseTimer::DoCancel
// -----------------------------------------------------------------------------
//	
void CSipProfileCSServerCloseTimer::DoCancel()
    {
	iTimer.Cancel();
    }
