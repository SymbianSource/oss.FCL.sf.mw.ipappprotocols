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




//  CLASS HEADER
#include "UT_TRtpUtil.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtputil.h"

// CONSTRUCTION
UT_TRtpUtil* UT_TRtpUtil::NewL()
    {
    UT_TRtpUtil* self = UT_TRtpUtil::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_TRtpUtil* UT_TRtpUtil::NewLC()
    {
    UT_TRtpUtil* self = new( ELeave ) UT_TRtpUtil();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_TRtpUtil::~UT_TRtpUtil()
    {
    }

// Default constructor
UT_TRtpUtil::UT_TRtpUtil()
    {
    }

// Second phase construct
void UT_TRtpUtil::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_TRtpUtil::SetupL(  )
    {

    }

void UT_TRtpUtil::Teardown(  )
    {

    }

void UT_TRtpUtil::UT_TRtpUtil_Min16L(  )
    {
	TInt result(KErrNone); 
    TInt returnValue(0);
    
	//""));
    //"TRtpUtil::Min16( TInt aX, TInt aY )") );
	// "Test description: Confirms that the function returns min value of two 16 bit integers" ) );

	/***************************TestCase (MIN, MAX)*********************************/
	returnValue = TRtpUtil::Min16(KTUInt16Min, KTUInt16Max);

   	if(KTUInt16Max == returnValue)
   	    {
   	    EUNIT_ASSERT(ETrue);
   	    }
   	else 
   	    {
   	    //"TRtpUtil::Min16(KTUInt16Min, KTUInt16Max) - FAILED"));
   	    //"aX = %d, aY = %d, returnValue = %d"), KTUInt16Min, KTUInt16Max, returnValue);
   	    EUNIT_ASSERT(EFalse);
   	    }
    /***************************TestCase (MAX, MIN)*********************************/ 
	returnValue = TRtpUtil::Min16(KTUInt16Max, KTUInt16Min);

   	if(KTUInt16Max == returnValue)
   	    {
   	    EUNIT_ASSERT(ETrue);
   	    }
   	else
   	    {
   	    //"TRtpUtil::Min16(KTSInt16Max, KTUInt16Min) - FAILED"));
   	    //"aX = %d, aY = %d, returnValue = %d"), KTSInt16Max, KTUInt16Min, returnValue);
   	    EUNIT_ASSERT(EFalse);
   	    }
	/***************************TestCase (MIN - 1, MAX)*************************/
    returnValue = TRtpUtil::Min16((KTUInt16Min - 1), KTUInt16Max);
    
    if( KTUInt16Max == returnValue )
        {
        EUNIT_ASSERT(ETrue);
        }
    else
        {
        //"TRtpUtil::Min16((KTUInt16Min - 1), KTSInt16Max) - FAILED"));
        //"aX = %d, aY = %d, returnValue = %d"), (KTUInt16Min - 1), KTUInt16Max, returnValue);
        EUNIT_ASSERT(EFalse);
        }
    /***************************TestCase (MAX, MIN - 1)*************************/
    TRtpUtil::Min16(KTSInt16Max, (KTUInt16Min - 1) );
    
    if( KTUInt16Max == returnValue )
        {
        EUNIT_ASSERT(ETrue);
        }
    else
        {
        //"TRtpUtil::Min16(KTSInt16Max, (KTUInt16Min - 1)) - FAILED"));
        //"aX = %d, aY = %d, returnValue = %d"), KTSInt16Max, (KTUInt16Min - 1), returnValue);
        EUNIT_ASSERT(EFalse);
        }
    /***************************TestCase (MIN, MAX + 1)*************************/
    returnValue = TRtpUtil::Min16(KTUInt16Min, (KTSInt16Max + 1));
    
    if( (KTSInt16Max + 1) == returnValue )
        {
        EUNIT_ASSERT(ETrue);
        }
    else
        {
        //"TRtpUtil::Min16(KTUInt16Min, (KTSInt16Max + 1)) - FAILED"));
        //"aX = %d, aY = %d, returnValue = %d"), KTUInt16Min, (KTSInt16Max + 1), returnValue);
        EUNIT_ASSERT(EFalse);
        }
     /***************************TestCase (MAX + 1, MIN)*************************/
     returnValue =  (TRtpUtil::Min16((KTSInt16Max + 1), KTUInt16Min) );
     
     if( (KTSInt16Max + 1) == returnValue ) 
         {
         EUNIT_ASSERT(ETrue);
         }
    else
        {
        //"TRtpUtil::Min16((KTSInt16Max + 1), KTUInt16Min)) - FAILED"));
        //"aX = %d, aY = %d, returnValue = %d"),(KTSInt16Max + 1), KTUInt16Min, returnValue);
        EUNIT_ASSERT(EFalse);
        }
  	/****************************RETURN**********************************************/
  	EUNIT_ASSERT(ETrue);
		// "RESULT - TRtpUtil::Min16( TInt aX, TInt aY ) - passed" ));
	
    }

void UT_TRtpUtil::UT_TRtpUtil_Wrap16L(  )
    {
    	TInt result(KErrNone); 
    TInt returnValue(0);
    
	//""));
    //"TRtpUtil::Wrap16( TInt aX, TInt aY )") );
	// "Test description: Confirms that function Wrap16() returns 1 if the distance between two values is longer than 1 << 15 else 0" ) );

	/***************************TestCase (MIN, MAX)*********************************/
	returnValue = TRtpUtil::Wrap16(KTUInt16Min, KTUInt16Max);
	
    if(TRUE == returnValue)
        {
        EUNIT_ASSERT(ETrue);
        //"TRtpUtil::Wrap16(KTUInt16Min, KTSInt16Max) - passed"));
        }
    else 
        {
        //"TRtpUtil::Wrap16(KTUInt16Min, KTSInt16Max) - FAILED"));
        //"aX = %d, aY = %d, returnValue = %d"), KTUInt16Min, KTSInt16Max, returnValue);
        EUNIT_ASSERT(EFalse);
        }
	/***************************TestCase (MAX, MIN)*********************************/ 
    returnValue = TRtpUtil::Wrap16(KTUInt16Max, KTUInt16Min);
	
	if(TRUE == returnValue)
        {
       	EUNIT_ASSERT(ETrue);
       	}
    else
        {
        //"TRtpUtil::Wrap16(KTSInt16Max, KTUInt16Min) - FAILED"));
        //"aX = %d, aY = %d, returnValue = %d"), KTUInt16Max, KTUInt16Min, returnValue);
        EUNIT_ASSERT(EFalse);
        }
	/***************************TestCase (MIN - 1, MAX)*********************************/ 
    returnValue = TRtpUtil::Wrap16( (KTUInt16Min - 1), KTUInt16Max); 
	
	if( FALSE == returnValue )
	    {
	    EUNIT_ASSERT(ETrue);
	    }
	else
	    {
    	//"TRtpUtil::Wrap16((KTUInt16Min - 1), KTSInt16Max) - FAILED"));
    	//"aX = %d, aY = %d, returnValue = %d"), (KTUInt16Min - 1), KTSInt16Max, returnValue);
    	EUNIT_ASSERT(EFalse);
    	}
	/***************************TestCase (MAX, MIN - 1)*********************************/ 
  	returnValue = TRtpUtil::Wrap16( KTUInt16Max, (KTUInt16Min - 1) );
	
	if( FALSE == returnValue )
	    {
	    EUNIT_ASSERT(ETrue);
	    }
	else
	    {
    	//"TRtpUtil::Wrap16( KTSInt16Max, (KTUInt16Min - 1) ) - FAILED"));
    	//"aX = %d, aY = %d, returnValue = %d"), KTSInt16Max, (KTUInt16Min - 1), returnValue);
    	EUNIT_ASSERT(EFalse);
    	}
    /***************************TestCase (MIN, (1 << 15) + 1)*********************************/
	returnValue = TRtpUtil::Wrap16(KTUInt16Min, ((1 << 15) + 1) );

   	if(TRUE == returnValue)
   	    {
   	    EUNIT_ASSERT(ETrue);
   	    }
   	else 
   	    {
   	    //"TRtpUtil::Wrap16(KTUInt16Min, ((1 << 15) + 1)) - FAILED"));
   	    //"aX = %d, aY = %d, returnValue = %d"), KTUInt16Min, ((1 << 15) + 1), returnValue);
   	    EUNIT_ASSERT(EFalse);
   	    }
    /***************************TestCase ( (1 << 15) + 1, MIN)*********************************/ 
	returnValue = TRtpUtil::Wrap16( ( (1 << 15) + 1), KTUInt16Min );

   	if(TRUE == returnValue)
   	    {
   	    EUNIT_ASSERT(ETrue);
   	    }
   	else
   	    {
   	    //"TRtpUtil::Wrap16( ((1 << 15) + 1), KTUInt16Min) - FAILED"));
   	    //"aX = %d, aY = %d, returnValue = %d"), ((1 << 15) + 1), KTUInt16Min, returnValue);
   	    EUNIT_ASSERT(EFalse);
   	    }
   	EUNIT_ASSERT(ETrue);    
    }

void UT_TRtpUtil::UT_TRtpUtil_Sub32L(  )
    {
	TInt result(KErrNone);  
	TInt error( KErrNone );
    TInt returnValue(0);
    
	//""));
    //"TRtpUtil::Sub32( TInt aX, TInt aY )") );
	// "Test description: Confirms that function Sub32() subtract y from x and result will be wrapped if needed" ) );

	/***************************TestCase (MIN, MIN)*********************************/
    returnValue = TRtpUtil::Sub32(KTUInt32Min, KTUInt32Min);
	
   	if(KTUInt32Min == returnValue)
	    {
	    //"TRtpUtil::Sub32(KTUInt32Min, KTUInt32Min) - passed"));
	    }
	else 
	    {
	    //"TRtpUtil::Sub32(KTUInt32Min, KTUInt32Min) - FAILED"));
	    //"aX = %d, aY = %d, returnValue = %d"), KTUInt32Min, KTUInt32Min, returnValue);
	    EUNIT_ASSERT(EFalse);
	    }
    /***************************TestCase (MAX, MAX)*********************************/ 
    returnValue = TRtpUtil::Sub32( KTUInt32Max, KTUInt32Max );
	
   	if(KTUInt32Min == returnValue)
   	    {
   	    //"TRtpUtil::Sub32(KTUInt32Max, KTUInt32Max) - passed"));
   	    }
	else
	    {
	    //"TRtpUtil::Sub32(KTUInt32Max, KTUInt32Max) - FAILED"));
	    //"aX = %d, aY = %d, returnValue = %d"), KTUInt32Max, KTUInt32Max, returnValue);
	    EUNIT_ASSERT(EFalse);
	    }
    
	/***************************TestCase (MIN - 1, MAX)*********************************/ 
    returnValue = TRtpUtil::Sub32( (KTUInt32Min - 1), KTUInt32Max);
	
	if( returnValue == KTUInt32Min )
	    {
	    //"TRtpUtil::Sub32( (KTUInt32Min - 1), KTSInt32Max) - passed"));
	    }
	else
	    {
    	//"TRtpUtil::Sub32((KTUInt32Min - 1), KTSInt32Max) - FAILED"));
    	//"aX = %d, aY = %d, returnValue = %d"), (KTUInt32Min - 1), KTSInt32Max, returnValue);
    	EUNIT_ASSERT(EFalse);
    	}
	/***************************TestCase (MAX, MIN - 1)*********************************/ 
    returnValue = TRtpUtil::Sub32( KTUInt32Max, (KTUInt32Min - 1) );
	
	if( returnValue == KTUInt32Min )
	    {
	    //"TRtpUtil::Sub32( KTUInt32Max, (KTUInt32Min - 1) ) - passed"));
	    }
	else
	    {
    	//"TRtpUtil::Sub32( KTUInt32Max, (KTUInt32Min - 1) ) - FAILED"));
    	//"aX = %d, aY = %d, returnValue = %d"), KTUInt32Max, (KTUInt32Min - 1), returnValue);
    	EUNIT_ASSERT(EFalse);
    	}
    /***************************TestCase (MIN, 1)*********************************/
	returnValue = TRtpUtil::Sub32(KTUInt32Min, 1 );
	
	if(KTUInt32Max == returnValue)
	    {
	    //"TRtpUtil::Sub32(KTUInt32Min, 1) - passed"));
	    }
	else 
	    {
	    //"TRtpUtil::Sub32(KTUInt32Min, 1) - FAILED"));
	    //"aX = %d, aY = %d, returnValue = %d"), KTUInt32Min, 1, returnValue);
	    EUNIT_ASSERT(EFalse);
	    }
    /***************************TestCase (1, MIN)*********************************/ 
   returnValue = TRtpUtil::Sub32( 1, KTUInt32Min );
	
	if( 1 == returnValue)
	    {
	    //"TRtpUtil::Sub32( 1, KTUInt32Min ) - passed"));
	    }
	else 
	    {
	    //"TRtpUtil::Sub32( 1, KTUInt32Min ) - FAILED"));
	    //"aX = %d, aY = %d, returnValue = %d"), 1, KTUInt32Min, returnValue);
	    EUNIT_ASSERT(EFalse); 
	    }
	EUNIT_ASSERT(ETrue);    
    }

void UT_TRtpUtil::UT_TRtpUtil_StrcpyL(  )
    {
    TInt result(KErrNone);
    TInt returnValue(KErrNone);
    
   	//""));
    //"TRtpUtil::Strcpy( TUint8* aTarget, const TUint8* aSource )") );
	// "Test description: Confirms that function StrCopy() copies a string" ) );
    
    TBuf8<15> data;
    data.Format( _L8( "dummydata" ) );
    
    TBuf8<15> tempData;
    
    TUint8* temp =  const_cast<TUint8*>(data.Ptr());    
    TRtpUtil::Strcpy(temp, data.Ptr()); 
    
    if( data.Compare( tempData ) )
        result = KErrNone;
    
    else
        EUNIT_ASSERT(EFalse);
 
    }

void UT_TRtpUtil::UT_TRtpUtil_GtGetTimeL(  )
    {
    TInt result(KErrNone);
    //"TRtpUtil::GtGetTime()") );
	// "Test description: Confirms that function GtGetTime() returns a value" ) );

    TUint32 returnValue = TRtpUtil::GtGetTime();
    
    if(returnValue < 0)
        EUNIT_ASSERT(EFalse);
  	EUNIT_ASSERT(ETrue);   
  }

void UT_TRtpUtil::UT_TRtpUtil_RandomL(  )
    {
    TInt result( KErrNone );

	//""));
    //"TRtpUtil::Random()") );
	// "Test description: Gets three random 32-bit numbers and" ) );
	// "                  checks that they are different" ) );
    
    TInt64 seed( 42 );
    TUint32 returnValue1 = TRtpUtil::Random( seed );
    TUint32 returnValue2 = TRtpUtil::Random( seed );
    TUint32 returnValue3 = TRtpUtil::Random( seed );
    
    if ( returnValue1 == returnValue2 ||
         returnValue1 == returnValue3 ||
         returnValue2 == returnValue3 )
        {
        EUNIT_ASSERT(EFalse);
        }
    EUNIT_ASSERT(ETrue);     
    }

void UT_TRtpUtil::UT_TRtpUtil_FloatRandomL(  )
    {
    TInt result( KErrNone );

   	//""));
    //"TRtpUtil::FloatRandom()") );
	// "Test description: Gets three random 64-bit numbers and" ) );
	// "                  checks that they are different" ) );
    
    TInt64 seed( 42 );
    TReal returnValue1 = TRtpUtil::FloatRandom( seed );
    TReal returnValue2 = TRtpUtil::FloatRandom( seed );
    TReal returnValue3 = TRtpUtil::FloatRandom( seed );
    
    if ( returnValue1 == returnValue2 ||
         returnValue1 == returnValue3 ||
         returnValue2 == returnValue3 )
        {
        EUNIT_ASSERT(EFalse);
        }
   EUNIT_ASSERT(ETrue);     
  }

void UT_TRtpUtil::UT_TRtpUtil_PrintL(  )
    {
    EUNIT_ASSERT( ETrue );
    }

void UT_TRtpUtil::UT_TRtpUtil_Print_1L(  )
    {
    EUNIT_ASSERT( ETrue );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_TRtpUtil,
    "TRtpUtil class",
    "UNIT" )

EUNIT_TEST(
    "Min16 - test ",
    "TRtpUtil",
    "Min16",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_Min16L, Teardown)

EUNIT_TEST(
    "Wrap16 - test ",
    "TRtpUtil",
    "Wrap16",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_Wrap16L, Teardown)

EUNIT_TEST(
    "Sub32 - test ",
    "TRtpUtil",
    "Sub32",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_Sub32L, Teardown)

EUNIT_TEST(
    "Strcpy - test ",
    "TRtpUtil",
    "Strcpy",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_StrcpyL, Teardown)

EUNIT_TEST(
    "GtGetTime - test ",
    "TRtpUtil",
    "GtGetTime",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_GtGetTimeL, Teardown)

EUNIT_TEST(
    "Random - test ",
    "TRtpUtil",
    "Random",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_RandomL, Teardown)

EUNIT_TEST(
    "FloatRandom - test ",
    "TRtpUtil",
    "FloatRandom",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_FloatRandomL, Teardown)

EUNIT_TEST(
    "Print - test ",
    "TRtpUtil",
    "Print",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_PrintL, Teardown)

EUNIT_TEST(
    "Print - test ",
    "TRtpUtil",
    "Print",
    "FUNCTIONALITY",
    SetupL, UT_TRtpUtil_Print_1L, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
