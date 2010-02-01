
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
#include    "SIPExStateLocal.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"
#include    "SIPExGameConstants.h"

// -----------------------------------------------------------------------------
// TSIPExStateLocal::CursorPressed
// Game state is updated.
// Move is sent to the remote peer. If WIN the state is Registered.
// otherwise turn is moved to remote.
// -----------------------------------------------------------------------------
// 
void TSIPExStateLocal::CursorPressed( CSIPExEngine* aContext )
    {
    TInt y = aContext->CalculatePos();
	if ( y != -1 ) 
        {
        aContext->SetBoard( aContext->Cursor(), y, 1 );
		aContext->IncreaseMovesBy( 1 );
		aContext->SendMessage( aContext->Cursor(), y );
		TInt ret = aContext->IsWin( aContext->Cursor(),y ); 
		
        if (ret == 1) 
            {
            ChangeState( aContext, aContext->iStateRemote );
            aContext->StatusInfoL( KYouWin() );
            EndGameL( aContext );
			}
        else  if ( aContext->Moves() == KMaxMoves ) 
            {
            ChangeState( aContext, aContext->iStateRemote );
			aContext->StatusInfoL( KGameTie() );
            EndGameL( aContext );
		    }         
        else 
            {
            ChangeState( aContext, aContext->iStateRemote );
            aContext->StatusInfoL( KWaitingRemote() );
			}
		}
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::CursorRight
// Game state is updated.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::CursorRight( CSIPExEngine* aContext )
    {
    aContext->SetCursor( aContext->Cursor() + 1 );
	if ( aContext->Cursor() > KBoxCountX - 1 )
        {
		aContext->SetCursor( 0 );
        }
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::CursorLeft
// Game state is updated.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::CursorLeft( CSIPExEngine* aContext )
    {
	aContext->SetCursor( aContext->Cursor() - 1 );
	if ( aContext->Cursor() < 0 )
        {
		aContext->SetCursor( KBoxCountX - 1 );
        }
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::DrawCursor
// Returns always ETrue, because it is local player's turn we have to draw the
// cursor.
// -----------------------------------------------------------------------------
//   
TBool TSIPExStateLocal::DrawCursor()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::SendInstantMsgL
// Calls base classes function to send the instant message.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::SendInstantMsgL( CSIPExEngine* aContext, 
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
// TSIPExStateLocal::InviteL
// Notifies the UI.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::InviteL( 
    CSIPExEngine* aContext, 
    const TDesC& /*aAddress*/ )
    {
    aContext->InfoL( KGameAlreadyRunning() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::DisableProfile
// Notifies the UI.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::DisableProfileL( CSIPExEngine* aContext )
    {
    aContext->InfoL( KGameAlreadyRunning() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::EndGameL
// Notifies the UI and changes the active state to the Registered.
// The socket engine will be deleted and the SIP session ended.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::EndGameL( CSIPExEngine* aContext )
    {
    aContext->SIPEngine()->EndSessionL();
    }

// -----------------------------------------------------------------------------
// TSIPExStateLocal::MoveCursorL
// Moves cursor into the new position.
// -----------------------------------------------------------------------------
//   
void TSIPExStateLocal::MoveCursorL( 
    CSIPExEngine* aContext, 
    const TInt aNewPosition )
    {
    aContext->SetCursor( aNewPosition );
    }

// End of file
