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

#include "getlocalentrystep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"

CgetlocalentryStep::~CgetlocalentryStep()
/**
 * Destructor
 */
	{
	}

CgetlocalentryStep::CgetlocalentryStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KgetlocalentryStep);
	}

TVerdict CgetlocalentryStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return 	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	}


TVerdict CgetlocalentryStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    CRtpSourceEntry *entry = NULL;
    
    INFO_PRINTF1(_L("CgetlocalentryStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    {
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    }
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() - Failed"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC3));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() - Failed"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    entry = collMgr->GetLocalEntry();
    if(!entry || entry->iSRC != iSSRC1)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() - Failed - Entry Returned is Not Local"));
    	delete collMgr;
    	return TestStepResult();
    }
   
    /* Just to flush the entry cache */
    entry = collMgr->FindEntry(iSSRC3);
    
   
    entry = collMgr->GetLocalEntry();
    if(!entry || entry->iSRC != iSSRC1)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() - Failed - Entry Returned is Not Local"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Testing Set and get Local SRC */
    collMgr->SetLocalSSRC(iSSRC4);
    if(collMgr->LocalSSRC() != iSSRC4)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() - Failed - Entry Returned is Not Local"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    TRAPD(ret,collMgr->SetLocalCnameL(iCname4));
    if(KErrNone == ret )
    	{
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() SetLocalCnameL- Failed -CName Shud not be updated"));
    	delete collMgr;
    	return TestStepResult();
    	}
 
 
    if(collMgr->LocalCname() != iCname1)
    	{
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CgetlocalentryStep::doStepL() LocalCname- Failed"));
    	delete collMgr;
    	return TestStepResult();
    	}
  
    
    delete collMgr;
	SetTestStepResult(EPass);    
	return TestStepResult();
	}



TVerdict CgetlocalentryStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
