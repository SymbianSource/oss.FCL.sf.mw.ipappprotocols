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
// class for handling rtp sessions
//
//

/**
 @file
*/

#include <e32base.h>
#include "rtp.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "rtpsession_internal.h"
#endif
#include "session.h"
#include "packet.h"
#include "events.h"
#include "receivestream.h"
#include "sendstream.h"
#include "rtputils.h"
#include "rtcppacket.h"

#include "rtcp.h"
#include <rtpprepostprocessingcallback.h>


LOCAL_C void CopyPacketToRR(TDesC8& aReport, TRtcpRRItem& aRr);
static const TInt KMaxSdesItemLength = 255;

/**
Opens the RTP session and initialises it.

No events will be generated until after control returns to the
active scheduler. After successfully opening the session, the client
has the opportunity to initialise the object by adding the event callbacks,
that it requires.

@param aServer     An already opened socket server session
@param aLocalAddr  The socket address to bind to at this end.
@param aRemoteAddr The remote socket address.
@param aMaxRXSize  The maximum expected size of a received packet. Note that this
				   includes the packet header (as it is profile dependent).
@param aConnection A started RConnection for the interface to open this session on.
@param aPriority   The priority to be used for internal active objects.
@param aCNAME      The CNAME (Canonical End-Point Identifier).

@see CActive
*/
EXPORT_C void RRtpSession::OpenL(RSocketServ& aServer, TSockAddr& aLocalAddr,
								 TSockAddr& aRemoteAddr,
								 TInt aMaxRXSize, RConnection& aConnection,
								 TInt aPriority, const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("RRtpSession::OpenL() ... "));
	__ASSERT_DEBUG(!iPtr, Panic(ERtpSessionAlreadyOpen));
	iPtr = CRtpSession::NewL(aServer, aLocalAddr, aRemoteAddr, aConnection, aPriority,
							 aMaxRXSize, aCNAME);
	}

/**
Opens the RTP session and initialises it.

No events will be generated until after control returns to the
active scheduler. After successfully opening the session, the client
has the opportunity to initialise the object by adding the event callbacks,
that it requires.

@param aServer     An already opened socket server session
@param aLocalAddr  The socket address to bind to at this end.
@param aRemoteAddr The remote socket address.
@param aMaxRXSize  The maximum expected size of a received packet. Note that this
				   includes the packet header (as it is profile dependent).
@param aPriority   The priority to be used for internal active objects.
@param aCNAME      The CNAME (Canonical End-Point Identifier).
				   RTCP is used only if CNAME is provided

  @see CActive
*/
EXPORT_C void RRtpSession::OpenL(RSocketServ& aServer, TSockAddr& aLocalAddr,
								 TSockAddr& aRemoteAddr,
								 TInt aMaxRXSize, TInt aPriority,
								 const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("RRtpSession::OpenL() ... "));
	__ASSERT_DEBUG(!iPtr, Panic(ERtpSessionAlreadyOpen));
	iPtr = CRtpSession::NewL(aServer, aLocalAddr, aRemoteAddr, aPriority,
							 aMaxRXSize, aCNAME);
	}


/**
Opens the session and initialises it with RTCP.

No events will be generated until after control returns to the
active scheduler. After successfully opening the session, the client
has the opportunity to initialise the object by adding the event callbacks,
that it requires.

@param aSocket    An already connected socket.
@param aSocket    An already connected Socket.
@param aMaxRXSize The maximum expected size of a received packet. Note that this
				   includes the packet header (as it is profile dependent).
@param aPriority  The priority to be used for internal active objects.

@see CActive
*/
EXPORT_C void RRtpSession::OpenL(RSocket& aSocket, TInt aMaxRXSize,
						RSocket& aRtcpSocket,
						TInt aPriority,
						const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("RRtpSession::OpenL() ... "));
	__ASSERT_DEBUG(!iPtr, Panic(ERtpSessionAlreadyOpen));
	iPtr = CRtpSession::NewL(aSocket, aMaxRXSize, aRtcpSocket, aPriority, aCNAME);
	}


/**
Opens the session and initialises it without RTCP.

No events will be generated until after control returns to the
active scheduler. After successfully opening the session, the client
has the opportunity to initialise the object by adding the event callbacks,
that it requires.

@param aSocket    An already connected socket.
@param aMaxRXSize The maximum expected size of a received packet. Note that this
				   includes the packet header (as it is profile dependent).
@param aPriority  The priority to be used for internal active objects.

@see CActive
*/
EXPORT_C void RRtpSession::OpenL(RSocket& aSocket, TInt aMaxRXSize,
								 TInt aPriority)
	{
	__RTP_LOG(_L("RRtpSession::OpenL() ... "));
	__ASSERT_DEBUG(!iPtr, Panic(ERtpSessionAlreadyOpen));
	iPtr = CRtpSession::NewL(aSocket, aMaxRXSize, aPriority);
	}




/**
Opens the RTP session and initialises it.

No events will be generated until after control returns to the
active scheduler. After successfully opening the session, the client
has the opportunity to initialise the object by adding the event callbacks,
that it requires.

@param aServer    		An already opened socket server session
@param aLocalAddr  		The socket address to bind to at this end.
@param aRemoteAddr 		The remote socket address.
@param aMaxRXSize  		The maximum expected size of a received packet. Note that this
				   		includes the packet header (as it is profile dependent).
@param aRSubConnection 	A started RSubConnection for the interface to open this session on.
@param aPriority   		The priority to be used for internal active objects.
@param aCNAME      		The CNAME (Canonical End-Point Identifier).
				   		RTCP is used only if CNAME is provided

  @see CActive
*/
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C void RRtpSession_Internal::OpenL(RSocketServ& aServer, TSockAddr& aLocalAddr,
								 TSockAddr& aRemoteAddr,
								 TInt aMaxRXSize, RSubConnection& aSubConnection,
								 TInt aPriority, const TDesC8& aCNAME)
#else
EXPORT_C void RRtpSession::OpenL(RSocketServ& aServer, TSockAddr& aLocalAddr,
								 TSockAddr& aRemoteAddr,
								 TInt aMaxRXSize, RSubConnection& aSubConnection,
								 TInt aPriority, const TDesC8& aCNAME)
#endif
	{
	__RTP_LOG(_L("RRtpSession::OpenL() ... "));
	__ASSERT_DEBUG(!iPtr, Panic(ERtpSessionAlreadyOpen));
	iPtr = CRtpSession::NewL(aServer, aLocalAddr, aRemoteAddr, aSubConnection, aPriority,
							 aMaxRXSize, aCNAME);
	}


/*
Closes the session.

All RRtpSendSource and RRtpReceiveSource objects associated with this
session are closed.

@see RRtpSendSource
@see RRtpReceiveSource
*/
EXPORT_C void RRtpSession::Close()
	{
	__RTP_LOG(_L("RRtpSession::Close() ... "));
	if (iPtr)
		iPtr->Close();
	iPtr = NULL;
	}




