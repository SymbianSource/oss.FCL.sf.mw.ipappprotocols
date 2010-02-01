/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TCleanResponseHandler.h
* Part of       : SIPSec
* Version       : SIP/4.0
*
*/



#ifndef CLEANRESPONSEHANDLER_H
#define CLEANRESPONSEHANDLER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPSec;
class CSIPSecResponseHandler;


/**
 * This class is used for changing the state of an UAC back to its previous
 * state, if passing a SIP response to the MTransactionOwner leaves.
 */
class TCleanResponseHandler
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
	 * @param aSIPSec, points to SIPSec whose Remove function will be called
	 * @param aResponseHandler , points to CSIPSecResponseHandler
	 * that will be removed
	 * @return -
	 */
	TCleanResponseHandler( CSIPSec* aSIPSec,
					       CSIPSecResponseHandler* aResponseHandler );

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
	 * @pre aCleanResponseHandler != NULL
	 * @post
	 *
	 * @see
	 *
	 * @param aCleanResponseHandler Pointer to a TCleanResponseHandler 
	 * containing information required to remove CSIPSecResponseHandler instance.
	 *	
	 */
	static void RemoveHandler(TAny* aCleanResponseHandler);

private:
	//Not owned	
	CSIPSec* iSIPSec;
	
	//Not owned
	CSIPSecResponseHandler* iResponseHandler;
	};

#endif // end of CLEANRESPONSEHANDLER_H

// End of File
