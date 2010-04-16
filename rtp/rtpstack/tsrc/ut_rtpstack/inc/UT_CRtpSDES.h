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




#ifndef __UT_CRTPSDES_H__
#define __UT_CRTPSDES_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include "rtpsdes.h"

//  FORWARD DECLARATIONS
class CRtpSDES;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_CRtpSDES )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CRtpSDES* NewL();
        static UT_CRtpSDES* NewLC();
        /**
         * Destructor
         */
        ~UT_CRtpSDES();

    private:    // Constructors and destructors

        UT_CRtpSDES();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CRtpSDES_NewLL();
        
        
         void UT_CRtpSDES_NewL_OneL();
        
        
         void UT_CRtpSDES_GetSDESL();
        
        
         void UT_CRtpSDES_SetSDESL();
        
        
         void UT_CRtpSDES_AddSsrcToSdesLL();
        
        
         void UT_CRtpSDES_GetSdesSsrcL();
        

    private:    // Data
		CRtpSDES* iRtpSdes;
        TRtpSdesParams iSdesParams;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CRTPSDES_H__

// End of file
