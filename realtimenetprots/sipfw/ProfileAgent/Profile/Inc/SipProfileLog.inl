/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilelog.inl
* Part of     : SIP Profile Server
* Interface   : private
* Macros for logging
* Version     : 1.0
*
*/


#include <flogger.h>

#include <e32base.h>
#include <bautils.h>
#include <utf.h>

_LIT(KSipLogFileDir, "Sip");
_LIT(KSipLogFileName, "SipLog.txt");
_LIT(KSipLogPath, "C:\\logs\\sip\\");
_LIT(KSeparator, "---------------");

inline void TSIPProfileDebug::Print(const TDesC16& aStr) 
	{
	_LIT(KProfileFormat, "SIPProfile: %S");
	TBuf<256> str;

	str.Format(KProfileFormat, &aStr);
		
	WriteLog( str );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr1, const TDesC16& aStr2) 
	{
	_LIT(KProfileFormat, "SIPProfile: %S, %S");
	TBuf<256> str;

	str.Format(KProfileFormat, &aStr1, &aStr2);
		
	WriteLog( str );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr, TUint32 aValue) 
	{
	_LIT(KProfileFormat, "SIPProfile: %S, %d");
	TBuf<256> str;

	str.Format(KProfileFormat, &aStr, aValue);
		
	WriteLog( str );
	}

inline void TSIPProfileDebug::Print(const TDesC16& aStr, TUint32 aValue1,
	TUint32 aValue2) 
	{
	_LIT(KProfileFormat, "SIPProfile: %S, %d, %d");
	TBuf<256> str;

	str.Format(KProfileFormat, &aStr, aValue1, aValue2);
		
	WriteLog( str );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr, TUint32 aValue1, 
	TUint32 aValue2, TUint32 aValue3) 
	{
	_LIT(KProfileFormat, "SIPProfile: %S, %d, %d, %d");
	TBuf<256> str;

	str.Format(KProfileFormat, &aStr, aValue1, aValue2, aValue3);
		
	WriteLog( str );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr1, const TDesC8& aStr2) 
	{
	_LIT(KProfileFormat, "SIPProfile: %S, %S");
	TBuf<100> str2;
	CnvUtfConverter::ConvertToUnicodeFromUtf8(str2, aStr2);
	TBuf<256> str;

	str.Format(KProfileFormat, &aStr1, &str2);
		
	WriteLog( str );
	}

inline TBool TSIPProfileDebug::LogDirExists( const TDesC& aFolderName )
	{
	RFs fs;
	TInt ret = fs.Connect();
	
	if ( ret != KErrNone )
		{
		fs.Close();
		return EFalse;
		}
	
	if ( aFolderName.Length() == 0 )
		{
		fs.Close();
		return EFalse;
		}
	
	TParse parse;
	
	TInt retcode = parse.SetNoWild( aFolderName, NULL, NULL );
 	
	if ( retcode != KErrNone )
		{
		fs.Close();
		return EFalse;
		}
 			
	if ( parse.NameOrExtPresent() )
		{
		if ( !fs.IsValidName( aFolderName ) )
			{
			fs.Close();
			return EFalse;
			}
		}
 	
	TPtrC dirName = parse.DriveAndPath();
	RDir dir;
	retcode = dir.Open( fs, dirName, 0 );
	
	if ( retcode == KErrNone )
		{
		dir.Close();
		}
	fs.Close();
	
	return ( retcode == KErrNone );
	}

inline void TSIPProfileDebug::WriteToFile( const TDesC& aStr )
	{
	RFileLogger::Write ( KSipLogFileDir, KSipLogFileName, 
		EFileLoggingModeAppend, aStr );
	RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
		EFileLoggingModeAppend, KSeparator);		
	}
	
inline void TSIPProfileDebug::WriteLog( const TDesC& aStr )
	{
	if ( LogDirExists( KSipLogPath ) )
		{
		WriteToFile( aStr );
		}
	else
		{
		RDebug::RawPrint( aStr );
		}
	}
