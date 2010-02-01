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




#ifndef __UT_TRTPUTIL_H__
#define __UT_TRTPUTIL_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class TRtpUtil;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

// CONSTANTS
// 0xf = 15 = 1111
// 0x7 =  7 = 0111

/////////////////////////16 BIT///////////////////////////////////////////////
// SIGNED 16 BIT INT
//1111 1111 1111 1111
const TInt16 KTSInt16Min = -(0x7fff);
//0111 1111 1111 1111
const TInt16 KTSInt16Max = 0x7fff;

// UNSIGNED 16 BIT INT
// 0000 0000 0000 0000
const TUint16 KTUInt16Min = 0;
//1111 1111 1111 1111 
const TUint16 KTUInt16Max = 0xffff;



////////////////////////32 BIT/////////////////////////////////////////////////
// SIGNED 32 BIT INT
//1111 1111 1111 1111 1111 1111 1111 1111
const TInt32 KTSInt32Min = -(0x7fffffff);
//0111 1111 1111 1111 1111 1111 1111 1111
const TInt32 KTSInt32Max = 0x7fffffff;

// UNSIGNED 16 BIT INT
// 0000 0000 0000 0000 0000 0000 0000 0000
const TUint32 KTUInt32Min = 0;
//1111 1111 1111 1111 1111 1111 1111 1111 
const TUint32 KTUInt32Max = 0xffffffff;
//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( UT_TRtpUtil )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_TRtpUtil* NewL();
        static UT_TRtpUtil* NewLC();
        /**
         * Destructor
         */
        ~UT_TRtpUtil();

    private:    // Constructors and destructors

        UT_TRtpUtil();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void UT_TRtpUtil_Min16L();
        
        
         void UT_TRtpUtil_Wrap16L();
        
        
         void UT_TRtpUtil_Sub32L();
        
        
         void UT_TRtpUtil_StrcpyL();
        
        
         void UT_TRtpUtil_GtGetTimeL();
        
        
         void UT_TRtpUtil_RandomL();
        
        
         void UT_TRtpUtil_FloatRandomL();
        
        
         void UT_TRtpUtil_PrintL();
        
        
         void UT_TRtpUtil_Print_1L();
        

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_TRTPUTIL_H__

// End of file
