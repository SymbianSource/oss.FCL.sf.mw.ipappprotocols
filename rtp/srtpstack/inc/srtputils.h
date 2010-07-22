/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Contains SRTP utility functions (mostly mathematical
*                 and calculation oriented).
*
*/




#ifndef __SRTPUTILS_H__
#define __SRTPUTILS_H__

// INCLUDES
#include <e32def.h>
#include <e32err.h>
#include <e32math.h>

#include <e32std.h>
#include <f32file.h>
#include <e32hal.h>

#ifdef SRTP_UNIT_TEST_COVERAGE

#undef _DEBUG
#undef SRTP_LOG
#endif


#if ( defined( _DEBUG ) && ( defined ( SRTP_LOG )  ) )
#define _SRTP_FILE                     

#endif   

         


#if ( defined( _DEBUG ) && !( defined ( _SRTP_FILE )  ) )

#define SRTP_DEBUG_DETAIL( a ) { TSRTPUtils::Print( _L( a ) ); }
#define SRTP_DEBUG_PACKET( a ) 
#define SRTP_DEBUG_TINT_VALUE( a, b ) { TSRTPUtils::Print( _L( a ), static_cast<TInt>( b ) ); }
#define SRTP_DEBUG_TUINT_VALUE( a, b ) { TSRTPUtils::Print( _L( a ), static_cast<TInt>( b ) );}
#define SRTP_DEBUG_TUINT32_VALUE( a, b ) { TSRTPUtils::Print( _L( a ), static_cast<TInt>( b ) );}

#endif

#ifdef _SRTP_FILE 

#define SRTP_DEBUG_DETAIL( a ) { TSRTPUtils::Print( KLogFile,  _L( a ) ); }
#define SRTP_DEBUG_PACKET( a ) { TSRTPUtils::SrtpPrintToFile( a ); }
#define SRTP_DEBUG_TINT_VALUE( a, b ) { TSRTPUtils::Print( KLogFile, _L( a ), b ); }
#define SRTP_DEBUG_TUINT_VALUE( a, b ) { TSRTPUtils::Print( KLogFile, _L( a ), static_cast<TInt>( b ) ); }
#define SRTP_DEBUG_TUINT32_VALUE( a, b ) { TSRTPUtils::Print( KLogFile, _L( a ), static_cast<TInt>( b ) ); }

#endif

#if ( !defined( _DEBUG )  )

#define SRTP_DEBUG_DETAIL( a )
#define SRTP_DEBUG_PACKET( a ) 
#define SRTP_DEBUG_TINT_VALUE( a, b ) 
#define SRTP_DEBUG_TUINT_VALUE( a, b ) 
#define SRTP_DEBUG_TUINT32_VALUE( a, b ) 

#endif


// DATA TYPES
_LIT( KDefaultPath, "c:\\system\\data\\" );
_LIT( KLogFile, "LogFileSRTP.dat" );
_LIT( KSRTPString, "SRTP : ");


// 
const TUint16 K2EXP16           = 65536;
const TUint32 K2EXP32           = 4294967296;
const TUint64 KSRTPIndexLength  = 0x1000000000000;
const TUint64 KSRTCPIndexLength  = 0x100000000;

