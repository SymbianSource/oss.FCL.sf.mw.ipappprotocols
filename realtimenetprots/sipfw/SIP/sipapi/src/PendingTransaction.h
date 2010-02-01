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
* Name          : pendingtransaction.h
* Part of       : SIP API
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef PENDINGTRANSACTION_H
#define PENDINGTRANSACTION_H

// INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPServerTransaction;

// CLASS DECLARATION

/**
 * This class stores ServerTransaction and an IAP-id
 */
class CPendingTransaction : public CBase
	{
public: // Constructors and destructor
	/**	
	 * Creates a new instance of CPendingTransaction
	 *
	 * @pre aTransaction != NULL
	 * @post
	 *
	 * @see
	 *
	 * @param aTransaction ServerTransaction, ownership isn't transferred
     * @param aIapId IAP-id
	 * @return new CPendingTransaction object, ownership is transferred
	 */
	static CPendingTransaction* NewLC(CSIPServerTransaction* aTransaction,
                                      TUint32 aIapId);

	~CPendingTransaction();

public: // New functions
	/**
	 * Return a pointer to the ServerTransaction.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return Pointer to CSIPServerTransaction, ownership isn't transferred
     *  as CPendingTransaction doesn't own the CSIPServerTransaction in the
     *  first place.
	 */
	CSIPServerTransaction* Transaction();

	/**	
	 * Return the IAP-id
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return IAP-id value
	 */
	TUint32 IapId() const;

private:
	/**	
	 * Constructor
	 *
	 * @pre aTransaction != NULL
	 * @post
	 *
	 * @see
	 *
	 * @param aTransaction ServerTransaction, ownership isn't transferred
     * @param aIapId IAP-id	 
	 * @return -
	 */
	CPendingTransaction(CSIPServerTransaction* aTransaction,
                        TUint32 aIapId);

private: // Data
    //ServerTransaction, not owned by CPendingTransaction
	CSIPServerTransaction* iTransaction;

	TUint32 iIapId;

private: // For testing purposes
	UNIT_TEST(CPendingTransaction_Test)
	};

#endif // end of PENDINGTRANSACTION_H

// End of File
