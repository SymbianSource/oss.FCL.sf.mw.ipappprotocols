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
 * ut_sipsettlistsipprofsetcontainer.h
 *
 *  Created on: 2009-11-6
 *      Author: cninshmi
 */

#ifndef UT_SIPSETTLISTSIPPROFSETCONTAINER_H_
#define UT_SIPSETTLISTSIPPROFSETCONTAINER_H_


//  EXTERNAL INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitmacros.h>
#include "SIPSettListSIPProfSetContainer.h"

//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 * Tester class for GSSIPProfileUtil. 
 * 
 */
class UT_SIPSettListSIPProfSetContainer : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_SIPSettListSIPProfSetContainer* NewL();
        static UT_SIPSettListSIPProfSetContainer* NewLC();
        
        /**
         * Destructor
         */
        ~UT_SIPSettListSIPProfSetContainer();

    private: // Constructors and destructors

    	UT_SIPSettListSIPProfSetContainer();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();

        void Teardown();

    private: // Test methods
		void TestNewL();
        void TestEditCurrentListItemL();
        void TestStoreSettingsL();
        void TestCountComponentControls();
        void TestComponentControl();
        void TestSizeChanged();
        void TestOfferKeyEventL();
        void TestHandleResourceChange();
        void TestGetHelpContext();
        void TestFocusChanged();
        
    private:    // Data

    	CSIPSettListSIPProfSetContainer* iSipProfContainer;

        EUNIT_DECLARE_TEST_TABLE;

    };


#endif /* UT_SIPSETTLISTSIPPROFSETCONTAINER_H_ */
