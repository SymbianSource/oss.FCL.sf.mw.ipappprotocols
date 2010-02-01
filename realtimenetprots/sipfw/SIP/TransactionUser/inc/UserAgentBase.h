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
* Name          : UserAgentBase.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef USERAGENTBASE_H
#define USERAGENTBASE_H

#include "SipStackServerDefs.h"

#include "TransactionReceiver.h"



/**
 * This class provides access to UserAgent objects, as needed by Transaction
 * subsystem.
 */
class CUserAgentBase :
    public CBase,
	public MTransactionReceiver	
	{
public: // Destructor

	virtual ~CUserAgentBase() {}
	
public:	// From MTransactionReceiver

	virtual void ReceiveL(CSIPRequest* aRequest) = 0;

	virtual void ReceiveL(CSIPResponse* aResponse) = 0;

	virtual void LeaveOccurred(TInt aReason) = 0;

	virtual const TSIPTransportParams& TransportParams() const = 0;

	virtual void IcmpErrorL(const TInetAddr& aAddress,
							CSipConnectionMgr::TICMPError aError) = 0;

public: // New functions

	/**
	 * The CTransaction owned by UA, has reached terminated state.
	 *
	 * @param aReason Reason why the transaction ends.
	 */
	virtual void TransactionEndsL(TInt aReason) = 0;

	/**
	 * The UA state machine is stopped and its information is removed from
	 * CTransactionStore. No more events will be passed to the state machine.
	 * CUserAgent and CTransaction will be deleted soon.
	 *
	 * @param aReason Reason why the UserAgent ends.
	 */
	virtual void Stop(TInt aReason) = 0;

	/**
	 * Obtains the TransactionId associated with the UA.
	 *
	 * @return value TransactionId
	 */
	virtual TTransactionId TransactionId() const = 0;
	};

#endif // end of USERAGENTBASE_H

// End of File
