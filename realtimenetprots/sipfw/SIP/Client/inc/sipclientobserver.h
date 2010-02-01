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
* Name          : sipclientobserver.h
* Part of       : SIPClient
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPCLIENTOBSERVER_H
#define MSIPCLIENTOBSERVER_H

#include <e32std.h>

class CSIPRequestElements;
class CSIPResponseElements;
class MSIPHttpDigestChallengeObserver2;

/**
* @brief an interface to be implemented by users of CSIPClient
*        to be able to receive requests timeout notifications from sip stack.
*/
class MSIPClientObserver
	{
public:

    /**
    * A SIP request has been received from the network.
    * The user should use the connectin indicated by the IAP id
    * to respond to this request.
    *
    * @param aIapId IAP Id that indicates the connection from which
    *        the SIP request was received
    * @param aRequest contains local address,
    *        remote address of a sip message,
    *        as well as optional SIP message method, headers and body.
    *        The ownership is transferred.
    * @param aRequestId the id to be used when sending a SIP response.
	*/
	virtual void IncomingRequestL(TUint32 aIapId,
                                  CSIPRequestElements* aRequest,
                                  TUint32 aRequestId) = 0;

    /**
	* The transaction related to the received SIP request timed out 
    * and it has been destroyed.
    * This will be called if the user fails to create a SIP connection
    * and does not send an appropriate SIP response.
    *
    * @param aRequestId the request id of the request
	*/
	virtual void TimeOut(TUint32 aRequestId) = 0; 

    /**
	* Finds a client transaction based on its request id and
	* passes the response to that client transaction and
	* notifies the client about the received challenge.
    *
    * @param aRequestId the request id of the transaction
    * @param aResponse the received response containing the challenge(s).
    *        The ownership is transferred.
	* @param aObserver the observer that the response should be passed to
	*/	 
	virtual void ChallengeReceivedL(
	    TUint32 aRequestId,
	    CSIPResponseElements* aResponse,
	    MSIPHttpDigestChallengeObserver2& aObserver) = 0;

    /**
	* Finds a refresh based on its id and passes the response 
	* to the refresh and notifies the client about the received challenge.
    *
    * @param aRefreshId the refresh id
    * @param aResponse the received response containing the challenge(s).
    *        The ownership is transferred.
	* @param aObserver the observer that the response should be passed to
	*/	 
	virtual void ChallengeReceivedInRefreshL(
	    TUint32 aRefreshId,
	    TUint32 aRequestId,
	    CSIPResponseElements* aResponse,
	    MSIPHttpDigestChallengeObserver2& aObserver) = 0;    
	};

#endif // MSIPCLIENTOBSERVER_H

// End of File
