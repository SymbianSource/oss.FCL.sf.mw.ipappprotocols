// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: M Class that gives the interface functions for pre/post processing callback
// functions for RTP/RTCP packets.
//

/**
@file
@internalComponent
*/

#ifndef PREPOSTPROCESSINGCALLBACK_H
#define PREPOSTPROCESSINGCALLBACK_H

/**
 * @internalComponent
 */
class MPrePostProcessingCallback
	{
	public:
	virtual TInt PreRtpProcessing(TDes8&) = 0;
	virtual void PostRtpProcessing(TDes8&) = 0;
	virtual TInt PreRtcpProcessing(TDes8&) = 0;
	virtual void PostRtcpProcessing(TDes8&) = 0;
	};
	
/**
@internalComponent

RTCP statistics class.

The class stores following RTCP statistics for the current RTP session

1. Cumulative number of packet sent since 
   the RTP session started until current SR report is generated
2. Number of payload octets sent since the RTP session started until 
   current SR report is generated
3. Inter arrival jitter 
4. Cumulative number of packet lost since the beginning of reception 
5. Fraction of packets lost current active RTP session since the previous 
   RR packet was sent

@see RRtpSession
@see RRtpSendSource
@see RRtpReceiveSource
*/
	
class TRtcpStatistics 
	{
private:
	TUint32 iPacketsSent;
	TUint32 iOctetsSent;
	TUint32 iJitter;
	TInt iPacketsLost;
	TUint8 iFractionLost;
public:
	inline TRtcpStatistics();
	inline TUint32 PacketsSent();
	inline TUint32 OctetsSent();
	inline TUint32 Jitter();
	inline TInt PacketsLost();
	inline TUint8 FractionLost();
	inline void SetPacketsSent(TUint32);
	inline void SetOctetsSent(TUint32);
	inline void SetJitter(TUint32);
	inline void SetPacketsLost(TInt);
	inline void SetFractionLost(TUint8);
	};

/**
Constructor for the RTCP statistics class
*/

inline TRtcpStatistics::TRtcpStatistics()
	{
	iPacketsSent = 0;
	iOctetsSent = 0;
	iJitter = 0;
	iPacketsLost = 0;
	iFractionLost = 0;
	}
	

/**
Sets the Packets sent remote RTCP Staistics for the current RTP session
    
@param Packets Sent for the current session

*/

inline void TRtcpStatistics::SetPacketsSent(TUint32 aPacketsSent)
	{
	iPacketsSent = aPacketsSent;
	}
	
/**
Sets the Octets sent remote RTCP Staistics for the current RTP session
    
@param Octets Sent for the current session

*/

inline void TRtcpStatistics::SetOctetsSent(TUint32 aOctetsSent)
	{
	iOctetsSent = aOctetsSent;
	}
	
/**
Sets the Jitter remote RTCP Staistics for the current RTP session
    
@param Jitter for the current session

*/

inline void TRtcpStatistics::SetJitter(TUint32 aJitter)
	{
	iJitter = aJitter;
	}
	
/**
Sets the Packets lost remote RTCP Staistics for the current RTP session
    
@param Packets lost for the current session

*/

inline void TRtcpStatistics::SetPacketsLost(TInt aPacketsLost)
	{
	iPacketsLost = aPacketsLost;
	}
	
/**
Sets the Fraction of Packets lost remote RTCP Staistics for the current RTP session
    
@param Fraction of Packets lost for the current session

*/

inline void TRtcpStatistics::SetFractionLost(TUint8 aFractionLost)
	{
	iFractionLost = aFractionLost;
	}
	
/**
Gets the Packets sent remote RTCP Staistics for the current RTP session
    
@return Packets Sent for the current session

*/

inline TUint32 TRtcpStatistics::PacketsSent()
	{
	return iPacketsSent;
	}

/**
Gets the Octets sent remote RTCP Staistics for the current RTP session
    
@return Octets Sent for the current session

*/

inline TUint32 TRtcpStatistics::OctetsSent()
	{
	return iOctetsSent;
	}
	
/**
Gets the Jitter remote RTCP Staistics for the current RTP session
    
@return Jitter for the current session

*/

inline TUint32 TRtcpStatistics::Jitter()
	{
	return iJitter;
	}
	
/**
Gets the Packets lost remote RTCP Staistics for the current RTP session
    
@return Packets lost for the current session

*/

inline TInt TRtcpStatistics::PacketsLost()
	{
	return iPacketsLost;
	}
	
/**
Gets the Fraction of Packets lost remote RTCP Staistics for the current RTP session
    
@return Fraction of Packets lost for the current session

*/

inline TUint8 TRtcpStatistics::FractionLost()
	{
	return iFractionLost;
	}

	
#endif // PREPOSTPROCESSINGCALLBACK_H
