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

inline void TSIPProfileDebug::Print(const TDesC16& aStr) 
	{
    OstTraceExt1( TRACE_NORMAL, TSIPPROFILEDEBUG_PRINT, "SIPProfile: %S", aStr );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr1, const TDesC16& aStr2) 
	{
    OstTraceExt2( TRACE_NORMAL, TSIPPROFILEDEBUG_PRINT_TWO_ARG, "SIPProfile: %S, %S", aStr1, aStr2 );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr, TUint32 aValue) 
	{
    OstTraceExt2( TRACE_NORMAL, DUP1_TSIPPROFILEDEBUG_PRINT, "SIPProfile: %S, %u", aStr, aValue );
	}

inline void TSIPProfileDebug::Print(const TDesC16& aStr, TUint32 aValue1,
	TUint32 aValue2) 
	{
    OstTraceExt3( TRACE_NORMAL, DUP2_TSIPPROFILEDEBUG_PRINT, "SIPProfile: %S, %u, %u", aStr, aValue1, aValue2 );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr, TUint32 aValue1, 
	TUint32 aValue2, TUint32 aValue3) 
	{
    OstTraceExt4( TRACE_NORMAL, DUP3_TSIPPROFILEDEBUG_PRINT, "SIPProfile: %S, %u, %u, %u", aStr, aValue1, aValue2, aValue3 );
	}


inline void TSIPProfileDebug::Print(const TDesC16& aStr1, const TDesC8& aStr2) 
	{
    OstTraceExt2( TRACE_NORMAL, DUP4_TSIPPROFILEDEBUG_PRINT, "SIPProfile: %S, %s", aStr1, aStr2 );
	}
