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
// Header for class specific to rtcp tests
// 
//

/**
 @file
 @internalComponent
*/

#if (!defined __RTCP_STEP_H__)
#define __RTCP_STEP_H__
#include <test/testexecutestepbase.h>
#include "../../../inc/rtpprepostprocessingcallback.h"
#include "te_rtcpsuiteserver.h"


class CRTCPStep : public CTestStep
	{
	public:
	CRTCPStep();
	~CRTCPStep();
	inline void SetServer(CTe_RTCPSuite* aServer);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	virtual void GotEvent(TRtpEventType) 
		{
		};
	protected:
	
	CTe_RTCPSuite* iServer;
	HBufC8*		iReadData;
	HBufC8*		iWriteData;

	private:	
	
	};
	
inline void CRTCPStep::SetServer(CTe_RTCPSuite* aServer)
	{
	iServer = aServer;
	}	
	
class COpenSession : public CRTCPStep
	{
	public:
    virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	};
class CCloseSession : public CRTCPStep
	{
	public:
 	virtual TVerdict doTestStepL();
	};
	
class CMakeRTPPacket : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};

class CSetFlags : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};

class CSetMarker : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};

class CSetPayload : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};
class CSetPayloadType : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};	
class CSetTimestamp : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};
class CEvent : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};	
class CStartSendingPackets : public CRTCPStep
	{
	CPeriodic *iPeriodic;
	TBool iSendComplete;
	public:
  	virtual TVerdict doTestStepPreambleL();
   	virtual TVerdict doTestStepL();
   	virtual TVerdict doTestStepPostambleL();
   	void GotEvent(TRtpEventType aType);
   	static TInt TimerCallback(TAny* aPtr);
	};	
class CStartScheduler : public CRTCPStep
	{
	public:
   	virtual TVerdict doTestStepL();
	};
class CCheckRTCPMinReq : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};
class CSendAPPPacket : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};
class CSendBYEPacket : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};   	   	   	
class CCheckRTCPInterval : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	}; 	
class CSetSDESPacket : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};	
class CSendRTCPPacket : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};
class CCheckForSRPacket : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};

class CCheckForSSRCConsistencyRR : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class COpenSession2 : public CRTCPStep
	{
	public:
	virtual TVerdict doTestStepPreambleL();
  	virtual TVerdict doTestStepL();
  	virtual TVerdict doTestStepPostambleL();
	};	
class CCloseSession2 : public CRTCPStep
	{
	public:
  	virtual TVerdict doTestStepL();
	};
#endif
	

class CRegisterNonRtcpDataStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CTimedStepRtcp: public CRTCPStep
	{
protected:
	CPeriodic *iPeriodic;
	TBool  iTimerExpired;
public:
	void InitTimerL(TThreadPriority aPriority);
	void DeInitTimerL();
	static TInt LocalTimerCallback(TAny *aPtr);
	};

class CSendNonRtcpDataStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CReceiveNonRtcpDataStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CRtcpPreAndPostProcessingTestStep : public CTimedStepRtcp, MPrePostProcessingCallback
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

class CChangeRemoteAddrToLoopStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSetupLocalRecvSockStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CRecvOnLocalSockStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSetAndGetOptStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};


class CRtcpStatisticsStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CrtcpSetandGetRtcpSocket : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};
	
class CRtcpStatisticsUnitTestStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSetRtcpIntervalStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSetRtpStreamParamsStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSetandGetAutoSendOptionStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};

class CSetSessionBandwidthStep : public CRTCPStep
	{
public:
	virtual TVerdict doTestStepL();
	};


_LIT(KRTCPStep,"RTCPStep");
_LIT(KOpenSession,"OpenSession");
_LIT(KCloseSession,"CloseSession");
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
_LIT(KOpenSession2,"OpenSession2");
_LIT(KCloseSession2,"CloseSession2");
#endif
_LIT(KMakeRTPPacket,"MakeRTPPacket");
_LIT(KRTCPSendPacket,"RTCPSendPacket");
_LIT(KSetFlags,"SetFlags");
_LIT(KSetMarker,"SetMarker");
_LIT(KSetPayload,"SetPayload");
_LIT(KSetPayloadType,"SetPayloadType");
_LIT(KSetTimestamp,"SetTimestamp");
_LIT(KEvent,"Event");
_LIT(KStartSendingPackets,"StartSendingPackets");
_LIT(KStartScheduler,"StartScheduler");
_LIT(KCheckRTCPMinReq,"CheckRTCPMinReq");
_LIT(KSendAPPPacket,"SendAPPPacket");
_LIT(KSendBYEPacket,"SendBYEPacket");
_LIT(KCheckRTCPInterval,"CheckRTCPInterval");
_LIT(KSetSDESPacket,"SetSDESPacket");
_LIT(KSendRTCPPacket,"SendRTCPPacket");
_LIT(KCheckForSRPacket,"CheckForSRPacket");
_LIT(KCheckForSSRCConsistencyRR,"CheckForSSRCConsistencyRR");
_LIT(KRegisterNonRtcpDataStep,"RegisterNonRtcpDataStep");
_LIT(KSendNonRtcpDataStep,"SendNonRtcpDataStep");
_LIT(KReceiveNonRtcpDataStep,"ReceiveNonRtcpDataStep");
_LIT(KRtcpPreAndPostProcessingTestStep,"RtcpPreAndPostProcessingTestStep");
_LIT(KChangeRemoteAddrToLoopStep,"ChangeRemoteAddrToLoopStep");
_LIT(KSetupLocalRecvSockStep,"SetupLocalRecvSockStep");
_LIT(KRecvOnLocalSockStep,"RecvOnLocalSockStep");
_LIT(KSetAndGetOptStep,"SetAndGetOptStep");
_LIT(KRtcpStatisticsStep,"RtcpStatisticsStep");
_LIT(KRtcpSetandGetRtcpSocket,"SetAndGetRtcpSocketStep");
_LIT(KRtcpStatisticsUnitTestStep,"RtcpStatisticsUnitTestStep");
_LIT(KSetRtcpIntervalStep,"SetRtcpIntervalStep");
_LIT(KSetRtpStreamParamsStep,"SetRtpStreamParamsStep");
_LIT(KSetandGetAutoSendOptionStep,"SetandGetAutoSendOptionStep");
_LIT(KSetSessionBandwidthStep,"SetSessionBandwidthStep");

#endif
