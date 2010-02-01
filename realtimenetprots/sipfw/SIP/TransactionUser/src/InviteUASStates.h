/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : InviteUASStates.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/


#ifndef INVITEUASSTATES_H
#define INVITEUASSTATES_H

// INCLUDES
#include "UserAgentState.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

// Waiting for INVITE from CTransaction.
class CInviteUAS_Start : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAS_Start();

	~CInviteUAS_Start();

public: // From CUserAgentState

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;
	
	void ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const;

	// TransactionOwner shouldn't send response yet since UA hasn't passed the
	// request to it yet, but in theory it is possible if TransactionOwner
	// happens to use this UAS's TransactionId.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

	TBool CancelL(CUserAgent& aUserAgent) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aGetTxOwner,
							CUserAgentState& aWaitRespFromApp,
							CUserAgentState& aRejected);
private: // Data

	// Next possible states	
	CUserAgentState* iGetTxOwner;
	CUserAgentState* iWaitRespFromApp;
	
	// If UAS or Dialogs subsystem sends a response, it is an error response
	// (not a 2xx).
	CUserAgentState* iRejected;
	};



// Waiting to obtain the MTransactionOwner callback to upper layer.
// The transaction can't be canceled yet, because TransactionId hasn't been
// passed to MTransactionOwner yet.
class CInviteUAS_GetTxOwner : public CUserAgentState
	{
public:	// Constructors and destructor

	CInviteUAS_GetTxOwner();

	~CInviteUAS_GetTxOwner();

public: // From CUserAgentState

	void OwnerFoundL(CUserAgent& aUserAgent, MTransactionOwner* aOwner) const;

	void OwnerNotFoundL(CUserAgent& aUserAgent, CSIPResponse* aResp) const;
	
	// TransactionOwner shouldn't send response yet as UA hasn't passed the
	// request to it yet, but in theory it is possible if TransactionOwner
	// happens to use this UAS's TransactionId.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

    void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

    void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

    void RequestRouterErrorL(CUserAgent& aUserAgent) const;

	TBool CancelL(CUserAgent& aUserAgent) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aWaitRespFromApp,
							CUserAgentState& aRejected);

private: // Data

	// Next possible states
	CUserAgentState* iWaitRespFromApp;
	CUserAgentState* iRejected;
	};



// Waiting SIP response from application
class CInviteUAS_WaitRespFromApp : public CUserAgentState
	{
public:	// Constructors and destructor

	CInviteUAS_WaitRespFromApp();

	~CInviteUAS_WaitRespFromApp();

public: // From CUserAgentState

	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	TBool ShouldUASStop() const;

	TBool CancelL(CUserAgent& aUserAgent) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aAccepted,
							CUserAgentState& aRejected);

private: // Data

	// Next possible states
	CUserAgentState* iAccepted;
	CUserAgentState* iRejected;
	};



// Waiting for ACK from the remote endpoint
class CInviteUAS_Accepted : public CUserAgentState
	{
public:	// Constructors and destructor

	CInviteUAS_Accepted();

	~CInviteUAS_Accepted();

public: // From CUserAgentState

	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	void ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const;

	void SendCompleteL(CUserAgent& aUserAgent) const;

	void SendFailedL(CUserAgent& aUserAgent, TInt aErrcode) const;	

	void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

    // Retransmission of a 2xx may cause ICMP error, but currently it isn't
    // handled.

	TBool CancelL(CUserAgent& aUserAgent) const;
	};



// Waiting for Transaction to end
class CInviteUAS_Rejected : public CUserAgentState
	{
public:	// Constructors and destructor

	CInviteUAS_Rejected();

	~CInviteUAS_Rejected();

public: // From CUserAgentState

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;
	
	// ICMP error is handled by transaction (not by UAS) as transaction sends
	// the error responses.

	// TransactionOwner shouldn't send response any more as the final response
	// has already been sent.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;	
	};

#endif // end of INVITEUASSTATES_H

// End of File
