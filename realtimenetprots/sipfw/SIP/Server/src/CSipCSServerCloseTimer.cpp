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
// Name          : CSipCSServerCloseTimer.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//



#include "CSipCSServerCloseTimer.h"

const TUint KMicroSecondsInMilliSecond = 1000;


// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::NewL
// -----------------------------------------------------------------------------
//
CSipCSServerCloseTimer* CSipCSServerCloseTimer::NewL ()
    {
    CSipCSServerCloseTimer* self = CSipCSServerCloseTimer::NewLC ();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::NewLC
// -----------------------------------------------------------------------------
//
CSipCSServerCloseTimer* CSipCSServerCloseTimer::NewLC ()
    {
    CSipCSServerCloseTimer* self = new (ELeave) CSipCSServerCloseTimer;
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::ConstructL
// -----------------------------------------------------------------------------
//
void CSipCSServerCloseTimer::ConstructL ()
    {
	User::LeaveIfError (iTimer.CreateLocal());
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::CSipCSServerCloseTimer
// -----------------------------------------------------------------------------
//
CSipCSServerCloseTimer::CSipCSServerCloseTimer ()
 : CActive(EPriorityStandard)
    {
	CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::~CSipCSServerCloseTimer
// -----------------------------------------------------------------------------
//
CSipCSServerCloseTimer::~CSipCSServerCloseTimer ()
    {
	CActive::Cancel();
	iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::StopActiveSchedulerAfter
// -----------------------------------------------------------------------------
//
void CSipCSServerCloseTimer::StopActiveSchedulerAfter (TUint aMilliSeconds)
    {
	TTimeIntervalMicroSeconds32 value(KMicroSecondsInMilliSecond*aMilliSeconds);
    if (!IsActive())
        {
        iTimer.After(iStatus,value);
        SetActive ();
        }
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::RunL
// -----------------------------------------------------------------------------
//
void CSipCSServerCloseTimer::RunL ()
    {
	CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// CSipCSServerCloseTimer::DoCancel
// -----------------------------------------------------------------------------
//	
void CSipCSServerCloseTimer::DoCancel ()
    {
	iTimer.Cancel();
    }
