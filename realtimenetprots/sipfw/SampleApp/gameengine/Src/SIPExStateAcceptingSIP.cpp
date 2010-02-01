
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
#include    "SIPExStateAcceptingSIP.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"

// -----------------------------------------------------------------------------
// TSIPExState::SocketState
// Changes the active state according the new state of the Socket engine.
// If the socket is connected the game is reseted first and the turn is given
// to the remote peer.
// If not connected, info UI and set Registered to a new active state.
// -----------------------------------------------------------------------------
//
void TSIPExStateAcceptingSIP::SocketState( 
    CSIPExEngine* aContext, 
    TInt aNewState )
    {
    aContext->SetPeer( CSIPExEngine::EServer );
    switch (aNewState)
        {
        case CSIPExSocketEngine::ENotConnected:
            ChangeState( aContext, aContext->iStateRegistered );
            aContext->InfoL( KNotConnected() );
            break;
        case CSIPExSocketEngine::EConnected:
            aContext->ResetGame();
            ChangeState( aContext, aContext->iStateRemote );
            aContext->StatusInfoL( KWaitingRemote() );
            aContext->InfoL( KConnected() );
            break;

        case CSIPExSocketEngine::ETimedOut:
            ChangeState( aContext, aContext->iStateRegistered );
            aContext->InfoL( KTimedOut() );
            break;
        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// TSIPExStateAcceptingSIP::EndGameL
// The SIP session will be ended.
// -----------------------------------------------------------------------------
//
void TSIPExStateAcceptingSIP::EndGameL( CSIPExEngine* aContext )
    {
    aContext->SIPEngine()->EndSessionL();
    }

// -----------------------------------------------------------------------------
// TSIPExStateAcceptingSIP::InvitationCancelled
// The invitation is cancelled. The active state is changed to the registered
// and the socket engine is destroyed.
// -----------------------------------------------------------------------------
//
void TSIPExStateAcceptingSIP::InvitationCancelled( CSIPExEngine* aContext)
    {
    ChangeState( aContext, aContext->iStateRegistered );
    TRAPD( ignore, aContext->SocketEngineL()->Stop() );
    aContext->DestroySocketEngine();
    }
    
// End of file
