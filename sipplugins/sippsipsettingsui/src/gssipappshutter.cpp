/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Exits the application when activated
*
*/


// INCLUDE FILES

#include    <aknview.h>
#include 	<gsplugininterface.h>
#include    "gssipappshutter.h"
#include    "gssipsettingsplugin.hrh"
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGSSIPAppShutter::CGSSIPAppShutter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSSIPAppShutter::CGSSIPAppShutter( 
    CGSPluginInterface* aAppUi ) : 
    CActive( EPriorityStandard ),
    iAppUi( aAppUi )
    {
    __GSLOGSTRING("CGSSIPAppShutter::CGSSIPAppShutter" )
    }

// -----------------------------------------------------------------------------
// CGSSIPAppShutter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSSIPAppShutter::ConstructL()
    {
    __GSLOGSTRING("CGSSIPAppShutter::ConstructL" )
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CGSSIPAppShutter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSSIPAppShutter* CGSSIPAppShutter::NewL( 
    CGSPluginInterface* aAppUi )
    {
    __GSLOGSTRING("CGSSIPAppShutter::NewL" )
    CGSSIPAppShutter* self = new ( ELeave ) CGSSIPAppShutter( aAppUi );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
CGSSIPAppShutter::~CGSSIPAppShutter()
    {
    __GSLOGSTRING("CGSSIPAppShutter::~CGSSIPAppShutter" )
    Cancel();
    }
 
// -----------------------------------------------------------------------------
// CGSSIPAppShutter::RequestAppShutdown
// Activates this active object
// -----------------------------------------------------------------------------
//        
void CGSSIPAppShutter::RequestAppShutdown()
    {
    __GSLOGSTRING("CGSSIPAppShutter::RequestAppShutdown Start" )
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    __GSLOGSTRING("CGSSIPAppShutter::RequestAppShutdown End" )
    }

// -----------------------------------------------------------------------------
// CGSSIPAppShutter::DoCancel
// Called when this active object is cancelled, does nothing
// -----------------------------------------------------------------------------
//
void CGSSIPAppShutter::DoCancel()    
    {    
    }

// -----------------------------------------------------------------------------
// CGSSIPAppShutter::RunL()
// Called when this active object request completion event occurs
// -----------------------------------------------------------------------------
//
void CGSSIPAppShutter::RunL()
    {
    __GSLOGSTRING("CGSSIPAppShutter::RunL" )
    iAppUi->HandleCommandL( EGSCmdAppShutterExit );    
    }

//  End of File  
