// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef SYMBIAN_SIPSCPRSTATES_H
#define SYMBIAN_SIPSCPRSTATES_H

#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/ss_corepractivities.h>

#include "sipdeftscpr.h"
#include "sipscpr.h"


//SIP Default SCPR states
namespace SipDeftSCprStates
{
typedef MeshMachine::TNodeContext<CSipDefaultSubConnectionProvider, SCprStates::TContext> TContext;
}



//SIP SCPR states
namespace SipSCprStates
{
typedef MeshMachine::TNodeContext<CSipSubConnectionProvider, SCprStates::TContext> TContext;

//-=========================================================
//
//States
//
//-=========================================================
 
//start conneciton activity
DECLARE_SMELEMENT_HEADER( TAwaitingSessionEstablished, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSessionEstablished )

//stop connection 
DECLARE_SMELEMENT_HEADER( TAwaitingSessionTerminated, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext  )
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingSessionTerminated )

//-=========================================================
//
//State Forks
//
//-=========================================================

//Start Connection
DECLARE_SMELEMENT_HEADER(TNoTagOrAlreadyStarted, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER(TNoTagOrAlreadyStarted )

//Stop Connection
DECLARE_SMELEMENT_HEADER( TNoTagOrAlreadyStopped, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();	
DECLARE_SMELEMENT_FOOTER( TNoTagOrAlreadyStopped )

//-=========================================================
//
//Transitions
//
//-=========================================================

// StartConnection Activity
DECLARE_SMELEMENT_HEADER( TStartSession, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStartSession )

// Stop Activity
DECLARE_SMELEMENT_HEADER( TStopSession, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStopSession )
/*
DECLARE_SMELEMENT_HEADER( TSendProviderDown, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendProviderDown )
*/
// ParamRequest Activity
DECLARE_SMELEMENT_HEADER( TSetParameters, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetParameters )

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_AGGREGATED_TRANSITION2(
   TStoreAndSetParams,
   PRStates::TStoreParams,
   SipSCprStates::TSetParameters   
   )
#else
DECLARE_AGGREGATED_TRANSITION2(
   TStoreAndSetParams,
   SCprStates::TStoreParams,
   SipSCprStates::TSetParameters   
   )
#endif //SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

/*
DECLARE_SMELEMENT_HEADER( TNotifyClientEvent, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TNotifyClientEvent )
*/

//===========================================================================================
//
// Provision Activity
//===========================================================================================
DECLARE_SMELEMENT_HEADER( TStoreProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreProvision )

}

#endif // 
