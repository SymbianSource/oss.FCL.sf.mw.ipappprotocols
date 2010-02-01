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
* Name          : MTransactionFinder.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef MTRANSACTIONFINDER_H
#define MTRANSACTIONFINDER_H

// INCLUDES
#include "CSipConnectionMgr.h"

// FORWARD DECLARATIONS
class CSIPMessage;
class MReceiverObserver;

// CLASS DECLARATION
class MTransactionFinder
{
public:
	virtual ~MTransactionFinder() {}

    /**
	 * Searches for the transaction which should handle the SIP message.
	 *
	 * @pre
	 * @post	
	 * 
	 * @param aMsg SIP message
	 *
	 * @return value Pointer to the MReceiverObserver interface of the found
	 *	transaction, NULL if not found. Ownership is not transferred.
	 */	
	virtual MReceiverObserver* Search(CSIPMessage& aMsg) = 0;

	/**
	 * An ICMP occurred on the specified remote address.
	 *
	 * @pre
	 * @post
	 *
	 * @param aAddress Remote address on which the error occurred
	 * @param aError Identifies the ICMP error
	 */
	virtual void IcmpErrorL(const TInetAddr& aAddress,
							CSipConnectionMgr::TICMPError aError) = 0;
};

#endif // end of MTRANSACTIONFINDER_H

// End of File
