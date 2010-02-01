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
// CStateSessionTerminate definition file
// 
//

/**
 @file
 @internalComponent
*/

#ifndef STATE_SESSION_TERMINATE_H
#define STATE_SESSION_TERMINATE_H


#include <e32std.h>
#include <e32base.h>

#include "SipStateMachine.h"

#ifdef __FLOG_ACTIVE
	// This will do the logging if logging is enabled
    #include <comms-infras/ss_log.h>
#endif

class CStateSessionTerminate : public CSipStateBase
	{
public:
	static CStateSessionTerminate * NewL(CSipStateMachine * aStateMachine);
	~CStateSessionTerminate();
	CSipStateBase * ProcessClientL(TRequestStatus & aStatus);
	CSipStateBase * ProcessServerL(TRequestStatus & aStatus);
	CSipStateBase * ProcessServerErrorL(TRequestStatus & aStatus);
private:
	// For Logging
	__FLOG_DECLARATION_MEMBER;
	CStateSessionTerminate(CSipStateMachine * aStateMachine);	
	};
	
	
#endif

