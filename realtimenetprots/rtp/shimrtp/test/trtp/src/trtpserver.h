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

#if (!defined __TRTPSERVER_H__)
#define TRTPSERVER_H
#include <testexecuteserverbase.h>
#include "rtp.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "rtpsession_internal.h"
#endif 

class CTrtpStep;

class CTrtpServer : public CTestServer
	{
public:
	static CTrtpServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CTrtpServer();

	// Member functions used by test steps
	static void StopScheduler(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NewSourceL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void GetPacketL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void Panic(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void LeaveL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NewSourceLeaveL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NotifyError(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CloseSession(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NewSourceCloseSessionL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NewSourceDelayedCloseSessionL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NewSourceLeaveCloseL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void LeaveAfterCloseL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CloseSourceFromSourceL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CloseSourceFromSourceLeaveL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CloseSource(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CloseSendSource(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void NoteEventStatus(CTrtpServer* aPtr, const TRtpEvent& aEvent);

	static void NewSrcForRtcpL(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CbERtpSDES(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CbERtpAPP(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CbERtpSR(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CbERtpRR(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	static void CbERtpBYE(CTrtpServer* aPtr, const TRtpEvent& aEvent);

	//
	static TUint32 Swap32(TUint32 aVal);
	static TUint16 Swap16(TUint16 aVal);

	// Members used by the steps
	TBool iOom;
	TBool iStopping;
	CTrtpStep* iCurrentStep;
	RRtpSendPacket iSendPacket;
	RRtpReceivePacket iReceivePacket;
	RRtpPacket* iPacket; // Used for checking the current 'interesting
						 // packet' has the right values. The same
						 // functions can work on both send and
						 // receive packets. This points to whichever
						 // one we're interested in.
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
	RRtpSession_Internal iSession;
#else
	RRtpSession iSession;
#endif 
	RRtpSendSource iSendSource;
	RArray<RRtpReceiveSource> iSources;
	RRtpReceiveSource iLastActiveSource;
	RSocketServ iSocketServer;
	RSocket iSocket;
	RFs iFs;
	TRtpEvent iLastEvent;
	TInt iStopCount;
	TInt iEventStatus;
	TBool iConnected;
	RSocket iRtpSocket;
	RSocket iRtcpSocket;
	RConnection iConnect;
	RSubConnection iSubConnect;
	CActiveScheduler* iScheduler;

 private:
	CTrtpServer() : iOom(EFalse), iStopping(EFalse), iLastEvent(ERtpAnyEvent, 0, 0) {};
	void ConstructL();
	};

class CWaiter : public CActive
	{
 public:
	static CWaiter* NewLC(CTrtpServer* aServer);

	~CWaiter();
	
	void RunL();
	void DoCancel();

	void Ready() { SetActive();};

 private:
	CWaiter(CTrtpServer* aServer) : CActive(0), iServer(aServer) 
			{
			CActiveScheduler::Add(this);
			};

	CTrtpServer* iServer;
	};

#endif
