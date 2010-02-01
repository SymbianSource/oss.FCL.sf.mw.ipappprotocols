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

#include "iscollisionorloopbackstep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"
#include "in_sock.h"

CiscollisionorloopbackStep::~CiscollisionorloopbackStep()
/**
 * Destructor
 */
	{
	}

CiscollisionorloopbackStep::CiscollisionorloopbackStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KiscollisionorloopbackStep);
	}

TVerdict CiscollisionorloopbackStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CiscollisionorloopbackStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    CRtpSourceEntry *entry = NULL;
    
    INFO_PRINTF1(_L("CiscollisionorloopbackStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    {
    	INFO_PRINTF1(_L("CiscollisionorloopbackStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    }
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CiscollisionorloopbackStep::doStepL() - Failed"));
    	delete collMgr;
    	return TestStepResult();
    }
    /* Set the RTP and RTCP Source address */
    TInetAddr addr(iIpAddr1,iPort1);
    entry->SetRtpSourceAddr(addr);
    addr.SetPort(iPort1+1);
    entry->SetRtcpSourceAddr(addr);
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC3));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CiscollisionorloopbackStep::doStepL() - Failed"));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Set the RTP and RTCP Source address */
    TInetAddr addr2(iIpAddr2,iPort2);
    entry->SetRtpSourceAddr(addr2);
    addr2.SetPort(iPort2+1);
    entry->SetRtcpSourceAddr(addr2);
    
    /* Now check a Normal recv from BABE2 */
    entry = collMgr->FindEntry(iSSRC2);
    if(collMgr->IsCollisionOrLoopback(entry,addr,ERtcp))
    {
    	/* This was not a Collision and was flagged as a Collision.*/
    	INFO_PRINTF1(_L("CiscollisionorloopbackStep::doStepL() - Failed-Normal Rec Flagged as Collision"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
    
    /* Now check a Collision */
    entry = collMgr->FindEntry(iSSRC2);
    /* Simulating a Collision. Make a Dummy IP Address */
    TInetAddr addr3(iIpAddr3,iPort3);
    
    if(!collMgr->IsCollisionOrLoopback(entry,addr3,ERtcp))
    {
    	/* This was a Collision and was flagged as not a Collision.*/
    	INFO_PRINTF1(_L("CiscollisionorloopbackStep::doStepL() - Failed - Collision Not Detected"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
    
    
    SetTestStepResult(EPass);
    delete collMgr;
    
	return TestStepResult();
	}



TVerdict CiscollisionorloopbackStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return 	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
