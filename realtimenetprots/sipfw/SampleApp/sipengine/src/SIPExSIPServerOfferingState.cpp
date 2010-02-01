
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
#include	"SIPExSIPServerOfferingState.h"
#include	"SIPExSIPEngine.h"

#include <sipstrings.h>
#include <sipstrconsts.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::CSIPExSIPServerOfferingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPExSIPServerOfferingState::CSIPExSIPServerOfferingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::~CSIPExSIPServerOfferingState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPServerOfferingState::~CSIPExSIPServerOfferingState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPServerOfferingState* CSIPExSIPServerOfferingState::NewL()
	{
	CSIPExSIPServerOfferingState* self =
		new (ELeave) CSIPExSIPServerOfferingState();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPServerOfferingState::LinkStates(
	CSIPExSIPStateBase& aServerEstablishingState,
	CSIPExSIPStateBase& aTerminatedState )
	{
	iServerEstablishingState = &aServerEstablishingState;
	iTerminatedState = &aTerminatedState;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::ByeReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerOfferingState::ByeReceivedL(
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
// CSIPExSIPServerOfferingState::CancelReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerOfferingState::CancelReceivedL(
	CSIPExSIPEngine& aEngine,
	CSIPClientTransaction& /*aTransaction */ )
	{
	aEngine.Observer()->SessionEnded();
	aEngine.SetCurrentState( iTerminatedState );
	}
	
// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::AcceptInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerOfferingState::AcceptInviteL( CSIPExSIPEngine& aEngine)
	{
	_LIT8( KMediaType, "application" );
	_LIT8( KMediaSubType, "sdp" );
	_LIT8( KLogEntry, "200 OK sent" );

	// Get the current transaction
	CSIPServerTransaction& tx = aEngine.ServerTx();
	// Create the Response Elements object
    CSIPResponseElements* respElem =
    	CSIPResponseElements::NewLC( 
    	    200, SIPStrings::StringF( SipStrConsts::EPhraseOk ) );
    
    // Set the message body - we need to communicate our IP Address
    CSIPMessageElements& msgElem = respElem->MessageElements();

	CSdpDocument* sdpDocument = aEngine.SdpDocumentLC();
	HBufC8* sdpBody = aEngine.SdpBodyL( sdpDocument );
	CleanupStack::PushL( sdpBody );

   	CSIPContentTypeHeader* ct =
   		CSIPContentTypeHeader::NewLC( KMediaType, KMediaSubType );
	msgElem.SetContentL( sdpBody, ct );

	// Use the transaction to send 200 (OK)
	tx.SendResponseL( respElem );

	CleanupStack::Pop( ct );
	CleanupStack::Pop( sdpBody );
	CleanupStack::PopAndDestroy( sdpDocument );
	CleanupStack::Pop( respElem );

	aEngine.SetCurrentState( iServerEstablishingState );
	aEngine.Observer()->WriteLog( KLogEntry );
	}



// -----------------------------------------------------------------------------
// CSIPExSIPServerOfferingState::DeclineInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPServerOfferingState::DeclineInviteL( CSIPExSIPEngine& aEngine )
	{
	// Get the current transaction
	CSIPServerTransaction& tx = aEngine.ServerTx();
	// Create the Response Elements object
    CSIPResponseElements* elem =
    	CSIPResponseElements::NewLC( 486, 
    	    SIPStrings::StringF( SipStrConsts::EPhraseBusyHere ) );

	// Use the transaction to send 486 (Busy Here)
	tx.SendResponseL( elem );

	CleanupStack::Pop( elem );
	aEngine.Observer()->SessionEnded();
	aEngine.SetCurrentState( iTerminatedState );
	}

