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
// utility class
// 
//

/**
 @file
 @internalComponent
*/
 
#include "rtputils.h"
#include "rtcppacket.h"
#include <random.h>
#include <sys/time.h>
#include <hal.h>
#include <e32math.h>


/** Fraction of the RTCP bandwidth (in percent)to be shared among active senders.  
(This fraction was chosen so that in a typical session with one or two active senders, 
the computed report time would be roughly equal to the minimum report time so that
we don't unnecessarily slow down receiver reports.)  
The receiver fraction must be 1 - the sender fraction. 
@internalComponent
*/


const TUint KRtcpSenderBwFraction = 25;
const TUint KRtcpRcvrBwFraction = (100-KRtcpSenderBwFraction);


_LIT(KNTPBaseTime,"19000001:000000");	// 00:00, Jan 1st 1900 

_LIT(KRTP_SOCKET, "RTP-SOCKET");

#if !defined(EKA2)
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}
#endif

GLDEF_C void Panic(TRtpCorePanicCode aPanicCode)
	{
	User::Panic(KRTP_SOCKET, aPanicCode);
	}

TUint32 RtpUtils::GenerateRandomNumber32L()
	{
	// The random number could be generated as described in RFC 3550 Appendix A.6
	// using the MD5 routines published in RFC 1321 [32].
	// We are using TRandom::Random since it is as close as we've got to a cryptographically random source 
	// thus should suffice...
	// However we could do more:
	// 1) we can seed an LFSR with the value from TRandom::Random,
	// 	therefore, no collisions over the period of the LFSR.
	// 2) we can feed each consecutive value from the LFSR into a one-way hash,
	// 	in this way one can't determine the previous value entered into the LFSR 
	//  and therfore even if the LFSR algorithm is known, can't predict the next number.
	// This would help to satisfy the RFC 3550 "The initial value of the sequence number
	// SHOULD be random (unpredictable) to make known-plaintext attacks on encryption more difficult, 
	// even if the source itself does not encrypt according to the method in Section 9."
	TBuf8<4> random(4);
	TRandom::RandomL(random);
	TUint32 number = *(reinterpret_cast<const TUint32*>(random.Ptr()));
	return number;
	}

TUint16 RtpUtils::GenerateRandomNumber16()
	{
	TBuf8<2> random(2);
	TRAPD(err,TRandom::RandomL(random));
	if (err)
		return 0;
	TUint16 number = *(reinterpret_cast<const TUint16*>(random.Ptr()));
	return number;
	}


TNTPTime::TNTPTime(TUint32 aSeconds,TUint32 aMicroSeconds)
	{	
	iSeconds = aSeconds;
	iMicroSeconds = aMicroSeconds;
	}

TNTPTime::TNTPTime()
	{
	iSeconds = 0;
	iMicroSeconds = 0;	
	}

TUint32 TNTPTime::GetCompactTime()
	{
	TUint32 compact = 0;
	compact = (0xffff0000&(iSeconds << 16));
	compact |= (0x0000ffff&(iMicroSeconds >> 16));
	return compact;
	}

TRTPTime::TRTPTime()
	{
	iLastRtpTime = 0;
	iLastSystemTick = 0;
	HAL::Get(HALData::ESystemTickPeriod, iSystemTickPeriod);
	}

/**
Supplies information about the profile-specific RTP timestamps. 
    
This is used to generate the RTP timestamps in RTCP packets.
It shall not be used to generate timestamps in RTP packets as they must
accurately relate to the time of the sample.
@internalComponent
@param aNow        The time now in RTP format
@param aConversion The number of nanoseconds per RTP timer tick.
*/ 
void TRTPTime::SetTimeConversion(TUint aNow, TUint aConversion)
	{	
	iLastRtpTime = aNow;
	// we have to update system tick as well!
	iLastSystemTick = User::TickCount();
	iRtpTickPeriod = aConversion;
	}

void TRTPTime::SetTimeStamp(TUint aNow)
	{	
	iLastRtpTime = aNow;
	// we have to update system tick as well!
	iLastSystemTick = User::TickCount();
	}

