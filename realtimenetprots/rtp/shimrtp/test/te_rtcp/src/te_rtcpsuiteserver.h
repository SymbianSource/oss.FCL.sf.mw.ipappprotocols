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
// Header for class specific to rtcp test component
// 
//

/**
 @file
 @internalComponent
*/

#if (!defined __TE_RTCP_SERVER_H__)
#define __TE_RTCP_SERVER_H__
#include <test/testexecuteserverbase.h>
#include "rtp.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "rtpsession_internal.h"
#endif
#include <in_sock.h>

class CRTCPStep;

const TInt KDefaultPriority = 0;

class CTe_RTCPSuite : public CTestServer
	{
	public:
	static CTe_RTCPSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	public:
	
	RFs iFs;
	RFile	iFile;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
	RRtpSession_Internal iSession;
#else
	RRtpSession iSession;
#endif 
	RSocketServ iSocketServer;
	RSocket iRtpSocket;
	RSocket iRtcpSocket;
	RSocket iControlSocket;
	RSocket iDataSocket;
	RConnection iConnect;
	RSubConnection iSubConnect;
	RRtpSendSource iSendSource;
	RRtpReceiveSource iReceiveSource;
	RRtpReceiveSource iLastActiveSource;
	RRtpSendPacket iSendPacket;
	RRtpReceivePacket iReceivePacket;
	RRtpPacket* iPacket;
	TBool iConnected;
	CActiveScheduler* iScheduler;
	CRTCPStep *iCurrentStep;
	TUint iRTCPSRLastSentTime;
	TUint iRTCPAvgInterval;
	TUint iRTCPMinInterval;
	TUint iRTCPMaxInterval;
	TUint iRTCPSRRecvCount;
	TUint iAPPReceiveCount;
	TUint iBYEReceiveCount;
	TUint iRTCPRRRecvCount;
	TUint iSDESReceiveCount;
	TUint iRTPSendCount;
	TInt  iSystemTickPeriod;
	TBool iGotSdes;
	TBool iGotApp;
	TBool iGotSR;
	TBool iGotRR;
	TBool iGotBye;
	TBool iGotNewRR;
	TBool iGotSendSucceeded;
	TBool iGotSendFail;
	TBool iGotSessionFail;
	TBool iGotNewSource;
	TBool iGotBufferOverflow;
	TBool iGotUndersizedPacket;
	TBool iGotPacketReceived;
	TBool iGotCodecChange;
	TBool iGotMarker;
	TBool iGotMisorder;
	TBool iGotDropout;
	TBool iGotAnyEvent;
	TBool iEsdesNONE;
	TBool iEsdesCNAME;
	TBool iEsdesNAME;
	TBool iEsdesEMAIL;
	TBool iEsdesPHONE;
	TBool iEsdesLOC;
	TBool iEsdesTOOL;
	TBool iEsdesNOTE;
	TBool iEsdesPRIV;
	TUint iRTPSSRC;
	TUint iRRItemAboutSSRC;
		
	static void CbERtpSendSucceeded(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpSendFail(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void StartScheduler();
	static void CbERtpNewSourceL(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpSDES(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpRR(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpSR(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpAPP(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpBYE(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERTCPSRCheck(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERTCPSDESCheck(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpSessionFail(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpBufferOverflow(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpUndersizedPacket(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpNewRR(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbERtpPacketReceivedL(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static void CbNonRtcpPacketReceivedL(CTe_RTCPSuite* aPtr, const TRtpEvent& aEvent);
	static TUint32 Swap32(TUint32 aVal);
	static TUint16 Swap16(TUint16 aVal);
	void UpdateRTCPStatistics(CTe_RTCPSuite* aPtr);
	void SetRemoteClientModeL(CTe_RTCPSuite* aPtr, TUint iMode,TInetAddr& aAddr);
	TBool DoRTCPIntervalTest(CTe_RTCPSuite* aPtr);
	void SetEvent(TRtpEventType aType);
	void ResetEvents();
	void SetSDESType(TUint aSDESType);
	void ResetSDESTypes();
	
	protected:
	

	private:

	};

#endif
