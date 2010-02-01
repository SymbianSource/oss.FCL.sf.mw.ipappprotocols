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

#if (!defined __TE_RTP_SERVER_H__)
#define __TE_RTP_SERVER_H__
#include <test/testexecuteserverbase.h>
#include <rtp.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "rtpsession_internal.h"
#endif
#include <in_sock.h>


class CrtpStep;

//#include "Te_rtpSuiteStepBase.h"


class CTe_rtpSuite : public CTestServer
	{
public:
	static CTe_rtpSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

	static void PacketSentSuccess(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent);
	static void NewSourceL(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent);
	static void PacketReceiveSuccess(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent);
	static void NonRtpDataReceived(CTe_rtpSuite* aPtr, const TRtpEvent& aEvent);
	void ConfigureReflectorModeL(TBool aMode, TInetAddr& aAddr);

public:
	CrtpStep* iCurrentStep;

	CActiveScheduler* sched;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
	RRtpSession_Internal rtpSession;
	RRtpSession_Internal rtpSessionOne;
	RRtpSession_Internal rtpSessionTwo;
#else
	RRtpSession rtpSession;
	RRtpSession rtpSessionOne;
	RRtpSession rtpSessionTwo;
#endif 

	RConnection rConn;
	RConnection rConnOne;
	RConnection rConnTwo;

	RSubConnection rSubConn;
	RSubConnection rSubConnOne;
	RSubConnection rSubConnTwo;

	RSocketServ sockServ;
	RSocketServ sockServOne;
	RSocketServ sockServTwo;

	RRtpSendSource rtpSendSrc;
	RRtpSendPacket rtpSendPacket;

	RRtpReceiveSource rtpRecvSrc;
	RRtpReceivePacket rtpRecvPacket;

	TInt StopScheduler;
	TInt checkMarker;
	
	//Local socket to be used for capturing 
	//loopbacked data ( well anything..)
	RSocket iDataSocket;
	TBool iCallBackPass;

// Please Add/modify your class members
private:
	};

#endif
