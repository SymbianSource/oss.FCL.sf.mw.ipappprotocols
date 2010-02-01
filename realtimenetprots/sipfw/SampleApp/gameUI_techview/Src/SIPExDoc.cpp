
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// INCLUDE FILES
//



#include    "SIPExDoc.h"
#include    "SIPExAppUi.h"
#include    "SIPExGameEngine.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExDoc::CSIPSettinsgDoc
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPExDoc::CSIPExDoc( CEikApplication& aApp ) 
    : CEikDocument( aApp )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExDoc::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPExDoc::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CSIPExDoc::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPExDoc* CSIPExDoc::NewL( CEikApplication& aApp )
    {
    CSIPExDoc* self = new( ELeave ) CSIPExDoc( aApp );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
  
// -----------------------------------------------------------------------------
// CSIPExDoc::~CSIPExDoc
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExDoc::~CSIPExDoc()
    {
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// CSIPExDoc::CreateAppUiL
// Creates the application user interface and return a pointer to it
// -----------------------------------------------------------------------------
//
CEikAppUi* CSIPExDoc::CreateAppUiL()
    {
    CSIPExAppUi* appUi = new ( ELeave ) CSIPExAppUi;
    return appUi;
    }

// -----------------------------------------------------------------------------
// CSIPExDoc::CreateEngineL
// Creates the application engine
// -----------------------------------------------------------------------------
//
CSIPExEngine* CSIPExDoc::CreateEngineL( MSIPExGameObserver& aObserver )
    {    
    iEngine = CSIPExEngine::NewL( aObserver );
    return iEngine;
    }

//  End of File  
