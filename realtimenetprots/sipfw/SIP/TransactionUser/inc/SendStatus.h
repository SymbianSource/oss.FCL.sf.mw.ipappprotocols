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
* Name          : SendStatus.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef SENDSTATUS_H
#define SENDSTATUS_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION
class MSendStatus
	{
public:
	virtual ~MSendStatus() {}	

	/**
	 * Send operation was successful, though an ICMP error can still occur
	 * after this.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return value -
	 */
	virtual void SendCompleteL() = 0;

	/**
	 * Sending failed.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aError Error code received from ConnectionMgr
	 * @return value -
	 */
	virtual void SendFailedL(TInt aError) = 0;

	/**
	 * Leave occurred during the execution of CTransmitter::RunL() and the
	 * error is passed to the observed associated with the CTransmitter object.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aReason Leave code
	 * @return value -
	 */
	virtual void LeaveFromTransmitter(TInt aReason) = 0;
	};

#endif // end of SENDSTATUS_H

// End of File
