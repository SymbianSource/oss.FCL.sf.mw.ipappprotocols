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

inline void SIPCRLog::Print(const TDesC16& aStr)
	{
    OstTraceExt1( TRACE_NORMAL, SIPCRLOG_PRINT_TEXT16, "SIP Client Resolver: %S", aStr );
	}

inline void SIPCRLog::Print(const TDesC16& aStr, TInt aValue) 
	{
    OstTraceExt2( TRACE_NORMAL, SIPCRLOG_PRINT_TEXT16_TINT, "SIP Client Resolver: %S: %d", aStr, aValue );
	}

inline void SIPCRLog::Print(const TDesC16& aStr1, const TDesC8& aStr2) 
	{
    OstTraceExt2( TRACE_NORMAL, SIPCRLOG_PRINT_TEXT16_TEXT8, "SIP Client Resolver: %S: %s", aStr1, aStr2 );
	}
