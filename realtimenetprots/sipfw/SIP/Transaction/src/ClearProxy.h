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
* Name          : ClearProxy.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/



#ifndef CLEARPROXY_H
#define CLEARPROXY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CClientTransaction;

// CLASS DECLARATION
/**
 * This class is used for removing an outbound proxy URI from a client
 * transaction.
 */
class TClearProxy
	{
public: // Constructor
	/**
	 * Constructor
	 *
	 * @param aTransaction Client transaction
	 */
	TClearProxy(CClientTransaction& aTransaction);

public: // New functions

    /**
	 * Returns a TCleanupItem which can be pushed into CleanupStack.
	 *
     * @return TCleanupItem
	 */
	TCleanupItem CleanupItem();

    /**
	 * When CleanupStack destroys the TCleanupItem, this function is called.
     * Clears the outbound proxy from client transaction.
	 *
	 * @pre aItem != NULL
	 *
	 * @param aItem Pointer to a TClearProxy containing the information
	 *	required to clear outbound proxy.
	 */
	static void ClearOutboundProxy(TAny* aItem);

private: // Data

	CClientTransaction& iTransaction;    
	};

#endif // end of CLEARPROXY_H

// End of File
