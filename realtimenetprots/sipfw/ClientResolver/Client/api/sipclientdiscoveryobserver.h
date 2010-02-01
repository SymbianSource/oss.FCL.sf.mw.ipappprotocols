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
* Name        : sipclientdiscoveryobserver.h
* Part of     : SIP Client Discovery API
* Interface   : 
* Version     : 1.0
*
*/



#ifndef MSIPCLIENTDISCOVERYOBSERVER_H
#define MSIPCLIENTDISCOVERYOBSERVER_H

#include <e32base.h>
#include <stringpool.h>

// FORWARD DECLARATIONS
class CSIPHeaderBase;

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* An observer that must be realized by the user in order to
* receive events about resolved clients' communication channels
* or occurred errors.
* @lib n/a
*/
class MSIPClientDiscoveryObserver
	{
     public: // New functions
     
		/**
		* Channel UID was resolved for the SIP request
		*
		* @param aChannel a channel UID to which the SIP request
		*		 must be routed
		* @param aRequestId a request ID
        */
		virtual void ChannelResolvedL(TUid aChannel,
		                              TUint32 aRequestId) = 0;

		/**
		* The client channel UID was not found.
        * The implementation of this callback should generate 
        * a SIP response with the provided elements and send it to network.
		*
		* @param aRequestId a request ID
		* @param aStatusCode contains a SIP response status code
        *        for the SIP response that should be generated.
		* @param aReasonPhrase contains a SIP response reason phrase
        *        for the SIP response that should be generated.
		* @param aHeaders contains well-formed optional SIP headers 
        *        that include additional information why the client 
        *        could not be found. For example Accept-headers that describe
        *        the content-types currently supported. 
        *        The ownership is transferred.
        * @param aContent the body of the response. NULL if not present.
        *          The ownership is transferred.
		*/
		virtual void ClientNotFoundL(TUint32 aRequestId,
								     TUint aStatusCode, 
								     RStringF aReasonPhrase, 
								     RPointerArray<CSIPHeaderBase> aHeaders,
								     HBufC8* aContent) = 0;

		/**
		* Error occurred during resolution of the client's channel UID.
        * This callback will also be called if one of the callbacks 
        * ChannelResolvedL or ClientNotFoundL leaves using the leave code.
		*
		* @param aError error code.
		* @param aRequestId a request ID 
		*/
		virtual void ErrorOccurred(TInt aError,
								   TUint32 aRequestId) =0;		
	};

#endif // MSIPCLIENTDISCOVERYOBSERVER_H