class TSRTPUtils
    {
    public: 
        /**
        * Count IV to be used as Integer Value in
        * AES-CM RTP/RTCP encryption/decryption.
        * @param aRes resulting 128 bit IV
        * @param aSalt 112 bit session salt (not padded)
        * @param aSSRC 32 bit SSRC value from packet (not padded)
        * @param aIndex 48 bit packet index (RTP, not padded) or
        *               31 bit SRTCP index (RTCP, not padded)
        * @return KErrNone/KErrArgument        
        */
        static TInt CountIV(TDes8& aRes, 
                        const TDesC8& aSalt,
                        TUint aSSRC,
                        TUint64 aIndex);
                        
                        
        /**
        * Add 16 bit padding for 112 bit session salt        
        * @param aRes resulting 128 bit padded session salt
        * @param aSalt 112 bit session salt 
        * @return void        
        */
        static void GetIVFirstTerm(TDes8& aRes, const TDesC8& aSalt);
        /**
        * Add 64 bit padding for 32 bit SSRC value
        * @param aRes resulting 96 bit padded SSRC value
        * @param aSSRC 32 bit SSRC value from packet
        * @return void        
        */
        static void GetIVSecondTerm(TDes8& aRes, TUint aSSRC);
        /**
        * Add 16 bit padding for RTP/RTCP packet index
        * @param aRes resulting 64/47 bit index
        * @param aIndex 48 bit packet index (RTP, not padded) or
        *               31 bit SRTCP index (RTCP, not padded)
        * @return void        
        */
        static void GetIVThirdTerm(TDes8& aRes, TUint64 aIndex);
    
        /*
        * r is calculated in following way:
        * 'index DIV key_derivation_rate'
        * This function is common to RTP and RTCP.
        * @param aIndex packet index
        * @param derivation rate 
        * @return 'index DIV key_derivation_rate'
        */
        static TUint64 Cnt_r(const TUint64 aIndex, const TUint32 aDerivRate);

        /*
        * key_id is calculated in following way:
        * 'label || r'
        * This function is common to RTP and RTCP.
        * @param aLabel 8 bit RTP/RTCP value
        * @param a_R, 'index DIV key_derivation_rate'
        * @param aIndexLength, packet index length (RTP or RTCP)
        * @return key_id ('label || r')
        */
        static TUint64 Cnt_key_id(const TUint8 aLabel,
                              const TUint64 a_R,
                              const TUint64 aIndexLength);

        /*
        * TUint16 SRTPUtils::Read16()
        * Read a 16-bit value given as 2 consecutive bytes in MSB order
        * Memory (at least 2 bytes) must have been allocated to pointer
        * before the function is called.
        */
        static TUint16 Read16( const TUint8* const aPointer );
         
        /*
        * Write a 16-bit value as 2 consecutive bytes in MSB order
        * Memory (at least 2 bytes) must have been allocated to pointer
        * before the function is called.
        */
        static void Write16( TUint8* const aPointer, TUint16 aValue );
        
        /*
        * TUint32 SRTPUtils::Read32()
        * Read a 32-bit value given as 2 consecutive bytes in MSB order
        * Memory (at least 2 bytes) must have been allocated to pointer
        * before the function is called.
        */
        static TUint32 Read32( const TUint8* const aPointer );
         
        /*
        * Write a 32-bit value as 2 consecutive bytes in MSB order
        * Memory (at least 2 bytes) must have been allocated to pointer
        * before the function is called.
        */
        static void Write32( TUint8* const aPointer, TUint32 aValue );
                
        /*
        * Calculates 'x', which is needed in key derivation.
        * Is calculated in following way:
        * 'key_id XOR master_salt'
        * This function is common to RTP and RTCP.
        * @param a_key_id, 
        * @param aRes, result
        * @param aSalt master salt
        * @return 'x', ('key_id XOR master_salt')
        */
        static void Cnt_x(const TUint64 a_key_id, TDes8& aRes, const TDesC8& aSalt);


        /*
        * Integer division of "a div b",
        * with exception that 'a div 0' == 0 for all a
        * This function is common to RTP and RTCP.
        * @param aA, value A
        * @param aB, value B
        * @return SRTP style "a div b"
        */
        static TUint64 SrtpDIV(const TUint64 aA ,const TUint64 aB);

        /*
        * Converts 8 bit descriptor ASCII values to hex values
        * @param aSrc, source descriptor
        * @param aDest, source descriptor
        * @return void
        */
        static void DeHex(const TDesC8 &aSrc, TDes8 &aDest);

        /*
        * Converts an 8 bit ASCII value to hex value
        * @param c, character
        * @return hex value 
        */
        static TUint8 HexVal(TUint8 c);

        /*
        * Converts a string into hex
        * @param aString, string to convert
        * @return none.
        */
        static void HexString(HBufC8& aString);

#ifdef _DEBUG        
		static TUint GtGetTime();
        
        static void Print( const TDesC& aP );
		static void Print( const TDesC& aP , TInt aValue );
		

#endif

#ifdef _SRTP_FILE
		static void Print( const TDesC& name, const TDesC& aP );
        static void Print( const TDesC& name, const TDesC& aP, TInt aValue );

		static void OctetToHexString(TPtrC8 aString, int length, const TDesC& aName);
		static void SrtpPrint( TPtrC8 aString );
		static void SrtpPrintToFile( TPtrC8 aString );
#endif
        
    };
    
#ifdef _SRTP_FILE    

class TFileUtil
    {
    public:
        static TInt LogMessage( const TFileName&, const TDesC& );
        static TInt InitLogFile( const TFileName& name );
    };

#endif

#endif // __SRTPUTILS_H__
