// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CStateSessionEstablished definition file
// 
//

/**
 @file
 @internalComponent
*/

#ifndef STATE_SESSION_ESTABLISHED_H
#define STATE_SESSION_ESTABLISHED_H

#include "SipStateMachine.h"

#ifdef __FLOG_ACTIVE
	// This will do the logging if logging is enabled
    #include <comms-infras/ss_log.h>
#endif

/**
This state is genrally follows the State Idle and Initaites a 
connection process between end points
*/
class CStateSessionEstablished : public CSipStateBase
	{
public:
	static CStateSessionEstablished * NewL(CSipStateMachine * aStateMachine);
	~CStateSessionEstablished();
	CSipStateBase * ProcessClientL(TRequestStatus & aStatus);
	CSipStateBase * ProcessServerL(TRequestStatus & aStatus);
	CSipStateBase * ProcessServerErrorL(TRequestStatus & aStatus);
private:
	// For Logging
	__FLOG_DECLARATION_MEMBER;

	CStateSessionEstablished(CSipStateMachine * aStateMachine);	
	TBool           iDoneUnSubscribe:1;
	};

#endif

