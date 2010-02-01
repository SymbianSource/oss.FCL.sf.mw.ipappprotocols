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


#ifndef SIPWLANLOG_H
#define SIPWLANLOG_H

#include <e32std.h>

// Logs are written only in debug builds
#ifdef _DEBUG

    class SipWlanLog 
	    {
	    public:

	        inline static void Print(const TDesC16& aStr);

	        inline static void Print(const TDesC16& aStr,
	                                 TInt aValue);	                                 
	    };
	    
	#include "sipwlanlog.inl"    
	                                 
    #define SIPWLAN_LOG( a ) SipWlanLog::Print( _L( a ) );
    #define SIPWLAN_INTLOG( a, b ) SipWlanLog::Print( _L( a ), b );

#else // _DEBUG

    #define SIPWLAN_LOG( a )
    #define SIPWLAN_INTLOG( a, b )

#endif // _DEBUG

#endif // SIPWLANLOG_H

// End of File
