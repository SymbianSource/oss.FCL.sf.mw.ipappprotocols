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

#include "getvalidentriesstep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"

CgetvalidentriesStep::~CgetvalidentriesStep()
/**
 * Destructor
 */
	{
	}

CgetvalidentriesStep::CgetvalidentriesStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KgetvalidentriesStep);
	}

TVerdict CgetvalidentriesStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return 	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	}


TVerdict CgetvalidentriesStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    CRtpSourceEntry *entry = NULL;
    
    INFO_PRINTF1(_L("CgetvalidentriesStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    {
    	INFO_PRINTF1(_L("CgetvalidentriesStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    }
    entry = collMgr->GetLocalEntry();
    entry->SetState(ERtpValid);
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetvalidentriesStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    entry->SetState(ERtpInvalid);
    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC3));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetvalidentriesStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    entry->SetState(ERtpValid);
    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC4));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetvalidentriesStep::doStepL() - Failed Res"));
    	delete collMgr;
    	return TestStepResult();
    }
    entry->SetState(ERtpInvalid);
    //we have 2 valid and 2 invalid entries.
    if(collMgr->GetValidEntries() == 2)
    	{
    	delete collMgr;
		SetTestStepResult(EPass);    
		return TestStepResult();	
    	}
    else
    	{
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetvalidentriesStep::doStepL() - Failed Valid Entries Wrong"));
    	delete collMgr;
    	return TestStepResult();   		
    	}
	}



TVerdict CgetvalidentriesStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CgetvalidentriesStep"));
	return 	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
