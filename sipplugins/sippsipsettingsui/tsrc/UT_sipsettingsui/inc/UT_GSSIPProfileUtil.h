/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __UT_GSSIPPROFILEUTIL__
#define __UT_GSSIPPROFILEUTIL__


//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>

//  FORWARD DECLARATIONS
class GSSIPProfileUtil;




//  CLASS DEFINITION
/**
 * Tester class for GSSIPProfileUtil. 
 * 
 */
class UT_GSSIPProfileUtil : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_GSSIPProfileUtil* NewL();
        static UT_GSSIPProfileUtil* NewLC();
        
        /**
         * Destructor
         */
        ~UT_GSSIPProfileUtil();

    private: // Constructors and destructors

        UT_GSSIPProfileUtil();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
		void UT_GSSIPProfileUtil_ProfileModificationsLL();
        void UT_GSSIPProfileUtil_CopyServerAttributesLL();
        
    private:    // Data

        GSSIPProfileUtil* iProfileUtil;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_GSSIPPROFILEUTIL__

// End of file