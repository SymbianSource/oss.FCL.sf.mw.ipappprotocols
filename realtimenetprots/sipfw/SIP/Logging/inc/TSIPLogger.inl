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
* Name          : TSIPLogger.inl
* Part of       : SIP Logging
* Version       :  
*
*/



#include <e32std.h>
#include <utf.h>
#include <e32svr.h>
#include <flogger.h>
#include <f32file.h>

#include "SipLogs.h"
#include "TSIPLogger.h"

_LIT(KSipLogFileDir, "Sip");
_LIT(KSipLogFileName, "SipLog.txt");
_LIT(KSeparator, "---------------");
_LIT(KIntMessageFormat,"%S: %d");
_LIT(KTwoIntMessageFormat,"%S: %d, %d");
_LIT(KAddrMessageFormat,"%S: %S port %d");
_LIT(KSipLogPath, "C:\\logs\\sip\\");

inline void TSIPLogger::Print(const TDesC& aStr)
	{
	if ( LogDirExists( KSipLogPath ) )
		{
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend,aStr);                                  
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KSeparator);
		}
	else
		{
		_LIT(KSIPFormat, "SIP: %S");
		TBuf<256> log;
		log.Format(KSIPFormat, &aStr);
		RDebug::RawPrint(log);
		}
	}


inline void TSIPLogger::Print(const TDesC& aStr, 
		                      const TDesC8& aMsg)
	{
	if ( LogDirExists( KSipLogPath ) )
		{
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName,
			EFileLoggingModeAppend, aStr);

		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
				EFileLoggingModeAppend, aMsg);

		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KSeparator);
		}
	else
		{
		TBuf<100> msg;
		TBuf<256> log;
		TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(msg, aMsg.Left(99));

		_LIT(KSIPFormat, "SIP: %S: %S");
		log.Format(KSIPFormat, &aStr, &msg);

		RDebug::RawPrint(log);
		}
	}


inline void TSIPLogger::Print(const TDesC& aStr, 
		                      const TDesC16& aMsg)
	{
	if ( LogDirExists( KSipLogPath ) )
		{
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, aStr);
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, aMsg);                                 
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KSeparator);
		}
	else
		{
		_LIT(KSIPFormat, "SIP: %S: %S");
		TBuf<256> log;
		log.Format(KSIPFormat, &aStr, &aMsg);
		RDebug::RawPrint(log);
		}
	}


inline void TSIPLogger::Print(const TDesC& aStr,
		                      TUint32 aValue)
	{
	if ( LogDirExists( KSipLogPath ) )
		{
		RFileLogger::WriteFormat (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KIntMessageFormat, &aStr, aValue);
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KSeparator);
		}
	else
		{
		_LIT(KSIPFormat, "SIP: %S: %d");
		TBuf<256> log;
		log.Format(KSIPFormat, &aStr, aValue);
		RDebug::RawPrint(log);
		}
	}


inline void TSIPLogger::Print(const TDesC& aStr,
		                      TUint32 aValue1,
		                      TUint32 aValue2)
	{
	if ( LogDirExists( KSipLogPath ) )
		{
		RFileLogger::WriteFormat (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KTwoIntMessageFormat, &aStr, 
			aValue1, aValue2);                                
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KSeparator);
		}
	else
		{
		_LIT(KSIPFormat, "SIP: %S: %d, %d");
		TBuf<256> log;
		log.Format(KSIPFormat, &aStr, aValue1, aValue2);
		RDebug::RawPrint(log);
		}
	}

  
inline void TSIPLogger::Print(const TDesC& aStr,
                              const TInetAddr& aAddr) 
	{
	const TInt KIPv6AddrMaxLen = 39;
	TBuf<KIPv6AddrMaxLen> addrBuf;
	aAddr.Output(addrBuf);

	if ( LogDirExists( KSipLogPath ) )
		{
		RFileLogger::WriteFormat (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KAddrMessageFormat, &aStr,
			&addrBuf, aAddr.Port());                                 
		RFileLogger::Write (KSipLogFileDir, KSipLogFileName, 
			EFileLoggingModeAppend, KSeparator);
		}
	else
		{
		_LIT(KSIPFormat, "SIP: %S: %S port %d");
		TBuf<256> log;
		log.Format(KSIPFormat, &aStr, &addrBuf, aAddr.Port());
		RDebug::RawPrint(log);
		}
	}

inline TBool TSIPLogger::LogDirExists( const TDesC& aFolderName )
	{
    TBool exists( EFalse );
    RFs fs;
    if ( KErrNone == fs.Connect() )
        {
        TEntry entry;
        exists = ( fs.Entry( aFolderName, entry ) == KErrNone );
        fs.Close();        
        }
    return exists;
	}
