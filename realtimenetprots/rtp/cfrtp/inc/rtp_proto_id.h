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
// RTP Protocol IDs.
// 
//

/**
 @file
 @internalAll
*/

#ifndef _RTP_PROTO_ID_
#define _RTP_PROTO_ID_

#include <e32def.h>
#include <rtpprepostprocessingcallback.h>

//The IDs specified in this file are temporary and might change
//when moved to an appropriate system header file.

/*
 * @internalAll
 */
const TInt KProtocolRtp  = 250;
/*
 * @internalAll
 */
const TInt KProtocolRtcp = 251;


/* RTP and RTCP socket options levels */
const TUint KSolRtp     = 0x406;
const TUint KSolRtcp    = 0x407;

/*
 * @internalAll
 */
// Supplies information about the profile-specific RTP timestamps
// See RRtpSession::SetRTPTimeConversion()
struct TRtpTimeConversion
	{
	//The time now in RTP format 
	TUint iRtpTimeNow;
	//The number of nanoseconds per RTP timer tick
	TUint iConversion;
	};

/*
 * @internalAll
 */
// Sets the number of sequential packets that must be received before a stream is considered good.
// Sets the maximum number of dropped packets to be considered a dropout, as opposed to an ended 
// and restarted stream.
// Sets the maximum number of packets by which a packet can be delayed before it is considered dropped.
// See RRtpSession::SetRtpStreamParameters()
struct TRtpStreamParams
	{
	//Minimum number of packets to be received
	//before stream is considered valid.
	TInt iMinSequential;
	//Maximum tolerable misorder in packets received
	TInt iMaxMisorder;
	TInt iMaxDropout;
	};
	
class TRtcpStatisticsParams: public TRtcpStatistics
	{
	public:
	TUint32 iSsrc;
	};

/* A Place holder for sampling rates */
class TSamplingRateInfo
	{
public:
	TInt  iPayloadType;
	TUint iSamplingRate;
	};


/**
 * RTP options
 *
 * Level: #KSolRtp
*/

/* The bandwidth allocated for an RTP Session
 * The option value is an integer
 */
const TUint KSoRtpBandwidth      = 0x700;

/* Option value is TPckgBuf of TRtpStreamParams.*/
/* set Stream specific params */
const TUint KSoStreamParams     = 0x701;

//NOTE: Mandatory Set"Opt"  
//So this is no longer an "option" :(
/* Option value TPckgBuf of TRtpTimeConversion
   Supplies information about the profile-specific RTP timestamps 
  */
const TUint KSoRtpTimeConvertion = 0x702;

//Fetches the LocalSSRC. This option cannot be Set.
//Argument is TUint
const TUint KSoLocalSSRC = 0x703;

//Enable sending/receiving non rtp data option
//Argument is TUint
const TUint KSoSendNonRtpData = 0x704;

//Disable sending/receiving non rtp data option
//Argument is TUint
const TUint KSoReceiveNonRtpData = 0x705;

/* Option value is TPckgBuf of TSamplingRateInfo */
/* Set Sampling rate info parameters */
const TUint KSoSamplingRate = 0x706;


/**
 * RTCP options
 *
 * Level: #KSolRtcp
*/

/* Option value TDes8
 * CNAME for an RTP Session.
 * Unfortunately there is a rather strange requirement here to SetThisoptioFirst.
 * Even after opening the RTCP if this option is not set RTCP is not enabled. Another
 * example of an Option not beain an "optional" any more :(   
 * */
const TUint KSoRtcpCNAME         = 0x800;

/* The bandwidth allocated for an RTP Session
 * The option value is an integer. 1 will enable autosend
 * and 0 will disable autosend.
 */
const TUint KSoRtcpAutoSend      = 0x801;

/* Option value is TTimeIntervalMicroSeconds32.
 * 
 * Sets the RTCP Auto Send interval. This disables the RFC based calculation of RTCP intervals. 
 * To re-enable auto calculation, set the interval to zero.
 * Also see RRtpSession::SetRtcpInterval()
 * */
const TUint KSoRtcpTimeInterval  = 0x802;

/* Sets the default destination address for RTCP reports.
 * See RFC 3550 Sec 6.4.2.
 * If no transmission has occured then the stack still should be
 * able to send empty RR reports to destination. If this option is 
 * not set empty RRs will not be sent before any RTP transmission or
 * receiving has occured
 */
const TUint KSoDefaultDestAddr   = 0x803;

//Enable sending/receiving non rtcp data option
//Argument is TUint
const TUint KSoSendNonRtcpData = 0x804;

//Disable sending/receiving non rtcp data option
//Argument is TUint
const TUint KSoReceiveNonRtcpData = 0x805;

//sets the remote RTCP port
//Argument is TUint
const TUint KSoRemoteRtcpPort = 0x806;

//sets the remote RTCP address
//Argument is TUint
const TUint KSoRemoteRtcpAddress = 0x807;

//RTCP statistics structure
//Argument is TUint
const TUint KSoRtcpStatistics = 0x808;

#endif // _RTP_PROTO_ID_

