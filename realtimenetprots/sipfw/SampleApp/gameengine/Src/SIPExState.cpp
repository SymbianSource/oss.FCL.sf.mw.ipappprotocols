
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



#include    "SIPExState.h"
#include    "SIPExGameEngine.h"
#include    "SIPExSIPEngine.h"
#include    "SIPExGameObserver.h"

// -----------------------------------------------------------------------------
// TSIPExState::ChangeState
// Changes the active state through the context.
// -----------------------------------------------------------------------------
//
void TSIPExState::ChangeState( CSIPExEngine* aContext, TSIPExState* aNewState )
    {
    aContext->ChangeState( *aNewState );
    }

// -----------------------------------------------------------------------------
// TSIPExState::Info
// Traps the leaving functions. Ignores if leaves.
// -----------------------------------------------------------------------------
//
void TSIPExState::Info( CSIPExEngine* aContext, const TDesC& aText )
    {
    TRAPD( ignore, aContext->InfoL( aText ) );
    }

// -----------------------------------------------------------------------------
// TSIPExState::StatusInfo
// Traps the leaving functions. Ignores if leaves.
// -----------------------------------------------------------------------------
//
void TSIPExState::StatusInfo( CSIPExEngine* aContext, const TDesC& aText )
    {
    TRAPD( ignore, aContext->StatusInfoL( aText ) );
    }

