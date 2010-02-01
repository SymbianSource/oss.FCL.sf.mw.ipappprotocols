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
* Name          : CTransaction.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CTRANSACTION_H
#define CTRANSACTION_H

// INCLUDES
#include <stringpool.h>
#include "SipStackServerDefs.h"
#include "SipLogs.h"    //for USE_SIP_LOGS
#include "Lwtimer.h"
#include "SendStatus.h"
#include "TSIPTransportParams.h"
#include "TimerValues.h"

#include "TransactionBase.h"
#include "sip.h"

// FORWARD DECLARATIONS
class CSIPMessage;
class CTransmitter;
class CUserAgentBase;
class CTransactionState;
class TTimerValues;

class CTransactionTimer;

// CLASS DECLARATION
/*
 * The base class for all transactions.
 */
class CTransaction :
	public CTransactionBase,
	public MExpirationHandler,
	public MSendStatus
	{
public: // Destructor

	~CTransaction();

public:	// From CTransactionBase

    void ReceiveL(CSIPRequest* aRequest);

	void ReceiveL(CSIPResponse* aResponse);

	void LeaveOccurred(TInt aReason);

	const TSIPTransportParams& TransportParams() const;

	virtual void IcmpErrorL(const TInetAddr& aAddress,
							CSipConnectionMgr::TICMPError aError) = 0;

	void SendRequestL(CSIPRequest& aReq,
					  const TInetAddr& aAddress,					  
					  RStringF aProtocol,
					  TBool aForceUDP,
					  const TSIPTransportParams& aParams,
					  CUri8* aOutboundProxy);

	void SendResponseL(CSIPResponse* aResp,					   
					   RStringF aProtocol,
					   const TSIPTransportParams& aParams);

	TBool ClearSendBuffer();

    void GetState(CSIPInternalStates::TState& aState);

	void Terminated();

	virtual void TerminatedL(TInt aReason);

	TBool HasTerminated() const;

	void DetachFromUserAgent();

	RStringF TransportProtocol() const;

public: // From MExpirationHandler

	void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam);
		
public:	// From MSendStatus

	void SendCompleteL();

	void SendFailedL(TInt aError);

	void LeaveFromTransmitter(TInt aReason);

public: // New pure virtual functions

	/**
	 * Delete the Transaction specific timer. Each separate transaction type
	 * has different timers. If this transaction has a matching timer, the
	 * timer will be deleted.
	 *
	 * @param aTimer Timer to be deleted
	 */
	virtual void DeleteTimer(const CTransactionTimer& aTimer) = 0;

	/**
	 * Sends a SIP message to CTransmitter.
	 *
	 * @pre iOutgoingMsg != NULL, iTransmitter isn't active
	 */
	virtual void SendToTransmitterL() = 0;	

protected: // New pure virtual functions

	/**
	 * Stops transactions's all timers.
	 */
	virtual void CancelAllTimers() = 0;

public: // New functions

	/**
	 * Tells if the transaction is an INVITE (client/server) transaction or a
	 * non-INVITE (client/server) transaction.
	 *
	 * @return ETrue if INVITE transaction, EFalse otherwise.
	 */
	virtual TBool IsInviteTransaction() const;

	/**
	 * Transaction enters a new state
	 *
	 * @param aNewState State which CTransaction enters	 
	 */
	void ChangeState(const CTransactionState& aNewState);	

	/**
	 * Sets the iTransportProtocol member according to the transport protocol
	 * in the top Via header of the aMsg.
	 *
	 * @param aMsg SIP message
	 * @return value ETrue: if successful, EFalse: otherwise
	 */
	TBool UpdateTransportProtocol(CSIPMessage& aMsg);

	/**	
	 * Checks if transaction is using unreliable transport.
	 *
	 * @return ETrue if an unreliable transport is used, EFalse otherwise
	 */	
	TBool IsUnreliableTransportUsed() const;

    /**	
	 * Checks if the CTransmitter is currently sending.
	 *	 
	 * @return ETrue: CTransmitter is sending, EFalse: otherwise
	 */	
	TBool IsTransmitterSending() const;

	/**	
	 * Returns a pointer to the current state.
	 *
	 * @return Transaction's current state, ownership isn't transferred.
	 */	
	const CTransactionState* State() const;

protected:

	/**
	 * Constructor
	 *
	 * @pre aTransmitter != NULL, aInitialState != NULL
	 *
	 * @param aUserAgent UserAgent owning the new CTransaction being created.
	 * @param aTransmitter CTransmitter used for sending messages.
	 * @param aTimers LwTimer subsystem
	 * @param aInitialState Initial state of the CTransaction object
	 * @param aTimerValues Values for calculating timer durations
	 */
	CTransaction(CUserAgentBase& aUserAgent,
				 CTransmitter& aTransmitter,
				 MTimerManager& aTimers,
				 CTransactionState& aInitialState,
				 TTimerValues& aTimerValues);

	/**
	 * Obtains the id of the transaction.
	 *
	 * @return TransactionId of the transaction
	 */
	virtual TTransactionId TransactionId() const;

protected: // Data:

	//UserAgent associated with this transaction. Can be NULL. Not owned.
	CUserAgentBase* iUserAgent;

	//CTransmitter used for sending messages, owned by UserAgent.
	//In a special case the CInviteClientTransaction can own the CTransmitter.
	CTransmitter* iTransmitter;

	//Timer subsystem
	MTimerManager& iTimers;

	//Outgoing SIP message.
	//
	//In case of ClientTransaction, UAC owns the message, except when
	//InviteClientTransaction has created the ACK (instead of UAC creating the
	//ACK).
	//
	//In case of ServerTransaction, CTransaction owns the message. Except if
	//InviteServerTransaction is sending a 2xx response, when the UAS owns it.	
	CSIPMessage* iOutgoingMsg;

	RStringF iTransportProtocol;

	//Transport parameters to be used when sending the SIP message
	TSIPTransportParams iTransportParams;

	//Copy of the values in CTimerValueRetriever::TTimerValues, at the time the
	//transaction was created. Later changes to
	//CTimerValueRetriever::TTimerValues won't reflect here.
	TTimerValues iTimerValues;

	//ETrue when CTransaction has reached terminated state.
	//There is no separate state object for the terminated state.
	TBool iTerminated;

private:

#if defined(USE_SIP_LOGS)
	/**
	 * Write the transaction state to a log file.
	 *
	 * @param aState Transaction state
	 */	
	void WriteStateToLog(const CTransactionState& aState) const;
#endif

private: // Data

	//Current state of the transaction FSM, not owned.
	const CTransactionState* iState;

	void __DbgTestInvariant() const;



#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
	friend class ClientTransaction_Test;
	friend class CServerTransaction_Test;
	friend class CInviteServerTa_Test;
	friend class CNormalCTa_Trying_Test;
	friend class CNormalCTa_Proceeding_Test;
	friend class CNormalCTa_Completed_Test;
	friend class CNormalSTa_Trying_Test;
	friend class CNormalSTa_Proceeding_Test;
	friend class CNormalSTa_Completed_Test;
	friend class CNormalUAS_GetTxOwner_Test;
	friend class CNormalUAS_Start_Test;
	friend class CUserAgentClient_Test;
	friend class CUserAgentServer_Test;
	friend class CInviteCTa_Calling_Test;
#endif
	};

#endif // end of CTRANSACTION_H

// End of File
