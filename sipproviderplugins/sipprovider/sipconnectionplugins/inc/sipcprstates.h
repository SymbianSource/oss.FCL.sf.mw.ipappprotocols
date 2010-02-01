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
// sip Connection Provider states
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef SIPCPRSTATES_H_
#define SIPCPRSTATES_H_

#include <comms-infras/ss_log.h>
#include <ss_glob.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/corecprstates.h>
#include <comms-infras/ss_coreprstates.h>
#include "sipprovisioninfo.h"
#include "sipcpr.h"

enum SipCprSpecificActivities
	{
	//This activity handles the ErrorOccurred call back function
	ECFActivitySipErrorOccurred = ESock::ECFActivityCustom
	};

namespace SipCprActivities
{
    typedef MeshMachine::TNodeContext<CSipConnectionProvider, CprStates::TContext> TContext;
	namespace BinderRequest
		{					
		
        DECLARE_SMELEMENT_HEADER( TSubConnectionTypeTag, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
        	virtual TInt TransitionTag();
        DECLARE_SMELEMENT_FOOTER( TSubConnectionTypeTag )			
		}
}

namespace SipCprStates
{
typedef MeshMachine::TNodeContext<CSipConnectionProvider, CprStates::TContext> TContext;

enum {
	KDeRegister = 1, 
	KRegistered	
	};


//====================================================================================
//
// BinderRequest Activity
//
//===================================================================================

// Provision Activity
DECLARE_SMELEMENT_HEADER( TSendProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendProvision )


//For comms binder request 
DECLARE_SMELEMENT_HEADER( TIntializeWaitForIncmgParams, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TIntializeWaitForIncmgParams )

//For comms binder request
DECLARE_SMELEMENT_HEADER(TWaitForIncomingOrUseExisting, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER(TWaitForIncomingOrUseExisting)

//For comms binder request
DECLARE_SMELEMENT_HEADER(TAwaitingIncomingConnection, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingIncomingConnection )


DECLARE_AGGREGATED_TRANSITION3(
   TProcessSCPRCreation,	
   CoreNetStates::TAddDataClient,
   SipCprStates::TSendProvision,
   PRActivities::CCommsBinderActivity::TStorePendingBinder
   )
  
DECLARE_AGGREGATED_TRANSITION2(
   TProcessWaitForIncmg,
   SipCprStates::TIntializeWaitForIncmgParams,
   PRStates::TCreateDataClient
   )


//===========================================================================================
//
// Provision Activity
//===========================================================================================
DECLARE_SMELEMENT_HEADER( TStoreProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TStoreProvision )


//=============================================================================================
//
// Stop Activity
//
//==============================================================================================
DECLARE_SMELEMENT_HEADER( TDeRegisterCpr, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TDeRegisterCpr )

DECLARE_SMELEMENT_HEADER( TAwaitingDeRegistrationComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingDeRegistrationComplete )

DECLARE_SMELEMENT_HEADER(TNoTagOrDeRegister, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext)
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER(TNoTagOrDeRegister)

DECLARE_SMELEMENT_HEADER( TSendStopped, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER(TSendStopped)


//=================================================================
//
// SipNoBearer activity
//
//=================================================================

DECLARE_SMELEMENT_HEADER(TAwaitingNoBearer, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER(TAwaitingNoBearer )

//=================================================================
//
// SipCprStartActivity
//
//=================================================================
DECLARE_SMELEMENT_HEADER( TNoTagOrRegistered, MeshMachine::TStateFork<TContext>, NetStateMachine::MStateFork, TContext )
	virtual TInt TransitionTag();
DECLARE_SMELEMENT_FOOTER( TNoTagOrRegistered )

DECLARE_SMELEMENT_HEADER( TRegisterCpr, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TRegisterCpr )

DECLARE_SMELEMENT_HEADER( TAwaitingRegistrationComplete, MeshMachine::TState<TContext>, NetStateMachine::MState, TContext)
	virtual TBool Accept();
DECLARE_SMELEMENT_FOOTER( TAwaitingRegistrationComplete )

DECLARE_SERIALIZABLE_STATE(
	TNoTagOrRegisteredBlockedByStop,
	MeshMachine::TActivityIdMutex<ESock::ECFActivityStop>,
	SipCprStates::TNoTagOrRegistered
	)
	
//================================================================
//Others
//
//==============================================================

DECLARE_SMELEMENT_HEADER( TSetPostedToScpr, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext  )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSetPostedToScpr )

} //namespace SipCprStates

#endif //SIPCPRSTATES_H_

