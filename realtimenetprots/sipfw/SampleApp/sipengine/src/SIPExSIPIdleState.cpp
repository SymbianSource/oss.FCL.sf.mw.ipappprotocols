
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
#include	"SIPExSIPIdleState.h"
#include	"SIPExSIPEngine.h"

#include <uri8.h>
#include <sipstrings.h>
#include <sipstrconsts.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPIdleState::CSIPExSIPIdleState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPExSIPIdleState::CSIPExSIPIdleState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPIdleState::~CSIPExSIPIdleState
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPIdleState::~CSIPExSIPIdleState()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPIdleState::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSIPIdleState* CSIPExSIPIdleState::NewL()
	{
	CSIPExSIPIdleState* self = new (ELeave) CSIPExSIPIdleState();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPIdleState::LinkStates()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPIdleState::LinkStates(
	CSIPExSIPStateBase& aClientEstablishingState,
	CSIPExSIPStateBase& aServerOfferingState)
	{
	iClientEstablishingState = &aClientEstablishingState;
	iServerOfferingState = &aServerOfferingState;
	}

// -----------------------------------------------------------------------------
// CSIPExSIPIdleState::SendInviteL()
// Create and send an INVITE request to the recipient.
// -----------------------------------------------------------------------------
void CSIPExSIPIdleState::SendInviteL( 
    CSIPExSIPEngine& aEngine,
	const TDesC8& aSipUri )
	{
	// Retrieve the active profile and connection
	CSIPProfile& prof = aEngine.Profile();
	CSIPConnection& conn = aEngine.ConnectionL();

    // Create CUri8 from passed descriptor
    CUri8* uri8 = aEngine.ConvertToUri8LC( aSipUri );
    
	// Get dialog association, save for future use
	// The ownership of uri8 is transferred
	CSIPInviteDialogAssoc* dialogAssoc =
		CSIPInviteDialogAssoc::NewL( conn, uri8, prof );
	CleanupStack::Pop( uri8 );	
		
	aEngine.SetDialogAssoc( *dialogAssoc ); //Ownership is transferred!!

	// Create the necessary message elements
	CSIPMessageElements* msgElements = aEngine.CreateMessageElementsLC();

	// Send the INVITE in the dialog
	// The ownership of msgElements is transferred
	CSIPClientTransaction* tx = dialogAssoc->SendInviteL( msgElements );
	CleanupStack::Pop( msgElements );

	// Save the pointer to the transaction to the Engine
	aEngine.SetClientTx( tx );

	// Change machine state
	aEngine.SetCurrentState( iClientEstablishingState );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPIdleState::InviteReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPIdleState::InviteReceivedL( 
    CSIPExSIPEngine& aEngine,
	CSIPServerTransaction* aTransaction )
	{
	_LIT8( KLogEntry, "180 Ringing sent" );

	// Get dialog association, save for future use
	CSIPInviteDialogAssoc* dialogAssoc =
		CSIPInviteDialogAssoc::NewL( *aTransaction );
	aEngine.SetDialogAssoc( *dialogAssoc );
    
	// Create the necessary response elements and send
    CSIPResponseElements* elem =
    	CSIPResponseElements::NewLC( 
    	    180, SIPStrings::StringF( SipStrConsts::EPhraseRinging ) );
    aTransaction->SendResponseL( elem );
    CleanupStack::Pop( elem );
	aEngine.Observer()->WriteLog( KLogEntry );

	// Get the From header from the request
	const CSIPRequestElements* reqElem = aTransaction->RequestElements();
	const CSIPFromHeader* fromHeader = reqElem->FromHeader();
	
	// Change machine state
	aEngine.SetCurrentState( iServerOfferingState );

    TUint32 iapId( 0 );
	User::LeaveIfError( 
	        aEngine.Profile().GetParameter( KSIPAccessPointId, iapId ) );
	
	// Send user name from the From header to the Engine Observer 
	// as a parameter. Send also id of used IAP.
	aEngine.Observer()->InviteReceived( 
	    fromHeader->SIPAddress().Uri8().Uri().Extract( EUriUserinfo ), iapId );
	}

// End of file