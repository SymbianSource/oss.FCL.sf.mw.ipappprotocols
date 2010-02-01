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

#include "createnewsourcelstep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"

CcreatenewsourcelStep::~CcreatenewsourcelStep()
/**
 * Destructor
 */
	{
	}

CcreatenewsourcelStep::CcreatenewsourcelStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KcreatenewsourcelStep);
	}

TVerdict CcreatenewsourcelStep::doTestStepPreambleL()
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	}


TVerdict CcreatenewsourcelStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    CRtpSourceEntry *entry = NULL;
    
    INFO_PRINTF1(_L("CcreatenewsourcelStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    {
    	INFO_PRINTF1(_L("CcreatenewsourcelStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    }
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CcreatenewsourcelStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC3));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CcreatenewsourcelStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Now verify the entry just created */
    entry = collMgr->FindEntry(iSSRC2);
    if(!entry || entry->iSRC != iSSRC2)
    {
    		
    	INFO_PRINTF1(_L("CcreatenewsourcelStep::doStepL() - Failed Wrong SSRC Entry/No Entry"));
    	/* Failed .. Return */
    	delete collMgr;
    	return TestStepResult();
    }
    
    
    /* Now verify the entry just created */
    entry = collMgr->FindEntry(iSSRC3);
    if(!entry || entry->iSRC != iSSRC3)
    {
    		
    	INFO_PRINTF1(_L("CcreatenewsourcelStep::doStepL() - Failed Wrong SSRC Entry/No Entry"));
    	/* Failed .. Return */
    	delete collMgr;
    	return TestStepResult();
    }
    
    
	SetTestStepResult(EPass);    
	return TestStepResult();
	}



TVerdict CcreatenewsourcelStep::doTestStepPostambleL()
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
