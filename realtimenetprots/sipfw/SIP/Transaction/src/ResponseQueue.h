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
* Name          : ResponseQueue.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef RESPONSEQUEUE_H
#define RESPONSEQUEUE_H

// FORWARD DECLARATIONS
class CResponseQueueItem;


/**
 * This class represents a queue for storing SIP responses in the order they
 * are sent. Server transactions need to queue the responses because
 * transaction can't send another response until the previous asynchronous send
 * has been completed.
 */
class CResponseQueue : public CBase
	{
public: // Constructor and destructor

	static CResponseQueue* NewL();

	~CResponseQueue();

public: // New functions

	/**	
	 * Adds a SIP response into the queue.
	 *
	 * @pre aResp != NULL
	 *
	 * @param aRespItem Response item which is stored to the queue, ownership is
	 *	transferred if Add returns KErrNone
	 * @return KErrNone If successful and the ownership of aRespItem is
	 *	transferred. Otherwise a system wide error code and the ownership of
	 *  aRespItem is not transferred.
	 */
	TInt Add(CResponseQueueItem* aRespItem);

	/**
	 * Extracts the oldest response item from the queue.
	 *
	 * @return value Response item, or NULL if there were no items in the queue.
	 *	Ownership is transferred.
	 */
	CResponseQueueItem* GetNext();

private: // Constructor, for internal use

    CResponseQueue();

private: // Data

	//Response queue. The items are owned.
	RPointerArray<CResponseQueueItem> iQueue;

	// For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CTransactionUser_Test;
#endif
	};

#endif // end of RESPONSEQUEUE_H

// End of File
