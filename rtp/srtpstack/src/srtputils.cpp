/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Contains SRTP utility functions.
*
*/




// INCLUDES
#include "srtputils.h"
#include <e32math.h>
#define DES_AS_8_BIT(str) (TPtrC8((TText8*)((str).Ptr()), (str).Size()))




// ---------------------------------------------------------------------------
// TSRTPUtils::CountIV()
// ---------------------------------------------------------------------------
//
TInt TSRTPUtils::CountIV(TDes8& aRes, 
                        const TDesC8& aSalt,
                        TUint aSSRC,
                        TUint64 aIndex)
    { 
    SRTP_DEBUG_DETAIL( "TSRTPUtils::CountIV ENNTRY");
            
    TBuf8<32> firstIVTerm;    
    TBuf8<32> secondIVTerm;    
    TBuf8<32> thirdIVTerm;    
            
    GetIVFirstTerm(firstIVTerm, aSalt);
    
    GetIVSecondTerm(secondIVTerm, aSSRC);
            
    GetIVThirdTerm(thirdIVTerm, aIndex);
	
	
     
    if (aRes.Length() != 16)
        {
        SRTP_DEBUG_TINT_VALUE( "the length is", aRes.Length() );
        return KErrArgument;        
        }
           
    //first XOR 1st (=padded session salt) and 2nd (= padded SSRC) terms
    for (TInt i=0; i<16; i++)
        {
        aRes[i] = firstIVTerm[i] ^ secondIVTerm[i];        
        }      

    //next XOR result with 1st term
    for (TInt i=0; i<16; i++)
        {
        aRes[i] = thirdIVTerm[i] ^ aRes[i];        
        }      
    SRTP_DEBUG_DETAIL( "TSRTPUtils::CountIV EXIT");
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TSRTPUtils::GetIVFirstTerm()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::GetIVFirstTerm(TDes8& aRes, const TDesC8& aSalt)
    {
    TInt count = 16 - aSalt.Length();
    aRes.Copy(aSalt);
    for (TInt i = 0; i<count; i++ )
        {
        aRes.Append(0x00);        
        }            
    }


// ---------------------------------------------------------------------------
// TSRTPUtils::GetIVSecondTerm()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::GetIVSecondTerm(TDes8& aRes, TUint aSSRC)
    {
    TBuf8<32> ssrc;    
    TBuf8<32> temp;    
    
    ssrc.AppendNumUC(aSSRC, EHex); 
    TSRTPUtils::DeHex(ssrc, aRes); 
    
    for (TInt i=0; i<8; i++)
        {
        aRes.Append(0x00);        
        }

    TInt count = 16 - aRes.Length();
    for (TInt i = 0; i<count; i++ )
        {
        temp.Append(0x00);        
        }                    
        
    aRes.Insert(0, temp);        
    }


// ---------------------------------------------------------------------------
// TSRTPUtils::GetIVThirdTerm()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::GetIVThirdTerm(TDes8& aRes, TUint64 aIndex)
    {
    TBuf8<32> temp;
    TBuf8<32> index;    
            
    index.AppendNumUC(aIndex, EHex); 
    TSRTPUtils::DeHex(index, aRes); 
    
    aRes.Append(0x00);        
    aRes.Append(0x00);        
            
    TInt count = 16 - aRes.Length();
    for (TInt i = 0; i<count; i++ )
        {
        temp.Append(0x00);        
        }                    
        
    aRes.Insert(0, temp);        
    }



// ---------------------------------------------------------------------------
// TSRTPUtils::Cnt_r()
// ---------------------------------------------------------------------------
//
TUint64 TSRTPUtils::Cnt_r(const TUint64 aIndex, 
                         const TUint32 aDerivRate)
    {   
    return SrtpDIV(aIndex, aDerivRate);
    }
    
// ---------------------------------------------------------------------------
// TSRTPUtils::Cnt_key_id()
// ---------------------------------------------------------------------------
//
TUint64 TSRTPUtils::Cnt_key_id(const TUint8 aLabel,
                              const TUint64 a_R,
                              const TUint64 aIndexLength)
    {   
    return (aLabel * aIndexLength + a_R);
    }    

// ---------------------------------------------------------------------------
// TUint16 TSRTPUtils::Read16()
// ---------------------------------------------------------------------------
//
TUint16 TSRTPUtils::Read16( const TUint8* const aPointer )
    {
    return static_cast<TUint16>( aPointer[1] + ( aPointer[0] << 8 ) );
    }    

// ---------------------------------------------------------------------------
// TSRTPUtils::Write16()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::Write16( TUint8* const aPointer, TUint16 aValue )
    {
    // check value range (16 bits)
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF00 ) >> 8 );
    aPointer[1] = static_cast<TUint8>( aValue & 0x00FF );
    }
    
