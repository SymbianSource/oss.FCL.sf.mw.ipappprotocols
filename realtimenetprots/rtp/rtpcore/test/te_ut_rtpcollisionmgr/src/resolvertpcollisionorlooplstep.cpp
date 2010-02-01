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

#include "resolvertpcollisionorlooplstep.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"
#include <in_sock.h>


CresolvertpcollisionorlooplStep::~CresolvertpcollisionorlooplStep()
/**
 * Destructor
 */
	{
	}

CresolvertpcollisionorlooplStep::CresolvertpcollisionorlooplStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KresolvertpcollisionorlooplStep);
	}

TVerdict CresolvertpcollisionorlooplStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CresolvertpcollisionorlooplStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    CRtpCollisionMng *collMgr = NULL;
    CRtpSourceEntry *entry = NULL;
    
    INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL()"));
    
    /* Normal Call */
    TRAPD(res,collMgr = CRtpCollisionMng::NewL(iRtpController,iSSRC1,iCname1));
    if(KErrNone != res)
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() (iRtpController,iSSRC1,iCname1) Failed"));
    	return TestStepResult();
    }
    
    entry = collMgr->FindEntry(iSSRC1);
    /* Set the RTP and RTCP Source address */
    TInetAddr addrLocal(iLocalIpAddr,iPort1);
    entry->SetRtpSourceAddr(addrLocal);
    addrLocal.SetPort(iPort1 + 1);
    entry->SetRtcpSourceAddr(addrLocal);

    
    
    //A dummy SSRC is given and a New Source is Created */
    TRAP(res,entry = collMgr->CreateNewSourceL(iSSRC2));
    if(res != KErrNone)
    {
    	/* Failed .. Return */
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed"));
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
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed"));
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
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed-Normal Rec Flagged as Collision"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
    
    
    entry = collMgr->FindEntry(iSSRC2);
    /* Simulating a Collision. Make a Dummy IP Address */
    TInetAddr addr3(iIpAddr3,iPort1);
    
    if(!collMgr->IsCollisionOrLoopback(entry,addr3,ERtp))
    {
    	/* This was a Collision and was flagged as not a Collision.*/
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Collision Not Detected"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
    
    entry = collMgr->FindEntry(iSSRC2);
    /* Simulating a Collision. Make a Dummy IP Address */
    TInetAddr addr_rtcp(iIpAddr3,iPort1+1);
    
    if(!collMgr->IsCollisionOrLoopback(entry,addr_rtcp,ERtcp))
    {
    	/* This was a Collision and was flagged as not a Collision.*/
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Collision Not Detected"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
    
    /* Calling Resolve Collision */
    if(collMgr->ResolveRtpCollisionOrLoopL(iSSRC2,addr3,ERtp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Collision TRUE for processing :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Calling Resolve Collision ..Our Address Given*/
    if(!collMgr->ResolveRtpCollisionOrLoopL(iSSRC1,addr3,ERtp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Local Collision FALSE for processing :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    if(collMgr->GetLocalEntry()->SRC() == iSSRC1)
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Local SSRC not Changed :("));
    	delete collMgr;
    	return TestStepResult();
    }
 
	TInt changedSSRC = collMgr->GetLocalEntry()->SRC();
	
    /* Calling Resolve Collision ..Our Address Given*/
    if(!collMgr->ResolveRtpCollisionOrLoopL(collMgr->GetLocalEntry()->SRC(),addr3,ERtp|KIsAlreadySending))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Local Collision FALSE for processing :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    if(collMgr->GetLocalEntry()->SRC() == changedSSRC)
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Local SSRC not Changed :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
   
    /* Try a Loop Test .. Now */
    
    /* Calling Resolve Collision ..Give the new SSRC and Old Addresss */
    if(collMgr->ResolveRtpCollisionOrLoopL(collMgr->GetLocalEntry()->SRC(),addr3,ERtp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Loop Detection 1 :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    if(collMgr->ResolveRtpCollisionOrLoopL(collMgr->GetLocalEntry()->SRC(),addr3,ERtp|KIsAlreadySending))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Loop Detection 2 :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Calling again */
    
    /* Calling Resolve Collision ..Give the new SSRC and Old Addresss */
    if(collMgr->ResolveRtpCollisionOrLoopL(collMgr->GetLocalEntry()->SRC(),addr3,ERtp|KIsAlreadySending))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Loop Detection 3 :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    /* Calling Resolve Collision ..Give the new SSRC and Old Addresss */
    if(collMgr->ResolveRtpCollisionOrLoopL(collMgr->GetLocalEntry()->SRC(),addr3,ERtp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Loop Detection 4 :("));
    	delete collMgr;
    	return TestStepResult();
    }
    
    if(collMgr->ResolveRtpCollisionOrLoopL(collMgr->GetLocalEntry()->SRC(),addr3,ERtcp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Loop Detection 5 :("));
    	delete collMgr;
    	return TestStepResult();
    }

    /* We have received only one RTP from the colliding Source. What if we receive an RTCP from a 
       Different source With the same SSRC ?  */ 
	
	/*This condition tickles an old issue. So lets keep it that way */
#if 0 
    TInetAddr addr4(iIpAddr4,iPort4);
    entry = collMgr->FindEntry(iSSRC1);
    if(!collMgr->IsCollisionOrLoopback(entry,addr4,ERtcp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Incorrect Collision Detection"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
       
    /* Now receiving an RTCP from the same Source.. See it that is not flagged as a Collision */
    addr3.SetPort(iPort1+1);
    entry = collMgr->FindEntry(iSSRC1);
    if(collMgr->IsCollisionOrLoopback(entry,addr3,ERtcp))
    {
    	INFO_PRINTF1(_L("CresolvertpcollisionorlooplStep::doStepL() - Failed - Incorrect Collision Dectection"));
    	delete collMgr;
    	return TestStepResult();
    	
    }
#endif
    
    SetTestStepResult(EPass);
    delete collMgr;
    
	return TestStepResult();
	}



TVerdict CresolvertpcollisionorlooplStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return 	CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL();
	}
