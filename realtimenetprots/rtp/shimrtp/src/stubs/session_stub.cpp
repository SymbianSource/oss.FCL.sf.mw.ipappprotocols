// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// session.cpp
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
EXPORT_C void RRtpSession::OpenL(RSocketServ& , TSockAddr& ,
								 TSockAddr& ,
								 TInt , RConnection& ,
								 TInt , const TDesC8& )
	{
	User::Leave(KErrNotSupported);	
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
EXPORT_C void RRtpSession::OpenL(RSocketServ& /*aServer*/, TSockAddr& /*aLocalAddr*/,
								 TSockAddr& /*aRemoteAddr*/,
								 TInt /*aMaxRXSize*/, TInt /*aPriority*/, 
								 const TDesC8& /*aCNAME*/)
	{
	User::Leave(KErrNotSupported);	
	}


EXPORT_C void RRtpSession::OpenL(RSocket& /*aSocket*/, TInt /*aMaxRXSize*/, 
						RSocket& /*aRtcpSocket*/,
						TInt /*aPriority*/, 
						const TDesC8& /*aCNAME*/)
	{
	User::Leave(KErrNotSupported);	
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
EXPORT_C void RRtpSession::OpenL(RSocket& /*aSocket*/, TInt /*aMaxRXSize*/,
								 TInt /*aPriority*/)
	{
	User::Leave(KErrNotSupported);	
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
EXPORT_C void RRtpSession_Internal::OpenL(RSocketServ& , TSockAddr& ,
								 TSockAddr& ,
								 TInt , RSubConnection& ,
								 TInt , const TDesC8& )
#else
EXPORT_C void RRtpSession::OpenL(RSocketServ& , TSockAddr& ,
								 TSockAddr& ,
								 TInt , RSubConnection& ,
								 TInt , const TDesC8& )
#endif

	{
	User::Leave(KErrNotSupported);
	}


/**
Closes the session.

All RRtpSendSource and RRtpReceiveSource objects associated with this
session are closed.

@see RRtpSendSource
@see RRtpReceiveSource
*/
EXPORT_C void RRtpSession::Close()
	{
	ASSERT(0); 
	}




/**
@internalComponent
*/
EXPORT_C void RRtpSession::PrivRegisterEventCallbackL(TUint /*aType*/, 
												  TRtpCallbackFunction /*aCallback*/, 
												  TAny* /*aPtr*/,
												  TInt /*aParameter*/)
	{	
	User::Leave(KErrNotSupported);	
	}




/**
@internalComponent
*/
EXPORT_C void RRtpSession::PrivRegisterEventCallbackL(TUint /*aType*/, 
												  TRtpCallbackFunction /*aCallback*/, 
												  TAny* /*aPtr*/)
	{
	User::Leave(KErrNotSupported);	
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
EXPORT_C void RRtpSession::SetMaxRXSize(TInt /*aMaxRXSize*/)
	{
	ASSERT(0);	
	}




/**
Gets the current expected maximum size of a received packet.

@return The current expected maximum receive size.

@panic RTP 3 if the session has not been opened and initialised.
             Note that this panic is raised in debug mode only.
*/
EXPORT_C TInt RRtpSession::MaxRXSize()
	{
	return 0;
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
	ASSERT(0);
	RRtpReceiveSource r = NewReceiveSourceLC();
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
	User::Leave(KErrNotSupported);	
	RRtpReceiveSource r;
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
	User::Leave(KErrNotSupported);	
	RRtpSendSource s;
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
	RRtpSendSource s;
	return s;
	}

/**
Sends an RTCP APP packet.

@param aName     Application name(4-bytes)
@param aAppData  Application specific data
@param aSubType Application defined subtype
*/
EXPORT_C void RRtpSession::SendAPPL(const TDesC8& /*aName*/, const TDesC8& /*aAppData*/, TUint8 /*aSubType*/)
	{
	User::Leave(KErrNotSupported);	
	}

/**
Stores an SDES packet information to be sent in the impending timer-exprity event.

@param aType     The item type
@param aValue  The item value. This must be less than 256 else the 
		       function returns KErrTooBig error.

@return KErrTooBig If the size of aValue is more than 255
		KErrNone Successful operation
*/
EXPORT_C TInt RRtpSession::SetSDESL(TInt /*aType*/, const TDesC8& /*aValue*/)
	{
	User::Leave(KErrNotSupported);	
	return 0;
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
EXPORT_C TInt RRtpSession::SetPRIVL(const TDesC8& /*aPrefix*/, const TDesC8&/* aValue*/)
	{
	User::Leave(KErrNotSupported);	
	return 0;
	}

/**
Enables or disables the automatic sending of RTCP SR (Sender Report),
RR (Receiver Report), and SDES (Source Description) packets.

Note that if automatic sending is not enabled and RTCP sending is required,
then explicit calls to SendRTCPPacketL() will be required.

@param aAutoSend ETrue, if RTCP packets need to be send automatically
	             EFalse otherwise.
*/
EXPORT_C void RRtpSession::SetRTCPAutoSend(TBool /*aAutoSend*/)
	{
	ASSERT(0);
	}

/** 
Gets the RTCP Auto Send status. 

@return True, if RTCP SR, RR and SDES packets are automatically sent,
	    false otherwise

*/
EXPORT_C TBool RRtpSession::RTCPAutoSend() const
	{
	ASSERT(0);
	return EFalse;
	}

/** 
Sets the RTCP Auto Send interval.
This disables the RFC based calculation of RTCP intervals.
To re-enable auto calculation, set the interval to zero.

@param aInterval Time interval after which an RTCP auto-send will take place
*/

EXPORT_C void RRtpSession::SetRtcpInterval(TTimeIntervalMicroSeconds32& /*aInterval*/)
	{
	ASSERT(0);
	}
	
	
/** 
Enables Receive only option for rtp or rtcp

@param aRtpOrRtcp Takes the values from the enum TPacketType

*/
EXPORT_C void RRtpSession::DontReceive(TInt /*aRtpOrRtcp*/)
	{
	ASSERT(0);
	}

/** 
Sets the estimated bandwidth used by the session.

This is used to decide when to send RTCP status packets.
The default	value is read from a resource file and should be a reasonably
sensible default based on the maximum bandwidth of the handset.

@param aBandwidth The bandwidth in bps.
*/
EXPORT_C void RRtpSession::SetBandwidth(TUint /*aBandwidth*/)
	{
	ASSERT(0);
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
EXPORT_C void RRtpSession::SetRtpStreamParameters(TInt /*aMinSequential*/, TInt /*aMaxMisorder*/, TInt /*aMaxDropout*/)
	{
	ASSERT(0);
	}

/** 
Sends an RTCP packet now. 

Calling this function doesn't make much sense if auto-send is
on, as it will result in packets being sent at rather
incoherent intervals.

@param aPacket  The RTCP packet to be sent
*/
EXPORT_C void RRtpSession::SendRTCPPacketL(TDesC8& /*aPacket*/)
	{
	User::Leave(KErrNotSupported);	
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
EXPORT_C void RRtpSession::SetRTPTimeConversion(TUint /*aNow*/, TUint /*aConversion*/)
	{
	ASSERT(0);
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
EXPORT_C TInt RRtpSession::GetSDES(const TInt /*aType*/, TDes8& /*aValue*/)
	{
	return KErrNotSupported;	
	}

/**
Gets an array contining the most recent RR (Receiver Report) from
all hosts who are reporting on our send stream.

@return The array of RRs.
*/
EXPORT_C RRtcpRRPart RRtpSession::GetRRs()
	{
	ASSERT(0);
	TRtcpRRPart trpPart;
	trpPart.ssrc = 0;
	RRtcpRRPart rrPart(trpPart);
	return rrPart;
	}

/**
Gets a handle to the RTP socket; 
it provides a direct access to the functionality of RTP socket.

@return The RTP socket for the current session
		
*/									  
EXPORT_C RSocket* RRtpSession::RtpSocket()
	{
	ASSERT(0);
	return 0;
	}
    
/**
Gets a handle to the RTCP socket; 
it provides a direct access to the functionality of RTCP socket.

@return The RTCP socket for the current session
		
*/
EXPORT_C RSocket* RRtpSession::RtcpSocket()
	{
	ASSERT(0);
	return 0;
	}
	
/**

Send a non-RTP (control) data packet asynchronously
    
@param aSocketType SocketType that tells whether non RTP data has to go through RTP or RTCP socket
    
@param aData Non RTP data that has to be sent
    
@param aStatus Tells whether the data has been sent successfully

*/
EXPORT_C void RRtpSession::SendDataL(TBool , const TDesC8& , TRequestStatus&  )
	{
	ASSERT(0);
	return;
	}
	
/**

Sets the remote RTP address for the current active session
    
@param aRemoteAddr Remote RTP address to be set
    
*/
EXPORT_C void RRtpSession::SetRemoteAddress(const TSockAddr &)
	{
	ASSERT(0);
	return;
	}

/**
Gets the last received non RTP data, If no data found function leaves
    
@return Returns the last received non RTP data
*/
EXPORT_C const TDesC8& RRtpSession::NonRtpDataL()
	{
	ASSERT(0);
	HBufC8 *buf = HBufC8::NewL(100);
	return *buf;
	}
	
/**
Gets the last received non RTCP data, If no data found function leaves
    
@return Returns the last received non RTCP data
*/
EXPORT_C TPtr8 RRtpSession::NonRtcpDataL()
	{
	ASSERT(0);
	HBufC8 *buf = HBufC8::NewL(100);
	return buf->Des();
	}

/**

Disables the receipt of non RTP data
    
*/
EXPORT_C void RRtpSession::DisableNonRtpData()
	{
	ASSERT(0);
	return;
	}
	
/**
Sets the remote RTCP port for the current active RTP session
    
@param Remote RTCP port value to be set
*/
	
EXPORT_C void RRtpSession::SetRemoteRtcpPort(TUint )
	{
	ASSERT(0);
	return;
	}
	
/**
Gets the remote RTCP Staistics for the current RTP session
    
@param SSRC of the sender
@param RTCP Statistics class that stores the RTCP statistics for the 
current RTP session
*/
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C TInt RRtpSession_Internal::RtcpStatistics(TUint32 , TRtcpStatistics &)
#else
EXPORT_C TInt RRtpSession::RtcpStatistics(TUint32 , TRtcpStatistics &)
#endif
	{
	ASSERT(0);
	return KErrNotSupported;
	};


#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
EXPORT_C void RRtpSession_Internal::SetPrePostProcessingRegisterCallback(MPrePostProcessingCallback* )
#else
EXPORT_C void RRtpSession::SetPrePostProcessingRegisterCallback(MPrePostProcessingCallback* )
#endif
	{
	ASSERT(0);
	return;
	}

/**
Sets the sampling rate for a Particular Payloadtype
@param aPayloadType  PayloadType ( Should be between 96-127 ) see RFC 3551
@param asamplingrate Sampling rate is in rtp ticks per msecs. ( eg for AMR-NB give 8000).
*/
EXPORT_C TInt RRtpSession::SetSamplingRate(TInt , TUint )
	{
	ASSERT(0);
	return KErrNotSupported;
	}


