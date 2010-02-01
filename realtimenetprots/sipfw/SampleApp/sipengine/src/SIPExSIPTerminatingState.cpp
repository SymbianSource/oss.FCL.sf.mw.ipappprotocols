
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include	"SIPExSIPTerminatingState.h"
#include	"SIPExSIPEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPTerminatingState::CSIPExSIPTerminatingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPExSIPTerminatingState::CSIPExSIPTerminatingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPTerminatingState::~CSIPExSIPTerminatingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPTerminatingState::~CSIPExSIPTerminatingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPTerminatingState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPTerminatingState* CSIPExSIPTerminatingState::NewL()
	{
	CSIPExSIPTerminatingState* self =
		new (ELeave) CSIPExSIPTerminatingState();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPTerminatingState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPTerminatingState::LinkStates(
	CSIPExSIPStateBase& aIdleState )
	{
	iIdleState = &aIdleState;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPTerminatingState::ResponseReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPTerminatingState::ResponseReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPClientTransaction& aTransaction )
	{
	const CSIPResponseElements* respElem = aTransaction.ResponseElements();

	if ( respElem->StatusCode() == 200 )
		{
		aEngine.Observer()->SessionEnded();
		aEngine.ClearClientTx();
		aEngine.SetCurrentState( iIdleState );
		}
	else
		{
		// Hmmm...
		}
	}

// End of file