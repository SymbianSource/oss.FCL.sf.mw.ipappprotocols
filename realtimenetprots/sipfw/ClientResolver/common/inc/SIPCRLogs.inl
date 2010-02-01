/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SIPCRLogs.inl
* Part of       : SIP Client Resolver
* Version       :  
*
*/




/**
 @internalComponent
*/

#include <e32base.h>
#include <e32svr.h>
#include <utf.h>
#include <flogger.h>
#include <bautils.h>

_LIT(KSipLogFileDir, "Sip");
_LIT(KSipLogFileName, "SipLog.txt");
_LIT(KSeparator, "---------------");
_LIT(KSipLogPath, "C:\\logs\\sip\\");

inline void SIPCRLog::Print(const TDesC16& aStr)
	{
	_LIT( KLogFormat, "SIP Client Resolver: %S" );
	TBuf<256> str;
	str.Format( KLogFormat, &aStr );
    
	if ( LogDirExists( KSipLogPath ) )
		{
		WriteToFile( str );
		}
	else
		{
		RDebug::RawPrint( str );
		}
	}

inline void SIPCRLog::Print(const TDesC16& aStr, TInt aValue) 
	{
	_LIT( KLogFormat, "SIP Client Resolver: %S: %d" );
	TBuf<256> str;
	str.Format( KLogFormat, &aStr, aValue );
	
	if ( LogDirExists( KSipLogPath ) )
		{
		WriteToFile( str );
		}
	else
		{
		_LIT(KLogFormat, "SIP Client Resolver: %S: %d");
		TBuf<256> str;
		str.Format(KLogFormat, &aStr, aValue);
		RDebug::RawPrint(str);
		}
	}

inline void SIPCRLog::Print(const TDesC16& aStr1, const TDesC8& aStr2) 
	{
	_LIT(KLogFormat, "SIP Client Resolver: %S: %S");
	TBuf<100> str2;
	CnvUtfConverter::ConvertToUnicodeFromUtf8(str2, aStr2);
	TBuf<256> str;
	str.Format(KLogFormat, &aStr1, &str2);
	
	if ( LogDirExists( KSipLogPath ) )
		{
		WriteToFile( str );
		}
	else
		{
		RDebug::RawPrint(str);    	
		}
	}

inline TBool SIPCRLog::LogDirExists( const TDesC& aFolderName )
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

inline void SIPCRLog::WriteToFile( const TDesC& aStr )
	{
	RFileLogger::Write( KSipLogFileDir, KSipLogFileName, 
		EFileLoggingModeAppend, aStr );
	RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
		EFileLoggingModeAppend, KSeparator);		
	}
