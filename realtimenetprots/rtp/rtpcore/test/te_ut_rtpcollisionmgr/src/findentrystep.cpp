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

#include "findentrystep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"

CfindentryStep::~CfindentryStep()
/**
 * Destructor
 */
	{
	}

CfindentryStep::CfindentryStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KfindentryStep);
	}

TVerdict CfindentryStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	}

 
TVerdict CfindentryStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    CRtpSourceEntry *entry = NULL;
    
    INFO_PRINTF1(_L("CfindentryStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    {
    	INFO_PRINTF1(_L("CfindentryStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    }
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CfindentryStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC3));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CfindentryStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Now verify the entry just created */
    entry = collMgr->FindEntry(iSSRC2);
    if(!entry || entry->iSRC != iSSRC2)
    {
    		
    	INFO_PRINTF1(_L("CfindentryStep::doStepL() - Failed Wrong SSRC Entry/No Entry"));
    	/* Failed .. Return */
    	delete collMgr;
    	return TestStepResult();
    }
    
    
    /* Now verify the entry just created */
    entry = collMgr->FindEntry(iSSRC3);
    if(!entry || entry->iSRC != iSSRC3)
    {
    		
    	INFO_PRINTF1(_L("CfindentryStep::doStepL() - Failed Wrong SSRC Entry/No Entry"));
    	/* Failed .. Return */
    	delete collMgr;
    	return TestStepResult();
    }
    
    
	SetTestStepResult(EPass);    
	return TestStepResult();
	}



TVerdict CfindentryStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
