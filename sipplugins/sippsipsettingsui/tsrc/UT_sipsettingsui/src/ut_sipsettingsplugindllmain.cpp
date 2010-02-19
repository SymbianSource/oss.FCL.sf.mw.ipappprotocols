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


//  USER INCLUDES
#include "ut_gssipprofileutil.h"
#include "ut_sipsettingscontainer.h"
#include "ut_sipsettlistsipprofsetcontainer.h"
#include "ut_sipsettlistsipsrvsetview.h"
#include "ut_sipSettlistsipprofsetview.h"
//  SYSTEM INCLUDES
#include <digia/eunit/ceunittestsuite.h>


// -----------------------------------------------------------------------------
// Test suite factory function.
// -----------------------------------------------------------------------------
//
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC( _L( "Main suite" ) );

    rootSuite->AddL( UT_GSSIPProfileUtil::NewLC() );
    CleanupStack::Pop();
    
    rootSuite->AddL( UT_CSIPSettingsContainer::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_SIPSettListSIPProfSetContainer::NewLC() );
    CleanupStack::Pop();    
    
    rootSuite->AddL( UT_SIPSettListSIPRegSetView::NewLC() );
    CleanupStack::Pop();
    
    rootSuite->AddL( UT_SIPSettListSIPProfSetView::NewLC() );
    CleanupStack::Pop();
    CleanupStack::Pop( rootSuite );

    return rootSuite;
    }
