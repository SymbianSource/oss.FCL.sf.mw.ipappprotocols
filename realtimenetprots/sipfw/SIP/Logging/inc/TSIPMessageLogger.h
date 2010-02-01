/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TSIPMessageLogger.h
* Part of       : Logging
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#if !defined(__SIP_MESSAGE_LOGGER_H__)
#define __SIP_MESSAGE_LOGGER_H__

#include <e32std.h>
class CSIPMessage;


// If this macro is defined, the SIP message descriptor is written also so
// that the invisible characters can be seen.
//#define WRITE_EXTRA_LOGS


class TSIPMessageLogger
	{
public:

	static void Write (const TDesC8& aSubsystemName,
                                CSIPMessage& aSIPMessage);

	static void Write (const TDesC8& aSubsystemName,
                                const TDesC8& aSIPMessage);

private:

    static void WriteParseError (const TDesC8& aSubsystemName, TInt aError);

    static void WriteSipMessage (const TDesC8& aSIPMessage);

		static TBool LogDirExists( const TDesC& aFolderName );
		
#ifdef WRITE_EXTRA_LOGS
	static void WriteExtraLogs(const TDesC8& aSipMessage);
#endif

private: // The unit test class is declared as a friend:
#ifdef CPPUNIT_TEST
    friend class CSIPMessageLoggerTest;
#endif
	};

#endif // end of

// End of File
