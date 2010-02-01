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
// @file
// @internalAll
// 
//

#include "te_ut_rtpcollisionmgrsuitestepbase.h"
#include "te_ut_rtpcollisionmgrsuitedefs.h"
#include <in_sock.h>
#include <utf.h>

/* The Sections in the config file */
_LIT(KTe_UT_SSRC1,"SSRC1");
_LIT(KTe_UT_SSRC2,"SSRC2");
_LIT(KTe_UT_SSRC3,"SSRC3");
_LIT(KTe_UT_SSRC4,"SSRC4");

_LIT(KTe_UT_LOCALADDR,"LOCALADDR");

_LIT(KTe_UT_IPADDR1,"IPADDR1");
_LIT(KTe_UT_IPADDR2,"IPADDR2");
_LIT(KTe_UT_IPADDR3,"IPADDR3");
_LIT(KTe_UT_IPADDR4,"IPADDR4");


_LIT(KTe_UT_CNAME1,"CNAME1");
_LIT(KTe_UT_CNAME2,"CNAME2");
_LIT(KTe_UT_CNAME3,"CNAME3");
_LIT(KTe_UT_CNAME4,"CNAME4");

_LIT(KTe_UT_RTPPORT1,"RTPPORT1");
_LIT(KTe_UT_RTPPORT2,"RTPPORT2");
_LIT(KTe_UT_RTPPORT3,"RTPPORT3");
_LIT(KTe_UT_RTPPORT4,"RTPPORT4");


_LIT(KTe_UT_RTP_SECT_DEF,"DefaultSection");

// Device driver constants

TVerdict CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	/* Create a CrtpController Instance to Pass on to the Collision Manager */
	iRtpController = CRtpController::NewL();
	/* Now read all the relevant data from the Config File */
	TPtrC strIpAddr1,strIpAddr2,strIpAddr3,strIpAddr4;
	TPtrC strCName1,strCName2,strCName3,strCName4;
	TPtrC strLocalAddr;
	
	if(!GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_CNAME1,strCName1))
	{
		return TestStepResult();
	}
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iCname1,strCName1);
	if(!GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_CNAME2,strCName2))
	{
		return TestStepResult();
	}
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iCname2,strCName2);
	if(!GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_CNAME3,strCName3))
	{
		return TestStepResult();
	}
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iCname3,strCName3);
	if(!GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_CNAME4,strCName4))
	{
		return TestStepResult();
	}
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iCname4,strCName4);
	
	/* Read all the IP Address */
	TInetAddr inetAddr;
	if(GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_IPADDR1,strIpAddr1))
	{
		if(KErrNone == inetAddr.Input(strIpAddr1))
		{
			iIpAddr1 = inetAddr.Address();
		}
		else
		{
			return TestStepResult();
		}
	}
	else
	{
		return TestStepResult();
	}
	
	if(GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_IPADDR2,strIpAddr2))
	{
		if(KErrNone == inetAddr.Input(strIpAddr2))
		{
			iIpAddr2 = inetAddr.Address();
		}
		else
		{
			return TestStepResult();
		}
	}
	else
	{
		return TestStepResult();
	}
	
	if(GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_IPADDR3,strIpAddr3))
	{
		if(KErrNone == inetAddr.Input(strIpAddr3))
		{
			iIpAddr3 = inetAddr.Address();
		}
		else
		{
			return TestStepResult();
		}
	}
	else
	{
		return TestStepResult();
	}
	
	if(GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_IPADDR4,strIpAddr4))
	{
		if(KErrNone == inetAddr.Input(strIpAddr4))
		{
			iIpAddr4 = inetAddr.Address();
		}
		else
		{
			return TestStepResult();
		}
	}
	else
	{
		return TestStepResult();
	}
	
	if(GetStringFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_LOCALADDR,strLocalAddr))
	{
		if(KErrNone == inetAddr.Input(strLocalAddr))
		{
			iLocalIpAddr = inetAddr.Address();
		}
		else
		{
			return TestStepResult();
		}
	}
	else
	{
		return TestStepResult();
	}
	
	/* Read SSRCs */
	if(!GetHexFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_SSRC1,iSSRC1))
	{
		return TestStepResult();
	}
	
	if(!GetHexFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_SSRC2,iSSRC2))
	{
		return TestStepResult();
	}
	if(!GetHexFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_SSRC3,iSSRC3))
	{
		return TestStepResult();
	}
	if(!GetHexFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_SSRC4,iSSRC4))
	{
		return TestStepResult();
	}
	
	/* Read Port numbers */
	if(!GetIntFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_RTPPORT1,iPort1))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_RTPPORT2,iPort2))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_RTPPORT3,iPort3))
	{
		return TestStepResult();
	}
	if(!GetIntFromConfig(KTe_UT_RTP_SECT_DEF,KTe_UT_RTPPORT4,iPort4))
	{
		return TestStepResult();
	}
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_UT_RtpCollisionMgrSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	return TestStepResult();
	}

CTe_UT_RtpCollisionMgrSuiteStepBase::~CTe_UT_RtpCollisionMgrSuiteStepBase()
	{
	delete iRtpController;
	}

CTe_UT_RtpCollisionMgrSuiteStepBase::CTe_UT_RtpCollisionMgrSuiteStepBase()
	{
	}
