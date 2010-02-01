/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/



/**
 @file
*/
 
#if (!defined __RTP0_STEP_H__)
#define __RTP0_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_rtpsuitestepbase.h"
#include <rtp.h>
#include <rtpprepostprocessingcallback.h>
#include "te_rtpsuiteserver.h"

class CrtpStep : public CTe_rtpSuiteStepBase
	{
public:
	CrtpStep();
	~CrtpStep();
	inline void SetServer(CTe_rtpSuite* aServer);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
protected:
	CTe_rtpSuite* iServer;
	TBool iGotSendSucceeded;
	TBool iGotPacketReceived;
	TBool iGotNewSource;
	};

inline void CrtpStep::SetServer(CTe_rtpSuite* aServer)
	{
		iServer = aServer;
	}

class CrtpOpenSessionStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class CrtpOpenSessionTwoStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpCloseSessionTwoStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};
#endif

class CrtpCloseSessionStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpMakePacketStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSendPacketStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpStartSchedulerStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSetMarkerStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpCheckMarkerStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpMultipleSessionStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class 	CrtpMultipleSessionCloseStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class CrtpMultipleSessionTwoStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class 	CrtpMultipleSessionTwoCloseStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};
#endif

class CrtpSetHeaderExtensionStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpCheckHeaderExtensionStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSetPayloadTypeStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpCheckPayloadTypeStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSetTimestampStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpCheckTimestampStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpRegisterNonRtpDataStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTimedStepRtp: public CrtpStep
	{
protected:
	CPeriodic *iPeriodic;
	TBool  iTimerExpired;
public:
	void InitTimerL(TThreadPriority aPriority);
	void DeInitTimerL();
	static TInt LocalTimerCallback(TAny *aPtr);
	};

class CrtpSendNonRtpDataStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpReceiveNonRtpDataStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpUnregisterNonRtpDataStep : public CTimedStepRtp
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

class CrtpRegisterNonRtpAgainStep : public CTimedStepRtp
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

class CrtpPreAndPostProcessingTestStep : public CTimedStepRtp, MPrePostProcessingCallback
	{
	TBool iProcessingOK;
	TBool iPassCallBack;
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TInt PreRtpProcessing(TDes8&);
	virtual void PostRtpProcessing(TDes8&);
	virtual TInt PreRtcpProcessing(TDes8&);
	virtual void PostRtcpProcessing(TDes8&);
	};

class CrtpChangeRemoteAddrToLoopStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSetupLocalRecvSockStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpRecvOnLocalSockStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSetAndGetOptStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpSetSamplingRateStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CrtpCheckRecvPayloadStep : public CrtpStep
	{
public:
	virtual TVerdict doTestStepL();
	};


_LIT(KrtpStep,"rtpStep");
_LIT(KrtpOpenSessionStep,"rtpOpenSessionStep");
_LIT(KrtpCloseSessionStep,"rtpCloseSessionStep");
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
_LIT(KrtpOpenSessionTwoStep,"rtpOpenSessionTwoStep");
_LIT(KrtpCloseSessionTwoStep,"rtpCloseSessionTwoStep");
_LIT(KrtpMultipleSessionTwoStep,"rtpMultipleSessionTwoStep");
_LIT(KrtpMultipleSessionTwoCloseStep,"rtpMultipleSessionTwoCloseStep");
#endif
_LIT(KrtpMakePacketStep,"rtpMakePacketStep");
_LIT(KrtpSendPacketStep,"rtpSendPacketStep");
_LIT(KrtpStartSchedulerStep,"rtpStartSchedulerStep");
_LIT(KrtpSetMarkerStep,"rtpSetMarkerStep");
_LIT(KrtpCheckMarkerStep,"rtpCheckMarkerStep");
_LIT(KrtpMultipleSessionStep,"rtpMultipleSessionStep");
_LIT(KrtpMultipleSessionCloseStep,"rtpMultipleSessionCloseStep");
_LIT(KrtpSetHeaderExtensionStep,"rtpSetHeaderExtensionStep");
_LIT(KrtpCheckHeaderExtensionStep,"rtpCheckHeaderExtensionStep");
_LIT(KrtpSetPayloadTypeStep,"rtpSetPayloadTypeStep");
_LIT(KrtpCheckPayloadTypeStep,"rtpCheckPayloadTypeStep");
_LIT(KrtpSetTimestampStep,"rtpSetTimestampStep");
_LIT(KrtpCheckTimestampStep,"rtpCheckTimestampStep");
_LIT(KrtpRegisterNonRtpDataStep,"rtpRegisterNonRtpDataStep");
_LIT(KrtpSendNonRtpDataStep,"rtpSendNonRtpDataStep");
_LIT(KrtpReceiveNonRtpDataStep,"rtpReceiveNonRtpDataStep");
_LIT(KrtpUnregisterNonRtpDataStep,"rtpUnregisterNonRtpDataStep");
_LIT(KrtpRegisterNonRtpAgainStep,"rtpRegisterNonRtpAgainStep");
_LIT(KrtpPreAndPostProcessingTestStep,"rtpPreAndPostProcessingTestStep");
_LIT(KrtpPreProcessingFailTestStep,"rtpPreProcessingFailTestStep");
_LIT(KrtpChangeRemoteAddrToLoopStep,"rtpChangeRemoteAddrToLoopStep");
_LIT(KrtpSetupLocalRecvSockStep,"rtpSetupLocalRecvSockStep");
_LIT(KrtpRecvOnLocalSockStep,"rtpRecvOnLocalSockStep");
_LIT(KrtpSetAndGetOptStep,"rtpSetAndGetOptStep");
_LIT(KrtpSetSamplingRateStep,"rtpSetSamplingRateStep");
_LIT(KrtpCheckRecvPayloadStep,"rtpCheckRecvPayloadStep");

#endif
