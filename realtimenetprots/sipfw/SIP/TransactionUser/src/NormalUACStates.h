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
* Name          : NormalUACStates.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef NORMALUACSTATES_H
#define NORMALUACSTATES_H

// INCLUDES
#include "UserAgentState.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

//Waiting SIP request from upper layer.
//The transaction can not be canceled yet, as TransactionId has not been given
//to MTransactionOwner yet.
class CNormalUAC_Start: public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAC_Start();

	~CNormalUAC_Start();

public: // From CUserAgentState

	void SendRequestL(CUserAgent& aUserAgent,
					  CSIPRequest* aReq,
					  TRegistrationId aRegisterId,
					  const CURIContainer& aRemoteTarget) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAddress);

private: // Data

	// Next possible state, not owned.
	CUserAgentState* iResolveAddress;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CNormalUAC_Start_Test;
#endif
	};



//Waiting next hop URI to be resolved.
class CNormalUAC_ResolveAddress : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAC_ResolveAddress();

	~CNormalUAC_ResolveAddress();	

public: // From CUserAgentState

	TBool IsResolving() const;

	void AddressResolvedL(CUserAgent& aUserAgent) const;

	void ResolvingFailed(CUserAgent& aUserAgent) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aWaitResponse);

private: // Data

	// Next possible state, not owned.
	CUserAgentState* iWaitResponse;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CNormalUAC_ResolveAddress_Test;
#endif
	};



//Waiting SIP response from network.
class CNormalUAC_WaitResponse : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAC_WaitResponse();

	~CNormalUAC_WaitResponse();	

public: // From CUserAgentState

	TBool CanContinueWithoutOutgoingMsg(TBool aFinalRespPassed) const;
	
	void ReceiveL(CUserAgent& aUserAgent, CSIPResponse* aResponse) const;

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAddress,
							CUserAgentState& aWaitAuthentication);

private: // Data

	// Next possible states, not owned
	CUserAgentState* iResolveAddress;
	CUserAgentState* iWaitAuthentication;	
	};



//Waiting SIPSec to add the authentication parameters
class CNormalUAC_WaitAuthentication : public CUserAgentState
	{
public: // Constructors and destructor

	CNormalUAC_WaitAuthentication();

	~CNormalUAC_WaitAuthentication();
	
public: // From CUserAgentState	

	void SIPSecCacheUpdatedL(CUserAgent& aUserAgent, TBool aSuccess) const;

	//No more final responses can come from transaction

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aResolveAddress,
							CUserAgentState& aWaitResponse);

private: // Data

	//Next possible states, not owned.
	CUserAgentState* iResolveAddress;
	CUserAgentState* iWaitResponse;	
	};

#endif // end of NORMALUACSTATES_H

// End of File
