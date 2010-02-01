
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
#include	"SIPExSIPServerEstablishingState.h"
#include	"SIPExSIPEngine.h"

#include <sipstrings.h>
#include <sipstrconsts.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::CSIPExSIPServerEstablishingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPExSIPServerEstablishingState::CSIPExSIPServerEstablishingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::~CSIPExSIPServerEstablishingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPServerEstablishingState::~CSIPExSIPServerEstablishingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPServerEstablishingState* CSIPExSIPServerEstablishingState::NewL()
	{
	CSIPExSIPServerEstablishingState* self =
		new (ELeave) CSIPExSIPServerEstablishingState();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPServerEstablishingState::LinkStates(
	CSIPExSIPStateBase& aEstablishedState,
	CSIPExSIPStateBase& aTerminatedState )
	{
	iEstablishedState = &aEstablishedState;
	iTerminatedState = &aTerminatedState;
	}
	
// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::ByeReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerEstablishingState::ByeReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPServerTransaction& aTransaction )
	{
	// Create and send response
    CSIPResponseElements* elem = 
        CSIPResponseElements::NewLC( 
            200, SIPStrings::StringF( SipStrConsts::EPhraseOk ) );
    aTransaction.SendResponseL( elem );
    CleanupStack::Pop( elem );

	// Inform observer
	aEngine.Observer()->SessionEnded();
	
	// Set state
	aEngine.SetCurrentState( iTerminatedState );
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::CancelReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerEstablishingState::CancelReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPClientTransaction& /*aTransaction */ )
	{
	aEngine.Observer()->SessionEnded();
	aEngine.SetCurrentState( iTerminatedState );
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerEstablishingState::AckReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerEstablishingState::AckReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPServerTransaction& /* aTransaction */ )
	{
	aEngine.Observer()->InviteAcceptedByUs();
	aEngine.SetCurrentState( iEstablishedState );
	}

