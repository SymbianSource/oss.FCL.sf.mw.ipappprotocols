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

#include "TSIPMessageLogger.h"
#include "TSIPLogLineParser.h"
#include "sipmessage.h"



// Leave some space for date and time:
const TInt KMaxLineLength = 100;

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

    __SIP_TXT8_LOG(aSubsystemName)

    WriteSipMessage (aSIPMessage);

#ifdef WRITE_EXTRA_LOGS

    __SIP_LOG("---")

    WriteExtraLogs(aSIPMessage);
#endif

    __SIP_LOG("---")

    }

// -----------------------------------------------------------------------------
// TSIPMessageLogger::WriteParseError
// -----------------------------------------------------------------------------
//
void TSIPMessageLogger::WriteParseError(
    const TDesC8& aSubsystemName,
    TInt aError)
    {
    __SIP_TXT8_LOG(aSubsystemName)
    __SIP_INT_LOG1("    SIP Codec parse error: %d", aError )



    }

// -----------------------------------------------------------------------------
// TSIPMessageLogger::WriteSipMessage
// -----------------------------------------------------------------------------
//
void TSIPMessageLogger::WriteSipMessage (const TDesC8& aSIPMessage)
    {
    TSIPLogLineParser lineParser( aSIPMessage, KMaxLineLength );

    while ( !lineParser.End() )
    {
        TPtrC8 ptr = lineParser.GetLine();
        __SIP_DES8_LOG("%s", ptr)
    }

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
            __SIP_TXT8_LOG(buf)
            buf.Zero();
            }
        }

    if (buf.Length() > 0)
        {
        __SIP_TXT8_LOG(buf)
        }
    }
#endif // WRITE_EXTRA_LOGS

#endif // USE_SIP_MESSAGE_LOG
