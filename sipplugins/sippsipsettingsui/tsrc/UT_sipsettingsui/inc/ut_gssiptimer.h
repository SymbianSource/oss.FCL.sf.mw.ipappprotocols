
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


#ifndef __UT_GSSIPTIMER_H__
#define __UT_GSSIPTIMER_H__


//  EXTERNAL INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitmacros.h>

#include "gssiptimer.h"

//  FORWARD DECLARATIONS
class CGSSIPTimer;


//  CLASS DEFINITION
/**
 * Tester class for UT_GSSIPTimer. 
 * 
 */
class UT_GSSIPTimer : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_GSSIPTimer* NewL();
        static UT_GSSIPTimer* NewLC();
        
        /**
         * Destructor
         */
        ~UT_GSSIPTimer();

    private: // Constructors and destructors

        UT_GSSIPTimer();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
        void UT_GSSIPTimer_Beat();
        void UT_GSSIPTimer_Synchronize();
        void UT_GSSIPTimer_StartTimer();
        void UT_GSSIPTimer_StopTimer();
        void UT_GSSIPTimer_IsStarted();
        
    private:    // Data
        EUNIT_DECLARE_TEST_TABLE;
        CGSSIPTimer* iTimer;
        CCoeControl* iCoeControl;
        CTextListItemDrawer* iItemDraw;

    };

#endif  // __UT_GSSIPTIMER_H__

// End of file
