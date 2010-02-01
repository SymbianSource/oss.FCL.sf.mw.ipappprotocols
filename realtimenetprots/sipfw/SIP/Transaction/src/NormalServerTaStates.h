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
* Name          : NormalServerTaStates.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef NORMALSERVERTASTATES_H
#define NORMALSERVERTASTATES_H

// INCLUDES
#include "TransactionState.h"

// CLASS DECLARATION

//Waiting for a SIP response from UAS
class CNormalServerTa_Trying : public CTransactionState
	{
public: // Constructors and destructor

	CNormalServerTa_Trying();
	~CNormalServerTa_Trying();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;	

    void ReceiveL(CTransaction& aTransaction, CSIPRequest* aRequest) const;

	void SendResponseL(CTransaction& aTransaction,
					   CSIPResponse* aResp,
					   RStringF aProtocol,
					   const TSIPTransportParams& aParams) const;

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
	friend class CNormalSTa_Trying_Test;
	friend class CNormalUAS_GetTxOwner_Test;
	friend class CNormalUAS_Start_Test;
	friend class CNormalUAS_WaitRespFromApp_Test;
	friend class CNormalUAS_FinalRespSent_Test;
	friend class CUserAgentServer_Test;
#endif
	};



//Waiting for a final response from UAS
class CNormalServerTa_Proceeding : public CTransactionState
	{
public: // Constructors and destructor

	CNormalServerTa_Proceeding();
	~CNormalServerTa_Proceeding();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

	void SendCompleteL(CTransaction& aTransaction) const;
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

    void ReceiveL(CTransaction& aTransaction, CSIPRequest* aRequest) const;

	void SendResponseL(CTransaction& aTransaction,
					   CSIPResponse* aResp,
					   RStringF aProtocol,
					   const TSIPTransportParams& aParams) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CTransactionState& aCompleted);

private: // Data

	//Next possible state, not owned.
	CTransactionState* iCompleted;

#ifdef CPPUNIT_TEST
	friend class CNormalSTa_Proceeding_Test;
#endif
	};



//Waiting for timer J to expire. If a retransmission of the original request is
//received, the most recent response is retransmitted.
class CNormalServerTa_Completed : public CTransactionState
	{
public: // Constructors and destructor

	CNormalServerTa_Completed();
	~CNormalServerTa_Completed();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

	void SendCompleteL(CTransaction& aTransaction) const;
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

    void ReceiveL(CTransaction& aTransaction, CSIPRequest* aRequest) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;
	};

#endif // end of NORMALSERVERTASTATES_H

// End of File
