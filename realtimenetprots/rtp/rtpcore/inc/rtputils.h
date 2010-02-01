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
// Header for utilty class
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(RTP_UTILS_H__)
#define RTP_UTILS_H__

#include <e32base.h>
#include "rtpcontroller.h"


GLREF_C void Panic(TRtpCorePanicCode aPanicCode);


// Logging
#ifdef _DEBUG

	#ifdef LOG_TO_DEBUGOUT
	#include <e32debug.h>
		#define __RTP_LOG_STMT(stmt)	stmt
		#define __RTP_LOG(text) RDebug::Print(text);
		#define __RTP_LOG1(text, a) RDebug::Print(text, a);
		#define __RTP_LOG2(text, a, b) RDebug::Print(text, a, b);
		#define __RTP_LOG3(text, a, b, c) RDebug::Print(text, a, b, c);
		#define __RTP_LOG4(text, a, b, c, d) RDebug::Print(text, a, b, c, d);
		#define __RTP_LOG5(text, a, b, c, d, e) RDebug::Print(text, a, b, c, d, e);
		#define __RTP_LOG6(text, a, b, c, d, e, f) RDebug::Print(text, a, b, c, d, e ,f);
		#define __RTP_LOG7(text, a, b, c, d, e, f, g) RDebug::Print(text, a, b, c, d, e, f, g);

	#else
	#include <flogger.h>
		#define __RTP_LOG_STMT(stmt)	stmt
		#define KRtpLogDir 	_L("rtp")
		#define KRtpcoreLogFileName _L("rtpcorelog.txt")
		#define __RTP_LOG(text) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text);
		#define __RTP_LOG1(text, a) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a);
		#define __RTP_LOG2(text, a, b) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a, b);
		#define __RTP_LOG3(text, a, b, c) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a, b, c);
		#define __RTP_LOG4(text, a, b, c, d) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a, b, c, d);
		#define __RTP_LOG5(text, a, b, c, d, e) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a, b, c, d, e);
		#define __RTP_LOG6(text, a, b, c, d, e, f) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a, b, c, d, e ,f);
		#define __RTP_LOG7(text, a, b, c, d, e, f, g) RFileLogger::WriteFormat(KRtpLogDir, KRtpcoreLogFileName, EFileLoggingModeAppend, text, a, b, c, d, e, f, g);
	#endif
	
#else
	#ifndef __RTP_LOG
	#include <flogger.h>
		#define __RTP_LOG_STMT(stmt)
		#define __RTP_LOG(text)
		#define __RTP_LOG1(text, a)
		#define __RTP_LOG2(text, a, b)
		#define __RTP_LOG3(text, a, b, c)
		#define __RTP_LOG4(text, a, b, c, d)
		#define __RTP_LOG5(text, a, b, c, d, e)
		#define __RTP_LOG6(text, a, b, c, d, e, f)
		#define __RTP_LOG7(text, a, b, c, d, e, f, g)
	#endif
	
#endif



/**
@internalComponent */
struct TNTPTime
	{
	TUint iSeconds;
	TUint iMicroSeconds;
	TNTPTime(TUint32 aSeconds,TUint32 aMicroSeconds);
	TNTPTime();
	TUint32 GetCompactTime();
	inline TNTPTime& operator=(TUint32 aMicroSeconds);
	inline TNTPTime& operator=(TTimeIntervalMicroSeconds32 aInterval);
	inline TNTPTime operator+(TTimeIntervalMicroSeconds32 aInterval);
	
	inline TNTPTime& operator+=(TNTPTime& aNTPTime);
	inline TNTPTime& operator-=(TNTPTime& aNTPTime);
	inline TNTPTime operator*(TReal32 aFraction);
	inline TNTPTime operator+(TNTPTime aNTPTime);
	inline TNTPTime operator-(TNTPTime aNTPTime);
	};

/**
@internalComponent */
class TRTPTime
	{
public:
	TRTPTime();
	void SetTimeConversion(TUint aNow, TUint aConversion);
	void SetTimeStamp(TUint aNow);
	TUint32 GetTimeStampL();
	inline TBool IsValid(); 
private:
	/** The latest time in RTP format */
	TUint iLastRtpTime;
	TUint iLastSystemTick;
	/** The number of microseconds per RTP timer tick. */
	TUint iRtpTickPeriod;
	TInt iSystemTickPeriod;
	};

/**
@internalComponent */
class RtpUtils
	{
public:
	static TUint32 GenerateRandomNumber32L();
	static TUint16 GenerateRandomNumber16();
	static TInt GetNTPTime(TUint32& aSeconds,TUint32& aMicroSeconds);
	static TInt GetNTPTime(TNTPTime& aTime);
	static TNTPTime GetNTPTimeL();
	static TTimeIntervalMicroSeconds32 RtcpIntervalMicroSeconds32(TUint aMembers,TUint aSenders,
																	TUint aRtcpBw,
																	TBool aSentRecently,
																	TUint aAvgRtcpSize,
																	TBool aNotSendingYet, TInt64& aRandSeed);
private:
	RtpUtils();
};

inline TNTPTime& TNTPTime::operator=(TUint32 aMicroSeconds)
	{
	iSeconds = aMicroSeconds/1000000;
	iMicroSeconds = aMicroSeconds % 1000000;
	return *this;
	}

inline TNTPTime& TNTPTime::operator=(TTimeIntervalMicroSeconds32 aInterval)
	{
	iSeconds = aInterval.Int()/1000000;
	iMicroSeconds = aInterval.Int() % 1000000;
	return *this;
	}

inline TNTPTime TNTPTime::operator+(TTimeIntervalMicroSeconds32 aInterval)
	{
	TNTPTime time;
	time.iSeconds += aInterval.Int()/1000000;
	time.iMicroSeconds += aInterval.Int() % 1000000;
	return time;
	}
	
inline TNTPTime& TNTPTime::operator+=(TNTPTime& aNTPTime)
	{
	iMicroSeconds += aNTPTime.iMicroSeconds;
	iSeconds += aNTPTime.iSeconds;
	return *this;
	}

inline TNTPTime& TNTPTime::operator-=(TNTPTime& aNTPTime)
	{
	iMicroSeconds -= aNTPTime.iMicroSeconds;
	iSeconds -= aNTPTime.iSeconds;
	return *this;
	}
	
inline TNTPTime TNTPTime::operator*(TReal32 aFraction)
	{
	TNTPTime time;
	TUint microseconds = static_cast<TUint>((iSeconds*1000000 + iMicroSeconds)*aFraction);
	time.iSeconds = microseconds/1000000;
	time.iMicroSeconds = microseconds % 1000000;
	return time;
	}

inline TNTPTime TNTPTime::operator+(TNTPTime aNTPTime)
	{
	TNTPTime time;
	time.iMicroSeconds += aNTPTime.iMicroSeconds;
	time.iSeconds += aNTPTime.iSeconds;
	return time;	
	}

inline TNTPTime TNTPTime::operator-(TNTPTime aNTPTime)
	{
	TNTPTime time;
	time.iMicroSeconds -= aNTPTime.iMicroSeconds;
	time.iSeconds -= aNTPTime.iSeconds;
	return time;	
	}

inline TBool TRTPTime::IsValid()
	{
	return (iRtpTickPeriod != 0);
	}

#endif

