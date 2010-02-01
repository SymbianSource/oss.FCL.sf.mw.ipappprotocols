/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_SIPADAPTER_H__
#define __T_SIPADAPTER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>

//  FORWARD DECLARATIONS
class CWPProvisioningFile;
class CWPEngine;

#include <e32def.h>
#include <e32base.h>
#include <f32file.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( T_SIPAdapter ) : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors
        /**
         * Two phase construction
         */
        static T_SIPAdapter* NewL();
        static T_SIPAdapter* NewLC();

        /**
         * Destructor
         */
        ~T_SIPAdapter();

    private:    // Constructors and destructors
        T_SIPAdapter();
        void ConstructL();

    public:     // From observer interface

    private:    // New methods
        void SetupL();
        void Setup2L();
        void SaveDocumentL( const TDesC16& aFilename );
        void Teardown();
        void StoreL();
        void CheckSettingsL();
        void ResetSettingsL();
        void DummyL();

    private:    // Data
        EUNIT_DECLARE_TEST_TABLE; 
        HBufC8* iProvisioningDoc;
        TPtr8 iProvData;
        RFs iFs;
        RFile iFile;
    };

#endif      //  __T_SIPADAPTER_H__

// End of file