/**
Calculate 'current' RTP time-stamp given the last recorded RTP time 
and a conversion rate.
    
This is used to generate the RTP timestamps in RTCP packets only.
It shall not be used to generate timestamps in RTP packets as they must
accurately relate to the time of the sample.
@internalComponent
@return The 'current' time in RTP format
@leave KErrDivideByZero If RTP tick period is not set it would result 
		in division by zero.
*/
TUint32 TRTPTime::GetTimeStampL()
	{
	if(0 != iRtpTickPeriod)
		{
		TUint systemTick = User::TickCount();
		TUint tempSystemTick = systemTick;
		// lets get time increase in ticks since last RTP time was recorded
		if (systemTick < iLastSystemTick)
			{
			// overflow
			}
		else
			{
			systemTick -= iLastSystemTick;
			}
		// lets get time increase in microseconds since last RTP was recorde
		TUint rtpTimeChange = systemTick*iSystemTickPeriod;
		// lets get time increase in RTP time units since last RTP was recorde
		rtpTimeChange /= iRtpTickPeriod;
		iLastRtpTime += rtpTimeChange;
		iLastSystemTick = tempSystemTick;
		}
	else
		{
		User::Leave(KErrDivideByZero);
		}
	return iLastRtpTime;
	}

TInt RtpUtils::GetNTPTime(TUint32& aSeconds,TUint32& aMicroSeconds)
	{	
	//We could have used gettimeofday which implements the code below anyway...
	//BUT unfotunately it uses HomeTime rather then UniversalTime :-(
	
  	TTime t;
	t.UniversalTime();
	
	// Made change as part of the fix for the defect DEF101908. 
	// Used NTP Time(00:00, Jan 1st 1900) instead of Unix based time(00:00,Jan 1st 1970).	
	TTime NtpBaseTime(KNTPBaseTime);
	
	TTimeIntervalMicroSeconds usec = t.MicroSecondsFrom(NtpBaseTime);
	aSeconds = usec.Int64() / 1000000;
	aMicroSeconds = usec.Int64() % 1000000;
	
	return 0;
	}


TInt RtpUtils::GetNTPTime(TNTPTime& aTime)
	{	
	TUint32 seconds = 0;
	TUint32 useconds = 0;
	TInt error = RtpUtils::GetNTPTime(seconds,useconds);
	aTime = TNTPTime(seconds,useconds);
	return error;
	}

TNTPTime RtpUtils::GetNTPTimeL()
	{
	TNTPTime time(0,0);
	User::LeaveIfError(RtpUtils::GetNTPTime(time));
	return time;	
	}	

