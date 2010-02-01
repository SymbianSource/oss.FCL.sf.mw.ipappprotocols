
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



// INCLUDES
#include    "SIPExStateInviting.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"

// -----------------------------------------------------------------------------
// TSIPExStateInviting::InviteAcceptedByRemote
// Resets game, sets the peer to client and starts to connect to the remote 
// peer. The active state is changed to the Connecting
// -----------------------------------------------------------------------------
//
void TSIPExStateInviting::InviteAcceptedByRemote( 
    CSIPExEngine* aContext, 
    const TInetAddr& aIPAddress, 
    const TUint32 aIapId )
    {
    Info( aContext, K200Received() ); 
    Info( aContext, KSendingAck() ); 
    ChangeState( aContext, aContext->iStateConnecting );
    aContext->ResetGame();
    aContext->SetPeer( CSIPExEngine::EClient );
    aContext->SocketEngineL()->ConnectL( aIapId, aIPAddress );
    aContext->InfoL( KConnecting() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateInviting::InviteDeclinedByRemote
// The active state is changed to the Registered and the UI is notified.
// -----------------------------------------------------------------------------
//  
void TSIPExStateInviting::InviteDeclinedByRemote( 
    CSIPExEngine* aContext, 
    const TInt aResponse )
    {
    ChangeState( aContext, aContext->iStateRegistered );
    TRAPD( ignore, 
        CombineAndShowInfoL( aContext, KInviteDeclinedByRemote(), aResponse ) );
    StatusInfo( aContext, KGameOver() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateInviting::InvitationCancelled
// Notifies the UI and changes the active state to the Registered.
// -----------------------------------------------------------------------------
//          
void TSIPExStateInviting::InvitationCancelled( CSIPExEngine* aContext ) 
    {
    ChangeState( aContext, aContext->iStateRegistered );
    Info( aContext, KInvitationCancelled() );
    StatusInfo( aContext, KGameOver() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateInviting::EndGameL
// The SIP session will be ended.
// -----------------------------------------------------------------------------
//
void TSIPExStateInviting::EndGameL( CSIPExEngine* aContext )
    {
    aContext->SIPEngine()->CancelInviteL();
    }
// End of file
