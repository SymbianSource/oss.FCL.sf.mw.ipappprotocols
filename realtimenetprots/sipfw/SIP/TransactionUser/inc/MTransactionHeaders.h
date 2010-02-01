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
* Name          : MTransactionHeaders.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef MTRANSACTIONHEADERS_H
#define MTRANSACTIONHEADERS_H

#include <e32base.h>

class CURIContainer;
class CSIPToHeader;
class CSIPFromHeader;
class CSIPCallIDHeader;
class CSIPRouteHeaderBase;
class CSIPContactHeader;


/**
 * This class is used to wrap together the SIP headers which can be requested
 * by upper subsystems.
 * If MTransactionHeaders is pushed into CleanupStack, TCleanupItem has to be
 * used.
 */
class MTransactionHeaders
	{
public:
	virtual ~MTransactionHeaders() {}


    /**
	 * Gets the Request-URI
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value Request-URI, or NULL if it doesn't exist.
	 *	The ownership is transferred
	 */
    virtual CURIContainer* RequestURI() = 0;


	/**
	 * Gets the To-header
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value To-header, or NULL if To-header doesn't exist.
	 *	The ownership is transferred
	 */
	virtual CSIPToHeader* ToHeader() = 0;


	/**
	 * Gets the From-header
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value From-header, or NULL if From-header doesn't exist.
	 *	The ownership is transferred
	 */
	virtual CSIPFromHeader* FromHeader() = 0;


	/**
	 * Gets the Call-ID header
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value Call-ID header, or NULL if Call-ID does not exist.
	 *	The ownership is transferred
	 */
	virtual CSIPCallIDHeader* CallIDHeader() = 0;


	/**
	 * Gets the CSeq number part.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value CSeq number
	 */
	virtual TUint CSeqNumber() const = 0;


	/**
	 * Gets the Record-route headers
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value Record-route headers, or NULL if Record-route headers
	 *	do not exist. The ownership is transferred
	 */
	virtual RPointerArray<CSIPRouteHeaderBase>* RecordRoute() = 0;


	/**
	 * Gets the Contact headers
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @return value Contact headers, or NULL if Contact headers do not exist.
	 *	The ownership is transferred
	 */
	virtual RPointerArray<CSIPContactHeader>* Contact() = 0;
	};

#endif // end of MTRANSACTIONHEADERS_H

// End of File
