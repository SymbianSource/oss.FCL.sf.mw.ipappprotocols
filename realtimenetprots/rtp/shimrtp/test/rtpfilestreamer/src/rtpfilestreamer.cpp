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
// class for sending and receiving rtp data
// 
//

/**
 @file
 @internalComponent
*/

#include "rtpfilestreamer.h"
#include <commdbconnpref.h>

/**
Symbian OS 2nd phase constructor.  Note that we:
	- initialise a new send source on the RRtpSession
 	- register to be called back when a packet is successfully sent by it
 	
*/
void CRtpFileSender::ConstructL(const TDesC& aSrcFilename)
	{
	User::LeaveIfError(iFile.Open(iFs,aSrcFilename,EFileRead));
	iSendSrc = iSession.NewSendSourceL();
	iSendSrc.SetDefaultPayloadSize(iPacketSize);
	//CRtpFileSender* Ptr = this;
	//iSendSrc.RegisterEventCallbackL(ERtpSendSucceeded, CRtpFileSender::PacketSent, Ptr);
	
	iSendSrc.PrivRegisterEventCallbackL(ERtpSendSucceeded, (TRtpCallbackFunction)CRtpFileSender::PacketSent, this);
	iSendSrc.PrivRegisterEventCallbackL(ERtpSendFail, (TRtpCallbackFunction)CRtpFileSender::SendError, this);

	iSendPacket = iSendSrc.NewSendPacketL();

	CActiveScheduler::Add(this);
	}

/**
This is the callback method which CRtpFileSender registers with the RtpSession to be called
when a packet is _successfully_ sent.  It is a static method so simply calls a processing
method on the class pointed to be aPtr (which is the CRtpFileSender which registered the cb).
*/
/*static*/ void CRtpFileSender::PacketSent(CRtpFileSender* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->DoPacketSent(aEvent);
	}

/**
An instance method which is called by the static callback function.  We simply notify the
observer (for the purposes of the demo app so status can be displayed) and start the RTimer
so that we can send another packet when the timer expires.
*/
void CRtpFileSender::DoPacketSent(const TRtpEvent& /*aEvent*/)
	{
	if (iObserver)
		{
		iObserver->NotifyPacketSent();
		}

	iSendIntervalTimer.After(iStatus,TTimeIntervalMicroSeconds32(iDelayMicroSecs));
	SetActive();
	}

/**
This is the callback method which CRtpFileSender registers with the RtpSession to be called
when a packet send fails.
*/
/*static*/ void CRtpFileSender::SendError(CRtpFileSender* aPtr, const TRtpEvent& aEvent)
	{
	aPtr->DoSendError(aEvent);
	}

/**
An instance method which is called by the static callback function.  We simply notify the
observer of the error.  Since we do not restart the timer, the file will stop streaming on
error - client code could implement any appropriate error handling here.
*/
void CRtpFileSender::DoSendError(const TRtpEvent& /*aEvent*/)
	{
	if (iObserver)
		{
		iObserver->NotifyError();
		}
	}


CRtpFileSender::~CRtpFileSender()
	{
	Cancel();
	iFile.Close();
	iSendIntervalTimer.Close();
	iSendPacket.Close();
	iSendSrc.Close();
	}


	
/*static*/ CRtpFileSender* CRtpFileSender::NewL(RRtpSession& aSession,RFs& aFs,const TDesC& aSrcFilename, TInt aPacketSize, TInt aDelayMicroSeconds)
	{
	if (!aSession.IsOpen())
		{
		User::Leave(KErrArgument);
		}
		
	CRtpFileSender* self = new (ELeave) CRtpFileSender(aSession,aFs,aPacketSize,aDelayMicroSeconds);
	CleanupStack::PushL(self);
	self->ConstructL(aSrcFilename);
	CleanupStack::Pop(self);
	return self;
	}
	
CRtpFileSender::CRtpFileSender(RRtpSession& aSession,RFs& aFs,TInt aPacketSize, TInt aDelayMicroSeconds) :
	CActive(0), iDelayMicroSecs(aDelayMicroSeconds), iPacketSize(aPacketSize), 
	iSession(aSession), iPayloadDesC(NULL,NULL), iFs(aFs)
		{
		}

