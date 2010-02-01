
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
#include    "SIPExStateConnecting.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"

#include    <e32svr.h>

// -----------------------------------------------------------------------------
// TSIPExStateConnecting::SocketState
// Changes the active state according the new state of the Socket engine.
// If the socket is connected the game is reseted first and the turn is given
// to the local user.
// If not connected, info UI and set Registered to a new active state.
// -----------------------------------------------------------------------------
//
void TSIPExStateConnecting::SocketState( 
    CSIPExEngine* aContext, 
    TInt aNewState )
    {
    switch (aNewState)
        {
        case CSIPExSocketEngine::ENotConnected:
            ChangeState( aContext, aContext->iStateRegistered );
            aContext->InfoL( KNotConnected() );
            break;
        case CSIPExSocketEngine::EConnected:
            if( aContext->Peer() == CSIPExEngine::EClient )
                {
                aContext->ResetGame();
                ChangeState( aContext, aContext->iStateLocal );
                aContext->StatusInfoL( KYourMove() );
                aContext->InfoL( KConnected() );
			    } 
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
// TSIPExStateInviting::EndGameL
// The SIP Session will be ended.
// -----------------------------------------------------------------------------
//
void TSIPExStateConnecting::EndGameL( CSIPExEngine* aContext )
    {
    aContext->SIPEngine()->EndSessionL();
    }


// End of file
