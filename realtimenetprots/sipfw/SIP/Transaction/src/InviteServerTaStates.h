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
* Name          : InviteServerTaStates.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef INVITESERVERTASTATES_H
#define INVITESERVERTASTATES_H

// INCLUDES
#include "SipLogs.h"    //for USE_SIP_LOGS

#include "TransactionState.h"

// CLASS DECLARATION

//Waiting for final response from UserAgent
class CInviteServerTa_Proceeding : public CTransactionState
	{
public: // Constructors and destructor

	CInviteServerTa_Proceeding();
	~CInviteServerTa_Proceeding();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	const TDesC8& Log() const;
#endif	

    void ReceiveL(CTransaction& aTransaction, CSIPRequest* aRequest) const;

	void SendResponseL(CTransaction& aTransaction,
					   CSIPResponse* aResp,
					   RStringF aProtocol,
					   const TSIPTransportParams& aParams) const;

	void SendCompleteL(CTransaction& aTransaction) const;
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

public: // New functions

	void SetNeighbourStates(CTransactionState& aCompleted,
							CTransactionState& aSending2xx);

private: // Data

	//Next possible states, not owned.
	CTransactionState* iCompleted;
	CTransactionState* iSending2xx;
	};



//Waiting for ACK from remote endpoint.
class CInviteServerTa_Completed : public CTransactionState
	{
public: // Constructors and destructor

	CInviteServerTa_Completed();
	~CInviteServerTa_Completed();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	const TDesC8& Log() const;
#endif	

	void SendCompleteL(CTransaction& aTransaction) const;
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

    void ReceiveL(CTransaction& aTransaction, CSIPRequest* aRequest) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CTransactionState& aConfirmed);

private: // Data

	//Next possible state, not owned.
	CTransactionState* iConfirmed;
	};



//Absorb any restransmissions of ACK
class CInviteServerTa_Confirmed : public CTransactionState
	{
public: // Constructors and destructor

	CInviteServerTa_Confirmed();
	~CInviteServerTa_Confirmed();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	const TDesC8& Log() const;
#endif

	TBool CanContinueWithoutOutgoingMsg() const;

	//These can happen if transaction entered Confirmed state before a
	//retransmission was completed.
	void SendCompleteL(CTransaction& aTransaction) const;
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;

    void ReceiveL(CTransaction& aTransaction, CSIPRequest* aRequest) const;

	void TimerExpiredL(CTransaction& aTransaction,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;	
	};



class CInviteServerTa_Sending2xx : public CTransactionState
	{
public: // Constructors and destructor

	CInviteServerTa_Sending2xx();
	~CInviteServerTa_Sending2xx();

public: // From CTransactionState

    void GetState(CSIPInternalStates::TState& aState) const;

#if defined(USE_SIP_LOGS)
	//Transition to Sending2xx state isn't written to log, but
	//CInviteServerTa_Sending2xx::Log() method is required to be able to write
	//the transition from Sending2xx to Terminated state into log
	const TDesC8& Log() const;
#endif

	TBool CanContinueWithoutOutgoingMsg() const;

	void SendCompleteL(CTransaction& aTransaction) const;
	void SendFailedL(CTransaction& aTransaction, TInt aError) const;
	
	//Since UAS clears CTransaction from CTransactionStore when it sends 2xx,
	//further ReceiveL events are directed to UAS	
	};

#endif // end of INVITESERVERTASTATES_H

// End of File
