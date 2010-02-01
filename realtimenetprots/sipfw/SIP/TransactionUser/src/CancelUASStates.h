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
* Name          : CancelUASStates.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef CANCELUASSTATES_H
#define CANCELUASSTATES_H

// INCLUDES
#include "UserAgentState.h"

// CLASS DECLARATION

//Waiting SIP request from CTransaction.
class CCancelUAS_Start : public CUserAgentState
	{
public: // Constructors and destructor

	CCancelUAS_Start();

	~CCancelUAS_Start();	

public: // From CUserAgentState

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	void ReceiveL(CUserAgent& aUserAgent, CSIPRequest* aRequest) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aFinalRespSent);

private: // Data

	//Next possible state
	CUserAgentState* iFinalRespSent;
	};



//Waiting for Transaction state machine to end
class CCancelUAS_FinalRespSent : public CUserAgentState
	{
public: // Constructors and destructor

	CCancelUAS_FinalRespSent();

	~CCancelUAS_FinalRespSent();	
	
public: // From CUserAgentState

	void TransactionEndsL(CUserAgent& aUserAgent, TInt aReason) const;

	//TransactionOwner should not know about this transaction as CANCEL is not
	//passed above TU.
	void SendResponseL(CUserAgent& aUserAgent,
					   CSIPResponse* aResp,
					   const TSIPTransportParams& aParams) const;
	};

#endif // end of CANCELUASSTATES_H

// End of File
