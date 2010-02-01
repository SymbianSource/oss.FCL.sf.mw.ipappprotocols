// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// rtpscprstates.h
// RTPSCPR States
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __DUMMY__RTPSCPRSTATES_H__
#define __DUMMY__RTPSCPRSTATES_H__

#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_coreprstates.h>
#include "dummy_rtpscpr.h"


//-=========================================================
//
// States
//
//-=========================================================
namespace DummyRtpSCprStates
{
enum {
	KRtp = 1,
	KNoRtp
	};	

typedef MeshMachine::TNodeContext<CRtpDummySubConnectionProvider, SCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TNoTagOrBearerPresentOrErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrBearerPresentOrErrorTag )

DECLARE_SMELEMENT_HEADER( TNoTagOrErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrErrorTag )

DECLARE_SMELEMENT_HEADER( TSendCommsBinderToCntrlProv, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendCommsBinderToCntrlProv )

DECLARE_SMELEMENT_HEADER( TCreateDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCreateDataClient )

DECLARE_SMELEMENT_HEADER( TRequestCommsBinder, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRequestCommsBinder )

DECLARE_SMELEMENT_HEADER( TAwaitingReadyForBind, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingReadyForBind )

DECLARE_SMELEMENT_HEADER( TAwaitingBinderRequest, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingBinderRequest )

DECLARE_SMELEMENT_HEADER( TSetPostedToFlow, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetPostedToFlow )

DECLARE_SMELEMENT_HEADER( TCheckParamsAndDoSomething, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TCheckParamsAndDoSomething )


typedef MeshMachine::TRetryTransition<DummyRtpSCprStates::TRequestCommsBinder, ESock::TCFServiceProvider::TCommsBinderRequest> TRequestCommsBinderRetry;
}

#endif // SYMBIAN_RTPSCPRSTATES_H
