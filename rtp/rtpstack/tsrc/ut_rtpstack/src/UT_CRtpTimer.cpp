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
#include "UT_CRtpTimer.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtptimer.h"

// CONSTRUCTION
UT_CRtpTimer* UT_CRtpTimer::NewL()
    {
    UT_CRtpTimer* self = UT_CRtpTimer::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpTimer* UT_CRtpTimer::NewLC()
    {
    UT_CRtpTimer* self = new( ELeave ) UT_CRtpTimer();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpTimer::~UT_CRtpTimer()
    {
    }

// Default constructor
UT_CRtpTimer::UT_CRtpTimer()
    {
    }

// Second phase construct
void UT_CRtpTimer::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpTimer::SetupL(  )
    {
	iCounter = 10;
    //Create a CRtpTimer object
    iRtpTimer = CRtpTimer::NewL( CActive::EPriorityStandard, *this );
    }

void UT_CRtpTimer::Teardown(  )
    {
    delete iRtpTimer;
    iRtpTimer = NULL;
    }

void UT_CRtpTimer::UT_CRtpTimer_NewLL(  )
    {
   	TInt result(KErrNone);
	
    /***************************Test case***************************************/
     // Start RtpTimer that calls CRtpUtRtpTimer::OnExpired 
	 iRtpTimer->After( 100 );
	
	// Start Scheduler
	CActiveScheduler::Start();
	
	//OnExpired() has been called "iCounter" times
	EUNIT_ASSERT(iCounter == 0);
	   
    }
void UT_CRtpTimer::OnExpiredL( TInt /*aStatus*/ )
    {
    --iCounter;
     
    //Close iRtpTimer and stop CActiveScheduler
    if ( iCounter == 0 )
        {
        
        //Confirm that iRtpTimer isn't active
        while ( iRtpTimer->IsActive() )
        {
        }
            
        iRtpTimer->Cancel();
            
        CActiveScheduler::Stop();
        }
            
    else
        iRtpTimer->After(100); 
        
    // _L( "CRtpTimer: OnExpired - iCounter: %d"), iCounter );
    }
	    

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpTimer,
    "CRtpTimer",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpTimer",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpTimer_NewLL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
