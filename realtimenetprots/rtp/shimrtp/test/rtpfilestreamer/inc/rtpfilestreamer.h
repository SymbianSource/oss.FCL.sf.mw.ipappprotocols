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
// This code demonstrates the use of the Symbian OS RTP APIs.
// Purposes of the classes:
// CRtpFileStreamer
// - This class has an RRtpSession member, it demonstrates
// the initialisation of RTP and how to register for callbacks
// in the RTP event model.  It also handles the receiving of
// rtp packets on the RRtpSession it owns - note that these
// events occur asyncronously, but do not require this object
// to be CActive derived because the callback mechanism allows
// this complexity to be abstracted away from handling classes.
// CRtpFileSender 
// - an Active Object which will send a block of a file every n
// microseconds as an RTP payload.  Note that this class needs to
// be CActive derived due to its timer functionality rather than
// as a consequence of its use of RTP APIs
// 
//

/**
 @file
*/

#ifndef __RTPFILESTREAMER_H__
#define __RTPFILESTREAMER_H__

#include "commdbconnpref.h"
#include <in_sock.h>
#include <f32file.h>
#include "rtp.h"

//

/**
A simple observer pattern interface so that the demo application
can display some status information.
*/
class MFileStreamerObserver
{
public:
	virtual void NotifyPacketSent() {}
	virtual void NotifyPacketReceived() {}
	virtual void NotifyComplete() {}
	virtual void NotifyError() {}
};

/**
This class has an RRtpSession member, it demonstrates
the initialisation of RTP and how to register for callbacks
in the RTP event model.  It also handles the receiving of
rtp packets on the RRtpSession it owns - note that these
events occur asyncronously, but do not require this object
to be CActive derived because the callback mechanism allows
this complexity to be abstracted away from handling classes.
*/
class CRtpFileSender : public CActive
{
public:
	//Callbacks for the sender
	static void PacketSent(CRtpFileSender* aPtr, const TRtpEvent& aEvent);
	void DoPacketSent(const TRtpEvent& aEvent);
	static void SendError(CRtpFileSender* aPtr, const TRtpEvent& aEvent);
	void DoSendError(const TRtpEvent& aEvent);
	
	static CRtpFileSender* NewL(RRtpSession& aSession,RFs& aFs,const TDesC& aSrcFilename, TInt aPacketSize, TInt aDelayMicroSeconds);
	void StartL();
	
	/*just for demo purposes*/
	void SetObserver(MFileStreamerObserver&	aObserver)
		{
		iObserver = &aObserver;
		}
	
	~CRtpFileSender();
		
private:
	CRtpFileSender(RRtpSession& aSession,RFs& aFs,TInt aPacketSize, TInt aDelayMicroSeconds);
	void ConstructL(const TDesC& aSrcFilename);
	void RunL();
	void DoCancel();
private:
	RTimer	iSendIntervalTimer;
	TInt	iDelayMicroSecs;
	TInt	iPacketSize;
	
	RRtpSession&	iSession;
	RRtpSendSource	iSendSrc;
	RRtpSendPacket	iSendPacket;
   	TPtr8			iPayloadDesC;
   	
   	/*Just for demo app purposes*/
   	MFileStreamerObserver*	iObserver;
   	
	RFile	iFile;
	RFs&	iFs;
};

class CRtpFileStreamer : public CBase
{
public:

    static CRtpFileStreamer* NewL(	RSocketServ& aRSocketServ,
    								const TDesC& aSrcFilename,
    								const TDesC& aDestFilename, 
    								TInt aBlockLen,
    								const TInetAddr& aDestAddr,
    								TUint aLocalPort,TInt aConnId);
    
    ~CRtpFileStreamer();
	
	//Callbacks for the receiver
	static void NewSource(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent);
	static void PacketArrived(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent);
	
	inline RRtpReceiveSource& ReceiveSrc() 
		{
		return iRtpRecvSrc;
		}
	
	void SendNextPacketL();
	void HandleReceivedPacketL();
	
	void SetObserver(MFileStreamerObserver& aObserver) 
		{
		iObserver = &aObserver;
		if (iSender) iSender->SetObserver(aObserver);
		}
	
	void StartL();
private:
   CRtpFileStreamer( RSocketServ& aSocketServ, const TInetAddr& aDestAddr, TUint aLocalPort);
   void ConstructL(const TDesC& aSrcFilename, const TDesC& aDestFilename, TInt aPacketSize, TInt aDelayMicroSecs, TInt aConnId);

public:
	RRtpReceivePacket iRecvPacket;
	
private:
    RSocketServ& iSocketServ;
    RSocket iSocket;
	RSocket iRtcpSocket;
  	RConnection	iConnection;
  	TInetAddr			iDestAddr;
   	TUint				iLocalPort;
    
    RRtpSession	   	  	iRtpSession;
    RRtpReceiveSource 	iRtpRecvSrc;

	MFileStreamerObserver*	iObserver;
  	CRtpFileSender*	iSender;
   	RFs		iRFs;
   	RFile	iDestFile;
};

#endif