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
* Name          : SIPCRLogs.h
* Part of       : SIP Client Resolver
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPCRLOGS_H
#define SIPCRLOGS_H

#include <e32std.h>

#ifdef _DEBUG 
    // Comment the following line to turn off all kind of logging:
    #define USE_SIP_CR_LOGS
#endif


#ifdef USE_SIP_CR_LOGS

    class SIPCRLog 
	    {
	    public:

	        inline static void Print(const TDesC16& aStr);

	        inline static void Print(const TDesC16& aStr1, 
	                                 const TDesC8& aStr2);

	        inline static void Print(const TDesC16& aStr,
	                                 TInt aValue);
			private:
					inline static TBool LogDirExists( const TDesC& aPath );
			
					inline static void WriteToFile( const TDesC& aStr );
	    };
	    
	#include "SIPCRLogs.inl"    
	                                 
    #define SIP_CR_LOG( a ) SIPCRLog::Print( _L( a ) );
    #define SIP_CR_STR_LOG( a, b ) SIPCRLog::Print( _L( a ), b );
    #define SIP_CR_INT_LOG( a, b ) SIPCRLog::Print( _L( a ), b );

#else // _USE_SIP_CR_LOGS

    #define SIP_CR_LOG( a )
    #define SIP_CR_STR_LOG( a, b )
    #define SIP_CR_INT_LOG( a, b )

#endif // USE_SIP_CR_LOGS


#endif // SIPCRLOGS_H

// End of File
