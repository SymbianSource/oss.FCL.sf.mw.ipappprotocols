/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <e32math.h>
#include <e32hal.h>
#include "rtputil.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Parameters:
//  x   16-LSB of a positive integer
//  y   16-LSB of a positive integer  
//
// Function: The function is taking care of wrap. If the distance
//           these two number is longer than 1 << 15, then we assume
//           the larger one is the min16 of these two numbers.
//           
// Returns:
//    the min of the two input values 
// ---------------------------------------------------------------------------
TUint16 TRtpUtil::Min16( TUint16 aX, TUint16 aY )
    {
    if ( ( aX <= aY && aX > aY - ( 1 << 15 ) ) || ( aX >= aY + ( 1 << 15 ) ) )
        {
        return aX;
        }
    else
       {
        return aY;
       }
    }

// ---------------------------------------------------------------------------
// Parameters:
//  x   16-bit unsigned int 
//  y   16-bit unsigned int   
//
// Function: if the distance between aX and aY is longer than 1 << 15,
//           we assume these two number are wrapped.
// Returns:
//    1 if there is wrap around between this two values 
// ---------------------------------------------------------------------------
TUint16 TRtpUtil::Wrap16( TUint16 aX, TUint16 aY )
    {
    if ( aX < aY - ( 1 << 15 ) || aY < aX - ( 1 << 15 ) )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// Parameters:
//  x   32-LSB of a positive integer
//  y   32-LSB of a positive integer 
// ---------------------------------------------------------------------------
TUint32 TRtpUtil::Sub32( TUint32 aX, TUint32 aY )
    {
    if ( aX < aY )
        return ( 0xFFFFFFFF - ( aY - aX ) + 1 );
    else
        return ( aX - aY );
    }

// ---------------------------------------------------------------------------
// TUint8* TRtpUtil::Strcpy()
// 
// ---------------------------------------------------------------------------
//
TUint8* TRtpUtil::Strcpy( TUint8* aTarget, const TUint8* aSource )
    {
    TPtr8 ptrTarget( aTarget, ( User::StringLength( aSource ) ) + 1 );

    ptrTarget.SetMax();
    ptrTarget.Copy( _L8( aSource ) );
    ptrTarget.ZeroTerminate();

    return aTarget;
    }

// ---------------------------------------------------------------------------
// TUint TRtpUtil::GtGetTime()
// Return time elapse from 01/01/1970 in tenth of millsecond
// ---------------------------------------------------------------------------
//
TUint TRtpUtil::GtGetTime()
    {
    TTimeIntervalMicroSeconds32 uspertick;     // scale factor: number of microseconds per system tick
    UserHal::TickPeriod( uspertick );
    return static_cast<TUint>( uspertick.Int() // number of '1us periods' per system tick 
            * User::TickCount()                // number of system ticks
            / 100 );                           // in tenths of ms, note still wraps after 5 days.
    }

// ---------------------------------------------------------------------------
// TUint32 TRtpUtil::Random()
// 
// ---------------------------------------------------------------------------
//
TUint32 TRtpUtil::Random( TInt64& aSeed )
    {
    TInt32 rand = Math::Rand( aSeed );
    return Abs( rand );
    }

// ---------------------------------------------------------------------------
// TReal TRtpUtil::FloatRandom()
// 
// ---------------------------------------------------------------------------
//
TReal TRtpUtil::FloatRandom( TInt64& aSeed )
    {
    TReal rand = Math::FRand( aSeed );
    return Abs( rand );
    }


#ifdef _DEBUG 
// ---------------------------------------------------------------------------
// TInt TRtpUtil::Print()
// 
// ---------------------------------------------------------------------------
//
void TRtpUtil::Print(  const TDesC8& aP )
    {
    const TUint KPrintLength = 5;
    const TUint KPrintExtLength = 20;
    TChar notAllowed('%');
	if ( aP.Locate( notAllowed ) == KErrNotFound )
		{
		HBufC8* msg = NULL;
	    TRAPD( msgError, msg = HBufC8::NewL( aP.Length() + KPrintExtLength ) );
	    if ( msgError != KErrNone )
	        return ;

	    TPtr8 b( msg->Des() );

	    b.Append( _L8( "RTP : " ) );
	    //    b = p;
	    //apend current time
	    b.AppendFormat( _L8( "%u - " ), ( TUint * ) ( TRtpUtil::GtGetTime() ) );

	    b.Append( aP );
	    b.Append( _L8( "\n" ) );

	    TInt ptrSize = static_cast <TInt>( b.Size() );
	    TBuf<256> buf;
	    if ( ptrSize < buf.MaxSize() )
	    	{
	   	    buf.Copy( b );
		    RDebug::Print( buf );
	    	}
	    else
	    	{
	    	_LIT(KRTPWarning, "RTP: Text is too long to be print all");
	    	RDebug::Print( KRTPWarning );	
	    	buf.Copy( b.Left( buf.MaxSize() - KPrintLength ));
	    	}
	    delete msg;
		}
	else
		{
		_LIT(KRTPWarning, "RTP: Text contains not allowed characters, log ignored");
		RDebug::Print( KRTPWarning );							
		}	

    }

// ---------------------------------------------------------------------------
// TInt TRtpUtil::Print()
// 
// ---------------------------------------------------------------------------
//
void TRtpUtil::Print(  const TDesC8& aP, TInt aValue )
    {
    const TUint KPrintLength = 5;
    const TUint KPrintExtLength = 106;
    TChar notAllowed('%');
	if ( aP.Locate( notAllowed ) == KErrNotFound )
		{
		HBufC8* msg = NULL;
	    TRAPD( msgError, msg = HBufC8::NewL( aP.Length() + KPrintExtLength ) );
	    if ( msgError != KErrNone )
	        return;

	    TPtr8 b( msg->Des() );
	    b.Append( _L8( "RTP : " ) );

	    //    b = p;
	    //apend current time
	    b.AppendFormat( _L8( "%u - " ), ( TUint * ) ( TRtpUtil::GtGetTime() ) );
	    b.Append( aP );
	    b.AppendFormat( _L8( "= <%d> " ), aValue );
	    b.Append( _L8( "\n" ) );

		TInt ptrSize = static_cast <TInt>( b.Size() );
	    TBuf<256> buf;
	    if ( ptrSize < buf.MaxSize() )
	    	{
	   	    buf.Copy( b );
		    RDebug::Print( buf );
	    	}
	    else
	    	{
	    	_LIT(KRTPWarning, "RTP: Text is too long to be print all");
	    	RDebug::Print( KRTPWarning );	
	    	buf.Copy( b.Left( buf.MaxSize() - KPrintLength ));
	    	}
	    delete msg;
		}
	else
		{
		_LIT(KRTPWarning, "RTP: Text contains not allowed characters, log ignored");
		RDebug::Print( KRTPWarning );							
		}	
    }
   
    
#endif


#ifdef _RTP_LOG_FILE 

// ---------------------------------------------------------------------------
// TInt TRtpUtil::Print()
// 
// ---------------------------------------------------------------------------
//
void TRtpUtil::Print( const TDesC& aName, const TDesC8& aP )
    {
    HBufC8* msg = NULL;
    TRAPD( msgError, msg = HBufC8::NewL( aP.Length() + 20 ) );
    if ( msgError != KErrNone )
        return;

    TPtr8 b( msg->Des() );

    b.Append( _L8( "RTP : " ) );
    //    b = p;
    //apend current time
    b.AppendFormat( _L8( "%u - " ), ( TUint * ) ( TRtpUtil::GtGetTime() ) );

    b.Append( aP );
    b.Append( _L8( "\n" ) );

    TFileUtil::LogMessage( aName, b );

    delete msg;
    }

// ---------------------------------------------------------------------------
// TInt TRtpUtil::Print()
// 
// ---------------------------------------------------------------------------
//
void TRtpUtil::Print( const TDesC& aName, const TDesC8& aP, TUint32 aValue )
    {
    HBufC8* msg = NULL;
    TRAPD( msgError, msg = HBufC8::NewL( aP.Length() + 106 ) );
    if ( msgError != KErrNone )
        return;

    TPtr8 b( msg->Des() );
    b.Append( _L8( "RTP : " ) );

    //    b = p;
    //apend current time
    b.AppendFormat( _L8( "%u - " ), ( TUint * ) ( TRtpUtil::GtGetTime() ) );
    b.Append( aP );
    b.AppendFormat( _L8( "<%d> " ), aValue );
    b.Append( _L8( "\n" ) );

    TFileUtil::LogMessage( aName, b );


    delete msg;
	}

// ---------------------------------------------------------------------------
// TInt TFileUtil::LogMessage()
// 
// ---------------------------------------------------------------------------
//
TInt TFileUtil::LogMessage( const TFileName& name, const TDesC8& mess )
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

    file.Write( mess );
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
#endif



//  End of File