/**
@internalComponent
*/
EXPORT_C void RRtpSession::PrivRegisterEventCallbackL(TUint aType,
												  TRtpCallbackFunction aCallback,
												  TAny* aPtr,
												  TInt aParameter)
	{
	__RTP_LOG(_L("RRtpSession::PrivRegisterEventCallbackL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
    CRtpControllerBase* rtpSocketController = iPtr->RtpController();
    if((aType & (~ERtpOneShot)) == ENonRtpDataReceived)
        {
        rtpSocketController->SetNonRtpData(ETrue);
        }
    else if((aType & (~ERtpOneShot)) == ENonRtcpDataReceived)
        {
        rtpSocketController->SetNonRtcpData(ETrue);
        }

	iPtr->iEventModel.RegisterEventCallbackL(aType, aCallback, aPtr, aParameter);
	}


/**
@internalComponent
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C void RRtpSession_Internal::SetPrePostProcessingRegisterCallback(MPrePostProcessingCallback* aPrePostProcessingCallback)
#else
EXPORT_C void RRtpSession::SetPrePostProcessingRegisterCallback(MPrePostProcessingCallback* aPrePostProcessingCallback)
#endif
	{
	__RTP_LOG(_L("RRtpSession::SetPrePostProcessingRegisterCallback() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SetPrePostProcessingCallback(aPrePostProcessingCallback);
	}




/**
@internalComponent
*/
EXPORT_C void RRtpSession::PrivRegisterEventCallbackL(TUint aType,
												  TRtpCallbackFunction aCallback,
												  TAny* aPtr)
	{
	__RTP_LOG(_L("RRtpSession::PrivRegisterEventCallbackL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	__RTP_LOG1(_L("\t[Type] == 0x%x"), aType);
	PrivRegisterEventCallbackL(aType, aCallback, aPtr,KRtpNoParameter);
	}




/**
Sets the expected maximum size of a received packet.

This does not affect the packet that is being downloaded or is being
waited for, so it is entirely possible that the first packet received
after correctly setting the receive packet size will still overflow.

@param aMaxRXSize The new expected maximum size.

@panic RTP 3 if the session has not been opened and initialised.
             Note that this panic is raised in debug mode only.
*/
EXPORT_C void RRtpSession::SetMaxRXSize(TInt aMaxRXSize)
	{
	__RTP_LOG(_L("RRtpSession::SetMaxRXSize() ... "));
	__RTP_LOG1(_L("\t[MaxRXSize] == %d"), aMaxRXSize);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRxSize = aMaxRXSize;
	}




/**
Gets the current expected maximum size of a received packet.

@return The current expected maximum receive size.

@panic RTP 3 if the session has not been opened and initialised.
             Note that this panic is raised in debug mode only.
*/
EXPORT_C TInt RRtpSession::MaxRXSize()
	{
	__RTP_LOG(_L("RRtpSession::MaxRXSize() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	__RTP_LOG1(_L("\t==> %d"), iPtr->iRxSize);
	return iPtr->iRxSize;
	}


/**
Creates a new receive stream, and returns the receive stream handle.

This function should only be called in response to an
ERtpNewSource event.

@return The receive stream handle.

@panic RTP 3 if the session has not been opened and initialised.
             Note that this panic is raised in debug mode only.

@see TRtpEventType
*/
EXPORT_C RRtpReceiveSource RRtpSession::NewReceiveSourceL()
	{
	__RTP_LOG(_L("RRtpSession::NewReceiveSourceL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	RRtpReceiveSource r = NewReceiveSourceLC();
	CleanupStack::Pop(r.iPtr);
	return r;
	}




/**
Creates a new receive stream, puts a pointer to the receive stream object onto
the cleanup stack, and returns the receive stream handle.

This function should only be called in response to an
ERtpNewSource event.

Note that the function does not put a pointer to the RRtpReceiveSource object
onto the cleanup stack as you might normally expect, but puts a pointer to
the receive stream object for which the returned RRtpReceiveSource is
the handle.

Note also that the API does not give you access to the receive stream
object for which the returned RRtpReceiveSource is
the handle.

@return The receive stream handle.

@panic RTP 3 if the session has not been opened and initialised.
             Note that this panic is raised in debug mode only.

@see TRtpEventType
@see CleanupStack
*/
EXPORT_C RRtpReceiveSource RRtpSession::NewReceiveSourceLC()
	{
	__RTP_LOG(_L("RRtpSession::NewReceiveSourceLC() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	TUint ssrc = iPtr->iNewSourcePacket->Header()->iSSRC;
	// Create the stream
	CRtpReceiveSource* stream =
		new (ELeave) CRtpReceiveSource(iPtr->iNewSourcePacket, iPtr);
	CleanupStack::PushL(stream);
	// And register into the array of streams in the session
	TReceiveSourceRegistration reg;
	reg.iSSRC = ssrc;
	reg.iSource = stream;
	User::LeaveIfError(iPtr->iReceiveSources.Append(reg));
	iPtr->iNewSourcePacket = NULL;
	iPtr->iNewSource = stream;
	RRtpReceiveSource r;
	r.iPtr = stream;
	return r;
	}




/**
Creates a new send stream, and returns the send stream handle.

Only one send stream per session is permitted, which means that you must not
call this function if the send stream has already been created.

As the send stream is an active object, it cannot handle request
completion events until control returns to the active scheduler.
Typically, clients will want to add event registrations before that.

@return The send stream handle.
*/
EXPORT_C RRtpSendSource RRtpSession::NewSendSourceL()
	{
	__RTP_LOG(_L("RRtpSession::NewSendSourceL() ... "));
	__ASSERT_DEBUG(!(iPtr->iSendSource), Panic(ERtpSendSourceAlreadyOpen));
	iPtr->iSendSource = new (ELeave) CRtpSendSource(iPtr->Priority(), iPtr);
	RRtpSendSource s;
	s.iPtr = iPtr->iSendSource;
	return s;
	}

/**
Returns the existing send stream handle.

As the send stream is an active object, it cannot handle request
completion events until control returns to the active scheduler.
Typically, clients will want to add event registrations before that.

@return The send stream handle.
*/
EXPORT_C RRtpSendSource RRtpSession::SendSource()
	{
	__RTP_LOG(_L("RRtpSession::SendSourceL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	RRtpSendSource s;
	s.iPtr = iPtr->iSendSource;
	return s;
	}

/**
Sends an RTCP APP packet.

@param aName     Application name(4-bytes)
@param aAppData  Application specific data
@param aSubType Application defined subtype
*/
EXPORT_C void RRtpSession::SendAPPL(const TDesC8& aName, const TDesC8& aAppData, TUint8 aSubType)
	{
	__RTP_LOG(_L("RRtpSession::SendAPP() ... "));
	//__RTP_LOG1(_L8("\t[aName] == %S"), &aName);
	//__RTP_LOG1(_L8("\t[aAppData] == %S"), &aAppData);
	__RTP_LOG1(_L("\t[aSubType] == %d"), aSubType);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SendAppPacketL(aName, aAppData, aSubType);
	}

/**
Stores an SDES packet information to be sent in the impending timer-exprity event.

@param aType     The item type
@param aValue  The item value. This must be less than 256 else the
		       function returns KErrTooBig error.

@return KErrTooBig If the size of aValue is more than 255
		KErrNone Successful operation
*/
EXPORT_C TInt RRtpSession::SetSDESL(TInt aType, const TDesC8& aValue)
	{
	__RTP_LOG(_L("RRtpSession::SetSDESL() ... "));
	__RTP_LOG1(_L("\t[ItemType] == %d"), aType);
	//__RTP_LOG1(_L8("\t[ItemValue] == %S"), &aValue);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	if(aValue.Length() > KMaxSdesItemLength)
		{
		return KErrTooBig;
		}
	iPtr->iRtpSocketController->SendSDESL(aType, aValue);
	return KErrNone;
	}

/**
Stores an SDES PRIV packet information to be sent in the impending
timer-exprity event.

@param aPrefix  The prefix name
@param aValue  The prefix value. This must be less than 256 - length of prefix string,
				 else the function returns KErrTooBig error.

@return KErrTooBig If the size of aValue is more than 255- length of prefix string
		KErrNone Successful operation
*/
EXPORT_C TInt RRtpSession::SetPRIVL(const TDesC8& aPrefix, const TDesC8& aValue)
	{
	__RTP_LOG(_L("RRtpSession::SetPRIVL() ... "));
	//__RTP_LOG1(_L8("\t[PrefixString] == %S"), &aPrefix);
	//__RTP_LOG1(_L8("\t[ValueString] == %S"), &aValue);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	if(aPrefix.Length() > KMaxSdesItemLength)
		{
		return KErrTooBig;
		}
	if(aValue.Length() > (KMaxSdesItemLength-aPrefix.Length()))
		{
		return KErrTooBig;
		}
	iPtr->iRtpSocketController->SetPRIVL(aPrefix, aValue);
	return KErrNone;
	}

/**
Enables or disables the automatic sending of RTCP SR (Sender Report),
RR (Receiver Report), and SDES (Source Description) packets.

Note that if automatic sending is not enabled and RTCP sending is required,
then explicit calls to SendRTCPPacketL() will be required.

@param aAutoSend ETrue, if RTCP packets need to be send automatically
	             EFalse otherwise.
*/
EXPORT_C void RRtpSession::SetRTCPAutoSend(TBool aAutoSend)
	{
	__RTP_LOG(_L("RRtpSession::SetRTCPAutoSend() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SetAutoSendOption(aAutoSend);
	}

/**
Gets the RTCP Auto Send status.

@return True, if RTCP SR, RR and SDES packets are automatically sent,
	    false otherwise

*/
EXPORT_C TBool RRtpSession::RTCPAutoSend() const
	{
	__RTP_LOG(_L("RRtpSession::RTCPAutoSend() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->iRtpSocketController->AutoSendOption();
	}

/**
Sets the RTCP Auto Send interval.
This disables the RFC based calculation of RTCP intervals.
To re-enable auto calculation, set the interval to zero.

@param aInterval Time interval after which an RTCP auto-send will take place
*/
EXPORT_C void RRtpSession::SetRtcpInterval(TTimeIntervalMicroSeconds32& aInterval)
	{
	__RTP_LOG(_L("RRtpSession::SetRtcpInterval() ... "));
//	__RTP_LOG1(_L("\t[Interval] == %d"), aInterval);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SetRtcpInterval(aInterval);
	}


/**
Enables Receive only option for rtp or rtcp

@param aRtpOrRtcp Takes the values from the enum TPacketType

*/
EXPORT_C void RRtpSession::DontReceive(TInt aRtpOrRtcp)
	{
	__RTP_LOG(_L("RRtpSession::DontReceive() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	__ASSERT_DEBUG(aRtpOrRtcp == ERtp || aRtpOrRtcp == ERtcp, Panic(ERtpWrongParameter));
	iPtr->iRtpSocketController->StopReceive(aRtpOrRtcp);
	}

/**
Sets the estimated bandwidth used by the session.

This is used to decide when to send RTCP status packets.
The default	value is read from a resource file and should be a reasonably
sensible default based on the maximum bandwidth of the handset.

@param aBandwidth The bandwidth in bps.
*/
EXPORT_C void RRtpSession::SetBandwidth(TUint aBandwidth)
	{
	__RTP_LOG(_L("RRtpSession::SetBandwidth() ... "));
	__RTP_LOG1(_L("\t[Bandwidth] == %d"), aBandwidth);
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SetSessionBandwidth(aBandwidth);
	}

/**
Sets the number of sequential packets that must be received
before a stream is considered good.

Sets the maximum number of dropped packets to be considered a
dropout, as opposed to an ended and restarted stream.

Sets the maximum number of packets by which a packet can be delayed
before it is considered dropped.

@param aMaxMisorder The maximum number of packets.

@param aMaxDropout The number of sequential packets.

@param aMinSequential The number of sequential packets.
@post The session will be 'restarted',
		i.e. it resets the sequence number sequence and resets all RTCP statistics.
*/
EXPORT_C void RRtpSession::SetRtpStreamParameters(TInt aMinSequential, TInt aMaxMisorder, TInt aMaxDropout)
	{
	__RTP_LOG(_L("RRtpSession::SetRtpStreamParameters() ... "));
	__RTP_LOG1(_L("\t[MinSequential] == %d"), aMinSequential);
	__RTP_LOG1(_L("\t[MaxMisorder] == %d"), aMaxMisorder);
	__RTP_LOG1(_L("\t[MaxDropout] == %d"), aMaxDropout);

	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	// Need to be careful here, order changes
	iPtr->iRtpSocketController->SetRtpStreamParameters(aMaxDropout, aMaxMisorder, aMinSequential);
	}

/**
Sends an RTCP packet now.

Calling this function doesn't make much sense if auto-send is
on, as it will result in packets being sent at rather
incoherent intervals.

@param aPacket  The RTCP packet to be sent
*/
EXPORT_C void RRtpSession::SendRTCPPacketL(TDesC8& aPacket)
	{
	__RTP_LOG(_L("RRtpSession::SendRTCPPacketL() ... "));
	__RTP_LOG1(_L("\t[PacketLen] == %d"), aPacket.Length());
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SendRTCPPacketL(aPacket);
	}

/**
Supplies information about the profile-specific RTP
timestamps.

This is used to generate the RTP timestamps in RTCP packets.
It is not used to generate timestamps in RTP packets as they must
accurately relate to the time of the sample.

@param aNow        The time now in RTP format
@param aConversion The number of nanoseconds per RTP timer tick.
*/
EXPORT_C void RRtpSession::SetRTPTimeConversion(TUint aNow, TUint aConversion)
	{
	__RTP_LOG(_L("RRtpSession::SetRTPTimeConversion ... "));
	__RTP_LOG1(_L("\t[aNow] == %d"), aNow);
	__RTP_LOG1(_L("\t[aConversion] == %d"), aConversion);

	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iRtpSocketController->SetRTPTimeConversion(aNow, aConversion);
	}

/**
Gets the data associated with the specified RTCP SDES (Source Description)
item.

Note a that the PRIV (Private Extensions) SDES item is not supported
by this mechanism.

@param aType  The value identifying the RTCP SDES item.

@param aValue A descriptor reference to the SDES item data.

@return KErrNotFound if the item type was not found in the most recent RTCP packet
		KErrNone if the operation is successful
*/
EXPORT_C TInt RRtpSession::GetSDES(const TInt aType, TDes8& aValue)
	{
	__RTP_LOG(_L("RRtpSession::GetSDES() ... "));
	return iPtr->GetSDES(aType, aValue);
	}

/**
Gets an array contining the most recent RR (Receiver Report) from
all hosts who are reporting on our send stream.

@return The array of RRs.
*/
EXPORT_C RRtcpRRPart RRtpSession::GetRRs()
	{
	__RTP_LOG(_L("RRtpSession::GetRRs() ... "));
	return RRtcpRRPart(iPtr->iLastRRPart);
	}

/**
Sets the sampling rate for a Particular Payloadtype
@param aPayloadType  PayloadType ( Should be between 96-127 ) see RFC 3551
@param asamplingrate Sampling rate is in rtp ticks per msecs. ( eg for AMR-NB give 8000).
*/
EXPORT_C TInt RRtpSession::SetSamplingRate(TInt aPayloadType, TUint aSamplingRate)
	{
	__RTP_LOG(_L("RRtpSession::SetSamplingRate() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->SetSamplingRate(aPayloadType, aSamplingRate);
	}

NONSHARABLE_CLASS(CRtcpListener) : public CBase, MRtpDataObserver
	{
public:
	static CRtcpListener* NewL(CRtpHandlerBase& aRtcpHandler, CRtpSession& aSession);
	void StartListening();
	virtual TInt HandleError(TInt aError);
	virtual ~CRtcpListener();
	//Notify Non RTCP packet received
	void NotifyNonRtcpReceivedL(TRtpEventType aEvent, TInt aError);
	TPtr8 RtcpRecvPtr();
	TPtr8 rtcpData();
 	void SetRtcpDataL(HBufC8* aData);
 	void DataAvailable(TInt aErrorCode);

private:
	virtual void CancelAllRequests();
	inline CRtcpListener(CRtpHandlerBase& aRtcpHandler, CRtpSession& aSession);
    void DoProcessRtcpDataL(TInt aErrorCode);

private:
	friend class CRtpSession;
	friend class CRtpReceiveSource;

	HBufC8* iRtcpRecvBuf;
	TPtr8   iRtcpRecvPtr;
	HBufC8* iRtcpData;

	CRtpHandlerBase& iReader;
	CRtpSession& iSession;
	};

/**
Second phase Constructor for CRtcpListener

@param aRtcpHandler     RTCP handler
@param aSession  The RTP session
*/
CRtcpListener* CRtcpListener::NewL(CRtpHandlerBase& aRtcpHandler, CRtpSession& aSession)
	{
	__RTP_LOG(_L("RRtpListener::NewL() ... "));
	CRtcpListener* self = new(ELeave) CRtcpListener(aRtcpHandler, aSession);
	CleanupStack::PushL(self);
	self->iRtcpRecvBuf = HBufC8::NewMaxL(KRtcpDataDefaultSize);
	self->iRtcpRecvPtr.Set(self->iRtcpRecvBuf->Des());
	CleanupStack::Pop();
	return self;
	}

/**
Destructor for the RTCP listener
*/
CRtcpListener::~CRtcpListener()
	{
	__RTP_LOG(_L("CRtcpListener::~CRtcpListener()"));
	CancelAllRequests();
	delete iRtcpRecvBuf;
	delete iRtcpData;
	}

/**
This is an asynchronous service call to read the RTCP packet
*/
void CRtcpListener::StartListening()
	{
	__RTP_LOG(_L("CRtcpListener::StartListening()"));
	iRtcpRecvPtr.Zero();
	iReader.Read(iRtcpRecvPtr);
	}

TPtr8 CRtcpListener::rtcpData()
	{
	return iRtcpData->Des();
	}

void CRtcpListener::SetRtcpDataL(HBufC8* aData)
	{
	if(iRtcpData)
		{
		delete iRtcpData;
		iRtcpData = NULL;
		}
	iRtcpData = HBufC8::NewMaxL(aData->Des().Length());
	*iRtcpData = *aData;
	}

/**
Returns the RTCP packet received
*/
TPtr8 CRtcpListener::RtcpRecvPtr()
	{
	return rtcpData();
	}
/**
This function Notifies Non Rtcp Packet

@param TRtpEventType     Rtcp Event
@param aError    aError
*/

void CRtcpListener::NotifyNonRtcpReceivedL(TRtpEventType aEvent, TInt aError)
	{
	__RTP_LOG(_L("CRtcpListener::NotifyNonRtcpReceivedL() ... "));
	iSession.iEventModel.HandleEventL(aEvent, aError, iRtcpRecvBuf);
	}

void CRtcpListener::DataAvailable(TInt aErrorCode)
    {
    __RTP_LOG(_L("CRtpSession::DataAvailable() ... "));
     TRAPD(err, DoProcessRtcpDataL(aErrorCode));
     if(err)
         {
         __RTP_LOG1(_L("\tDoProcessRtcpDataL Left ErrorCode == %d"), err);
         HandleError(aErrorCode);
         }
    }

/**
Handles error that may get generated during reading of the RTCP packet
*/
TInt CRtcpListener::HandleError(TInt aError)
    {
     __RTP_LOG1(_L("CRtcpListener::HandleError() ... Error %d "), aError);
     // Eat the error, and start listening again
     TRAP(aError, iSession.HandleErrorL(aError));
     StartListening();
     return KErrNone;
     }

/**
Completion routine that gets called when the RTCP packet is read
*/
void CRtcpListener::DoProcessRtcpDataL(TInt aErrorCode)
	{
	__RTP_LOG(_L("CRtcpListener::DoProcessRtcpDataL()"));
	const TInt totLen = iRtcpRecvBuf->Length();
	TInt parsed = 0;
	TInt len = 0;
	TUint packetType = 0;
	TUint packetCount = 0;
	TUint32 ssrc = 0;
	TPtrC8 fixed(KNullDesC8);
	TInt err = aErrorCode;
	//non RTCP data is received
	if(KErrCorrupt == err)
		{
		SetRtcpDataL(iRtcpRecvBuf);
		NotifyNonRtcpReceivedL(ENonRtcpDataReceived, KErrNone);
		}

	// Here, we only proceed if we get a valid packet
	if (KErrNone != err)
		User::Leave(err);
	// We parse the packet
	while (parsed < totLen)
		{
		fixed.Set(iRtcpRecvBuf->Mid(parsed));
		TRtcpFixedHdrMemLayout header(fixed);
		len = (header.Length()+1)*4;
		parsed += len;
		ssrc = header.SSRC();
		packetType = header.PacketType();
		__RTP_LOG1(_L("\tPacketype %d"), packetType);
		packetCount = header.Count();
		// Notify the Session that a packet of following type arrived
		switch(packetType)
			{
			case ERtcpSR:
			case ERtcpRR:
				{
				fixed.Set(fixed.Mid(KRtcpSRHeaderSize));
				iSession.GotRtcpReportPacketL(packetType, ssrc, fixed, packetCount);
				break;
				}
			case ERtcpSDES:
				{
				fixed.Set(fixed.Mid(KRtcpSDesFixedSize));
				iSession.GotRtcpSdesPacketL(ssrc, fixed, packetCount);
				break;
				}
			case ERtcpAPP:
				{
				/*patch for poc, trigger nonrtcp data received callback,
				 * hence set data 
				 */
				SetRtcpDataL(iRtcpRecvBuf);
				fixed.Set(fixed.Mid(KRtcpAppFixedSize));
				iSession.GotRtcpAppPacketL(ssrc, fixed, packetCount);
				break;
				}
			case ERtcpBYE:
				{
				// Find the reason
				const TInt sizeWithoutReason = KRtcpFixedHdrSize + 4*packetCount;
				// For every ssrc in the Bye packets - give each one the reason
				for (TUint count = 1; count <= packetCount; count++)
					{
					ssrc = TRtcpMemLayout::Swap32(*reinterpret_cast<TUint32*>(const_cast<TUint8*>(fixed.Ptr())+ 4*count));
					TPtrC8 reason(fixed.Ptr() + sizeWithoutReason, len - sizeWithoutReason);
					iSession.GotRtcpByePacketL(ssrc, reason);
					}
				break;
				}
			default:
				break;
			};
		}
	StartListening();
	}

/**
This function implements the cancellation policy for reading the RTCP packet
*/
void CRtcpListener::CancelAllRequests()
	{
	__RTP_LOG(_L("RRtpSession::CancelAllRequests() ... "));
	iReader.Cancel();
	}

/**
First phase Constructor for CRtcpListener

@param aRtcpHandler     RTCP handler
@param aSession  The RTP session
*/
CRtcpListener::CRtcpListener(CRtpHandlerBase& aRtcpHandler, CRtpSession& aSession)
	:iRtcpRecvPtr(NULL, 0, 0), iReader(aRtcpHandler), iSession(aSession)
	{
	}

/**
Gets a handle to the RTP socket;
it provides a direct access to the functionality of RTP socket.

@return The RTP socket for the current session

*/
EXPORT_C RSocket* RRtpSession::RtpSocket()
	{
	__RTP_LOG(_L("RRtpSession::RtpSocket() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->RtpSocket();
	}

/**
Gets a handle to the RTCP socket;
it provides a direct access to the functionality of RTCP socket.

@return The RTCP socket for the current session

*/
EXPORT_C RSocket* RRtpSession::RtcpSocket()
	{
	__RTP_LOG(_L("RRtpSession::RtcpSocket() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->RtcpSocket();
	}

/**

Send a non-RTP (control) data packet asynchronously

@param aSocketType SocketType that tells whether non RTP data has to go through RTP or RTCP socket

@param aData Non RTP data that has to be sent

@param aStatus Tells whether the data has been sent successfully

*/
EXPORT_C void RRtpSession::SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus)
	{
	__RTP_LOG(_L("RRtpSession::SendDataL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->SendDataL(aSocketType,aData,aStatus);
	return;
	}

/**

Sets the remote RTP address for the current active session

@param aRemoteAddr Remote RTP address to be set

*/
EXPORT_C void RRtpSession::SetRemoteAddress(const TSockAddr &aRemoteAddr)
	{
	__RTP_LOG(_L("RRtpSession::SetRemoteAddress() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->SetRemoteAddress(aRemoteAddr);
	return;
	}

/**
Gets the last received non RTP data, If no data found function leaves

@return Returns the last received non RTP data
*/
EXPORT_C const TDesC8& RRtpSession::NonRtpDataL()
	{
	__RTP_LOG(_L("RRtpSession::NonRtpDataL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->NonRtpDataL();
	}

/**
Gets the last received non RTCP data, If no data found function leaves

@return Returns the last received non RTCP data
*/
EXPORT_C TPtr8 RRtpSession::NonRtcpDataL()
	{
	__RTP_LOG(_L("RRtpSession::NonRtcpDataL() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->NonRtcpDataL();
	}

/**

Disables the receipt of non RTP data

*/
EXPORT_C void RRtpSession::DisableNonRtpData()
	{
	__RTP_LOG(_L("RRtpSession::DisableNonRtpData() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->iEventModel.RemoveCallback(ENonRtpDataReceived);
	iPtr->iEventModel.RemoveCallback(ENonRtcpDataReceived);
	iPtr->iRtpSocketController->DisableNonRtpdata();
	}

/**
Sets the remote RTCP port for the current active RTP session

@param Remote RTCP port value to be set
*/

EXPORT_C void RRtpSession::SetRemoteRtcpPort(TUint aPort)
	{
	__RTP_LOG(_L("RRtpSession::SetRemoteRtcpPort() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	iPtr->SetRemoteRtcpPort(aPort);
	}

/**
Gets the remote RTCP Staistics for the current RTP session

@param SSRC of the sender
@param RTCP Statistics class that stores the RTCP statistics for the
current RTP session
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C TInt RRtpSession_Internal::RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics)
#else
EXPORT_C TInt RRtpSession::RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics)
#endif

	{
	__RTP_LOG(_L("RRtpSession::RtcpStatistics() ... "));
	__ASSERT_DEBUG(iPtr, Panic(ERtpSessionIsClosed));
	return iPtr->RtcpStatistics(aSsrc,aRtcpStatistics);
	};



/**********************************************/

/**
Creates a new session and initialises it.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aServer     The socket server
@param aLocalAddr  The local address of the device from which an RTP packet is being sent
@param aRemoteAddr The remote address of the device to which we need to send the packet
@param aConnection  The connection to be used
@param aRxSize  The recvieve size of the buffer
@param aPriority   The priority to be used for internal active objects
@param aCNAME      The CNAME. A sensible default is supplied if none is
                   provided.

@return Pointer to newly created CRtpSession object
*/
CRtpSession* CRtpSession::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
							   TSockAddr& aRemoteAddr, RConnection& aConnection,
							   TInt aPriority, TUint aRxSize,
							   const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpSession::NewL() ... "));
	CRtpSession* that = new (ELeave) CRtpSession(aPriority, aRxSize);
	CleanupStack::PushL(that);
	that->ConstructL(aServer, aLocalAddr, aRemoteAddr, aConnection, aCNAME);
	CleanupStack::Pop(that);
	return that;
	}

/**
Creates a new session and initialises it.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aServer     The socket server
@param aLocalAddr  The local address of the device from which an RTP packet is being sent
@param aRemoteAddr The remote address of the device to which we need to send the packet
@param aRxSize  The recvieve size of the buffer
@param aPriority   The priority to be used for internal active objects
@param aCNAME      The CNAME. A sensible default is supplied if none is
                   provided.

@return Pointer to newly created CRtpSession object
*/

CRtpSession* CRtpSession::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
							   TSockAddr& aRemoteAddr,
							   TInt aPriority, TUint aRxSize,
							   const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpSession::NewL() ... "));
	CRtpSession* that = new (ELeave) CRtpSession(aPriority, aRxSize);
	CleanupStack::PushL(that);
	that->ConstructL(aServer, aLocalAddr, aRemoteAddr, aCNAME);
	CleanupStack::Pop(that);
	return that;
	}

/**
Opens the session and initialises it.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aSocket     The connected socket.
@param aMaxRXSize  The maximum size of a received packet.
@param aRtcpSocket The RTCP socket.
@param aPriority   The priority to be used for internal active objects
@param aCNAME      The CNAME. A sensible default is supplied if none is
                   provided.

@return Pointer to newly created CRtpSession object
*/
CRtpSession* CRtpSession::NewL(RSocket& aSocket, TUint aMaxRXSize,
							   RSocket& aRtcpSocket,
							   TInt aPriority,
							   const TDesC8& aCNAME)

	{
	__RTP_LOG(_L("CRtpSession::NewL() ... "));
	CRtpSession* that = new (ELeave) CRtpSession(aPriority, aMaxRXSize);
	CleanupStack::PushL(that);
	that->ConstructL(aSocket, aRtcpSocket, aCNAME);
	CleanupStack::Pop(that);
	return that;
	}

/**
Opens the session and initialises it.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aSocket     The connected socket.
@param aMaxRXSize  The maximum size of a received packet.
@param aPriority   The priority to be used for internal active objects

@return Pointer to newly created CRtpSession object
*/
CRtpSession* CRtpSession::NewL(RSocket& aSocket, TUint aMaxRXSize,
							   TInt aPriority)
	{
	__RTP_LOG(_L("CRtpSession::NewL() ... "));
	CRtpSession* that = new (ELeave) CRtpSession(aPriority, aMaxRXSize);
	CleanupStack::PushL(that);
	that->ConstructL(aSocket);
	CleanupStack::Pop(that);
	return that;
	}



// Version 2

CRtpSession* CRtpSession::NewL(RSocketServ& aServer, TSockAddr& aLocalAddr,
							   TSockAddr& aRemoteAddr, RSubConnection& aSubConnection,
							   TInt aPriority, TUint aRxSize,
							   const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpSession::NewL() ... "));
	CRtpSession* that = new (ELeave) CRtpSession(aPriority, aRxSize);
	CleanupStack::PushL(that);
	that->ConstructL(aServer, aLocalAddr, aRemoteAddr, aSubConnection, aCNAME);
	CleanupStack::Pop(that);
	return that;
	}

/**
Closes the session but does not delete the heap-allocated memory for the session.
*/
void CRtpSession::Close()
	{
	__RTP_LOG(_L("CRtpSession::Close() ... "));
	iErrorHandler->CloseSession();
	}

/**
Closes the session and also deletes the heap-allocated memory for the session.

Note that this is asynchronous close and hence the memory may not be deleted immediately after
this call returns.
*/
void CRtpSession::CloseSession()
	{
	__RTP_LOG(_L("CRtpSession::CloseSession() ... "));
	if (iEventModel.CanCloseNow())
		delete this;
	}

/**
Closes the session and also deletes the heap-allocated memory for the session.

Note that this is an synchronous close and hence the memory is deleted immediately .
*/
void CRtpSession::CloseNow()
	{
	__RTP_LOG(_L("CRtpSession::CloseNow() ... "));
	delete this;
	}

/**
Deletes all the heap-allocated memory of this session.

No events will be generated  after the destructor is called.
*/
CRtpSession::~CRtpSession()
	{
	__RTP_LOG(_L("CRtpSession::~CRtpSession()"));
	CancelAllRequests();
	delete iWaitingPacket;
	delete iNewSourcePacket;
	delete iSendSource;
	delete iRtcpListener;
	delete iRtpSocketController;
	delete iCNAME;
	delete iNonRtpData;
	iLastRRPart.Reset();
	iLastSdesPart.Reset();
	iEventModel.Close();
	// Remove the streams backwards as they remove themselves from the
	// array as they close
	for (TInt i = iReceiveSources.Count() - 1; i >= 0; i--)
		{
		__RTP_LOG1(_L("iReceiveSources %d"), i);
		delete iReceiveSources[i].iSource;
		}
	iReceiveSources.Close();
	}

/* The Non Leaver calls the leaving variant. This is just lazy coding
 * so the artefacts from CActive days can be reused. DoProcessRtpDataL >> CActive::RunL()
 * and HandleError >> CActive::RunError().
 */
void CRtpSession::DataAvailable(TInt aErrorCode)
    {
    __RTP_LOG(_L("CRtpSession::DataAvailable() ... "));
    TRAPD(err, DoProcessRtpDataL(aErrorCode));
    if(err)
        {
        __RTP_LOG1(_L("\tDoProcessRtpDataL Left ErrorCode == %d"), err);
        HandleError(err);
        }
    }

void CRtpSession::DoProcessRtpDataL(TInt aErrorCode)
	{
	__RTP_LOG(_L("CRtpSession::DoProcessRtpDataL() ... "));
	// Start waiting for the new packet first, so that if anything
	// leaves, we are rady for next time. We need to copy the status
	// and packet before they get overwritten by WaitForPacketL
	// Note that packet is not on the cleanup stack; currently nothing
	// can leave before the packet is either deleted or ownership is
	// transferred. Make sure this stays true.
	TRtpReceivePacket* packet = iWaitingPacket;
     TInt status = aErrorCode;
	TBool shouldAllocate = ETrue;
	if(KErrNoMemory == status)
		{
		// An asynchronous operation in rtpcore may have left with KErrNoMemory
		// and although we might succeed in allocation new packet this time
		// there not much point to try because the current packet might hold corrupted data anyway.
		// Not to mention if rtpcore leaves with KErrNoMemory even if we succeed with allocation
		// in WaitForPacketL() we are going to leave anyway just after that...
		__RTP_LOG(_L("\tstatus == KErrNoMemory"));
		shouldAllocate = EFalse;
		}
	if(KErrDisconnected == status || KErrNotReady == status)
		{
		// If the status is KErrDisconnected and KErrNotReady, allocation not required
		__RTP_LOG1(_L("\tstatus == %d"), status);
		shouldAllocate = EFalse;
		}

	if(KErrNone != status)
		{
		__RTP_LOG1(_L("\tstatus == %d"), status);
		shouldAllocate = EFalse;
		}

	//non RTP data is received
	if(KErrCorrupt == status)
		{
		__RTP_LOG1(_L("\tstatus == %d"), status);
		shouldAllocate = ETrue;
		}

     WaitForPacketL(shouldAllocate);
	if(KErrCorrupt == status)
		{
		delete iNonRtpData;
		iNonRtpData = packet;
		iEventModel.HandleEventL(ENonRtpDataReceived, KErrNone, this);
		}
	// will leave if rtpcore completed with an error but will be ready for next packet!
	User::LeaveIfError(status);
	TRtpEventType type;
	// upload the iHeader in this packet so we can use it from now on via Header()
	packet->ReadIntoHeader();
	if (packet->Data().Size() == packet->Data().MaxSize())
		{
		// The buffer is actually 1 byte bigger than it 'should
		// be'. So if it's full, the packet is actually too
		// big. Throw it away and generate an error. (We ignore
		// the situation where the packet is actually 1 byte too
		// big, and assume that we've got a partial packet that's
		// useless to us, because we don't know how big it really is.)
		delete packet;
		type = ERtpBufferOverflow;
		}
	else if (packet->IsTooShort())
		{
		// The packet is shorter than its header should be, so we'll
		// overrun the descriptor if we try to make sense of it. Throw
		// it away
		delete packet;
		type = ERtpUndersizedPacket;
		}
	else
		{
		// See if there already exists a relevant stream for this packet.
		CRtpReceiveSource* source = FindSource(packet->SSRC());
		if (source)
			{
			// NewPacketL always takes ownership of the new packet.
			source->NewPacketL(packet, iErrorHandler);
			iErrorHandler = this;
			type = ERtpPacketReceived;
			}
		else
			{
			// New stream needed.
			delete iNewSourcePacket;
			iNewSourcePacket = packet;
			type = ERtpNewSource;
			status = iNewSourcePacket->Header()->iSSRC;
			}
		}

	__RTP_LOG1(_L("\t[Type] == 0x%x"), type);

	if (type < ERtpSessionEventEnd)
		{
		iEventModel.HandleEventL(type, status, this);

		// If it's a new stream event and a new stream has been
		// created in response to it then activate the stream
		// now. (The callbacks will have set up callbacks on the new
		// stream, so the first ERtpPacketReceived event can now be
		// sent)
		if (type == ERtpNewSource && iNewSource)
			iNewSource->ActivateL(iErrorHandler);
		iErrorHandler = this;

		iNewSource = NULL;
		}
	}

/**
    Does the Error Handling 
*/
TInt CRtpSession::HandleError(TInt aError)
	{
	__RTP_LOG(_L("CRtpSession::HandleError() ... "));
	__RTP_LOG1(_L("\t[Error] == %d"), aError);

	if (!CheckForClose())
		{
		TInt error;
		TRAP(error, iErrorHandler->HandleErrorL(aError));
		CheckForClose();
		__ASSERT_DEBUG(error == KErrNone ||
					   error == KErrCloseSession ||
					   error == KErrCloseCurrent,
					   Panic(ERtpLeaveInResponseToError));
		}

	return KErrNone;
	}

/**
Checks if the session can be closed.
*/
TBool CRtpSession::CheckForClose()
	{
	__RTP_LOG(_L("CRtpSession::CheckForClose() ... "));
	TInt error = iErrorHandler->WhatShouldClose();
	if (error == KErrCloseSession)
		{
		delete this;
		__RTP_LOG(_L("\tETrue"));
		return ETrue;
		}
	if (error == KErrCloseCurrent)
		{
		/* If the Error handler was not CRtpSession then we need to
		   set it to CRtpSession because the Closed handle is no longer
		   valid */
		MEventSource *currHandler = iErrorHandler;
		MEventSource *sessionHandler = this;
		if(iErrorHandler != sessionHandler)
			{
			__RTP_LOG(_L("\tSetting Error Handler to RtpSession"));
			iErrorHandler = this;
			}
		currHandler->CloseNow();

		__RTP_LOG(_L("\tETrue"));
		return ETrue;
		}
	__RTP_LOG(_L("\tEFalse"));
	return EFalse;
	}

/**
Finds out from the event model what should get closed.
*/
TInt CRtpSession::WhatShouldClose()
	{
	__RTP_LOG(_L("RRtpSession::WhatShouldClose() ... "));
	return iEventModel.WhatShouldClose();
	}

/**
This function simply forwards the call to the event model for
handling the error.

@param aError     The error type
*/
void CRtpSession::HandleErrorL(TInt aError)
	{
	// Handle errors by sending an event
	__RTP_LOG(_L("CRtpSession::HandleError() ..."));
	__RTP_LOG1(_L("\t[Error] == %d"), aError);
	iEventModel.HandleEventL(ERtpSessionFail, aError, this);
	}

/**
 Cancel any outstanding requests from us and our siblings.
*/
void CRtpSession::CancelAllRequests()
	{
	/* This function maynot be required */
	//TODO Verify if this function is really needed.
	//All requests are cancelled when the object is deleted.
	__RTP_LOG(_L("CRtpSession::CancelAllRequests() ... "));
	// The handlers use thier own iStatus for Recv
	if(iRtpSocketController)
	    {
	    if(iRtpSocketController->RtpHandler())
	        {
	        iRtpSocketController->RtpHandler()->Cancel();
	        }
	    if (iRtcpListener)
	        iRtcpListener->CancelAllRequests();
	    }
	}

/**
Waits for an RTP packet to be received. This is actually an asynchrnous
call as this active objects' RunL() wont be called till an RTP packet is
not received.

@param aDoAllocate     Indicates whether a new buffer should be allocated internally
					   to receive the RTP packet
*/
void CRtpSession::WaitForPacketL(TBool aDoAllocate)
	{
	// Create the buffer 1 byte 'too big'. That way, if it is full, we
	// know the packet has overflowed.
	__RTP_LOG(_L("CRtpSession::WaitForPacketL()"));
	__RTP_LOG1(_L("\t[DoAllocate] == %d"), aDoAllocate);

	// If the allocation fails, we wait again with the previous packet
	// but leave with KErrNoMemory. This means that packets will be
	// discarded after receiving them until memory is available to
	// process them, but means that the system is always waiting for
	// new packets, and so can recover if memory becomes
	// available. (The alternative would be to keep the packet which
	// has just been received in the hope that the client can find
	// enough memory to process it, but stop receiving any new
	// packets, which is probably far less useful). However, when this
	// is called during construction, there is no pre-existing packet
	// so we should just leave if the allocation fails.
	TRtpReceivePacket* newPacket = NULL;
	// This function might be called in oom situatin so it wouldn't
	// make much sence to try to allocate another memory and old packet will be re-used
	if(aDoAllocate)
	    {
	    if((TUint)iRxSize<=5120*3) //15K mainly done for VS App!
	        {
	        newPacket = TRtpReceivePacket::New(iRxSize + 1);
	        }
	    else
	        {
			__RTP_LOG1(_L("\tInValid PacketSize [iRxSize] == %d"), iRxSize);
	        User::Leave(KErrArgument);
	        }
	        
	    }


	if (newPacket)
		iWaitingPacket = newPacket;

	if (iWaitingPacket)
		{
		__ASSERT_DEBUG(iRtpSocketController,Panic(ERtpCoreController));
		iRtpSocketController->RtpHandler()->Read(iWaitingPacket->Data());
		}

	if (!newPacket)
		{
		__RTP_LOG(_L("\tUser::Leave(KErrNoMemory)"));
		User::Leave(KErrNoMemory);
		}
	}

/**
This function removes the given RTP receive source from the
ongoing RTP-session.

@param aSource     Pointer to the RTP source to be removed from the RTP-session
*/
void CRtpSession::RemoveRXSource(const CRtpReceiveSource* aSource)
	{
	__RTP_LOG(_L("CRtpSession::RemoveRXSource() ..."));
	__RTP_LOG1(_L("\t[CRtpReceiveSource] == %d"), aSource);
	for (TInt i = 0;; i++)
		{
		if (iReceiveSources[i].iSource == aSource)
			{
			iReceiveSources.Remove(i);
			return;
			}
		}
	}

/**
Sends an RTP packet.

@param aPacket     The RTP packet to be sent
@param aStatus     The status of the request - whether the RTP packet was sent
				   or not.
*/
void CRtpSession::Send(TRtpSendPacket* aPacket, TRequestStatus& aStatus)
	{
	__RTP_LOG(_L("CRtpSession::Send() ..."));
	__ASSERT_DEBUG(iRtpSocketController,Panic(ERtpCoreController));
	iRtpSocketController->RtpHandler()->Send(aPacket->Data(),aStatus);
	}

/**
Second phase constructor that allocates memory for inernal data members.

This function also activates the RTP session.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aServer     The socket server
@param aLocalAddr  The local address of the device from which an RTP packet is being sent
@param aRemoteAddr The remote address of the device to which we need to send the packet
@param aConnection  The connection to be used
@param aCNAME      The CNAME. A sensible default is supplied if none is
                   provided.
*/
void CRtpSession::ConstructL(RSocketServ& aServer,
							TSockAddr& aLocalAddr,
							TSockAddr& aRemoteAddr,
							RConnection& aConnection,
							const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpSession::ConstructL() ... "));
	__RTP_LOG1(_L("\t[LocalAddrPort] == %u"), aLocalAddr.Port());
	__RTP_LOG1(_L("\t[LocalAddrFamily] == %u"), aLocalAddr.Family());
	__RTP_LOG1(_L("\t[RemoteAddrPort] == %u"), aRemoteAddr.Port());
	__RTP_LOG1(_L("\t[RemoteAddrFamily] == %u"), aRemoteAddr.Family());
	//__RTP_LOG1(_L8("\t[CNAME] == %S"), &aCNAME);
	iRemoteAddr = aRemoteAddr;
	iCNAME = aCNAME.AllocL();
	iRtpSocketController = CRtpControllerBase::NewL(aServer,aLocalAddr,aCNAME,EPriorityNormal);
	User::LeaveIfError(iRtpSocketController->SetRtpStreamParameters(KRtpDefaultMaxDropout, KRtpDefaultMaxMisorder, 1));
	iRtpSocketController->SetRemoteAddr(aRemoteAddr);
	iRtpSocketController->ActivateL(ERtpCL,aConnection);
	
	/* Set RTP Data observer. Session waits for RTP Packets */
	iRtpSocketController->RtpHandler()->SetDataObserver(this);
	//Trying Out a RTCP Recv
	/**/
	CRtpHandlerBase* rtcpHandler = iRtpSocketController->RtcpHandler();
	if (rtcpHandler)
		{
		iRtcpListener = CRtcpListener::NewL(*rtcpHandler, *this);
        rtcpHandler->SetDataObserver(iRtcpListener);
		iRtcpListener->StartListening();
		}
	WaitForPacketL();
	}

/**
Second phase constructor that allocates memory for inernal data members.

This function also activates the RTP session.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aServer     The socket server
@param aLocalAddr  The local address of the device from which an RTP packet is being sent
@param aRemoteAddr The remote address of the device to which we need to send the packet
@param aCNAME      The CNAME. A sensible default is supplied if none is
                   provided.
*/
void CRtpSession::ConstructL(RSocketServ& aServer,
							TSockAddr& aLocalAddr,
							TSockAddr& aRemoteAddr,
							const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpSession::ConstructL() ... "));
	__RTP_LOG1(_L("\t[LocalAddrPort] == %u"), aLocalAddr.Port());
	__RTP_LOG1(_L("\t[LocalAddrFamily] == %u"), aLocalAddr.Family());
	__RTP_LOG1(_L("\t[RemoteAddrPort] == %u"), aRemoteAddr.Port());
	__RTP_LOG1(_L("\t[RemoteAddrFamily] == %u"), aRemoteAddr.Family());
	//__RTP_LOG1(_L8("\t[CNAME] == %S"), &aCNAME);
	iRemoteAddr = aRemoteAddr;
	iCNAME = aCNAME.AllocL();
	iRtpSocketController = CRtpControllerBase::NewL(aServer,aLocalAddr,aCNAME,EPriorityNormal);
	iRtpSocketController->SetRtpStreamParameters(KRtpDefaultMaxDropout, KRtpDefaultMaxMisorder, 1);
	iRtpSocketController->SetRemoteAddr(aRemoteAddr);
	iRtpSocketController->ActivateL(ERtpCL);

   /* Set RTP Data observer. Session waits for RTP Packets */
   iRtpSocketController->RtpHandler()->SetDataObserver(this);

	CRtpHandlerBase* rtcpHandler = iRtpSocketController->RtcpHandler();
	if (rtcpHandler)
		{
		iRtcpListener = CRtcpListener::NewL(*rtcpHandler, *this);
        rtcpHandler->SetDataObserver(iRtcpListener);
		iRtcpListener->StartListening();
		}
	WaitForPacketL();
	}


/**
Second phase constructor of RTP session

@param aSocket     The socket containing address of local and remote host.
*/
void CRtpSession::ConstructL(RSocket& aSocket)
	{
	__RTP_LOG(_L("CRtpSession::ConstructL() ... "));
	iRtpSocketController = CRtpControllerBase::NewL(aSocket,EPriorityNormal);
	User::LeaveIfError(iRtpSocketController->SetRtpStreamParameters(KRtpDefaultMaxDropout, KRtpDefaultMaxMisorder, 1));
	TSockAddr addr;
	aSocket.RemoteName(addr);
	iRtpSocketController->SetRemoteAddr(addr);
	iRtpSocketController->ActivateL(ERtpConnected);
    /* Set RTP Data observer. Session waits for RTP Packets */
    iRtpSocketController->RtpHandler()->SetDataObserver(this);

	WaitForPacketL();
	}


/**
Second phase constructor that allocates memory for inernal data members.

This function also activates the RTP session.

No events will be generated until after control returns to the
active scheduler. The client has this time to initialise the
object by adding event callbacks.

@param aServer     The socket server
@param aRtcpSocket The RTCP socket containing the address of both the local as
				   well as the remote address.
@param aCNAME      The CNAME. A sensible default is supplied if none is
                   provided.
*/
void CRtpSession::ConstructL(RSocket& aSocket,
							RSocket& aRtcpSocket,
							const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("CRtpSession::ConstructL() ... "));
	__RTP_LOG1(_L("\t[LocalAddrPort] == %u"), aSocket.LocalPort());
	__RTP_LOG1(_L("\t[RtcpLocalAddrPort] == %u"), aRtcpSocket.LocalPort());
	//__RTP_LOG1(_L8("\t[CNAME] == %S"), &aCNAME);
	iCNAME = aCNAME.AllocL();
	iRtpSocketController = CRtpControllerBase::NewL(aSocket, aRtcpSocket,aCNAME,EPriorityNormal);
	iRtpSocketController->SetRtpStreamParameters(KRtpDefaultMaxDropout, KRtpDefaultMaxMisorder, 1);
	TSockAddr addr;
	aSocket.RemoteName(addr);
	iRtpSocketController->SetRemoteAddr(addr);
	iRtpSocketController->ActivateL(ERtpConnected);

    /* Set RTP Data observer. Session waits for RTP Packets */
    iRtpSocketController->RtpHandler()->SetDataObserver(this);

	__ASSERT_DEBUG(iRtpSocketController->RtcpHandler(),Panic(ERtpCoreController));
	CRtpHandlerBase* rtcpHandler = iRtpSocketController->RtcpHandler();
	if(rtcpHandler)
		{
		iRtcpListener = CRtcpListener::NewL(*rtcpHandler, *this);
        rtcpHandler->SetDataObserver(iRtcpListener);
		iRtcpListener->StartListening();
		}
	WaitForPacketL();
	}


void CRtpSession::ConstructL(RSocketServ& aServer,
							TSockAddr& aLocalAddr,
							TSockAddr& aRemoteAddr,
							RSubConnection& aSubConnection,
							const TDesC8& aCNAME)
	{
	__RTP_LOG(_L("RRtpSession::ConstructL() ... "));
	iRemoteAddr = aRemoteAddr;
	iCNAME = aCNAME.AllocL();
	// Last Parameter -  version(2)
	iRtpSocketController =CRtpControllerBase::NewL(aServer,aLocalAddr,EPriorityNormal);
	iRtpSocketController->ActivateL(ERtpCF,aSubConnection);
	iRtpSocketController->SetRemoteAddr(aRemoteAddr);

    /* Set RTP Data observer. Session waits for RTP Packets */
    iRtpSocketController->RtpHandler()->SetDataObserver(this);
	//Trying Out a RTCP Recv
	/**/
	if (0 != aCNAME.Compare(KNullDesC8))
	{
		CRtpHandlerBase* rtcpHandler = iRtpSocketController->RtcpHandler();
		if (rtcpHandler)
			{
			iRtcpListener = CRtcpListener::NewL(*rtcpHandler, *this);
			 rtcpHandler->SetDataObserver(iRtcpListener);
			iRtcpListener->StartListening();
			}
	}
	WaitForPacketL();
	}

// Version 2
/**
This function gets one RTCP part (minus the header) at a time
 * So rr and sr arrive separately
 * if type is App, aPtr is name+appData, aCount is SubType
 * if type is Bye, aPtr is the reason, aCount is Nothing
 * if type is SDES, aPtr is the item, aCount is the type of item
 * if type is SR, aCount is the total number of reports
 * if type is RR, ssrc is that of sender, aCount is the index of the report
 */
void CRtpSession::GotRtcpReportPacketL(const TUint aPacketType, const TUint aSenderSsrc, TDesC8& aPtr, const TUint aCount)
	{
	__RTP_LOG(_L("RRtpSession::GotRtcpReportPacketL() ... "));
	TPtrC8 data;
	TInt start = 0;

	CRtpReceiveSource* source = FindSource(aSenderSsrc);
	switch(aPacketType)
		{
		case ERtcpSR:
			{
			__RTP_LOG(_L("RtcpSR Recieved ... "));
			if (source)
				{
				data.Set(aPtr.Left(KRtcpSRSenderInfoSize));
				source->SetLastSR(data, aSenderSsrc);
				source->NotifyRtcpReceivedL(ERtpSR, KErrNone);
				// Dont need to reset iLastSRPart
				}
			start += KRtcpSRSenderInfoSize;
			} //Follow through
		case ERtcpRR:
			{
			__RTP_LOG(_L("RtcpRR Recieved ... "));
			TRtcpRRItem rr;
			const TUint32 local = iRtpSocketController->GetLocalSSRC();
			for (TUint count = 0; count < aCount; count++)
				{
				// Fix for Codenomican Defect 5988-6004
				if (count != 0)
					{
					start += KRtcpRRBlockSize;
					}
				// Fix for Codenomican Defect 5988-6004
				data.Set(aPtr.Mid(start, KRtcpRRBlockSize));
				CopyPacketToRR(data, rr);
				if (count == 0)
					{
					// Reset the list if this is the first item
					iLastRRPart.Reset();
					iLastRRPart.ssrc = aSenderSsrc;
					}
				// If RR is about the source, put it in session's list

				// This condition is bypassed ..in version 2 case...Symbian 17/06/08
				if (local == rr.ssrc ||( ECFRtp == iRtpSocketController->RtpHandler()->Type() ))
					{
					iLastRRPart.iRRItems.AppendL(rr);
					}
				}
			break;
			}
		default:
			break;
		}

	if (iLastRRPart.iRRItems.Count() > 0)
		{
		// This can only be the session
		NotifyRtcpReceivedL(ERtpNewRR, aSenderSsrc);
		iLastRRPart.Reset();
		}
	}

/**
This function notifies the RTP receive source whenever an APP RTCP packet is received.

@param aSenderSsrc		The RTP receive source ID
@param aPtr				The pointer to APP specific data
@param aSubType			The subtype as determined by the applciation
*/
void CRtpSession::GotRtcpAppPacketL(const TUint aSenderSsrc, TDesC8& aPtr, TUint aSubType)
	{
	__RTP_LOG(_L("RtcpAPP Recieved ... "));
	CRtpReceiveSource* source = FindSource(aSenderSsrc);
	if (source)
		{
		if (aPtr.Length() >= 4)
			{
			TPtrC8 name = aPtr.Left(4);
			TPtrC8 data = aPtr.Mid(4);
			source->SetLastAppL(name, data, aSubType);
			}
		source->NotifyRtcpReceivedL(ERtpAPP, aSenderSsrc);
		source->iAppPart.Reset();
		}
	else
	    {/*patch for poc, nokia stack supports only app pkts in compound pkts
	    error in pkt parsing cause app pkts to be notified as non rtcp data 
	    */
	    /*in wrapper call non rtcp received call back on session level app pkt received callback
	     * change wrapper code and give app pkt callback here*/
	    iRtcpListener->NotifyNonRtcpReceivedL(ENonRtcpDataReceived, KErrNone);
	     }
	}

/**
This function notifies the RTP receive source whenever a BYE RTCP packet is received.

@param aSenderSsrc		The RTP receive source ID
@param aPtr				The pointer to reason data
*/
void CRtpSession::GotRtcpByePacketL(const TUint aSenderSsrc, TDesC8& aPtr)
	{
	__RTP_LOG(_L("RtcpBye Recieved ... "));
	CRtpReceiveSource* source = FindSource(aSenderSsrc);
	if (source)
		{
		if (aPtr.Length() > 0)
			{
			source->SetLastByeL(aPtr);
			}
		source->NotifyRtcpReceivedL(ERtpBYE, aSenderSsrc);
		// Dont need to reset iLastByeReason
		}
	}

/**
This function notifies the RTP receive source whenever a SDES RTCP packet is received.

@param aSenderSsrc		The RTP receive source ID
@param aPtr				The pointer to APP specific data
@param aChunkCount		The number of the Chunks present in the SDES packet
*/
void CRtpSession::GotRtcpSdesPacketL(const TUint aSenderSsrc, TDesC8& aPtr, const TUint aChunkCount)
	{
	__RTP_LOG(_L("RtcpSdes Recieved ... "));
	TInt parsedInSDes = 0;
	TInt itemLen = 0;
	TInt sdesIndex =0;
	TRtcpSdesItem sdes;

	for (TUint count = 1; count <= aChunkCount; count++)
		{
		parsedInSDes =0;
		sdesIndex =0;
		TPtrC8 chunk(aPtr.Mid(parsedInSDes));
		const TUint32 aboutssrc = TRtcpMemLayout::Swap32(*reinterpret_cast<TUint32*>(const_cast<TUint8*>(chunk.Ptr())));
		// We are only interested in packets from the other ssrc
		CRtpReceiveSource* source = FindSource(aboutssrc);
		// if SDES from Unknown source, notify that a new Sendsource has been created.
		// Store SDES from this SendSource
		parsedInSDes += KRtpSsrcSize;
		FOREVER
			{
			//TPtrC8 chunkItem(chunk.Mid(parsedInSDes));
			TRtcpSdesItemMemLayout item(chunk, parsedInSDes);
			__RTP_LOG1(_L("\t ERtcpSDES - Item %d"), item.GetType());
			parsedInSDes+= KRtcpSDesItemFixedSize;
			sdes.iType = item.GetType();
			if (item.GetType() == EsdesNONE)
				{
				parsedInSDes+= itemLen;
				break;
				}
			itemLen = item.GetLength();
			__RTP_LOG1(_L("\t ERtcpSDES - itemLen %d"), itemLen);

			sdes.iData = chunk.Mid(parsedInSDes, itemLen);
			parsedInSDes+= itemLen;

			if (source)
				{
				source->iLastSdesPart.AppendL(sdes);
				}
			else
				{
				iLastSdesPart.AppendL(sdes);
				}

			sdesIndex += parsedInSDes;
			if (sdesIndex >= aPtr.Length())
				{
				break;
				}

			if (!(aPtr[sdesIndex]))
				{
				break;
				}
			}
		if (source)
			{
			source->NotifyRtcpReceivedL(ERtpSDES, aSenderSsrc);
			source->iLastSdesPart.Reset();
			}
		else
			{
			// This can only be for the session
			NotifyRtcpReceivedL(ERtpSDES, aSenderSsrc);
			// if the user has created a new source, we assign the sdes list to it, Activate it, and notify it
			iLastSdesPart.Reset();
			}
		}
	}


/**
This function cancels any outstanding request for sending a RTP packet
*/
void CRtpSession::CancelSend()
	{
	// RTP only
	__RTP_LOG(_L("CRtpSession::CancelSend() ... "));
	__ASSERT_DEBUG(iRtpSocketController,Panic(ERtpCoreController));
	iRtpSocketController->RtpHandler()->CancelSend();
	};

/**
This function retrieves the value of an item from SDES packet given the item type.

@param aType     The type of the item
@param aValue    The value of the item
*/
TInt CRtpSession::GetSDES(const TInt aType, TDes8& aValue)
	{
	__RTP_LOG(_L("CRtpSession::GetSDES() ... "));
	return GetSDESPart(aType, aValue, iLastSdesPart);
	}

/**
This function retrieves the value of an item from SDES packet given the item type.

@param aSsrc     The Source Synchronisation identifier
@return			  Pointer to Rtp Receive Source
*/

CRtpReceiveSource* CRtpSession::FindSource(const TUint aSsrc) const
	{
	__RTP_LOG(_L("CRtpSession::FindSource() ... "));
	const TInt count = iReceiveSources.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iReceiveSources[i].iSSRC == aSsrc)
			{
			return iReceiveSources[i].iSource;
			}
		}
	return NULL;
	}

/**
This function Notifies Rtcp Packet

@param TRtpEventType     Rtcp Event
@param aError    aError
*/

void CRtpSession::NotifyRtcpReceivedL(TRtpEventType aEvent, TInt aError)
	{
	__RTP_LOG(_L("CRtpSession::NotifyRtcpReceivedL() ... "));
	iEventModel.HandleEventL(aEvent, aError, this);
	}

/**
Gets a handle to the RTP socket;
it provides a direct access to the functionality of RTP socket.

@return The RTP socket for the current session

*/
RSocket* CRtpSession::RtpSocket()
	{
	__RTP_LOG(_L("CRtpSession::RtpSocket() ... "));
	if(iRtpSocketController->RtpHandler())
		{
		RSocket &rtpSock = iRtpSocketController->RtpSocket();
		return &rtpSock;
		}
	else
		{
		return NULL;
		}
	}

/**
Gets a handle to the RTCP socket;
it provides a direct access to the functionality of RTCP socket.

@return The RTCP socket for the current session

*/
RSocket* CRtpSession::RtcpSocket()
	{
	__RTP_LOG(_L("CRtpSession::RtcpSocket() ... "));
	if(iRtpSocketController->RtcpHandler())
		{
		RSocket &rtcpSock = iRtpSocketController->RtcpSocket();
		return &rtcpSock;
		}
	else
		{
		return NULL;
		}
	}

/**
Gets a handle to the RTP controller;

@return The RTP controller handle for the current session

*/
CRtpControllerBase* CRtpSession::RtpController()
	{
	return iRtpSocketController;
	}

/**

Sets the remote RTP address for the current active session

@param aRemoteAddr Remote RTP address to be set

*/
void CRtpSession::SetRemoteAddress(const TSockAddr& aRemoteAddr)
	{
	__RTP_LOG(_L("CRtpSession::SetRemoteRtpAddress() ..."));
	iRtpSocketController->SetRemoteAddr(aRemoteAddr);
	}

/**

Gets the last received non RTP data, If no data found function leaves

@return Returns the last received non RTP data
*/
const TDesC8& CRtpSession::NonRtpDataL()
	{
	__RTP_LOG(_L("CRtpSession::NonRtpDataL() ..."));
	User::LeaveIfNull(iNonRtpData);
	return iNonRtpData->Data();
	}

/**
Gets the last received non RTCP data, If no data found function leaves

@return Returns the last received non RTCP data
*/
TPtr8 CRtpSession::NonRtcpDataL()
	{
	__RTP_LOG(_L("CRtpSession::NonRtcpDataL() ..."));
	User::LeaveIfNull(iRtcpListener);
	return iRtcpListener->RtcpRecvPtr();
	}

/**
Sets the remote RTCP port for the current active RTP session

@param Remote RTCP port value to be set
*/

void CRtpSession::SetRemoteRtcpPort(TUint aPort)
	{
	__RTP_LOG(_L("CRtpSession::SetRemoteRtcpPort() ..."));
	iRtpSocketController->SetRemoteRtcpPort(aPort);
	}

/**
Gets the remote RTCP Staistics for the current RTP session

@param SSRC of the sender
@param RTCP Statistics class that stores the RTCP statistics for the
current RTP session
*/
TInt CRtpSession::RtcpStatistics(TUint32 aSsrc, TRtcpStatistics &aRtcpStatistics)
	{
	__RTP_LOG(_L("CRtpSession::RtcpStatistics() ..."));
	TInt err = KErrNone;
	err = iRtpSocketController->RtcpStatistics(aSsrc,aRtcpStatistics);
	return err;
	}

/**
This function copies the packet data to RR report block

@param aReport     The RR report block
@param aRr    The SR block
*/
void CopyPacketToRR(TDesC8& aReport, TRtcpRRItem& aRr)
	{
	__RTP_LOG(_L("CopyPacketToRR() "));
	TRtcpReportBlockMemLayout report(aReport, 0);
	aRr.ssrc = report.SSRC();
	aRr.fraction = report.Fraction();
	aRr.lost = report.CumulativeLost();
	aRr.last_seq = report.SequenceHigh();
	aRr.jitter = report.Jitter();
	aRr.lsr = report.LastSR();
	aRr.dlsr = report.LastDelay();
	}



/**
Send a non-RTP (control) data packet asynchronously

@param aSocketType SocketType that tells whether non RTP data has to go through RTP or RTCP socket

@param aData Non RTP data that has to be sent

@param aStatus Tells whether the data has been sent successfully

*/
void CRtpSession::SendDataL(TBool aSocketType, const TDesC8& aData, TRequestStatus&  aStatus)
	{
	__RTP_LOG(_L("CRtpSession::SendDataL() ..."));
	CRtpControllerBase* rtpSocketController = RtpController();
	rtpSocketController->SendDataL(aSocketType,aData,aStatus);
	return;
	}

/**
Sets the sampling rate for a Particular Payloadtype
@param aPayloadType  PayloadType ( Should be between 96-127 ) see RFC 3551
@param asamplingrate Sampling rate is in rtp ticks per msecs. ( eg for AMR-NB give 8000).
*/
TInt CRtpSession::SetSamplingRate(TInt aPayloadType, TUint aSamplingRate)
	{
	return iRtpSocketController->SetSamplingRate(aPayloadType, aSamplingRate);
	}

