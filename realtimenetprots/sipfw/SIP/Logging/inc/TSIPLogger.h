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
* Name          : TSIPLogger.h
* Part of       : Logging
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPLOGGER_H
#define TSIPLOGGER_H

#include <e32std.h>
#include <in_sock.h>

class TSIPLogger
	{
	public:
	
	    inline static void Print(const TDesC& aStr);
	    
	    
	    inline static void Print(const TDesC& aStr, 
		                         const TDesC8& aMsg);
		                         
	    inline static void Print(const TDesC& aStr, 
		                         const TDesC16& aMsg);		                         
		                         	                             
		inline static void Print(const TDesC& aStr,
		                         TUint32 aValue);
		
		inline static void Print(const TDesC& aStr,
		                         TUint32 aValue1,
		                         TUint32 aValue2);
		                         
		inline static void Print(const TDesC& aStr,
                                 const TInetAddr& aAddr);
                                 
    private:   

		inline static TBool LogDirExists( const TDesC& aFolderName );                                    
    };

#include "TSIPLogger.inl"

#endif // TSIPLOGGER_H

// End of File
