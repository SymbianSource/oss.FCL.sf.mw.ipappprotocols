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
* Name          : StopUserAgent.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef STOPUSERAGENT_H
#define STOPUSERAGENT_H

// INCLUDES
#include <e32base.h>

#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class CTransactionStore;


// CLASS DECLARATION
/**
 * This class is used for stopping a UA, in case a leave occurs.
 */
class TStopUserAgent
	{
public:

	/**
	 * Constructor
	 * @param aTransactionId TransactionId of the UA
	 * @param aStore TransactionStore
	 * @param aReason Reason for stopping
	 */
	TStopUserAgent(TTransactionId aTransactionId,
				   CTransactionStore& aStore,
				   TInt aReason);

    /**
	 * Returns a TCleanupItem which can be pushed into CleanupStack. If leave
     * occurs, cleanup operation is invoked.
     * @return TCleanupItem
	 */
	TCleanupItem CleanupItem();

    /**
	 * When CleanupStack destroys the TCleanupItem, this function is called.
     * If the UA exists, it is stopped.
	 *
	 * @pre aStopUserAgent != NULL
	 * @param aStopUserAgent Pointer to a TStopUserAgent containing the
     *  information required by the cleanup operation. Ownership is not
     *  transferred.
	 */
	static void StopIfFound(TAny* aStopUserAgent);

private: // Data

    // TransactionId used for searching the UA
    TTransactionId iTransactionId;

    // UA is searched from this store
    CTransactionStore& iStore;
 
	// Reason for the stop
    TInt iReason;
    };

#endif // end of STOPUSERAGENT_H

// End of File
