// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Example CTestStep derived implementation
// @file
// @internalAll
// 
//

#include "newlstep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"
#include "rtpcollisionmng.h"

#include <e32base.h>
#include <in_sock.h>


CnewlStep::~CnewlStep()
/**
 * Destructor
 */
	{
	}

CnewlStep::CnewlStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KnewlStep);
	}

TVerdict CnewlStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	/* Create a CrtpController Instance to Pass on to the Collision Manager */
	iRtpController = CRtpController::NewL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CnewlStep::doTestStepL()
	{ 
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    
    INFO_PRINTF1(_L("CnewlStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(newlRes,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != newlRes)
    	{
    	INFO_PRINTF1(_L("CnewlStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    	}
    delete collMgr;
 
#if defined(_DEBUG)
    /* Null RTP Controller */
    TRAP(newlRes,collMgr = CRtpCollisionMng::NewL(NULL,iSSRC1,iCname1));
    if(KErrNone == newlRes)
    	{
    
    	delete collMgr;
    	INFO_PRINTF1(_L("CnewlStep::doStepL() (NULL,0xBABE,kUtCname) Failed - Panic Expected"));
    	return TestStepResult();
    	}
#endif
    
    INFO_PRINTF1(_L("CnewlStep::doStepL() -- Done"));
    SetTestStepResult(EPass);	
	return TestStepResult();
	}



TVerdict CnewlStep::doTestStepPostambleL()
/**
 * @return - TVerdict code * Override of base class virtual
 */
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
