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


#ifndef __UT_CSIPSETTLISTSIPSRVTAKEOVERCBAKEYPRESS__
#define __UT_CSIPSETTLISTSIPSRVTAKEOVERCBAKEYPRESS__


//  EXTERNAL INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitmacros.h>

class CSIPSettListSIPSrvTakeOverCBAKeyPress;

//  CLASS DEFINITION
/**
 * Tester class for CSIPSettListSIPSrvTakeOverCBAKeyPress. 
 * 
 */
class UT_CSIPSettListSIPSrvTakeOverCBAKeyPress : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSIPSettListSIPSrvTakeOverCBAKeyPress* NewL();
        static UT_CSIPSettListSIPSrvTakeOverCBAKeyPress* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CSIPSettListSIPSrvTakeOverCBAKeyPress();

    private: // Constructors and destructors

        UT_CSIPSettListSIPSrvTakeOverCBAKeyPress();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
		void UT_CSIPSettListSIPSrvTakeOverCBAKeyPress_OfferKeyEventLL();
        
    private:    // Data

        CSIPSettListSIPSrvTakeOverCBAKeyPress* iTakeOverCBAKeys;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CSIPSETTLISTSIPSRVTAKEOVERCBAKEYPRESS__

// End of file