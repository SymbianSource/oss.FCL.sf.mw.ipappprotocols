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
* Name          : InviteClientTaStates.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef INVITECLIENTTASTATES_H
#define INVITECLIENTTASTATES_H

// INCLUDES
#include "SipLogs.h"    //for USE_SIP_LOGS

#include "TransactionState.h"

// CLASS DECLARATION

//Waiting for response from remote endpoint
class CInviteClientTa_Calling : public CTransactionState
	{
public: // Constructors and destructor

	CInviteClientTa_Calling();

	~CInviteClientTa_Calling();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	const TDesC8& Log() const;
#endif	

	void SendRequestL(CTransaction& aTransaction,
					  CSIPRequest& aReq,
					  const TInetAddr& aAddress,					  
					  RStringF aProtocol,
					  TBool aForceUDP,
					  const TSIPTransportParams& aParams,
					  CUri8* aOutboundProxy) const;

	void SendCompleteL(CTransaction& aTransaction) const;

	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

	void ReceiveL(CTransaction& aTransaction, CSIPResponse* aResponse) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CTransactionState& aProceeding,
							CTransactionState& aCompleted);

private: // Data	

	//Next possible states, not owned.
	CTransactionState* iProceeding;
	CTransactionState* iCompleted;

#ifdef CPPUNIT_TEST
	friend class CInviteCTa_Calling_Test;
#endif
	};



//Waiting for the final response from remote endpoint
class CInviteClientTa_Proceeding : public CTransactionState
	{
public: // Constructors and destructor

	CInviteClientTa_Proceeding();
	~CInviteClientTa_Proceeding();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	const TDesC8& Log() const;
#endif	

	//Can happen if transaction entered Proceeding state before receiving a
	//acknowledgement event for a request retransmission.
	void SendCompleteL(CTransaction& aTransaction) const;

	//Can happen if transaction entered Proceeding state before receiving a
	//acknowledgement event for a request retransmission.
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

	void ReceiveL(CTransaction& aTransaction, CSIPResponse* aResponse) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CTransactionState& aCompleted);

private: // Data

	//Next possible state, not owned.
	CTransactionState* iCompleted;
	};



//Waiting timer D to expire. During that time, transaction retransmits ACK when
//it receives a retransmission of a final response.
class CInviteClientTa_Completed : public CTransactionState
	{
public: // Constructors and destructor

	CInviteClientTa_Completed();
	
	~CInviteClientTa_Completed();	

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	const TDesC8& Log() const;
#endif

	void SendCompleteL(CTransaction& aTransaction) const;
	
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

	void ReceiveL(CTransaction& aTransaction, CSIPResponse* aResponse) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;
	};

#endif // end of INVITECLIENTTASTATES_H

// End of File
