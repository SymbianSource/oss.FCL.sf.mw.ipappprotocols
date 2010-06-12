/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// CLASS HEADER
#include "CMSRPScheduler.h"

// INTERNAL INCLUDES
#include "MSRPCommon.h"
#include "CMSRPServer.h"

// ================= MEMBER FUNCTIONS ==========================================

// The class constructor.
CMSRPScheduler::CMSRPScheduler()
    {
    MSRPLOG( "CMSRPScheduler: CMSRPScheduler::CMSRPScheduler()" )
    }

// -----------------------------------------------------------------------------
// CMSRPScheduler::SetServer
// Gives a pointer of the current CMSRPServer instance to the scheduler.
// -----------------------------------------------------------------------------
//
void CMSRPScheduler::SetServer( CMSRPServer* aServer )
    {
    MSRPLOG( "CMSRPScheduler::SetServer enter" )
    iServer = aServer;
    MSRPLOG( "CMSRPScheduler::SetServer exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPScheduler::Error
// Overridden to handle leaves in request functions and unexpected situations.
// -----------------------------------------------------------------------------
//
void CMSRPScheduler::Error( TInt /*aError*/ ) const
    {
    MSRPLOG( "CMSRPScheduler: CMSRPScheduler::Error enter" )
    if ( iServer->IsActive() )
        {
        __DEBUGGER()
        }
    else
        {
        // Relay the error code to the client and restart the server.
        iServer->ReStart();
        }
    MSRPLOG( "CMSRPScheduler: CMSRPScheduler::Error exit" )
    }

//  End of File