// ---------------------------------------------------------------------------
// TSRTPUtils::Write32()
// Write a 32-bit aValue as 4 consecutive bytes in MSB order
// Memory (at least 4 bytes) must have been allocated to aPointer
// before the function is called.
// ---------------------------------------------------------------------------
void TSRTPUtils::Write32( TUint8* const aPointer, TUint32 aValue )
    {
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF000000 ) >> 24 );
    aPointer[1] = static_cast<TUint8>( ( aValue & 0x00FF0000 ) >> 16 );
    aPointer[2] = static_cast<TUint8>( ( aValue & 0x0000FF00 ) >> 8 );
    aPointer[3] = static_cast<TUint8>( aValue & 0x000000FF );
    }

// ---------------------------------------------------------------------------
// TUint32 TSRTPUtils::Read32()
// Read a 32-bit aValue given as 4 consecutive bytes in MSB order
// Memory (at least 4 bytes) must have been allocated to aPointer
// before the function is called.
// ---------------------------------------------------------------------------
TUint32 TSRTPUtils::Read32( const TUint8* const aPointer )
    {
    return ( aPointer[3] +
             ( static_cast<TUint32>( aPointer[2] ) << 8 ) +
             ( static_cast<TUint32>( aPointer[1] ) << 16 ) +
             ( static_cast<TUint32>( aPointer[0] ) << 24 ) );
    }
                             
// ---------------------------------------------------------------------------
// TSRTPUtils::Cnt_x()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::Cnt_x(const TUint64 a_key_id, TDes8& aRes, const TDesC8& aSalt)
    {  
                    
    TBuf8<32> key_id;    
    TBuf8<16> key_id_dehexed;    
            
    key_id.AppendNumUC(a_key_id, EHex); 
    TSRTPUtils::DeHex(key_id, key_id_dehexed); 
        
    aRes.Copy(aSalt);
    
    TUint key_length = key_id_dehexed.Length();
    TUint salt_length = aRes.Length();
    TInt i,j;
    
    for (i=key_length-1, j=salt_length-1; i>=0; i--, j--)
        {
        aRes[j] = key_id_dehexed[i] ^ aRes[j];        
        }      
    }



// ---------------------------------------------------------------------------
// TSRTPUtils::SrtpDIV()
// ---------------------------------------------------------------------------
//
TUint64 TSRTPUtils::SrtpDIV(const TUint64 aA ,const TUint64 aB)
    {   
    if (aB == 0)
        {
        return 0;        
        }
    else
        {
        return (aA / aB);
        }
    }


