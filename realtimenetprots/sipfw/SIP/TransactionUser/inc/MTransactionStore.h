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
* Name          : MTransactionStore.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef MTRANSACTIONSTORE_H
#define MTRANSACTIONSTORE_H

// INCLUDES
#include "SipStackServerDefs.h"

#include <e32base.h>

// CLASS DECLARATION

/**
 * MTransactionStore provides limited access to CTransactionStore componen of
 * the TransactionUser subsystem.
 */
class MTransactionStore
	{
public:

	virtual ~MTransactionStore() {}

	/**
	 * Updates TransactionId of the transaction identified by aOldTransactionId.
	 * @param aOldTransactionId Identifies the transaction to be modified
	 * @param aNewTransactionId New value for TransactionId
	 * @return KErrNone if successful
     *         KErrNotFound if not found
	 */
	virtual TInt UpdateTransactionId(TTransactionId aOldTransactionId,
									 TTransactionId aNewTransactionId) = 0;

	/**
	 * Removes the specified transaction from the store, but does not delete the
	 * UA and transaction instances.
	 * @param aTransactionId Identifies the transaction to be removed.
	 */
	virtual void Remove(TTransactionId aTransactionId) = 0;
	};

#endif // end of MTRANSACTIONSTORE_H

// End of File
