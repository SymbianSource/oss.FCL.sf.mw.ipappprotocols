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
* Name          : NormalUASStates.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef NORMALUASSTATES_H
#define NORMALUASSTATES_H

// INCLUDES
#include "UserAgentState.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

// Waiting SIP request from transaction layer. Transaction can't be canceled now,
// as TransactionId hasn't yet been passed to MTransactionOwner.
class CNormalUAS_Start : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAS_Start();

	~CNormalUAS_Start();

public: // From CUserAgentState

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;
	
	void ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const;

	// TransactionEndsL does not occur in this state

	// Upper layer shouldn't send response yet, as UA hasn't passed the request
	// yet, but in theory this can happen if upper layer happens to use this
	// UAS's TransactionId.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aGetTxOwner,
							CUserAgentState& aWaitRespFromApp,
							CUserAgentState& aFinalRespSent);

private: // Data

	// Next possible states, not owned.
	CUserAgentState* iGetTxOwner;
	CUserAgentState* iWaitRespFromApp;
	CUserAgentState* iFinalRespSent;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CNormalUAS_Start_Test;
#endif
	};



// Waiting to obtain MTransactionOwner. Transaction can't be canceled now,
// as TransactionId hasn't yet been passed to MTransactionOwner.
class CNormalUAS_GetTxOwner : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAS_GetTxOwner();

	~CNormalUAS_GetTxOwner();
	
public: // From CUserAgentState

	void OwnerFoundL(CUserAgent& aUserAgent, MTransactionOwner* aOwner) const;

	void OwnerNotFoundL(CUserAgent& aUserAgent, CSIPResponse* aResp) const;

	// Upper layer shouldn't send response yet, as UA hasn't passed the request
	// yet, but in theory this can happen if upper layer happens to use this
	// UAS's TransactionId.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

    void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

    void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

    void RequestRouterErrorL(CUserAgent& aUserAgent) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aWaitRespFromApp,
							CUserAgentState& aFinalRespSent);	

private: // Data

	// Next possible states, not owned.
	CUserAgentState* iWaitRespFromApp;
	CUserAgentState* iFinalRespSent;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CNormalUAS_GetTxOwner_Test;
	friend class CUserAgentServer_Test;
#endif
	};



//Waiting upper layer to send a SIP response.
class CNormalUAS_WaitRespFromApp : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAS_WaitRespFromApp();

	~CNormalUAS_WaitRespFromApp();
	
public: // From CUserAgentState

	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aFinalRespSent);

private: // Data

	// Next possible state, not owned.
	CUserAgentState* iFinalRespSent;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CNormalUAS_WaitRespFromApp_Test;
#endif
	};



//Waiting transaction state machine to end
class CNormalUAS_FinalRespSent : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAS_FinalRespSent();

	~CNormalUAS_FinalRespSent();
	
public: // From CUserAgentState

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	// Upper layer shouldn't send response anymore, as a final response has
	// already been sent.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CNormalUAS_FinalRespSent_Test;
#endif
	};

#endif // end of NORMALUASSTATES_H

// End of File
