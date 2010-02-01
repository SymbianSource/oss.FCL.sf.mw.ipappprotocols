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
* Name        : sipprofilelog.h
* Part of     : SIP Profile Server
* Interface   : private
* Macros for logging
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPPROFILELOG_H
#define SIPPROFILELOG_H

#include <e32std.h>    

#ifdef _DEBUG
    // RFileLogger has been redefined in Commsdebugutility.H
    // This is a bug in Symbian 9.5.
    // Using __FLOG_ACTIVE from Commsdebugutility.H to avoid build break
   #if !defined(CPPUNIT_TEST)
        // Comment the following line to turn off all kind of logging:
        #define USE_SIP_PROFILE_LOGS
    #endif
 
#endif


#ifdef USE_SIP_PROFILE_LOGS
// CLASS DECLARATION

	/**
 	* Class provides logging through RDebug (or RFileLogger) in debug builds.
 	* 'SIPProfile: ' is added automatically in front of every logged line.
 	*/
    class TSIPProfileDebug
	    {
	    public:
			inline static void Print( const TDesC16& aStr );

			inline static void Print( const TDesC16& aStr1,
				const TDesC16& aStr2 );

			inline static void Print( const TDesC16& aStr, TUint32 aValue );

			inline static void Print( const TDesC16& aStr, TUint32 aValue1,
				TUint32 aValue2 );

			inline static void Print(const TDesC16& aStr, TUint32 aValue1, 
				TUint32 aValue2, TUint32 aValue3);

			inline static void Print(const TDesC16& aStr1,
				const TDesC8& aStr2);
			private:
			inline static TBool LogDirExists( const TDesC& aPath );

			inline static void WriteToFile( const TDesC& aStr );
	
			inline static void WriteLog( const TDesC& aStr );
	    };

	#include "SipProfileLog.inl"

	/**
 	* Macro for logging single string
 	* PROFILE_DEBUG1("Hello")
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG1( a ) { TSIPProfileDebug::Print( _L( a ) ); }

	/**
 	* Macro for logging two strings
 	* PROFILE_DEBUG2("Hello", "Back")
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG2( a, b ) { TSIPProfileDebug::Print( _L( a ), _L( b ) ); }

	/**
 	* Macro for logging a string and integer
 	* PROFILE_DEBUG3("Hello", 1)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG3( a, b ) { TSIPProfileDebug::Print( _L( a ), b ); }

	/**
 	* Macro for logging a string and 2 integers
 	* PROFILE_DEBUG4("Hello", 1, 2)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG4( a, b, c ) { TSIPProfileDebug::Print( _L( a ), b , c ); }

 	/**
 	* Macro for logging a string and 3 integers
 	* PROFILE_DEBUG5("Hello", 1, 2, 3)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG5( a, b, c, d ) { TSIPProfileDebug::Print( _L( a ), b , c, d ); }

 	/**
 	* Macro for logging a string and variable string
 	* PROFILE_DEBUG6("Hello", string)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG6( a, b) { TSIPProfileDebug::Print( _L( a ), b ); }

#else // _USE_SIP_PROFILE_LOGS
	/**
 	* Macro for logging single string
 	* PROFILE_DEBUG1("Hello")
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG1( a )

	/**
 	* Macro for logging two strings
 	* PROFILE_DEBUG2("Hello", "Back")
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG2( a, b )

	/**
 	* Macro for logging a string and integer
 	* PROFILE_DEBUG3("Hello", 1)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG3( a, b )

	/**
 	* Macro for logging a string and 2 integers
 	* PROFILE_DEBUG4("Hello", 1, 2)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG4( a, b, c )

	/**
 	* Macro for logging a string and 3 integers
 	* PROFILE_DEBUG5("Hello", 1, 2, 3)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG5( a, b, c, d )

 	/**
 	* Macro for logging a string and variable string
 	* PROFILE_DEBUG6("Hello", string)
 	* Note, max length of printed line 256 characters
 	*/ 
	#define PROFILE_DEBUG6( a, b)

#endif // USE_SIP_PROFILE_LOGS


#endif // SIPPROFILELOG_H

// End of File
