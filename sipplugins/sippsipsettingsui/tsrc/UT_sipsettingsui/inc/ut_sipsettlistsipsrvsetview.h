
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


#ifndef __UT_SIPSettListSIPRegSetView_H__
#define __UT_SIPSettListSIPRegSetView_H__


//  EXTERNAL INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitmacros.h>
#include "SIPSettListSIPRegSetView.h"
#include "GSSIPModel.h"

//  FORWARD DECLARATIONS
class CSIPSettListSIPSrvSetView;

//  CLASS DEFINITION
/**
 * Tester class for UT_SIPSettListSIPRegSetView. 
 * 
 */
class UT_SIPSettListSIPRegSetView : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_SIPSettListSIPRegSetView* NewL();
        static UT_SIPSettListSIPRegSetView* NewLC();
        
        /**
         * Destructor
         */
        ~UT_SIPSettListSIPRegSetView();

    private: // Constructors and destructors

    	UT_SIPSettListSIPRegSetView();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
      
        void  UT_SIPSettListSIPRegSetView_DynInitMenuPaneL();
        
    private:    // Data
        EUNIT_DECLARE_TEST_TABLE;
        CSIPSettListSIPSrvSetView* iView;
        CGSSIPModel* iSipModel;

    };

#endif  // __UT_GSSIPTIMER_H__

// End of file
