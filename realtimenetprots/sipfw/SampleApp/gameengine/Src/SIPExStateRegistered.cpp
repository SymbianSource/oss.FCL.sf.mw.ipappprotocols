
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
#include    "SIPExStateRegistered.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"


// -----------------------------------------------------------------------------
// TSIPExStateRegistered::InviteL
// SIP engine is signaled to send invite to the specified address.
// The UI is notified and the active state changed to the Inviting.
// -----------------------------------------------------------------------------
//   
void TSIPExStateRegistered::InviteL( 
    CSIPExEngine* aContext, 
    const TDesC& aAddress )
    {
    ChangeState( aContext, aContext->iStateInviting );
    
    HBufC8* address = HBufC8::NewLC( aAddress.Length() );
    address->Des().Copy( aAddress );
    aContext->SIPEngine()->SendInviteL( *address );
    aContext->InfoL( KInviting() );
    aContext->InfoL( aAddress );
    aContext->StatusInfoL( KGameStarting() );
    CleanupStack::PopAndDestroy( address );
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistered::SendInstantMsgL
// Calls base classes function to send the instant message.
// -----------------------------------------------------------------------------
//   
void TSIPExStateRegistered::SendInstantMsgL( CSIPExEngine* aContext, 
            const TDesC& aAddress, const TDesC& aMsg )
    {
    HBufC8* addr = HBufC8::NewLC( aAddress.Length() );
    addr->Des().Copy( aAddress );

    HBufC8* msg = HBufC8::NewLC( aMsg.Length() );
    msg->Des().Copy( aMsg );

    aContext->SIPEngine()->CreateIML( *msg, *addr );

    CleanupStack::PopAndDestroy( msg );
    CleanupStack::PopAndDestroy( addr );
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistered::SessionEnded
// Notifies the UI.
// -----------------------------------------------------------------------------
//
void TSIPExStateRegistered::SessionEnded( CSIPExEngine* aContext )
    {
    // Keep track whether session ended while accept dialog was shown to user.
    iEnded = ETrue;

    Info( aContext, KSessionEnded() );
    StatusInfo( aContext, KGameOver() );
    TRAPD( ignore, aContext->SocketEngineL()->Stop() );
    aContext->DestroySocketEngine();
    }
    
// -----------------------------------------------------------------------------
// TSIPExStateRegistered::InviteReceived
// The acceptance is asked from the user and if accepted we reset game, start
// listening socket and signal the SIP engine to send Accepted to the remote
// peer.
// -----------------------------------------------------------------------------
//   
void TSIPExStateRegistered::InviteReceived( 
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
        StatusInfo( aContext, KGameStarting() );
        ChangeState( aContext, aContext->iStateAcceptingSIP );
        aContext->ResetGame();
        aContext->SetPeer( CSIPExEngine::EServer );
        Info( aContext, KListening() );
        TInetAddr addr;
        TRAP( ignore, addr = 
                        aContext->SocketEngineL()->StartListeningL( aIapId ) );
        Info( aContext, KAccepting() );
        TRAP( ignore, aContext->SIPEngine()->AcceptInviteL( addr ) );
        Info( aContext, KWaitingRemoteConn() );
        }
    else 
        {
        TRAP( ignore, aContext->SIPEngine()->DeclineInviteL() );
        Info( aContext, KInviteDeclined() );
        }
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistered::DisableProfile
// Disable profile and notify the UI.
// -----------------------------------------------------------------------------
//   
void TSIPExStateRegistered::DisableProfileL( CSIPExEngine* aContext )
    {
    ChangeState( aContext, aContext->iStateIdle );
    aContext->SIPEngine()->DisableProfileL();
    aContext->InfoL( KProfileDisabled() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistered::SocketState
// Notify the UI.
// -----------------------------------------------------------------------------
//  
void TSIPExStateRegistered::SocketState( CSIPExEngine* aContext, TInt aNewState )
    {
    if( aNewState == CSIPExSocketEngine::ENotConnected )
        {
        Info( aContext, KConnectionDropped() );
        StatusInfo( aContext, KGameOver() );
        }
    }

// End of file
