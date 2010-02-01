
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
#include    "SIPExStateIdle.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"

// -----------------------------------------------------------------------------
// TSIPExStateIdle::EnableProfile
// Notifies the UI, signals SIP engine to enable profile and changes the active
// state to the Registering. If no registration is needed the state is changed
// straight to the Registered.
// -----------------------------------------------------------------------------
//
void TSIPExStateIdle::EnableProfileL( CSIPExEngine* aContext )
    {
    // Notify UI
    aContext->InfoL( KEnablingProfile() );

    // The registration is required
    if( !aContext->SIPEngine()->EnableProfileL() )
        {
        ChangeState( aContext, aContext->iStateRegistering );
        }
    // The profile was succesfully enabled and registered
    else
        {
        aContext->InfoL( KProfileEnabled() );
        ChangeState( aContext, aContext->iStateRegistered );
        }
    }

// -----------------------------------------------------------------------------
// TSIPExStateIdle::SessionEnded
// Notifies the UI.
// -----------------------------------------------------------------------------
//
void TSIPExStateIdle::SessionEnded( CSIPExEngine* aContext )
    {
    // Keep track whether session ended while accept dialog was shown to user.
    iEnded = ETrue;
    
    Info( aContext, KSessionEnded() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateIdle::InviteReceived
// The acceptance is asked from the user and if accepted we reset game, start
// listening socket and signal the SIP engine to send Accepted to the remote
// peer.
// -----------------------------------------------------------------------------
//   
void TSIPExStateIdle::InviteReceived( 
    CSIPExEngine* aContext, 
    const TDesC8& aFrom, 
    const TUint32 aIapId )
    {
    iEnded = EFalse;
    
    TBool retVal( EFalse );
    TRAPD( ignore, retVal = aContext->AcceptInvitationL( aFrom ) );
    if ( iEnded )
        {
        return;
        }
    if( retVal )
        {
        ChangeState( aContext, aContext->iStateAcceptingSIP );
        // The game data is reseted
        aContext->ResetGame();
        // The role for the peer is set
        aContext->SetPeer( CSIPExEngine::EServer );
        // Start listen incoming connections
        TInetAddr addr;
        TRAP( ignore, 
                addr = aContext->SocketEngineL()->StartListeningL( aIapId ) );
        // Signal SIP engine
        TRAP( ignore, aContext->SIPEngine()->AcceptInviteL( addr ) );
        Info( aContext, KWaitingRemoteConn() );
        }
    else 
        {
        // Signal SIP engine
        TRAP( ignore, aContext->SIPEngine()->DeclineInviteL() );
        Info( aContext, KInviteDeclined() );
        }
    }

// End of file
