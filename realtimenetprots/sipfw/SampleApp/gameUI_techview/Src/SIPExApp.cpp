
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



#include    "SIPExApp.h"
#include    "SIPExDoc.h"

#ifdef EKA2
#include <eikstart.h>
#endif

    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExApp::CreateDocumentL
// Creates a document and returns a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CSIPExApp::CreateDocumentL()
    {      
    CApaDocument* document = CSIPExDoc::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CSIPExApp::AppDllUid
// Returns the UID for the SIPEx application
// -----------------------------------------------------------------------------
//
TUid CSIPExApp::AppDllUid() const
    {
    // Return the UID for the SIP Example game application
    return KUidSIPExApp;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// NewApplication() implements the creation of the application class
// Creates an application, and return a pointer to it
// Returns: CApaApplication*: Pointer to the application class instance
// -----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication() 
    {
    CApaApplication* app = new CSIPExApp;
    return app;
    }

#ifdef EKA2

// -----------------------------------------------------------------------------
// E32MAin() is the entry point to the application DLL in application 
// framework V2
// Returns: TInt:   Always KErrNone
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
    
#else

// -----------------------------------------------------------------------------
// E32Dll() is the entry point to the application DLL
// DLL entry point in EKA1
// Returns: TInt:   Always KErrNone
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Dll( TDllReason /*aReason*/ )
    {
    return KErrNone;
    }
    
#endif // EKA2


// End of file

