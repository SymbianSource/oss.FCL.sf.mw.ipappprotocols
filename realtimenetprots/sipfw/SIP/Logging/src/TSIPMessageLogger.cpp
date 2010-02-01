// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TSIPMessageLogger.cpp
// Part of       : Logging
// Version       : SIP/4.0 
//



#include "SipLogs.h"

#ifdef USE_SIP_MESSAGE_LOG

#include <flogger.h>
#include "TSIPMessageLogger.h"
#include "TSIPLogLineParser.h"
#include "sipmessage.h"

_LIT(KSIPMessageLogFileDir, "Sip");
_LIT(KSIPMessageLogFileName, "SipLog.txt");
//_LIT(KSipLogPath, "C:\\logs\\sip\\");

_LIT8(KFormatParseError, "    SIP Codec parse error: %d!");
_LIT8(KFooter, "-----");

// Leave some space for date and time:
const TInt KMaxLineLength = KLogBufferSize-50;

#ifdef WRITE_EXTRA_LOGS
const TInt KSensibleLineLength = 60;
_LIT8(KShortFooter, "---");
#endif

// -----------------------------------------------------------------------------
// TSIPMessageLogger::Write
// -----------------------------------------------------------------------------
//
void TSIPMessageLogger::Write(
    const TDesC8& aSubsystemName,
    CSIPMessage& aSIPMessage )
    {
    CBufBase* sipMessageAsText = 0;
    TRAPD( err, sipMessageAsText = aSIPMessage.ToTextHeaderPartL() );
    if (err != KErrNone)
        {
        WriteParseError( aSubsystemName, err );
        }
    else
        {
        Write( aSubsystemName, sipMessageAsText->Ptr( 0 ) );
        delete sipMessageAsText;
        }
    }

// -----------------------------------------------------------------------------
// TSIPMessageLogger::Write
// -----------------------------------------------------------------------------
//
void TSIPMessageLogger::Write(
    const TDesC8& aSubsystemName,
    const TDesC8& aSIPMessage)
    {
    TBool exists = LogDirExists( KSipLogPath );
    
    if ( exists )
        {
        RFileLogger::Write( KSIPMessageLogFileDir,KSIPMessageLogFileName,
                            EFileLoggingModeAppend, aSubsystemName);
        }
    else
        {
        RDebug::RawPrint( aSubsystemName );
        }
  
    WriteSipMessage (aSIPMessage);
    
#ifdef WRITE_EXTRA_LOGS
    if ( exists )
        {
        RFileLogger::Write( KSIPMessageLogFileDir,KSIPMessageLogFileName,
                            EFileLoggingModeAppend,KShortFooter);
      }
    else
        {
        RDebug::RawPrint( KShortFooter );
        }
    WriteExtraLogs(aSIPMessage);
#endif

    if ( exists )
        {
        RFileLogger::Write( KSIPMessageLogFileDir,KSIPMessageLogFileName,
                            EFileLoggingModeAppend,KFooter);
        }
    else
        {
        RDebug::RawPrint( KFooter );
        }
    }

// -----------------------------------------------------------------------------
// TSIPMessageLogger::WriteParseError
// -----------------------------------------------------------------------------
//
void TSIPMessageLogger::WriteParseError(
    const TDesC8& aSubsystemName,
    TInt aError)
    {
    if ( LogDirExists( KSipLogPath ) )
        {
        RFileLogger::Write( KSIPMessageLogFileDir,KSIPMessageLogFileName,
                            EFileLoggingModeAppend,
                            aSubsystemName);

        RFileLogger::WriteFormat( KSIPMessageLogFileDir,KSIPMessageLogFileName,
                                  EFileLoggingModeAppend,
                                  KFormatParseError, aError);

        RFileLogger::Write( KSIPMessageLogFileDir,KSIPMessageLogFileName,
                            EFileLoggingModeAppend,KFooter);
        }
    else
        {
        _LIT8( KFmt, "%d" );
        TBuf8<32> buf;
        buf.Format( KFmt, aError );
        RDebug::RawPrint( aSubsystemName );
        RDebug::RawPrint( buf );
        RDebug::RawPrint( KFooter );
        }
    }

// -----------------------------------------------------------------------------
// TSIPMessageLogger::WriteSipMessage
// -----------------------------------------------------------------------------
//
void TSIPMessageLogger::WriteSipMessage (const TDesC8& aSIPMessage)
    {
    TSIPLogLineParser lineParser( aSIPMessage, KMaxLineLength );
    
    if ( LogDirExists( KSipLogPath ) )
        { 
        while ( !lineParser.End())
            {
            RFileLogger::Write( KSIPMessageLogFileDir,KSIPMessageLogFileName, 
                                EFileLoggingModeAppend,
                                lineParser.GetLine());
            }
        }
    else
        {
        while ( !lineParser.End() )
            {
            RDebug::RawPrint( lineParser.GetLine() );
            }
        }
    }

// -----------------------------------------------------------------------------
// TSIPMessageLogger::LogDirExists
// -----------------------------------------------------------------------------
//
TBool TSIPMessageLogger::LogDirExists( const TDesC& aFolderName )
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
    
// -----------------------------------------------------------------------------
// TSIPMessageLogger::WriteExtraLogs
// -----------------------------------------------------------------------------
//
#ifdef WRITE_EXTRA_LOGS
void TSIPMessageLogger::WriteExtraLogs(const TDesC8& aSIPMessage)
    {
    TBuf8<KMaxLineLength> buf;
    for (TInt i = 0; i < aSIPMessage.Length(); i++)
        {
        if (aSIPMessage[i] > 0x20 && aSIPMessage[i] < 0x7f)
            {
            buf.Append(aSIPMessage[i]);
            }
        else
            {
            //Print few of the most common invisible characters with abbreviations
            switch(aSIPMessage[i])
                {
                case 0x0:
                    buf.Append(_L8("<nul>")); break;
                case 0x9:
                    buf.Append(_L8("<tab>")); break;
                case 0xa:
                    buf.Append(_L8("<lf>")); break;
                case 0xd:
                    buf.Append(_L8("<cr>")); break;
                case 0x1b:
                    buf.Append(_L8("<esc>")); break;
                case 0x20:
                    buf.Append(_L8("<sp>")); break;
                default:
                    buf.Append(_L8("#"));
                }

            }
        
        if (buf.Length() >= KSensibleLineLength)
            {
            //Buffer full, write it to log file
            RFileLogger::Write (KSIPMessageLogFileDir,KSIPMessageLogFileName,
                                EFileLoggingModeAppend, buf);
            buf.Zero();
            }
        }

    if (buf.Length() > 0)
        {
        if ( LogDirExists( KSipLogPath ) )
            {
            RFileLogger::Write (KSIPMessageLogFileDir,KSIPMessageLogFileName, 
                            EFileLoggingModeAppend, buf);
          }
        else
            {
            RDebug::RawPrint( buf );
            }
        }
    }
#endif // WRITE_EXTRA_LOGS

#endif // USE_SIP_MESSAGE_LOG