// -----------------------------------------------------------------------------
// TSIPExState::CombineAndShowInfoL
// Combine desc and number value. E.g. aText="Hello", aNum=2, result="Hello 2".
// -----------------------------------------------------------------------------
//
void TSIPExState::CombineAndShowInfoL( 
    CSIPExEngine* aContext, 
    const TDesC& aText, 
    TInt aNum )
    {
    HBufC* buf = HBufC::NewLC( 64 );
    buf->Des().Append( aText );
    buf->Des().AppendNum( aNum );
    Info( aContext, *buf );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// TSIPExState::InviteReceived
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::InviteReceived( CSIPExEngine* /*aContext*/, 
                const TDesC8& /*aFrom*/, const TUint32 /*aIapId*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::InviteAcceptedByRemote
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::InviteAcceptedByRemote( 
    CSIPExEngine* /*aContext*/, 
    const TInetAddr& /*aIPAddress*/, 
    const TUint32 /*aIapId*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::InviteDeclinedByRemote
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::InviteDeclinedByRemote( 
    CSIPExEngine* /*aContext*/, 
    const TInt /*aResponse*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::InvitationCancelled
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::InvitationCancelled( CSIPExEngine* /*aContext*/) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::CancelFailed
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::CancelFailed( CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::ProfileEnabled
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::ProfileEnabled( 
    CSIPExEngine* /*aContext*/, 
    TUint32 /*aSIPProfileId*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::SocketState
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::SocketState( CSIPExEngine* /*aContext*/, TInt /*aNewState*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::SocketData
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::SocketData( CSIPExEngine* /*aContext*/, TDesC8& /*aData*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::EnableProfileL
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::EnableProfileL( CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::DisableProfileL
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::DisableProfileL( CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::CursorPressed
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::CursorPressed( CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::CursorRight
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::CursorRight( CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::CursorLeft
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::CursorLeft(  CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::MoveCursorL
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::MoveCursorL( 
    CSIPExEngine* /*aContext*/, 
    const TInt /*aNewPosition*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::SendInstantMsgL
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::SendInstantMsgL( 
    CSIPExEngine* /*aContext*/, 
    const TDesC& /*aAddress*/, 
    const TDesC& /*aMsg*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::EndGameL
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::EndGameL( CSIPExEngine* /*aContext*/ ) 
    { 
    }

// -----------------------------------------------------------------------------
// TSIPExState::InviteL
// Default implementation for virtual function.
// -----------------------------------------------------------------------------
//
void TSIPExState::InviteL( CSIPExEngine* /*aContext*/, const TDesC& /*aAddress*/ )
    {
    }

// -----------------------------------------------------------------------------
// TSIPExState::DrawCursor
// Returns always EFalse.
// -----------------------------------------------------------------------------
//
TBool TSIPExState::DrawCursor()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TSIPExState::DrawBoard
// Returns always ETrue.
// -----------------------------------------------------------------------------
//
TBool TSIPExState::DrawBoard() 
    { 
    return ETrue; 
    }

// -----------------------------------------------------------------------------
// TSIPExState::CalculatePos
// Calculates the position next free position in the cursor's column.
// -----------------------------------------------------------------------------
//
TInt TSIPExState::CalculatePos( CSIPExEngine* aContext )
    {
    TInt y( KErrNotFound );

	for ( TInt i = KBoxCountX - 1; i >= 0; i-- ) 
        {
		if ( aContext->BoardValue( aContext->Cursor(), i ) == -1 ) 
            {
			y = i;
			return y;
		    }
	    }

	return y;
    }

// -----------------------------------------------------------------------------
// TSIPExState::IsWin
// Checks that if the specified move is the win move.
// -----------------------------------------------------------------------------
//
TInt TSIPExState::IsWin( CSIPExEngine* aContext, const TInt aX, const TInt aY )
    {
	TInt ret = -1;

	TInt player = aContext->BoardValue( aX, aY );
	TInt line1 = 1;
	TInt line2 = 1;
	TInt line3 = 1;
	TInt line4 = 1;

	TInt i = 0;
	TInt j = 0;

	for (i = aX + 1; i < 8; i ++) 
        {
		if ( aContext->BoardValue( i, aY ) == player )
			line1 ++;
		else
			break;
	    }

	for (i = aX - 1; i >= 0; i --) 
        {
		if ( aContext->BoardValue( i, aY ) == player )
			line1 ++;
		else
			break;
	    }

	if (line1 >= 4)
		return player;

	for (i = aY + 1; i < 8; i ++) 
        {
		if ( aContext->BoardValue( aX, i ) == player )
			line2 ++;
		else
			break;
	    }

	for (i = aY - 1; i >= 0; i --)
        {
		if ( aContext->BoardValue( aX, i ) == player )
			line2 ++;
		else
			break;
	    }

	if (line2 >= 4)
		return player;


	j = aY;
	for (i = aX + 1; i < 8; i ++) 
        {
		j = j + 1;
		if (j > 7)
			break;

		if ( aContext->BoardValue( i, j ) == player )
			line3 ++;
		else
			break;
	    }

	j = aY;
	for (i = aX - 1; i >= 0; i --) 
        {
		j = j - 1;
		if (j < 0)
			break;

		if ( aContext->BoardValue( i, j ) == player )
			line3 ++;
		else
			break;
	    }

	if (line3 >= 4)
		return player;

	j = aY;
	for (i = aX + 1; i < 8; i ++) 
        {
		j = j - 1;
		if (j < 0)
			break;

		if ( aContext->BoardValue( i, j ) == player )
			line4 ++;
		else
			break;
	    }

	j = aY;
	for (i = aX - 1; i >= 0; i --) 
        {
		j = j + 1;
		if (j > 7)
			break;

		if ( aContext->BoardValue( i, j ) == player )
			line4 ++;
		else
			break;
	    }

	if (line4 >= 4)
		return player;

	return ret;
    }

// -----------------------------------------------------------------------------
// TSIPExState::SessionEnded
// The remote peer has ended the session. 
// The state is changed to Idle and UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::SessionEnded( CSIPExEngine* aContext )
    {
    ChangeState( aContext, aContext->iStateRegistered );
    Info( aContext, KSessionEnded() );
    StatusInfo( aContext, KGameOver() );
    TRAPD( ignore, aContext->SocketEngineL()->Stop() );
    aContext->DestroySocketEngine();
    }

// -----------------------------------------------------------------------------
// TSIPExState::ConnectionLost
// The connection has lost. 
// The state is changed to Idle and UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::ConnectionLost( CSIPExEngine* aContext )
    {
    ChangeState( aContext, aContext->iStateIdle );
    Info( aContext, KConnectionLost() );
    StatusInfo( aContext, KGameOver() );
    }

// -----------------------------------------------------------------------------
// TSIPExState::ProfileError
// The error in profile operation
// The state is changed to Idle and UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::ProfileError( CSIPExEngine* aContext, TInt aError )
    {
    ChangeState( aContext, aContext->iStateIdle );
    TRAPD( ignore, CombineAndShowInfoL( aContext, KProfileError(), aError ) );
    StatusInfo( aContext, KGameOver() );
    }

// -----------------------------------------------------------------------------
// TSIPExState::IMReceived
// The error in profile operation
// The state is changed to Idle and UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::IMReceived( 
    CSIPExEngine* aContext, 
    const TDesC8& aFrom, 
    const TDesC8& aMessage )
    {
    TRAPD( ignore, aContext->GameObserver().IMReceivedL( aFrom, aMessage ) );
    }

// -----------------------------------------------------------------------------
// TSIPExState::EngineError
// The error in SIP Engine
// The state is changed to Idle and UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::EngineError( CSIPExEngine* aContext, TInt aError )
    {
    ChangeState( aContext, aContext->iStateIdle );
    TRAPD( ignore, CombineAndShowInfoL( aContext, KErrorInSIPEngine(), aError ) );
    StatusInfo( aContext, KGameOver() );
    }

// -----------------------------------------------------------------------------
// TSIPExState::InviteReceivedByRemote
// Callback from SIP engine observer.
// The UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::InviteReceivedByRemote( CSIPExEngine* aContext, 
                                          const TInt aResponse )
    {
    TRAPD( ignore, CombineAndShowInfoL( aContext, KProvResponse(), aResponse ) );
    }

// -----------------------------------------------------------------------------
// TSIPExState::InviteAcceptedByUs
// Callback from SIP engine observer.
// The UI is notified.
// -----------------------------------------------------------------------------
//
void TSIPExState::InviteAcceptedByUs( CSIPExEngine* aContext ) 
    { 
    Info( aContext, KAckReceived() ); 
    }

// End of file
