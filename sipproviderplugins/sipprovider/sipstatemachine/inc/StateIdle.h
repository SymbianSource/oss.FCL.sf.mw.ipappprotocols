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
// CStateIdle definition file
// 
//

/**
 @file
 @internalComponent
*/

#ifndef STATE_IDLE_H
#define STATE_IDLE_H

#include "SipStateMachine.h"

#ifdef __FLOG_ACTIVE
	// This will do the logging if logging is enabled
    #include <comms-infras/ss_log.h>
#endif

/**
This class will contain the default state when the state machine
will come into existance. Any change in state can only be done by
an external stimulus.
*/
class CStateIdle : public CSipStateBase
	{
public:
	static CStateIdle * NewL(CSipStateMachine * aStateMachine);
	~CStateIdle();
	CSipStateBase * ProcessClientL(TRequestStatus & aStatus);
	CSipStateBase * ProcessServerL(TRequestStatus & aStatus);
	CSipStateBase * ProcessServerErrorL(TRequestStatus & aStatus);
private:		
	// For Logging
	__FLOG_DECLARATION_MEMBER;
	CStateIdle(CSipStateMachine * aStateMachine);	
	};


#endif

