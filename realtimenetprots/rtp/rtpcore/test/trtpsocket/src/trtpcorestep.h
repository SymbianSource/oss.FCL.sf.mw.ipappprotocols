// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for class specific to rtp unit tests
// 
//

/**
 @file
 @internalComponent
*/

#if (!defined RTP_CORE_STEP_H)
#define RTP_CORE_STEP_H
#include <testexecutestepbase.h>
#include "rtpcontrollerbase.h"
class CTRtpCoreServer;


class CTRtpCoreStep : public CTestStep
	{
public:
	~CTRtpCoreStep() {};
	inline void SetServer(CTRtpCoreServer* aServer);
	virtual TVerdict doTestStepL();
	virtual TVerdict doRtpTestStepL() = 0;
	TInt iError;
	TBool iIgnoreIncrease;
protected:

	CTRtpCoreServer* iServer;
	};

inline void CTRtpCoreStep::SetServer(CTRtpCoreServer* aServer)
	{
	iServer = aServer;
	}

class CCreateRtpController : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CActivateRtpController : public CTRtpCoreStep
	{
public:
	//virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CDeleteRtpController : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CTidyUp : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class COom : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class COpenRtpSocket : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CBasicSend : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CRtpController;
class CReceive : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	TVerdict doRtcpBeginL(TDes8& aRecvBuf, MRtpDataObserver *aDataObs, CRtpControllerBase* iController, CRtpControllerBase* iSendController);
	};

class CBasicReceive : public CReceive
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CInitialReceive : public CReceive
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CFixedHeader : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CRRFixedHeader : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CBYEReceive : public CReceive
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CIntervalTest : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

class CConsiderationTest : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};

_LIT(KCreateRtpController,"CreateRtpController");
_LIT(KDeleteRtpController,"DeleteRtpController");
_LIT(KActivateRtpController,"ActivateRtpController");
_LIT(KBasicSend,"BasicSend");
_LIT(KBasicReceive,"BasicReceive");
_LIT(KInitialReceive,"InitialReceive");
_LIT(KFixedHeader,"FixedHeader");
_LIT(KRRFixedHeader,"RRFixedHeader");
_LIT(KBYEReceive,"BYEReceive");
_LIT(KOom,"Oom");
_LIT(KIntervalTest,"RtcpInterval");
_LIT(KConsiderationTest,"Consideration");

#endif

