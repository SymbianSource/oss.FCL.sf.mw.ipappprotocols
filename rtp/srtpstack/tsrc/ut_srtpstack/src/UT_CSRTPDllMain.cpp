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
#include "UT_CSRTPStream.h"
#include "UT_CSRTPSession.h" 
#include "UT_CSRTPStreamIn.h" 
#include "UT_CSRTPStreamOut.h"
#include "UT_SRTPUtils.h"
#include "UT_SRTPAuthentication_HMAC_SHA1.h"
#include "UT_SRTPAuthentication_NULL.h"
#include "UT_CSRTPCryptoHandler.h"
#include "UT_CSRTPCryptoHandlerSRTCP.h"
#include "UT_CSRTPCryptoHandlerSRTP.h"
#include "UT_CSRTPCipherAESCM128.h"
#include "UT_CSRTPCipherNULL.h"
#include "UT_CSRTPKeyDerivation_AESCM128.h"
#include "UT_CSRTPCryptoContext.h"
#include "UT_CSRTPMasterKey.h"
#include "UT_CSRTPMasterSalt.h"
#include "UT_CSrtpAESCTRCrypto.h"
#include "UT_CSrtpAuthentication_RCC.h"


//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>



/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("Wizard generated"));

rootSuite->AddL( UT_CSRTPSession::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPStream::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPStreamIn::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPStreamOut::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_SRTPUtils::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPCryptoHandler::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPCryptoHandlerSRTCP::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPCryptoHandlerSRTP::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_SRTPAuthentication_HMAC_SHA1::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_SRTPAuthentication_NULL::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPCipherAESCM128::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPCipherNULL::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPKeyDerivation_AESCM128::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPCryptoContext::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPMasterKey::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSRTPMasterSalt::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSrtpAESCTRCrypto::NewLC() );
CleanupStack::Pop();

rootSuite->AddL( UT_CSrtpAuthentication_RCC::NewLC() );
CleanupStack::Pop();


CleanupStack::Pop(); // rootSuite

return rootSuite;
    }

/**
 * Standard Symbian DLL entry point function.
 */
/*GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }  */


//  END OF FILE






