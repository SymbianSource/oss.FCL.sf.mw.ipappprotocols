
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
#include	"SIPExSIPEstablishedState.h"
#include	"SIPExSIPEngine.h"

#include <sipstrings.h>
#include <sipstrconsts.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::CSIPExSIPEstablishedState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPExSIPEstablishedState::CSIPExSIPEstablishedState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::~CSIPExSIPEstablishedState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPEstablishedState::~CSIPExSIPEstablishedState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPEstablishedState* CSIPExSIPEstablishedState::NewL()
	{
	CSIPExSIPEstablishedState* self =
		new (ELeave) CSIPExSIPEstablishedState();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPEstablishedState::LinkStates(
	CSIPExSIPStateBase& aTerminatingState,
	CSIPExSIPStateBase& aTerminatedState )
	{
	iTerminatingState = &aTerminatingState;
	iTerminatedState = &aTerminatedState;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::EndSessionL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEstablishedState::EndSessionL( CSIPExSIPEngine& aEngine )
	{
	// Get the current Dialog Association
	CSIPInviteDialogAssoc& dialogAssoc = aEngine.DialogAssoc();
	// Create the Message Elements object
	CSIPMessageElements* msgElements = aEngine.CreateMessageElementsLC();

	// Use the Dialog Association to send BYE request
	CSIPClientTransaction* tx = dialogAssoc.SendByeL( msgElements );
	aEngine.SetClientTx( tx );

	CleanupStack::Pop( msgElements );

	aEngine.SetCurrentState( iTerminatingState );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::ByeReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEstablishedState::ByeReceivedL(
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

	// Set state ???
	aEngine.SetCurrentState( iTerminatedState );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPEstablishedState::ResponseReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEstablishedState::ResponseReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPClientTransaction& aTransaction )
	{
	// We might receive some 200 (OK) responses from the network
	// after the session has been established. These should be
	// ACKed, other responses indicate errors.

	const CSIPResponseElements* respElem = aTransaction.ResponseElements();

	TUint statusCode = respElem->StatusCode();

	if ( statusCode >= 200 && statusCode < 300 )
		{
		// Send ACK to remote party
		CSIPInviteDialogAssoc& dialogAssoc = aEngine.DialogAssoc();
		dialogAssoc.SendAckL( aTransaction );
		}
	else
		{
		aEngine.Observer()->EngineError( statusCode );
		}
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::AckReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPEstablishedState::AckReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPServerTransaction& /* aTransaction */)
	{
	aEngine.Observer()->InviteAcceptedByUs();
	}
	
// End of file
