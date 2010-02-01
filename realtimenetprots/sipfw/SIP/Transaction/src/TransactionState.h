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
* Name          : TransactionState.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef TRANSACTIONSTATE_H
#define TRANSACTIONSTATE_H

// INCLUDES
#include <in_sock.h>

#include "SipLogs.h"    //for USE_SIP_LOGS
#include "Lwtimer.h"
#include "sipinternalstates.h"

// FORWARD DECLARATIONS
class CUri8;
class RStringF;
class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;

class CTransaction;

// CLASS DECLARATION
/*
 * Base class for all states of both client and server transactions.
 * Since client and server transactions receive mostly the same events,
 * their states are derived from this common class.
 */
class CTransactionState : public CBase
	{
public:

	virtual ~CTransactionState();

	virtual void SendRequestL(CTransaction& aTransaction,
							  CSIPRequest& aReq,
							  const TInetAddr& aAddress,
							  RStringF aProtocol,
							  TBool aForceUDP,
							  const TSIPTransportParams& aParams,
							  CUri8* aOutboundProxy) const;
	virtual void SendResponseL(CTransaction& aTransaction,
							   CSIPResponse* aResp,
							   RStringF aProtocol,
							   const TSIPTransportParams& aParams) const;

	virtual void SendCompleteL(CTransaction& aTransaction) const;
	virtual void SendFailedL(CTransaction& aTransaction, TInt aError) const;

    virtual void ReceiveL(CTransaction& aTransaction,
                          CSIPRequest* aRequest) const;	
	virtual void ReceiveL(CTransaction& aTransaction,
                          CSIPResponse* aResponse) const;

	virtual void TimerExpiredL(CTransaction& aTransaction,
							   TTimerId aTimerId,
							   TAny* aTimerParam) const;

	/**
	 * Tells whether the transaction can continue without the outgoing SIP
	 * message in the send buffer.
	 *	 
	 * @return value ETrue if the transaction can continue, EFalse otherwise
	 */
	virtual TBool CanContinueWithoutOutgoingMsg() const;

    /**
	 * Obtains transaction's current state.
	 *	 
	 * @param aState OUT: Transaction state
	 */
    virtual void GetState(CSIPInternalStates::TState& aState) const = 0;

#if defined(USE_SIP_LOGS)
	virtual const TDesC8& Log() const;

    static const TDesC8& EnteringThisStateIsntLogged();
#endif

protected:
	CTransactionState();

    /**
	 * Process timer expiration.
	 *
	 * @pre aTimerParam != NULL
	 *
     * @param aTransaction The transaction who owns the expired timer.
     * @param aTimerId Timer identifier
     * @param aTimerParam User specified value that was specified when the
     *  timer was started. Ownership isn't transferred.
	 */
	void HandleTimerExpirationL(CTransaction& aTransaction,
								TTimerId aTimerId,
								TAny* aTimerParam) const;
	};

#endif // end of TRANSACTIONSTATE_H

// End of File
