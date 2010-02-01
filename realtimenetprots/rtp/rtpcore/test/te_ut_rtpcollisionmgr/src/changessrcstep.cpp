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
// Example changessrcStep implementation
// @file
// @internalAll
// 
//

#include "changessrcstep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"

CchangessrcStep::~CchangessrcStep()
/**
 * Destructor
 */
	{
	}

CchangessrcStep::CchangessrcStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KchangessrcStep);
	}

TVerdict CchangessrcStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	}


TVerdict CchangessrcStep::doTestStepL()
	{
	__UHEAP_MARK;
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    
    //for removing warning 11/06/2008
    //CRtpSourceEntry *iEntry = NULL;
    
    INFO_PRINTF1(_L("CchangessrcStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    	{
    	INFO_PRINTF1(_L("CchangessrcStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	delete collMgr;
    	return TestStepResult();
    	}
    collMgr->ChangeSSRC(iSSRC2);
    if(collMgr->GetLocalEntry()->SRC() != iSSRC2)
    	{
    	INFO_PRINTF1(_L("CchangessrcStep::doStepL() SSRC Not Changed"));
    	delete collMgr;
    	return TestStepResult();
    	}
    delete collMgr;
    SetTestStepResult(EPass);
	__UHEAP_MARKEND;
    return TestStepResult();
	}




TVerdict CchangessrcStep::doTestStepPostambleL()
	{
	return CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
