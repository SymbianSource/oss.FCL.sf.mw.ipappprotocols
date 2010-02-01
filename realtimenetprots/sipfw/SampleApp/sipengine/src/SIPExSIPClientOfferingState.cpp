
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
#include	"SIPExSIPClientOfferingState.h"
#include	"SIPExSIPEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPClientOfferingState::CSIPExSIPClientOfferingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPExSIPClientOfferingState::CSIPExSIPClientOfferingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPClientOfferingState::~CSIPExSIPClientOfferingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPClientOfferingState::~CSIPExSIPClientOfferingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPClientOfferingState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPClientOfferingState* CSIPExSIPClientOfferingState::NewL()
	{
	CSIPExSIPClientOfferingState* self =
		new (ELeave) CSIPExSIPClientOfferingState();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPClientOfferingState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPClientOfferingState::LinkStates(
	CSIPExSIPStateBase& aIdleState,
	CSIPExSIPStateBase& aEstablishedState,
	CSIPExSIPStateBase& aTerminatedState )
	{
	iIdleState = &aIdleState;
	iEstablishedState = &aEstablishedState;
	iTerminatedState = &aTerminatedState;
	}


// -----------------------------------------------------------------------------
// CSIPExSIPClientOfferingState::CancelInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPClientOfferingState::CancelInviteL( CSIPExSIPEngine& aEngine )
	{
	// Get the current Client Transaction
	CSIPClientTransaction& clientTA = aEngine.ClientTx();

	// Use it the to send CANCEL request
    CSIPClientTransaction* ta = clientTA.CancelL();
    delete ta;

	aEngine.SetCurrentState( iIdleState );
	aEngine.Observer()->InvitationCancelled();
	}



// -----------------------------------------------------------------------------
// CSIPExSIPClientOfferingState::ResponseReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPClientOfferingState::ResponseReceivedL( 
    CSIPExSIPEngine& aEngine,
    CSIPClientTransaction& aTransaction )
	{
	const CSIPResponseElements* respElem = aTransaction.ResponseElements();

	TUint statusCode = respElem->StatusCode();

	if ( statusCode >= 200 && statusCode < 300 )
		{
		// Final Response received from network.
		// Get the IP Address of the remote party from the Response Elements
		const TInetAddr addr = 
		            aEngine.IPAddressFromResponseElementsL( *respElem );


		// Then, send ACK to remote party
		CSIPInviteDialogAssoc& dialogAssoc = aEngine.DialogAssoc();
		dialogAssoc.SendAckL( aTransaction );

        // Get used iap's id
		TUint32 iapId( 0 );
		CSIPConnection* conn = aEngine.DialogAssoc().Dialog().Connection();
		if ( conn )
		    {
		    iapId = conn->IapId();
		    }

		// After ACKing, notify observer, giving IP Address and IAP id
		// as parameter
		aEngine.Observer()->InviteAcceptedByRemote( addr, iapId );


		// Finally, change engine state
		aEngine.SetCurrentState( iEstablishedState );
		}
	else if ( statusCode >= 100 && statusCode < 200 )
		{
		// Provisional Response received from network.
		aEngine.Observer()->InviteReceivedByRemote( statusCode );
		}
	else
		{
		aEngine.Observer()->InviteDeclinedByRemote( statusCode );
		aEngine.SetCurrentState( iTerminatedState );
		}
	}

// End of file