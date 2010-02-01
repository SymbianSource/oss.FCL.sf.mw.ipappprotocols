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
* Name          : NormalClientTaStates.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef NORMALCLIENTTASTATES_H
#define NORMALCLIENTTASTATES_H

// INCLUDES
#include "TransactionState.h"


// CLASS DECLARATION

//Waiting for a response from the remote endpoint
class CNormalClientTa_Trying: public CTransactionState
	{
public: // Constructors and destructor

	CNormalClientTa_Trying();

	~CNormalClientTa_Trying();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;	

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

	//Next possible states, not owned
	CTransactionState* iProceeding;
	CTransactionState* iCompleted;

#ifdef CPPUNIT_TEST
	friend class CNormalCTa_Trying_Test;
#endif
	};



//Waiting for a final response from the remote endpoint
class CNormalClientTa_Proceeding : public CTransactionState
	{
public: // Constructors and destructor

	CNormalClientTa_Proceeding();

	~CNormalClientTa_Proceeding();

public: // From CTransactionState	

    void GetState(CSIPInternalStates::TState& aState) const;	

	void SendCompleteL(CTransaction& aTransaction) const;

	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

	void ReceiveL(CTransaction& aTransaction, CSIPResponse* aResponse) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CTransactionState& aCompleted);

private: // Data

	//Next possible state, not owned
	CTransactionState* iCompleted;

#ifdef CPPUNIT_TEST
	friend class CNormalCTa_Proceeding_Test;
#endif
	};



//Waiting the timer K to expire. During that time, Transaction absorbs any
//possible retransmitted SIP responses from the remote.
class CNormalClientTa_Completed : public CTransactionState
	{
public: // Constructors and destructor

	CNormalClientTa_Completed();

	~CNormalClientTa_Completed();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

	TBool CanContinueWithoutOutgoingMsg() const;
	
	//Can happen if transaction entered Completed state before it receiving
	//acknowledgement event to a send.
	void SendCompleteL(CTransaction& aTransaction) const;

	//Can happen if transaction entered Completed state before it receiving
	//acknowledgement event to a send.
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

	void ReceiveL(CTransaction& aTransaction, CSIPResponse* aResponse) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;
	};

#endif // end of NORMALCLIENTTASTATES_H

// End of File