void CRtpFileSender::StartL()
	{
	User::LeaveIfError(iSendIntervalTimer.CreateLocal());
	iSendIntervalTimer.After(iStatus,TTimeIntervalMicroSeconds32(iDelayMicroSecs));
	SetActive();
	}

/**
Called when the timer expires - Sends the next packet (if there is data to send).

Note the sequence of events here:
	- Get a new RtpPacket to send from the send source (the header has some default values
	  initialised, but it is otherwise empty).
	- Set the RTP timestamp of the packet (NB 'real' code would use a sample timer).
	- (read a block of data from the file)
	- [if data was left in file]
		 - Send the packet (note this simply involves calling the Send() method, the session
		   has already been initialised with destination address etc)
	  [else]
	  	 - Notify completion and close the input file
*/
void CRtpFileSender::RunL()
	{
	//The timestamp in this example is rather bogus since we're not streaming
	//media, but this is how you would set it.
	iSendPacket.SetTimestamp(User::FastCounter());
	
	iPayloadDesC.Set(const_cast<TUint8*>(iSendPacket.WritePayload().Ptr()),iPacketSize,iPacketSize);
	User::LeaveIfError(iFile.Read(iPayloadDesC));

	if (iPayloadDesC.Length()>0)
		{
		TInt tmp = iPayloadDesC.Length();
		iSendPacket.WritePayload().SetLength(tmp);
		iSendPacket.Send();
		}
	else
		{
		if (iObserver)
			{
			iObserver->NotifyComplete();
			}
		//NotifyComplete in this implementation actually causes the file to be
		//closed anyway.
		//iFile.Close();
		
		}	
	}
	
void CRtpFileSender::DoCancel()
	{
	iSendIntervalTimer.Cancel();
	}

//

CRtpFileStreamer::CRtpFileStreamer(RSocketServ& aSocketServ, const TInetAddr& aDestAddr, TUint aLocalPort) :
      iSocketServ(aSocketServ),
      iDestAddr(TInetAddr(aDestAddr)),
      iLocalPort(aLocalPort)
   	{
   	}


/*static*/ CRtpFileStreamer* CRtpFileStreamer::NewL(RSocketServ& aSocketServ,
    											const TDesC& aSrcFilename,
    											const TDesC& aDestFilename, 
    											TInt aBlockLen,
    											const TInetAddr& aDestAddr,
    											TUint aLocalPort, TInt aConnId)
	{
	CRtpFileStreamer* self = new (ELeave) CRtpFileStreamer(aSocketServ,aDestAddr,aLocalPort);
	CleanupStack::PushL(self);
	self->ConstructL(aSrcFilename,aDestFilename,aBlockLen,10000,aConnId);
	CleanupStack::Pop(self);
	return self;
	}

/**
Symbian OS 2nd phase constructor for CRtpFileStreamer

This method 

- Opens an Rtp session using the pre-connected socket overload (NB you
could also pass a src and dest TSockAddr and have Rtp connect the socket).

- registers for the ERtpNewSource callback in the Rtp event model which
will file when an Rtp packet with an SSRC which hasn't been seen before on this
session is received.

- creates a CRtpFileSender AO to stream aSrcFilename (see above).

Since we have connected the RSocket to the local loopback address, we will start
receiving packets when StartL() is called on the sender.
*/		
void CRtpFileStreamer::ConstructL(const TDesC& aSrcFilename, const TDesC& aDestFilename, TInt aPacketSize, TInt aDelayMicroSeconds, TInt aConnId)
	{
	User::LeaveIfError(iRFs.Connect());
	User::LeaveIfError(iDestFile.Replace(iRFs,aDestFilename,EFileWrite));
	TInetAddr localAddr;
	localAddr.SetPort(iLocalPort);
	_LIT8(KCname, "test");
		
	if (aConnId!=KErrNotFound)
		{
		User::LeaveIfError(iConnection.Open(iSocketServ));
		TCommDbConnPref prefs;
		prefs.SetIapId(aConnId);
   		prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
   		TRequestStatus stat;
   		iConnection.Start(prefs, stat);
   		User::WaitForRequest( stat );
    	User::LeaveIfError( stat.Int() );
    	
		iRtpSession.OpenL(iSocketServ, localAddr,iDestAddr,aPacketSize+12,iConnection, EPriorityNormal, KCname);	
		}
	else
		{
		User::LeaveIfError(iSocket.Open(iSocketServ, KAfInet,KSockDatagram, KProtocolInetUdp));	
		User::LeaveIfError(iSocket.Bind(localAddr));

		localAddr.SetPort(iLocalPort + 1);
		User::LeaveIfError(iRtcpSocket.Open(iSocketServ, KAfInet,KSockDatagram, KProtocolInetUdp));	
		User::LeaveIfError(iRtcpSocket.Bind(localAddr));
		
		TRequestStatus stat;
		iSocket.Connect(iDestAddr,stat);
		User::WaitForRequest(stat);
		User::LeaveIfError(stat.Int());
		
		iDestAddr.SetPort(iDestAddr.Port() + 1);
		iRtcpSocket.Connect(iDestAddr,stat);
		User::WaitForRequest(stat);
		User::LeaveIfError(stat.Int());
		
		iRtpSession.OpenL(iSocket, aPacketSize+12, iRtcpSocket);
		iRtpSession.SetRTPTimeConversion(100, 100);
		}				

	iRtpSession.PrivRegisterEventCallbackL(ERtpNewSource, (TRtpCallbackFunction)CRtpFileStreamer::NewSource, this);
	
	iSender = CRtpFileSender::NewL(iRtpSession,iRFs,aSrcFilename, aPacketSize, aDelayMicroSeconds);
	
	}

    
