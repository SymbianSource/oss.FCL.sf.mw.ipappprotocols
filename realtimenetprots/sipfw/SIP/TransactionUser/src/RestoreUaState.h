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
* Name          : RestoreUaState.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef RESTOREUASTATE_H
#define RESTOREUASTATE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUserAgent;
class CUserAgentTimer;


/**
 * This class is used for changing the state of an UAC back to its previous
 * state, if passing a SIP response to the MTransactionOwner leaves.
 */
class TRestoreUaState
	{
public:
	/**
	 * Constructor
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aFinalRespPassed Flag whose value will be restored
	 * @param aUserAgent If non-NULL, points to UserAgent whose timer is stopped
	 * @param aTimer If non-NULL, points to timer that will be stopped
	 * @return -
	 */
	TRestoreUaState(TBool& aFinalRespPassed,
					CUserAgent* aUserAgent=NULL,
					CUserAgentTimer* aTimer=NULL);


    /**
	 * Returns a TCleanupItem which can be pushed into CleanupStack.
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
     * @return TCleanupItem
	 */
	TCleanupItem CleanupItem();


    /**
	 * When CleanupStack destroys the TCleanupItem, this function is called.     
	 *
	 * @pre aRestoreState != NULL
	 * @post
	 *
	 * @see
	 *
	 * @param aRestoreState Pointer to a TRestoreUaState containing information
	 *	required to restore UAC's state.
	 */
	static void Restore(TAny* aRestoreState);

private:
	//Reference to UAC's iFinalRespPassed flag	
	TBool& iFinalRespPassed;
	
	//Value that will be set to iFinalRespPassed
	TBool iOriginalValue;
	
	//UA whose timer is stopped, not owned
	CUserAgent* iUserAgent;

	//Timer to be stopped, not owned
	CUserAgentTimer* iTimer;
	};

#endif // end of RESTOREUASTATE_H

// End of File
