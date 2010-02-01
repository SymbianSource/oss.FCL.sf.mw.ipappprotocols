
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
#include    "SIPExStateRemote.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"

// -----------------------------------------------------------------------------
// TSIPExStateRegistering::SocketData
// Resolves the move or message from remote player.
// -----------------------------------------------------------------------------
//
void TSIPExStateRemote::SocketData( CSIPExEngine* aContext, TDesC8& aData )
    {
    // The length is fixed to 4. Format is "x, y".
	if ( aData.Length() == 4 ) 
        {
        TBuf<1> buffer;
        buffer.Copy( aData.Mid( 0, 1 ) );
        TLex lex( buffer );
		TInt x( 0 );
		TInt y( 0 );
        lex.Val( x );
        buffer.Copy( aData.Mid( 3, 1 ) );
        lex.Assign( buffer );
        lex.Val( y );

		aContext->SetRemote( x, y );
        aContext->IncreaseMovesBy( 1 );

        TInt ret = aContext->IsWin(x,y);
		aContext->SetCursor( x );
		if (ret > 0) 
            {
            ChangeState( aContext, aContext->iStateRegistered );
            StatusInfo( aContext, KYouLose() );
		    } 
        else  if ( aContext->Moves() == KMaxMoves) 
            {
            ChangeState( aContext, aContext->iStateRegistered );
			StatusInfo( aContext, KGameTie() );
		    } 
        else 
            {
            ChangeState( aContext, aContext->iStateLocal );
			StatusInfo( aContext, KYourMove() );
		    }
	    } 
    else 
        {
        HBufC* buf = HBufC::NewLC( aData.Length() );
        buf->Des().Copy( aData );
        aContext->InfoL( *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistering::SendInstantMsgL
// Calls base classes function to send the instant message.
// -----------------------------------------------------------------------------
//
void TSIPExStateRemote::SendInstantMsgL( CSIPExEngine* aContext, 
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
// TSIPExStateRegistering::DisableProfile
// Notifies the UI.
// -----------------------------------------------------------------------------
//
void TSIPExStateRemote::DisableProfileL( CSIPExEngine* aContext )
    {
    aContext->InfoL( KGameAlreadyRunning() );
    }

// -----------------------------------------------------------------------------
// TSIPExStateRegistering::EndGameL
// Notifies the UI and changes the active state to the Registered.
// The socket engine will be deleted and the SIP session ended.
// -----------------------------------------------------------------------------
//
void TSIPExStateRemote::EndGameL( CSIPExEngine* aContext )
    {
    aContext->SIPEngine()->EndSessionL();
    }

// End of file
