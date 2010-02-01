/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <e32svr.h>

// -----------------------------------------------------------------------------
// SipWlanLog::Print
// -----------------------------------------------------------------------------
//
inline void SipWlanLog::Print(const TDesC16& aStr)
    {
    _LIT(KLogFormat, "SIP WLAN monitor: %S");
    TBuf<256> str;
    str.Format(KLogFormat, &aStr);
	RDebug::RawPrint(str);
    }

// -----------------------------------------------------------------------------
// SipWlanLog::Print
// -----------------------------------------------------------------------------
//
inline void SipWlanLog::Print(const TDesC16& aStr, TInt aValue) 
    {
    _LIT(KLogFormat, "SIP WLAN monitor: %S: %d");
    TBuf<256> str;
    str.Format(KLogFormat, &aStr, aValue);
	RDebug::RawPrint(str);
    }
