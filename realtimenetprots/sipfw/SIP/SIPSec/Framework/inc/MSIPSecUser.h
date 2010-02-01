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
* Name          : MSIPSecUser.h
* Part of       : SIPSec
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPSECUSER_H
#define MSIPSECUSER_H

// INCLUDES
#include <e32std.h>
#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class CSIPResponse;

// CLASS DECLARATION
/**
* A callback used for identifying a user of SIPSec cache entries and
* requesting credentials for SIP responses containing challenges.
*
*  @lib SipServer.exe
*/
class MSIPSecUser
    {
public:
	
	/**
	 * Checks whether only the realms for each challenge
	 * should be passed to the user or the whole responses containing 
	 * 1..n challenges in WWW-Authenticate or/and Proxy-Authenticate-headers.
	 * @return ETrue if only realms should be passed to the user.
	 *         EFalse if the whole responses should be passed to the user.        
	 */	
	virtual TBool PassOnlyRealmsToUser() const = 0;
	
	/**
	 * An asynchronous call to get username and password for a realm. 
	 * CSIPSec::SetCredentialsL will be called when this asyncronous request
	 * completes.
	 * @param aRealm the realm for the username and password.
	 */
	virtual void RequestCredentialsL(const TDesC8& aRealm) = 0;
	
	/**
	 * An asynchronous call to get usernames and passwords for 
	 * the challenges contained in the SIP response. 
	 * CSIPSec::SetCredentialsL or CSIPSec::IgnoreChallenge 
	 * will be called for each challenge in the SIP response.
	 * @param aResponse response that contains 1..n challenges.
	 * @param aTransactionId transaction ID
	 * @param aRefreshId refresh ID, KEmptyRefreshId if not known
	 */
	virtual void RequestCredentialsL(CSIPResponse& aResponse,
	                                 TTransactionId aTransactionId,
	                                 TRefreshId aRefreshId) = 0;
	                                 
	/**
	 * Returns a user with which the credentials can be shared with. 
	 * For example if a cache entry for a realm can be found for 
	 * a challenge for another user, but not this one,
	 * this function is used to check if the existing credentials can be used. 
	 *
	 * @param aRegistrationId registration ID associated with the trusted user
	 * @return a trusted user if it can be found, otherwise NULL. 
	 */	                                 
	virtual const MSIPSecUser* TrustedUser(TRegistrationId aRegistrationId) = 0;
	
	/**
	 * Checks whether the requests and responses should be passed
	 * to SIPSec or should SIPSec be bypassed.
	 * @return ETrue, if SIPSec should be bypassed, otherwise EFalse.
	 */
	virtual TBool ByPassSIPSec() const = 0;	
	};

#endif // MSIPSECUSER_H

// End of File
