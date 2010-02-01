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
* Name          : SipRequestHandlerObserver.h
* Part of       : SIP Request Handler
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPREQUESTHANDLEROBSERVER_H
#define MSIPREQUESTHANDLEROBSERVER_H

#include "SipRequestHandlerBase.h"
class CSIPResponse;


class MSIPRequestHandlerObserver
	{
	public:

		/**
		* Destructor.
    	*/
    	virtual ~MSIPRequestHandlerObserver () { }
    
		/**
		* A client UID was resolved for the SIP request
		* @param aRequestId the request ID
		* @param aUid a client UID to which the SIP request must be routed
    	*/
		virtual void ClientFoundL(TUint32 aRequestId, TUid aUid) = 0;

		/**
		* A client UID was not found.
        * The implementation of this callback should generate 
        * a SIP error response with the provided elements and 
        * send it to network.
		* @param aRequestId a request ID
		* @param aSIPResponse a SIP error response. 
		*        The ownership is transferred.
		*/
		virtual void ClientNotFoundL(TUint32 aRequestId,
								     CSIPResponse* aSIPResponse) = 0;

		/**
		* Error occurred during resolution of the client's channel UID.
        * This callback will also be called if one of the callbacks 
        * ClientFoundL or ClientNotFoundL leaves using the leave code.
        * @param aRequestId a request ID
		* @param aError error code. 
		*/
		virtual void ErrorOccurred(TUint32 aRequestId, TInt aError) =0;
	};

#endif // MSIPREQUESTHANDLEROBSERVER_H
