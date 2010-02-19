/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * ut_sipsettingscontainer.h
 *
 *  Created on: Nov 9, 2009
 *      Author: cninjayu
 */

#ifndef UT_SIPSETTINGSCONTAINER_H_
#define UT_SIPSETTINGSCONTAINER_H_

//  EXTERNAL INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitmacros.h>
#include "SIPSettingsContainer.h"


//  CLASS DEFINITION
/**
 * Tester class for CSIPSettingsContainer. 
 * 
 */
class UT_CSIPSettingsContainer : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CSIPSettingsContainer* NewL();
        static UT_CSIPSettingsContainer* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CSIPSettingsContainer();

    private: // Constructors and destructors

        UT_CSIPSettingsContainer();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
		void UT_CSIPSettingsContainer_OfferKeyEventLL();
        void UT_CSIPSettingsContainer_FocusChangedL();
        void UT_CSIPSettingsContainer_ProcessCommandLL();
        void UT_CSIPSettingsContainer_SetEmphasisL();

        
    private:    // Data

        CSIPSettingsContainer* iSipSettingsContainer;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif /* UT_SIPSETTINGSCONTAINER_H_ */