// ---------------------------------------------------------------------------
// TSRTPUtils::HexVal()
// ---------------------------------------------------------------------------
//
TUint8 TSRTPUtils::HexVal(TUint8 c)
	{
	if (c >= 'a' && c <= 'f')
		return (TUint8)(c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (TUint8)(c - 'A' + 10);
	else if (c >= '0' && c <= '9')
		return (TUint8)(c - '0');
	else
		return 0;
	}

// ---------------------------------------------------------------------------
// TSRTPUtils::HexString()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::HexString(HBufC8& aString)
    {
    TPtr8 ptr=aString.Des();
    if (aString.Length()%2)
        {
        ptr.SetLength(0);
        return;
        }
    TInt i;
    for (i=0;i<aString.Length();i+=2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
        ptr[i/2]=tmp;
        }
    ptr.SetLength(aString.Length()/2);
    }	

	
// ---------------------------------------------------------------------------
// TSRTPUtils::DeHex()
// ---------------------------------------------------------------------------
//
void TSRTPUtils::DeHex(const TDesC8 &aSrc, TDes8 &aDest)
	{
	const TUint8 *s = aSrc.Ptr();
	int i = aSrc.Length();
	TUint8 d1, d2;

    TReal remainder;
    Math::Mod(remainder,i,2);
    if (remainder>0)
        {
		d1 = TSRTPUtils::HexVal(*s++);        
		aDest.Append((TUint8)(d1));	
		i--;	    		    
        }	
	    while (i > 0)
		    {
		    d1 = TSRTPUtils::HexVal(*s++);
		    d2 = TSRTPUtils::HexVal(*s++);		    
		    aDest.Append((TUint8)(d1 * 16 + d2));		    
		    i -= 2;
		    }        
	}

#ifdef _SRTP_FILE

// ---------------------------------------------------------------------------
// TInt TSRTPUtils::Print()
// 
// ---------------------------------------------------------------------------
//

void  TSRTPUtils::Print( const TDesC& aName, const TDesC& aP )
    {
    HBufC* msg = NULL;
    TRAPD( msgError, msg = HBufC::NewL( aP.Length() + 20 ) );
    if ( msgError != KErrNone )
       	{
    	delete msg; msg = NULL;
    	return;
    	}
    	
    TPtr16 b( msg->Des() );

    b.Append( KSRTPString );
   
    b.AppendFormat( _L( "%u - " ), ( TUint * ) ( TSRTPUtils::GtGetTime() ) );

    b.Append( aP );
    b.Append( _L( "\n" ) );

    TFileUtil::LogMessage( aName, b );

    delete msg;
    
    }

// ---------------------------------------------------------------------------
// TInt TSRTPUtils::Print()
// 
// ---------------------------------------------------------------------------
//
void TSRTPUtils::Print( const TDesC& aName, const TDesC& aP, TInt aValue )
    {
    HBufC* msg = NULL;
    TRAPD( msgError, msg = HBufC::NewL( aP.Length() + 106 ) );
    if ( msgError != KErrNone )
        {
    	delete msg; msg = NULL;
    	return;
    	}

    TPtr16 b( msg->Des() );
    b.Append( KSRTPString );

    //apend current time
    b.AppendFormat( _L( "%u - " ), ( TUint * ) ( TSRTPUtils::GtGetTime() ) );
    b.Append( aP );
    b.AppendFormat( _L( "<%d> " ), aValue );
    b.Append( _L( "\n" ) );

    TFileUtil::LogMessage( aName, b );

    delete msg;
    }

// ---------------------------------------------------------------------------
// TInt TFileUtil::LogMessage()
// 
// ---------------------------------------------------------------------------
//
TInt TFileUtil::LogMessage( const TFileName& name, const TDesC& mess )
    {
    RFs fs;
    RFile file;
    TFileName nameWithPath;

    nameWithPath = KDefaultPath;
    nameWithPath.Append( name );
    nameWithPath.ZeroTerminate();
    TInt err = fs.Connect();

    if ( err != KErrNone )
        return err;

    err = file.Open( fs, nameWithPath, EFileStream | EFileWrite | EFileShareExclusive );
    if ( err == KErrNotFound )
        err = file.Create( fs, nameWithPath, EFileStream | EFileWrite | EFileShareExclusive );
    if ( err != KErrNone )
        return err;

    TInt off( 0 );
    err = file.Seek( ESeekEnd, off );
    if ( err != KErrNone )
        return err;
  
    file.Write( DES_AS_8_BIT (mess) );
    file.Flush();
    file.Close();
    fs.Close();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt TFileUtil::InitLogFile()
// 
// ---------------------------------------------------------------------------
//
TInt TFileUtil::InitLogFile( const TFileName& name )
    {
    RFs fs;
    RFile file;
    TFileName nameWithPath;

    nameWithPath = KDefaultPath;
    nameWithPath.Append( name );
    nameWithPath.ZeroTerminate();
    TInt err = fs.Connect();

    if ( err != KErrNone )
        return err;

    err = file.Replace( fs, nameWithPath, EFileStream | EFileWrite | EFileShareExclusive );
    if ( err != KErrNone )
        return err;

    file.Close();
    fs.Close();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// void SrtpPrint()
// Utility function for logging a string
// ---------------------------------------------------------------------------
//
void TSRTPUtils::SrtpPrintToFile( TPtrC8 aString )
    {
	TSRTPUtils::OctetToHexString( aString, aString.Length(), KLogFile );      	
    }

// ---------------------------------------------------------------------------
// void TSRTPUtils::OctetToHexString()
// Write the Hex Character to the files
// ---------------------------------------------------------------------------
//
void
TSRTPUtils::OctetToHexString(TPtrC8 aString, int length, const TDesC& aName) 
	{
  
	  TBuf<256> test;
	  int i;
	  
	  /* double length, since one octet takes two hex characters */
	  TInt len= length *2;
	  if (len<256)
	  	{
	  	
	    for (i=0;i<length;i++)
	        {
	        TUint8 di;
	        di= (TUint8)((aString[i]>>4)&0xF);
	        test.AppendNumUC(di, EHex);
	        di=(TUint8)(aString[i]&0xF);
	        test.AppendNumUC(di, EHex);
	        }
	    test.Append( _L( "\n" ));
			
	   	TFileUtil::LogMessage( aName, test );
	  	}
	}
	
#endif

#ifdef _DEBUG
	    
// ---------------------------------------------------------------------------
// TInt TSRTPUtils::Print()
// 
// ---------------------------------------------------------------------------
//

void TSRTPUtils::Print( const TDesC& aP )
    {
    HBufC* msg = NULL;
    TRAPD( msgError, msg = HBufC::NewL( aP.Length() + 20 ) );
    if ( msgError != KErrNone )
    	{
    	delete msg; msg = NULL;
    	return;
    	}
        
    TPtr16 b( msg->Des() );

    b.Append( KSRTPString );
   
    //apend current time
    b.AppendFormat( _L( "%u - " ), ( TUint * ) ( TSRTPUtils::GtGetTime() ) );

    b.Append( aP );
    b.Append( _L( "\n" ) );

    TBuf<128> buf;
    buf.Copy( b );
    RDebug::Print( buf );

    delete msg;
    }



// ---------------------------------------------------------------------------
// TInt TSRTPUtils::Print()
// 
// ---------------------------------------------------------------------------
//
void TSRTPUtils::Print( const TDesC& aP, TInt aValue )
    {
    HBufC* msg = NULL;
    TRAPD( msgError, msg = HBufC::NewL( aP.Length() + 106 ) );
    if ( msgError != KErrNone )
       	{
    	delete msg; msg = NULL;
    	return;
    	}
    	
    TPtr16 b( msg->Des() );
    b.Append( KSRTPString );

    //apend current time
    b.AppendFormat( _L( "%u - " ), ( TUint * ) ( TSRTPUtils::GtGetTime() ) );
    b.Append( aP );
    b.AppendFormat( _L( "<%d> " ), aValue );
    b.Append( _L( "\n" ) );

    TBuf<128> buf;
    buf.Copy( b );
    RDebug::Print( buf );

    delete msg;
    }


// ---------------------------------------------------------------------------
// TUint TSRTPUtils::GtGetTime()
// Return time elapse from 01/01/1970 in tenth of millsecond
// ---------------------------------------------------------------------------
//
TUint TSRTPUtils::GtGetTime()
    {
    TTimeIntervalMicroSeconds32 uspertick;     // scale factor: number of microseconds per system tick
    UserHal::TickPeriod( uspertick );
    return static_cast<TUint>( uspertick.Int() // number of '1us periods' per system tick 
            * User::TickCount()                // number of system ticks
            / 100 );                           // in tenths of ms, note still wraps after 5 days.
    }


#endif


