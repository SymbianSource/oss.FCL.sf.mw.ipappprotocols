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
* Name          : MSipClient.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef MSIPCLIENT_H
#define MSIPCLIENT_H

#include <e32std.h>

class CSIPHeaderBase;
class CSIPFeatureSet;
class CSdpDocument;


class MSipClient
    {
public:

	/**
	 * Gets the UID of the client.
	 *
     * @return a client UID
	 */
	virtual const TUid& ClientUid() const = 0;
	
	/**
	 * Tells if the client is ROM based.
	 *
     * @return ETrue application is ROM based, EFalse otherwise
	 */
	virtual TBool RomBased() const = 0;	
	
	/**
	 * Tells if the application can be started by the SIP stack.
	 *
     * @return ETrue application can be started, EFalse otherwise
	 */
	virtual TBool AllowStarting() const = 0;

	/**
	 * Gets the SIP Headers describing the capabilities of the client.
     *
	 * @return an array of SIP Headers. 
	 *         The ownership of the headers is NOT transferred.    
	 */
    virtual RPointerArray<CSIPHeaderBase>& SIPHeaders() = 0;

	/**
	 * Gets the SIP feature sets supported by the client.
     *
	 * @return the SIP feature set
	 */
    virtual CSIPFeatureSet& SIPFeatureSet() = 0;

	/**
	 * Gets the SDP capabilities of the application.
	 *
     * @return CSdpDocument instance, if present for the client, 
	 *         otherwise NULL. The ownership of object is NOT transferred.
	 */
	virtual CSdpDocument& SdpDocument() = 0;
    };

#endif // MSIPCLIENT_H

// End of File
