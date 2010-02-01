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

#include "deleteentrystep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"

CdeleteentryStep::~CdeleteentryStep()
/**
 * Destructor
 */
	{
	}

CdeleteentryStep::CdeleteentryStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KdeleteentryStep);
	}

TVerdict CdeleteentryStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	}


TVerdict CdeleteentryStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    
    INFO_PRINTF1(_L("CdeleteentryStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    	{
    	INFO_PRINTF1(_L("CdeleteentryStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    	}
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    	{
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CdeleteentryStep::doStepL() - Failed "));
    	delete collMgr;
    	return TestStepResult();
    	}
    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,collMgr->CreateNewSourceL(iSSRC3));
    if(res != KErrNone)
    	{
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CdeleteentryStep::doStepL() - Failed "));
    	delete collMgr;
    	return TestStepResult();
    	}
    	
    collMgr->DeleteEntry(iSSRC2);
    if(collMgr->IsExistingSource(iSSRC2))
    {
    	INFO_PRINTF1(_L("CdeleteentryStep::doStepL() - Failed Entry not deleted"));
    	delete collMgr;
		return TestStepResult();
    }
    delete collMgr;
    SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CdeleteentryStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CdeleteentryStep"));
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
