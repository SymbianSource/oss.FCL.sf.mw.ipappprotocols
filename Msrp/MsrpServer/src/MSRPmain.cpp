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

// INTERNAL INCLUDES
#include "MsrpCommon.h"
#include "MSRPServerCommon.h"
#include "CMSRPScheduler.h"
#include "CMSRPServer.h"


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PanicServer
// Panics the MSRP Server
// -----------------------------------------------------------------------------
//
void PanicServer(
    TMSRPPanicCodes aPanicCode ) // Panic code
    {
    MSRPLOG( "MSRPServer: PanicServer enter" );
    User::Panic( KMSRPServerName, aPanicCode );
    MSRPLOG( "MSRPServer: PanicServer exit" );
    }

// -----------------------------------------------------------------------------
// StartServerL
// Starts the MSRPServer
// -----------------------------------------------------------------------------
//
LOCAL_C void StartServerL()
    {
    MSRPLOG( "MSRPServer: StartServerL enter" )

    // create server
    TFindServer findMsrpServer( KMSRPServerName );
    TFullName pathName;

    // Search for the server.
    if ( findMsrpServer.Next( pathName ) != KErrNone )
        {
		// Start scheduler and server.
		CMSRPScheduler* scheduler = new (ELeave) CMSRPScheduler;
		__ASSERT_ALWAYS( scheduler !=
			NULL, PanicServer( EMSRPServerSchedulerError ) );

		CleanupStack::PushL( scheduler );
		CActiveScheduler::Install( scheduler );

		// Rename the thread.
		User::RenameThread( KMSRPServerName );

		// Create the server
		CMSRPServer* server = CMSRPServer::NewLC();

		// The scheduler needs access to the server instance.
		//lint -e{613} scheduler cannot be null, due assertion in creation.
		scheduler->SetServer( server );

		// client alert
		RProcess::Rendezvous( KErrNone );

		// start fielding requests from clients
		CActiveScheduler::Start(); // codescanner::activestart

		// finished when the scheduler stops
		CleanupStack::PopAndDestroy( 2 ); // scheduler, server, codescanner::cleanup
		}
	else
		{
		// already exists
	    MSRPLOG( "MSRPServer: MSRPServer already running" )
	    User::Leave( KErrAlreadyExists );
		}

    MSRPLOG( "MSRPServer: StartServerL exit" )
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Main implements the executable entry function.
// Target type of the is EXE.
// Creates a cleanup stack and runs the server.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    // Get a new clean-up stack.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD( error, StartServerL() );
    delete cleanup;
    __UHEAP_MARKEND;
    return error;
    }

//  End of File
