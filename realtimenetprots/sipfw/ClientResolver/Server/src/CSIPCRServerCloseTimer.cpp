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
// Name          : CSIPCRServerCloseTimer.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPCRServerCloseTimer.h"

const TUint KMicroSecondsInMilliSecond = 1000; 


// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::NewL
// ----------------------------------------------------------------------------
//
CSIPCRServerCloseTimer* CSIPCRServerCloseTimer::NewL()
    {
    CSIPCRServerCloseTimer* self = CSIPCRServerCloseTimer::NewLC ();
    CleanupStack::Pop (self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::NewLC
// ----------------------------------------------------------------------------
//
CSIPCRServerCloseTimer* CSIPCRServerCloseTimer::NewLC ()
    {
    CSIPCRServerCloseTimer* self = new(ELeave)CSIPCRServerCloseTimer;
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPCRServerCloseTimer::ConstructL ()
    {
	User::LeaveIfError (iTimer.CreateLocal());
    }
    
    
// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::ConstructL
// ----------------------------------------------------------------------------
//
CSIPCRServerCloseTimer::CSIPCRServerCloseTimer ()
: CActive(EPriorityStandard)
    {
	CActiveScheduler::Add(this);
    }


// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::~CSIPCRServerCloseTimer
// ----------------------------------------------------------------------------
//
CSIPCRServerCloseTimer::~CSIPCRServerCloseTimer ()
    {
	CActive::Cancel();
	iTimer.Close();
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::StopActiveSchedulerAfter
// ----------------------------------------------------------------------------
//
void CSIPCRServerCloseTimer::StopActiveSchedulerAfter (TUint aMilliSeconds)
    {
    TTimeIntervalMicroSeconds32 time(KMicroSecondsInMilliSecond*aMilliSeconds);
    if (!IsActive())
        {
        iTimer.After(iStatus,time);
        SetActive ();
        }
    }

// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::RunL
// ----------------------------------------------------------------------------
//
void CSIPCRServerCloseTimer::RunL ()
    {
	CActiveScheduler::Stop();
    }

// ----------------------------------------------------------------------------
// CSIPCRServerCloseTimer::DoCancel
// ----------------------------------------------------------------------------
//	
void CSIPCRServerCloseTimer::DoCancel ()
    {
	iTimer.Cancel();
    }

//  End of File  

