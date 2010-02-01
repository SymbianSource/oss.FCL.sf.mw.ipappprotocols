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
// CStateSessionTerminate implementation file.
// 
//

/**
 @file
 @internalComponent
*/
 
#include <e32std.h>
#include <e32base.h>

#include "StateSessionTerminate.h"

CStateSessionTerminate * CStateSessionTerminate::NewL( CSipStateMachine * aStateMachine )
/**
Instantiaion static function
*/
	{
	return new(ELeave) CStateSessionTerminate (aStateMachine);
	}

CStateSessionTerminate::CStateSessionTerminate ( CSipStateMachine * aStateMachine ) :
CSipStateBase (aStateMachine, TSipHLConsts::EStateSessionTerminated)
/**
Constuctor
*/
	{
	// Open Logs
	__FLOG_OPEN(_L8("SipStateMachine"), _L8("StateSessionTerminate"));
	__FLOG_1(_L("[StateSessionTerminate]:[%x]::CStateSessionTerminate() "), this);
	}

CStateSessionTerminate::~CStateSessionTerminate ()
/**
Destructor
*/	
	{
	// Close Logging
	__FLOG_0(_L("[StateSessionTerminate]: CStateSessionTerminate::~CStateSessionTerminate()"));
	__FLOG_CLOSE;
	}

CSipStateBase * CStateSessionTerminate::ProcessClientL(TRequestStatus & aStatus)
/**
Function which process the request and responses from the client
*/
	{
	__FLOG_1(_L("[StateSessionTerminate]:CStateSessionTerminate[%x]::ProcessClientL()"), this);
	iSipSM->Cleanup();
	iSipSM->CallTerminateCallback();
	iSipSM->SetSuspendRequest(ETrue);
	TRequestStatus * stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
	// This will tell the state machine that the work has been completed 
	// and the state can be deleted
	__FLOG_1(_L("[StateSessionTerminate]:CStateSessionTerminate[%x]::ProcessClientL() <<Next State --> NULL>> "), this);
	return (iNext = NULL);
	}

CSipStateBase * CStateSessionTerminate::ProcessServerL( TRequestStatus & aStatus )
/**
Function which process the request and responses from the Server
*/
	{
	__FLOG_1(_L("[StateSessionTerminate]:CStateSessionTerminate[%x]::ProcessServerL()"), this);
	// Cleanup
	iSipSM->Cleanup();
	iSipSM->CallTerminateCallback();
	iSipSM->SetSuspendRequest(ETrue);
	TRequestStatus * stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
	__FLOG_1(_L("[StateSessionTerminate]:CStateSessionTerminate[%x]::ProcessServerL() <<Next State --> NULL>> "), this);	
	return (iNext = NULL);	
	}

CSipStateBase * CStateSessionTerminate::ProcessServerErrorL(TRequestStatus & aStatus)
/**
Function which process the request and responses from the Server in case of error
*/
	{
	__FLOG_1(_L("[StateSessionTerminate]:CStateSessionTerminate[%x]::ProcessServerErrorL()"), this);
	// call the callback and let off
	iSipSM->CallTerminateCallback();
	TRequestStatus * stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
	__FLOG_1(_L("[StateSessionTerminate]:CStateSessionTerminate[%x]::ProcessErrorServerL() <<Next State --> NULL>> "), this);	
	return NULL;
	}

