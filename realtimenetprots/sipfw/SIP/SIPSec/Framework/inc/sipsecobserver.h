/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsecobserver.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPSECOBSERVER_H
#define MSIPSECOBSERVER_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
* A callback that is used for notifying when SIPSec cache has been
* updated after receving a response which lead to an asynchronous
* operation in SIPSec.
*
*  @lib SipServer.exe
*/
class MSIPSecObserver
	{
	public:

		/**
		 * Called when an asynchronous cache update has been completed.
		 * May lead to a direct call to CSIPSec::AddSecurityParamsL.
		 * @param aSuccess status of the cache update
		 *        ETrue if the cache has been successfully updated.
		 *        Otherwise EFalse.
		 */
		virtual void SIPSecCacheUpdated(TBool aSuccess) = 0;
	};
#endif // MSIPSECOBSERVER_H

// End of File
