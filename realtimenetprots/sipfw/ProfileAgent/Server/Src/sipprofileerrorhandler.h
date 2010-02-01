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
* Name          : sipprofileerrorhandler.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPPROFILEERRORHANDLER_H
#define MSIPPROFILEERRORHANDLER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPProfileCacheItem;

// CLASS DECLARATION
/**
* Callback for passing profile errors
*
* @lib sipsrofilesrv.exe
*/
class MSipProfileErrorHandler
    {
	public:

		/**
        * Handles a profile error
		* @param aItem The related profile
		* @param aOwnError Internal error to handle
		* @return ETrue Work normally
		*         EFalse Don't send events to client.
        */
		virtual TBool HandleProfileError(CSIPProfileCacheItem& aItem,
						  		 		 TInt aOwnError) = 0;
    };

#endif // MSIPPROFILEERRORHANDLER_H
