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


#ifndef __UT_CNSMLDMSIPADAPTER_H__
#define __UT_CNSMLDMSIPADAPTER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES
#include <nsmldmmodule.h>
#include "smldmadapter.h"
#include "SIPProfileRegistryObserver.h"

//  FORWARD DECLARATIONS
class CNSmlDmSIPAdapter;
class CSmlDmCallback_stub;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
class UT_CNSmlDmSIPAdapter : public CEUnitTestSuiteClass, public MSIPProfileRegistryObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CNSmlDmSIPAdapter* NewL();
        static UT_CNSmlDmSIPAdapter* NewLC();
        /**
         * Destructor
         */
        ~UT_CNSmlDmSIPAdapter();

    private:    // Constructors and destructors

        UT_CNSmlDmSIPAdapter();
        void ConstructL();

    private:
    
        void ProfileRegistryEventOccurred(TUint32 aProfileId, MSIPProfileRegistryObserver::TEvent aEvent);

        void ProfileRegistryErrorOccurred(TUint32 aProfileId, TInt aError);
        
    private:

        void SetupL();
         
        void Setup2L();
        
        void Teardown();
         
        void Teardown2();
         
        void UT_CNSmlDmSIPAdapter_DDFVersionLL();
         
        void UT_CNSmlDmSIPAdapter_UpdateLeafObjectLL();
         
        void UT_CNSmlDmSIPAdapter_UpdateLeafObjectOtherLL();         
                 
        void UT_CNSmlDmSIPAdapter_DeleteObjectLL();  
        
        void UT_CNSmlDmSIPAdapter_FetchLeafObjectLL();
         
        void UT_CNSmlDmSIPAdapter_FetchLeafObjectOtherLL();        
    
        void UT_CNSmlDmSIPAdapter_ChildURIListLL();
    
        void UT_CNSmlDmSIPAdapter_AddNodeObjectLL();
         
        void UT_CNSmlDmSIPAdapter_DesToIntL();
         
        void UT_CNSmlDmSIPAdapter_RemoveLastURISegL();
         
        void UT_CNSmlDmSIPAdapter_NumOfURISegsL();
         
        void UT_CNSmlDmSIPAdapter_LastURISegL();
         
        void UT_CNSmlDmSIPAdapter_OtherFunctionsLL();
         
    private:    // Data
        EUNIT_DECLARE_TEST_TABLE; 
        CNSmlDmSIPAdapter* iNSmlDmSIPAdapter;

        CSmlDmCallback_stub* iDmCallback;
        
        HBufC8* iLuid1;
        
    };

#endif      //  __UT_CNSMLDMSIPADAPTER_H__

// End of file