CRtpFileStreamer::~CRtpFileStreamer()
	{
	if (iSender)
		{
		delete iSender;	
		}
	
	iDestFile.Close();
	iRFs.Close();
	
	iRecvPacket.Close();
	iRtpRecvSrc.Close();
	
	iRtpSession.Close();
	iSocket.Close();
	iRtcpSocket.Close();
	}
	
void CRtpFileStreamer::StartL()
	{
	iSender->StartL();
	}

/**
This is the callback method which CRtpFileStreamer registers with the RtpSession to be called
when a packet with a new SSRC arrives.

In this method we:
 - close the existing receive source if there is one already open (NB 'real' applications are
   more likely to keep existing sources open and add the new source (SSRC) to an array of currently
   'contributing' sources.
   
 - create a new receive source to handle packets from this SSRC.
 - register to receive a callback when a new packet is received from this source.
*/
/*static*/ void CRtpFileStreamer::NewSource(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent)
	{
	if (aPtr->ReceiveSrc().IsOpen())
		{
		aPtr->ReceiveSrc().Close();
		}
	
	TRAPD(err,
		aPtr->ReceiveSrc() = aEvent.Session().NewReceiveSourceL();
		aPtr->ReceiveSrc().PrivRegisterEventCallbackL(ERtpPacketReceived, (TRtpCallbackFunction)CRtpFileStreamer::PacketArrived, aPtr);)
		
	if (err!=KErrNone)
		{
		//In a 'real' application we would handle this error
		__DEBUGGER();
		}
	}
	
/**
The static callback method which was registered to be called on packet arrival:
	- close the (old) receive packet that was open from last time
	- set aPtr->iRecvPacket to be the newly received Rtp packet
	- (check that it is not already open)
	- call an instance method on the class pointed to by aPtr to continue handling
	  this packet.
*/
/*static*/ void CRtpFileStreamer::PacketArrived(CRtpFileStreamer* aPtr, const TRtpEvent& aEvent)
	{	
	aPtr->iRecvPacket.Close();
	aPtr->iRecvPacket = aEvent.ReceiveSource().Packet();
	if (aEvent.ReceiveSource().Packet().IsOpen())
		{
		//In a 'real' application we would handle this error
		__DEBUGGER();
		}
		
	TRAPD(err,aPtr->HandleReceivedPacketL());
	
	if (err!=KErrNone)
		{
		//In a 'real' application we would handle this error
		__DEBUGGER();
		}
	}

/**
An instance method called by the static callback function which was registered to
be called on packet arrival:
 - write the received packet payload to disk
*/	
void CRtpFileStreamer::HandleReceivedPacketL()
	{
	if (iObserver)
		{
		iObserver->NotifyPacketReceived();
		}
	
	TPtrC8 writeDesc(iRecvPacket.Payload().Ptr(),iRecvPacket.Payload().Length());
	User::LeaveIfError(iDestFile.Write(writeDesc));
	}


