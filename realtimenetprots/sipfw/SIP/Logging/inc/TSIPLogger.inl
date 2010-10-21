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

inline void TSIPLogger::Print(const TDesC& aStr)
	{
    OstTraceExt1( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16, "SIP: %S", aStr );
	}

inline void TSIPLogger::Print(const TDesC8& aStr)
    {
    //OstTraceExt1( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16, "SIP: %S", aStr );
    OstTraceExt1( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT8, "SIP: %s", aStr );
    }

inline void TSIPLogger::Print(const TDesC& aStr,
		                      const TDesC8& aMsg)
	{
    OstTraceExt2( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16_TEXT8, "SIP: %S: %s", aStr, aMsg );
	}


inline void TSIPLogger::Print(const TDesC& aStr,
		                      const TDesC16& aMsg)
	{
    OstTraceExt2( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16_TEXT16, "SIP: %S: %S", aStr, aMsg );
	}


inline void TSIPLogger::Print(const TDesC& aStr,
		                      TUint32 aValue)
	{
    OstTraceExt2( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16_UINT32, "SIP: %S: %u", aStr, aValue );
	}


inline void TSIPLogger::Print(const TDesC& aStr,
		                      TUint32 aValue1,
		                      TUint32 aValue2)
	{
    OstTraceExt3( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16_UINT32_UINT32, "SIP: %S: %u, %u", aStr, aValue1, aValue2 );
	}


inline void TSIPLogger::Print(const TDesC& aStr,
                              const TInetAddr& aAddr)
	{
	const TInt KIPv6AddrMaxLen = 39;
	TBuf<KIPv6AddrMaxLen> addrBuf;
	aAddr.Output(addrBuf);

	OstTraceExt3( TRACE_NORMAL, TSIPLOGGER_PRINT_TEXT16_TEXT16_UINT32, "SIP: %S: %S port %u", aStr, addrBuf, aAddr.Port() );
	}

