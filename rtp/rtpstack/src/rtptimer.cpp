/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




// INCLUDE FILES
#include "rtptimer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpTimer::CRtpTimer( const TInt aPriority, MTimeOutNotify& aNotify ) 
    : CTimer( aPriority ),
    iNotify( &aNotify )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpTimer* CRtpTimer::NewL( const TInt aPriority, MTimeOutNotify& aNotify )
    {
    CRtpTimer* self = new ( ELeave ) CRtpTimer( aPriority, aNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpTimer::~CRtpTimer()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CRtpTimer::RunL()
// 
// ---------------------------------------------------------------------------
//
void CRtpTimer::RunL()
    {
    iNotify->OnExpiredL( iStatus.Int() );
    }

//  End of File
