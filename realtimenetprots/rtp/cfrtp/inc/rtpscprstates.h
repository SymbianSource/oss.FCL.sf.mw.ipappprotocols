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
// RTPSCPR States
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_RTPSCPRSTATES_H
#define SYMBIAN_RTPSCPRSTATES_H

#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>
#include <comms-infras/ss_coreprstates.h>
#include <rtp_subconparams.h>
#include "rtpscpr.h"


//-=========================================================
//
// States
//
//-=========================================================
namespace RtpSCprStates
{
enum {
	KRtp = 1,
	KNoRtp
	};	

typedef MeshMachine::TNodeContext<CRtpSubConnectionProvider, SCprStates::TContext> TContext;


DECLARE_SMELEMENT_HEADER( TNoTagOrErrorTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
  	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrErrorTag )

DECLARE_SMELEMENT_HEADER( TSendCommsBinderToCntrlProv, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendCommsBinderToCntrlProv )

DECLARE_SMELEMENT_HEADER( TCreateDataClient, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
    virtual void InitLocalParameterBundleL(CSubConRTPGenericParamSet* aReqGenericParams);
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

typedef MeshMachine::TRetryTransition<RtpSCprStates::TRequestCommsBinder, ESock::TCFServiceProvider::TCommsBinderRequest> TRequestCommsBinderRetry;
}

#endif // SYMBIAN_RTPSCPRSTATES_H
