/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : responsehandlerowner.h
* Part of       : SIPSec
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_RESPONSEHANDLEROWNER_H__
#define __SIPSEC_RESPONSEHANDLEROWNER_H__

// FORWARD DECLARATIONS
class CSIPResponse;
class TInetAddr;
class CSIPSecResponseHandler;
class MSIPSecUser;

// CLASS DECLARATION

class MResponseHandlerOwner
	{
public:
	/**
	* Informs the framework that the first phase of handling the SIP response
	* has been done.
	* @param aResponse SIP response
	* @param aNextHopAddr Next hop ip-address
	* @param aNextHop Next hop as descriptor
	* @param aUser The user for the possible SA entry
	* @param aUseSecurityServerHeaders Indicates if the Security-Server headers
	*		 present in aResponse should be used.
	*/
	virtual void ResponseFirstPhaseDoneL( CSIPResponse& aResponse,
							    		  TInetAddr& aNextHopAddr,
							    		  const TDesC8& aNextHop,
							    		  MSIPSecUser& aUser,
							    		  TBool aUseSecurityServerHeaders ) = 0;

	virtual void Remove( CSIPSecResponseHandler& aResponseHandler ) = 0;
	};

#endif // __SIPSEC_RESPONSEHANDLEROWNER_H__

// end of file
