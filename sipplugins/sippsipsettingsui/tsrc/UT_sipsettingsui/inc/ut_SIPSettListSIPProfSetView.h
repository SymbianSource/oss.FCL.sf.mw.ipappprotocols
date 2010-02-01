
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


#ifndef __UT_SIPSettListSIPProfSetView_H__
#define __UT_SIPSettListSIPProfSetView_H__


//  EXTERNAL INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitmacros.h>
#include "SIPSettListSIPProfSetView.h"
#include "GSSIPModel.h"

//  FORWARD DECLARATIONS
class CSIPSettListSIPProfSetView;

//  CLASS DEFINITION
/**
 * Tester class for UT_SIPSettListSIPProfSetView. 
 * 
 */
class UT_SIPSettListSIPProfSetView : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_SIPSettListSIPProfSetView* NewL();
        static UT_SIPSettListSIPProfSetView* NewLC();
        
        /**
         * Destructor
         */
        ~UT_SIPSettListSIPProfSetView();

    private: // Constructors and destructors

    	UT_SIPSettListSIPProfSetView();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
      
        void  UT_SIPSettListSIPProfSetView_DynInitMenuPaneL();
        
    private:    // Data
        EUNIT_DECLARE_TEST_TABLE;
        CSIPSettListSIPProfSetView* iView;
        CGSSIPModel* iSipModel;
    };

#endif  // __UT_SIPSettListSIPProfSetView_H__

// End of file
