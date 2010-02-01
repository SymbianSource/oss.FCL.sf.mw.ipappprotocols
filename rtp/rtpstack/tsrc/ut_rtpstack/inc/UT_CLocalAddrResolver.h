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




#ifndef __UT_CLOCALADDRRESOLVER_H__
#define __UT_CLOCALADDRRESOLVER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include <in_sock.h>

//  FORWARD DECLARATIONS
class CLocalAddrResolver;

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
NONSHARABLE_CLASS( UT_CLocalAddrResolver )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CLocalAddrResolver* NewL();
        static UT_CLocalAddrResolver* NewLC();
        /**
         * Destructor
         */
        ~UT_CLocalAddrResolver();

    private:    // Constructors and destructors

        UT_CLocalAddrResolver();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_CLocalAddrResolver_NewLL();
        
        
         void UT_CLocalAddrResolver_NewLCL();
        
        
         void UT_CLocalAddrResolver_GetLocalAddrLL();
        
		 void UT_CLocalAddrResolver_SetAddrL(  );

		void UT_CLocalAddrResolver_CheckAndSetAddrL();
	
	private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
		private:    // Data
        RSocketServ iSocketServ;
        RConnection iConnection;
        CLocalAddrResolver* iLocalAddrResolver;
    };

#endif      //  __UT_CLOCALADDRRESOLVER_H__

// End of file
