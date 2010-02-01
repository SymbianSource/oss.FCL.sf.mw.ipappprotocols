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
* Name          : CancelUACStates.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef CANCELUACSTATES_H
#define CANCELUACSTATES_H

// INCLUDES
#include "UserAgentState.h"

// CLASS DECLARATION

//Waiting TransactionId of the INVITE client transaction to be canceled, from
//CTransactionUser.
class CCancelUAC_Start : public CUserAgentState
	{
public: // Constructors and destructor

	CCancelUAC_Start();

	~CCancelUAC_Start();	

public: // From CUserAgentState

	void SendCancelL(CUserAgent& aUserAgent, TTransactionId aInviteTaId) const;

public: // New functions

	void SetNeighbourStates(CUserAgentState& aWaitResponse);

private: // Data

	//Next possible state
	CUserAgentState* iWaitResponse;
	};

#endif // end of CANCELUACSTATES_H

// End of File
