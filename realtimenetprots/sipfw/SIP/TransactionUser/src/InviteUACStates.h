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
* Name          : InviteUACStates.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef INVITEUACSTATES_H
#define INVITEUACSTATES_H

// INCLUDES
#include "UserAgentState.h"

// CLASS DECLARATION

// Waiting INVITE from application.
// The transaction can't be canceled yet, as TransactionId hasn't been passed
// to MTransactionOwner yet.
class CInviteUAC_Start : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_Start();

	~CInviteUAC_Start();

public: // From CUserAgentState

	void SendRequestL(CUserAgent& aUserAgent,
					  CSIPRequest* aReq,
					  TRegistrationId aRegisterId,
					  const CURIContainer& aRemoteTarget) const;

	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;
				  
public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAddress);

private: // Data

	// Next possible state
	CUserAgentState* iResolveAddress;

#ifdef CPPUNIT_TEST
	friend class CInviteUAC_Start_Test;
#endif
	};



// Waiting the remote address for INVITE to be resolved.
class CInviteUAC_ResolveAddress : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_ResolveAddress();

	~CInviteUAC_ResolveAddress();	

public: // From CUserAgentState

	TBool IsResolving() const;

	void AddressResolvedL(CUserAgent& aUserAgent) const;

	void ResolvingFailed(CUserAgent& aUserAgent) const;

	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;
				  
public: // New functions

	void SetNeighbourStates(CUserAgentState& aWaitResponse);

private: // Data

	// Next possible state
	CUserAgentState* iWaitResponse;

#ifdef CPPUNIT_TEST
	friend class CInviteUAC_ResolveAddress_Test;
#endif
	};



// Waiting a SIP response from transaction.
class CInviteUAC_WaitResponse : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_WaitResponse();

	~CInviteUAC_WaitResponse();

public: // From CUserAgentState

	void ReceiveL(CUserAgent& aUserAgent, CSIPResponse* aResponse) const;

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aWaitAckFromApp,
							CUserAgentState& aWaitTransactionToEnd,
							CUserAgentState& aResolveAddress,
							CUserAgentState& aWaitAuthentication);

private: // Data

	// Next possible states
	CUserAgentState* iWaitAckFromApp;
	CUserAgentState* iWaitTransactionToEnd;
	CUserAgentState* iResolveAddress;
	CUserAgentState* iWaitAuthentication;

#ifdef CPPUNIT_TEST
	friend class CInviteUAC_WaitResponse_Test;
#endif
	};



// Waiting application to send ACK
class CInviteUAC_WaitAckFromApp : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_WaitAckFromApp();

	~CInviteUAC_WaitAckFromApp();	

public: // From CUserAgentState

	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;

	// Further 2xx received
	void ReceiveL(CUserAgent& aUserAgent, CSIPResponse* aResponse) const;

	void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;
					   
public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAckAddress);

private: // Data

	// Next possible state
	CUserAgentState* iResolveAckAddress;
	};



// Waiting the remote address for an ACK to be resolved
class CInviteUAC_ResolveAckAddress : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_ResolveAckAddress();

	~CInviteUAC_ResolveAckAddress();	

public: // From CUserAgentState

	TBool IsResolving() const;

	void AddressResolvedL(CUserAgent& aUserAgent) const;

	void ResolvingFailed(CUserAgent& aUserAgent) const;
	
	// Upper layer sends another ACK
	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;

	// Further 2xx received
	void ReceiveL(CUserAgent& aUserAgent, CSIPResponse* aResponse) const;

	void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;
					   
public: // New functions

	void SetNeighbourStates(CUserAgentState& aSendingAck);

private: // Data

	// Next possible state
	CUserAgentState* iSendingAck;

#ifdef CPPUNIT_TEST
	friend class CInviteUAC_ResolveAckAddress_Test;
#endif
	};



// Waiting ACK to be sent to network.
class CInviteUAC_SendingAck : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_SendingAck();

	~CInviteUAC_SendingAck();
	
public: // From CUserAgentState

	void SendCompleteL(CUserAgent& aUserAgent) const;

	void SendFailedL(CUserAgent& aUserAgent, TInt aError) const;

	// Upper layer sends another ACK
	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;

	// Further 2xx received
	void ReceiveL(CUserAgent& aUserAgent, CSIPResponse* aResponse) const;
	
	void IcmpErrorL(CUserAgent& aUserAgent,
					const TInetAddr& aAddress,
					CSipConnectionMgr::TICMPError aError) const;
	
	void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAckAddress);

private: // Data

	// Next possible state
	CUserAgentState* iResolveAckAddress;
	};



// Waiting for transaction to end after it has processed a non-2xx final
// response.
class CInviteUAC_WaitTransactionToEnd : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_WaitTransactionToEnd();

	~CInviteUAC_WaitTransactionToEnd();

public: // From CUserAgentState

	TBool CanContinueWithoutOutgoingMsg(TBool aFinalRespPassed) const;
	
	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;
	};



// Waiting SIPSec to process a response
class CInviteUAC_WaitAuthentication : public CUserAgentState
	{
public: // Constructors and destructor

	CInviteUAC_WaitAuthentication();

	~CInviteUAC_WaitAuthentication();

public: // From CUserAgentState

	void SIPSecCacheUpdatedL(CUserAgent& aUserAgent, TBool aSuccess) const;
	
	// WaitAuthentication is entered only when a 4xx response has been received,
	// so no more responses should come from transaction. No ReceiveL needed.
	
	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	void SendAckL(CUserAgent& aUserAgent,
				  CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch) const;

	void TimerExpiredL(CUserAgent& aUserAgent,
					   TTimerId aTimerId,
					   TAny* aTimerParam) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAddress,
							CUserAgentState& aWaitResponse,
							CUserAgentState& aWaitAckFromApp,
							CUserAgentState& aWaitTransactionToEnd);

private: // Data

	// Next possible states	
	CUserAgentState* iResolveAddress;
	CUserAgentState* iWaitResponse;
	CUserAgentState* iWaitAckFromApp;
	CUserAgentState* iWaitTransactionToEnd;
	};

#endif // end of INVITEUACSTATES_H

// End of File
