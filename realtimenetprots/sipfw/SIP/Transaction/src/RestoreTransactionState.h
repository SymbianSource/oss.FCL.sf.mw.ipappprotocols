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
* Name          : RestoreTransactionState.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef RESTORETRANSACTIONSTATE_H
#define RESTORETRANSACTIONSTATE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CClientTransaction;
class CTransactionState;
class CTransactionTimer;


/**
 * This class is used for changing the state of a client transaction back to
 * its previous state, if passing a SIP response to the UAC leaves.
 */
class TRestoreTransactionState
	{
public: // Constructor

	/**
	 * Constructor
	 *
	 * @param aTransaction Client transaction, whose state will be restored
	 * @param aState State which the transaction will enter
	 * @param aTimer Timer to stop when state is restored. If NULL, then timer
	 *		  is not stopped. Ownership is not transferred.
	 * @param aTimer2 Timer to stop when state is restored. If NULL, then timer
	 *		  is not stopped. Ownership is not transferred.
	 */
	TRestoreTransactionState(CClientTransaction& aTransaction,
                             const CTransactionState& aState,
                       		 CTransactionTimer* aTimer = NULL,
                       		 CTransactionTimer* aTimer2 = NULL);

public: // New functions

    /**
	 * Returns a TCleanupItem which can be pushed into CleanupStack.
	 *
     * @return TCleanupItem
	 */
	TCleanupItem CleanupItem();

    /**
	 * When CleanupStack destroys the TCleanupItem, this function is called.
     * The state of iTransaction is restored to iState.
	 *
	 * @pre aRestoreState != NULL
	 *
	 * @param aRestoreState Pointer to a TRestoreTransactionState containing
     *  the information required to restore transaction's state. Ownership is
     *  not transferred.
	 */
	static void Restore(TAny* aRestoreState);

private: // Data

    //Transaction
	CClientTransaction& iTransaction;

    //State, which the transaction will enter if TCleanupItem is destroyed
    const CTransactionState& iState;

    //Timer that is stopped when restoring is done. Not owned.
    CTransactionTimer* iTimer;
    
    //Timer that is stopped when restoring is done. Not owned.
    CTransactionTimer* iTimer2;
	};

#endif // end of RESTORETRANSACTIONSTATE_H

// End of File
