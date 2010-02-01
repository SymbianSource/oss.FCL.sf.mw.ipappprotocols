/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __RTPUTIL_H
#define __RTPUTIL_H

// INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <f32file.h>

// If you need logs in release mode, just comment out the "defined( _DEBUG )"
// or change it to 1
#if ( defined( _DEBUG ) && defined ( RTP_OUTPUT_TO_FILE )  )

#define _RTP_LOG_FILE                     

#endif

#ifdef RTP_UNIT_TEST_COVERAGE

#undef _DEBUG
#undef _RTP_LOG_FILE  

#endif              
// CONSTANTS

// Note: the whole rtp module is using  tenth of ms as the internal time unit.
const TUint KTenthOfmsPerSecond = 10000;
const TUint32 KMicrosecondPerSecond = 1000000;
const TUint KMicrosecondPerMillSecond = 1000;
const TInt KRandMax = 0x7fff;

// how often a packet is dropped, ex. if 10, then a packet is drop every 10 packets 
// 9.09 (1/11) percent loss ratio 
const TUint KPacketLoss = 50;   

const TInt KPayloadTypeMax = 0x7f;

// DATA TYPES

_LIT( KDefaultPath, "c:\\system\\data\\" );
_LIT( KLogFile, "LogFileRTP.dat" );

enum TRtpRtcpEnum
    {
    ERTCP_PACKET_ERROR          = -1,       // indicates RTCP compound packet error
    ERTCP_NO_ERROR              = KErrNone, // indicates if RTCP compound is completely processed
    ERTCP_PACKET_MORE           = 1,        // indicates if RTCP compound is not completely processed
    ERTCP_FOUND_RXSTREAM        = 2         // indicates an receive stream has been found
};

enum TPortType
    {
    ERTPPort        = 0,
    ERTCPPort
    }; 



#ifdef _DEBUG 

#define RTP_DEBUG_DETAIL( a ) { TRtpUtil::Print( _L8( a ) ); }
#define RTP_DEBUG_DETAIL_DVALUE( a, b ) { TRtpUtil::Print( _L8( a ), b ); }

#define RTP_DEBUG_PACKET( a ) { TRtpUtil::Print( _L8( a ) ); }
#define RTP_DEBUG_PACKET_DVALUE( a, b ) { TRtpUtil::Print( _L8( a ), b ); }

#define RTP_DEBUG_STAT( a ) { TRtpUtil::Print( _L8( a ) ); }
#define RTP_DEBUG_STAT_DVALUE( a, b ) { TRtpUtil::Print( _L8( a ), b ); }

#define RTCP_DEBUG_DETAIL( a ) { TRtpUtil::Print( _L8( a ) ); }
#define RTCP_DEBUG_DETAIL_DVALUE( a, b ) { TRtpUtil::Print( _L8( a ), b ); }

#else

#define RTP_DEBUG_DETAIL( a ) 
#define RTP_DEBUG_DETAIL_DVALUE( a, b )
#define RTP_DEBUG_PACKET( a ) 
#define RTP_DEBUG_PACKET_DVALUE( a, b ) 
#define RTP_DEBUG_STAT( a ) 
#define RTP_DEBUG_STAT_DVALUE( a, b ) 
#define RTCP_DEBUG_DETAIL( a ) 
#define RTCP_DEBUG_DETAIL_DVALUE( a, b )

#endif

// CLASS DECLARATION

/**
*  RTP utility class. 
*
*  @lib RtpService.dll
*/
class TRtpUtil
    {
    public:
        static TUint16 Min16( TUint16 aX, TUint16 aY );
        static TUint16 Wrap16( TUint16 aX, TUint16 aY );
        static TUint32 Sub32( TUint32 aX, TUint32 aY );

        static TUint8* Strcpy( TUint8* aTarget, const TUint8* aSource );

        static TUint GtGetTime();
        static TUint32 Random( TInt64& aSeed );
        static TReal FloatRandom( TInt64& aSeed );
		
		#ifdef _DEBUG 
        static void Print( const TDesC8& p );
        static void Print( const TDesC8& p, TInt aValue );
    	#endif
    	
    	#ifdef _RTP_LOG_FILE   
    	static void Print( const TDesC& name, const TDesC8& p );
        static void Print( const TDesC& name, const TDesC8& p, TUint32 aValue );
    	#endif
    
    };

#ifdef _RTP_LOG_FILE  
class TFileUtil
    {
    public:
        static TInt LogMessage( const TFileName&, const TDesC8& );
        static TInt InitLogFile( const TFileName& name );
    };
#endif

#endif /* __RTPUTIL_H*/

// End of File