/** 
Computes the deterministic calculated interval, measured in microseconds.

Base upon example from RFC3550 A.7 Computing the RTCP Transmission Interval

This function would have to be extended for an implementation that
allows the RTCP bandwidth fractions for senders and non-senders to be
specified as explicit parameters rather than fixed values of 25% and 75%.  
The extended implementation would need to avoid division by zero 
if one of the parameters was zero.

The microsecond time interval is represented as 32bit number so the range is +2147483647, 
which is +35 minutes, 47 seconds. It would have to be a session with really low bandwidth 
and hundreds of participants to go over half hour.

@internalComponent
@param aMembers The most current estimate for the number of session members
@param aSenders The most current estimate for the number of senders in the session
@param aRtcpBw  The target RTCP bandwidth, i.e., the total bandwidth
      that will be used for RTCP packets by all members of this session,
      in octets per second.  This will be a specified fraction of the
      "session bandwidth" parameter supplied to the application at startup.
@param aSentRecently Flag that is true if the application has sent data
      since the 2nd previous RTCP report was transmitted.
@param aAvgRtcpSize The average compound RTCP packet size, in octets,
      over all RTCP packets sent and received by this participant.  The
      size includes lower-layer transport and network protocol headers
      (e.g., UDP and IP) as explained in Section 6.2.
@param aNotSendingYet Flag that is true if the application has not yet sent
      an RTCP packet.
@return Calculated RTCP interval - max(KRtcpMinTimeMicroSeconds32,+35 minutes, 47 seconds)

NOTE: Not fully implemented yet and not tested
*/
TTimeIntervalMicroSeconds32
RtpUtils::RtcpIntervalMicroSeconds32(TUint aMembers,TUint aSenders,
									TUint aRtcpBw,
									TBool aSentRecently,
									TUint aAvgRtcpSize,
									TBool aNotSendingYet, TInt64& aRandSeed)
	{
	__ASSERT_ALWAYS(aRtcpBw>0,Panic(ERtpBandWidthNotSet));
	TUint32 rtcpMinTime = KRtcpMinTimeMicroSeconds32;
	TInt n = aMembers;	// no. of members for computation
    // Very first call at application start-up uses half the min
    // delay for quicker notification while still allowing some time
    // before reporting for randomization and to learn about other
    // sources so the report interval will converge to the correct
    // interval more quickly.
	if (aNotSendingYet) 
		{
		rtcpMinTime /= 2;
		}
	// Dedicate a fraction of the RTCP bandwidth to senders unless
    // the number of senders is large enough that their share is
    // more than that fraction.
	//if (aSenders <= aMembers * 100/KRtcpSenderBwFraction) 
	if (aSenders <= aMembers * KRtcpSenderBwFraction/100)
		{
		if (aSentRecently) 
			{
			aRtcpBw *= 100;
			aRtcpBw /= KRtcpSenderBwFraction;
			n = aSenders;
			} 
		else 
			{
			aRtcpBw *= 100;
			aRtcpBw /= KRtcpRcvrBwFraction;
			n -= aSenders;
			}
		}
	//__RTP_LOG2(_L("RTCP Bandwidth  %u \n Senders %d  "),aRtcpBw,aSenders);

	// The effective number of sites times the average packet size is
	// the total number of octets sent when each site sends a report.
	// Dividing this by the effective bandwidth gives the time
	// interval over which those packets must be sent in order to
	// meet the bandwidth target, with a minimum enforced.  In that
	// time interval we send one report so this time is also our
	// average time between reports.

#ifdef EKA2	
	TInt64 t = ((aAvgRtcpSize*n)*1000000)/aRtcpBw;
	if (t < rtcpMinTime) 
		t = rtcpMinTime;
#else
	TInt64 t;
	t.Set(0,aAvgRtcpSize);
	t *= TInt64(n);
	t = t * 1000000;
    t /= TInt64(aRtcpBw);

	if (t.Low() < rtcpMinTime && t.High() == 0 ) 
		t.Set(0,rtcpMinTime);
#endif    
   
	// FROM RFC "To avoid traffic bursts from unintended synchronization with
	// other sites, we then pick our actual next report interval as a
	// random number uniformly distributed between 0.5*t and 1.5*t."
	// We don't really want to use float point arithmetics here..
	// so lets get uniformly distributed random number between 
	// lets say between 0 - 10^3
	const TInt uniformUpperLimit = 1000;
	// Rand generates a stream of uniformly distributed pseudo-random integers. 
	// The numbers are in the range, 0 to KMaxTInt
	TInt ranno = Math::Rand(aRandSeed);
	// And we want to transform this into a random number in the range 0-uniformUpperLimit
	// so we calculate an integer scale factor S:
	const TInt S = KMaxTInt / uniformUpperLimit ;

    TInt rannoUpperLimit = S * uniformUpperLimit;
	while (ranno > rannoUpperLimit)
		{
		ranno = Math::Rand(aRandSeed);
		}
	
	ranno %= uniformUpperLimit;
    // Now we can get time  between 0.5*t and 1.5*t
	t = (t * (ranno + uniformUpperLimit/2) )/uniformUpperLimit;
	// To compensate for "timer reconsideration" converging to a value below the intended average.
    // defined in RFC as double const COMPENSATION = 2.71828 - 1.5;
    // and used as t = t / COMPENSATION;
	const TInt compensation = 2718280 - 1500000;
	t = t * 1000000 / compensation;
		
	//__ASSERT_DEBUG(t > 0,Panic(xx));
#ifdef EKA2	
	return TTimeIntervalMicroSeconds32(t);
#else
	return TTimeIntervalMicroSeconds32(t.GetTInt());
#endif
	};

