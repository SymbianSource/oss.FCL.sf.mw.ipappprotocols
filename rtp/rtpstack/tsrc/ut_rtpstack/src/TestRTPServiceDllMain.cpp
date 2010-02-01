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
#include "UT_CLocalAddrResolver.h"
#include "UT_CRtpAPI.h"
#include "UT_CRtpComm.h"
#include "UT_CRtpCommRecv.h"
#include "UT_CRtpCommSend.h"
#include "UT_CRtpManager.h"
#include "UT_CRtpPacket.h"
#include "UT_CRtpRecvStream.h"
#include "UT_CRtpSDES.h"
#include "UT_CRtpSendItem.h"
#include "UT_CRtpSession.h"
#include "UT_CRtpStream.h"
#include "UT_CRtpTimer.h"
#include "UT_CRtpTranStream.h"
#include "UT_CRtpStpPacket.h"
#include "UT_TRtpHeaderExtension.h"
#include "UT_TRtpSendHeader.h"
#include "UT_TRtpRecvHeader.h"
#include "UT_TRtpUtil.h"
#include "UT_CRtpSessionSRTP.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>

/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    CEUnitTestSuite* rootSuite = CEUnitTestSuite::NewLC(_L("Wizard generated"));
    
    rootSuite->AddL( UT_CLocalAddrResolver::NewLC() );
    CleanupStack::Pop();
    
    rootSuite->AddL( UT_CRtpAPI::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpComm::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpCommRecv::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpCommSend::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpManager::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpPacket::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpRecvStream::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpSDES::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpSendItem::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpSession::NewLC() );
    CleanupStack::Pop();

	rootSuite->AddL( UT_CRtpSessionSrtp::NewLC() );
    CleanupStack::Pop();
    
    rootSuite->AddL( UT_CRtpStream::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpTimer::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpTranStream::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_CRtpStpPacket::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_TRtpHeaderExtension::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_TRtpSendHeader::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_TRtpRecvHeader::NewLC() );
    CleanupStack::Pop();

    rootSuite->AddL( UT_TRtpUtil::NewLC() );
    CleanupStack::Pop();
    
    CleanupStack::Pop(); // rootSuite
    
    return rootSuite;
    }

#ifndef __SECURE_API__
/**
 * Standard Symbian DLL entry point function.
 */
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

//  END OF FILE






