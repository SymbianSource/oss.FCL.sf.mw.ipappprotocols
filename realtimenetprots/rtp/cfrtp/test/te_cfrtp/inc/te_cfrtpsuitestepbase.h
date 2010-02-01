/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* 
*
*/



/**
 @file 
 @internalAll
*/

#if (!defined __TE_CFRTP_STEP_BASE__)
#define __TE_CFRTP_STEP_BASE__



#include <e32base.h>
#include <e32cons.h>

#include <in_sock.h>
#include <es_sock.h>
#include <test/testexecutestepbase.h>

#include "rtp_uid.h"

#include <cs_subconparams.h>
#include <commdbconnpref.h>
#include "rtp_subconparams.h"
#include "dummy_rtp_subconparams.h"

/* The Sections in the config file */
_LIT(KTe_Cfrtp_LocalAddr,"LOCALADDR");

_LIT(KTe_Cfrtp_DestIpAddr1,"DESTIPADDR1");
_LIT(KTe_Cfrtp_DestIpAddr2,"DESTIPADDR2");

_LIT(KTe_Cfrtp_MaxDropOut,"MAXDROPOUT");
_LIT(KTe_Cfrtp_MaxMisOrder,"MAXMISORDER");
_LIT(KTe_Cfrtp_MinSequential,"MINSEQUENTIAL");
_LIT(KTe_Cfrtp_AutoSend,"AUTOSEND");
_LIT(KTe_Cfrtp_RtpTimeConv,"RTPTIMECONV");
_LIT(KTe_Cfrtp_RtpTimeNow,"RTPTIMENOW");
_LIT(KTe_Cfrtp_BandWidth,"BANDWIDTH");
_LIT(KTe_Cfrtp_TierId,"TIERID");

_LIT(KTe_Cfrtp_CNAME1,"CNAME1");

_LIT(KTe_Cfrtp_DestPort1,"DESTRTPPORT1");
_LIT(KTe_Cfrtp_DestPort2,"DESTRTPPORT2");
_LIT(KTe_Cfrtp_LocalPort1,"LOCALRTPPORT1");
_LIT(KTe_Cfrtp_RtcpInterval,"RTCPINTERVAL");

/* Section where all the default values are present */
_LIT(KTe_Cfrtp_SectDefault,"DefaultSection");


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_CfrtpSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_CfrtpSuiteStepBase();
	CTe_CfrtpSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	virtual TInt InitSubConL();
	virtual TInt InitSocketsL();
	virtual TInt InitDummySubConL(CSubConRTPGenericParamSetDummy::TTestcaseId aTestId);

protected:
    RSocketServ    iSocketServ; 
    RConnection    iCon;
    RSubConnection iSubCon;
	RSocket        iRtpSocket;
	RSocket        iRtcpSocket;
	
	/* Initialization Parameters for RTP Stack */
	TInetAddr iIpAddrLocal;
	TInetAddr iIpAddrDest1; /* Destination where packets needs to be send */
	TInetAddr iIpAddrDest2;
	
	TInt iDestPort1;
	TInt iDestPort2;
	
	TInt  iLocalPort1;
	TInt  iMaxDropOut, iMaxMisOrder, iBandWidth, iRtpTimeNow, iRtpTimeConv;
	TInt  iMinSeq;
	TUint iTierId;
	TBool iAutoSend;
	TInt iRtcpInterval;
	
	TBuf8<100> iCName;
	};

#endif
