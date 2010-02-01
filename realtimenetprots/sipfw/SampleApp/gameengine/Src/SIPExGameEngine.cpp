
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
#include    "SIPExGameEngine.h"
#include    "SIPExStateViewNotifier.h"
#include    "SIPExGameObserver.h"
#include    "SIPExStateAcceptingSIP.h"
#include    "SIPExStateConnecting.h"
#include    "SIPExStateIdle.h"
#include    "SIPExStateInviting.h"
#include    "SIPExStateLocal.h"
#include    "SIPExStateRemote.h"
#include    "SIPExStateRegistering.h"
#include    "SIPExStateRegistered.h"
#include    "SIPExSocketEngine.h"
#include    "SIPExSIPEngine.h"
#include    "SIPExEngine.pan"
#include    <in_sock.h> // TInetAddr

// Remove exports in unit test build
#ifdef CPPUNIT_TEST
#undef EXPORT_C
#define EXPORT_C
#endif

// CONSTANTS
const TUid KUidSIPExApp = { 0xA00001EB };

// -----------------------------------------------------------------------------
// CSIPExEngine::NewL
// Static constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExEngine* CSIPExEngine::NewL( MSIPExGameObserver& aGameObserver )
    {
    CSIPExEngine* self = NewLC( aGameObserver );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::NewLC
// Static constructor. The instance is left to the CleanupStack.
// -----------------------------------------------------------------------------
//    
EXPORT_C CSIPExEngine* CSIPExEngine::NewLC( MSIPExGameObserver& aGameObserver )
    {
    CSIPExEngine* self = new (ELeave) CSIPExEngine( aGameObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::CSIPExEngine
// C++ default constructor. Initializes the observer member variable.
// -----------------------------------------------------------------------------
//
CSIPExEngine::CSIPExEngine( MSIPExGameObserver& aGameObserver )
: iGameObserver( aGameObserver )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::~CSIPExEngine
// Destructor. Destroy all members.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExEngine::~CSIPExEngine()
    {
    DestroySocketEngine();
    delete iSIPEngine;

    // Delete all state objects
    delete iStateIdle;
    delete iStateRegistering;
    delete iStateRegistered;
    delete iStateInviting;
    delete iStateConnecting;
    delete iStateLocal;
    delete iStateRemote;
    delete iStateAcceptingSIP;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// Initializes the game data, socket and sip engines and all state objects.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::ConstructL()
    {
    // Init all state objects
    iStateIdle          = new (ELeave) TSIPExStateIdle;
    iStateRegistering   = new (ELeave) TSIPExStateRegistering;
    iStateRegistered    = new (ELeave) TSIPExStateRegistered;
    iStateInviting      = new (ELeave) TSIPExStateInviting;
    iStateConnecting    = new (ELeave) TSIPExStateConnecting;
    iStateLocal         = new (ELeave) TSIPExStateLocal;
    iStateRemote        = new (ELeave) TSIPExStateRemote;
    iStateAcceptingSIP  = new (ELeave) TSIPExStateAcceptingSIP;

    iSocketEngine = CSIPExSocketEngine::NewL( *this );
    iSIPEngine = CSIPExSIPEngine::NewL( KUidSIPExApp, this );

    // Set the first state as active state
    iActiveState = iStateIdle;

    // Reset game data
    ResetGame();
    }


// ----------------------------------------------------------------------------
// New functions.
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSIPExEngine::SetViewNotifier
// Sets a new reference to the iNotifier.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::SetViewNotifier( MSIPExStateViewNotifier& aNotifier )
    {
    iNotifier = &aNotifier;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::ResetGame
// Resets the game area, moves and cursor position.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::ResetGame()
    {
    for ( TInt i=0 ; i < KBoxCountX; i++ ) 
        {
		for ( TInt j=0 ; j < KBoxCountY; j++ ) 
            {
			iBoard[i][j] = -1;
            }
        }

	iCursor = 0;
    iMoves = 0;
    iPeer = EUnknown;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SetBoard
// Sets the value (aValue) to the board to the place specified in aX and aY.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SetBoard( const TInt aX, const TInt aY, const TInt aValue )
    {
    __ASSERT_DEBUG( aX < KBoxCountX && aY < KBoxCountY, 
                    Panic( EBoardValueOOR ) );
    iBoard[ aX ][ aY ] = aValue;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::IncreaseMovesBy
// Increases the iMoves by aAmount.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::IncreaseMovesBy( const TInt aAmount )
    {
    iMoves += aAmount;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::BoardValue
// Returns the board value from the specified place.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPExEngine::BoardValue( TInt aX, TInt aY )
    {
    __ASSERT_DEBUG( aX < KBoxCountX && aY < KBoxCountY, 
                    Panic( EBoardValueOOR ) );
    return iBoard[ aX ][ aY ];
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::Cursor
// Returns the iCursor value.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPExEngine::Cursor()
    {
    return iCursor;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SetCursor
// Sets a new value to the iCursor.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SetCursor( const TInt aNewValue )
    {
    iCursor = aNewValue;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::ChangeState
// Changes a new value to the iActiveState and notifies the game observer.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::ChangeState( TSIPExState& aNewState )
    {
    iActiveState = &aNewState;

    TEngineState state( EIdle );
    if( iActiveState == iStateIdle || iActiveState == iStateRegistering )
        {
        state = EIdle;
        }
    else if( iActiveState == iStateRegistered )
        {
        state = EEnabled;
        }
    else if ( iActiveState == iStateInviting ||
              iActiveState == iStateAcceptingSIP ||
              iActiveState == iStateConnecting )
        {
        state = EActivating;
        }
    else if( iActiveState == iStateLocal || iActiveState == iStateRemote )      
        {
        state = EActive;
        }
    else { /*Nothing to do*/ }

    iGameObserver.GameStateChanged( state );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SendMessage
// Formats and sends the message to the socket.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SendMessage( const TInt aX, const TInt aY )
    {
    __ASSERT_DEBUG( aX < KBoxCountX && aY < KBoxCountY, 
                    Panic( EBoardValueOOR ) );

    TBuf8<8> msg;
    msg.Format( KMoveFormatStr(), aX, aY );

    iSocketEngine->Write( msg );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::Info
// Calls the view notifier.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InfoL( const TDesC& aInfoTxt )
    {
    __ASSERT_DEBUG( iNotifier, Panic( ENoGameViewNotifier ) );

    iNotifier->ShowInfoL( aInfoTxt );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::StatusInfo
// Calls the view notifier.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::StatusInfoL( const TDesC& aTxt )
    {
    __ASSERT_DEBUG( iNotifier, Panic( ENoGameViewNotifier ) );

    iNotifier->ShowStatusInfoL( aTxt );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SetRemote
// Sets the remote peer move to the board into the specified place.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SetRemote( const TInt aX, const TInt aY )
    {
    SetBoard( aX, aY, 2 );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::DestroySocketEngine
// Deletes the socket engine.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::DestroySocketEngine()
    {
    delete iSocketEngine;
    iSocketEngine = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SIPEngine
// Returns the pointer to the SIP engine. The ownership is not transfered.
// -----------------------------------------------------------------------------
//
CSIPExSIPEngine* CSIPExEngine::SIPEngine() 
    { 
    return iSIPEngine; 
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SocketEngine
// Returns the pointer to the socket engine. The ownership is not transfered.
// The engine will be created if not already done.
// -----------------------------------------------------------------------------
//
CSIPExSocketEngine* CSIPExEngine::SocketEngineL() 
    { 
    if( !iSocketEngine )
        {
        iSocketEngine = CSIPExSocketEngine::NewL( *this );
        }
    return iSocketEngine;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::GameObserver
// Returns the reference to the game observer.
// -----------------------------------------------------------------------------
//
MSIPExGameObserver& CSIPExEngine::GameObserver()
    { 
    return iGameObserver; 
    }

// ----------------------------------------------------------------------------
// From Game engine
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSIPExEngine::InviteL
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::InviteL( const TDesC& aAddress )
    {
    iActiveState->InviteL( this, aAddress );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::EnableProfile
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::EnableProfileL()
    {
    iActiveState->EnableProfileL( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::DisableProfile
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::DisableProfileL()
    {
    iActiveState->DisableProfileL( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SendInstantMsgL
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::SendInstantMsgL( 
    const TDesC& aAddress, 
    const TDesC& aMsg )
    {
    iActiveState->SendInstantMsgL( this, aAddress, aMsg );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::EndGameL
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::EndGameL()
    {
    iActiveState->EndGameL( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::DrawCursor
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPExEngine::DrawCursor()
    {
    return iActiveState->DrawCursor();
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::DrawBoard
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPExEngine::DrawBoard()
    {
    return iActiveState->DrawBoard();
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::CursorLeft
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::CursorLeft() 
    {
    iActiveState->CursorLeft( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::CursorRight
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::CursorRight() 
    {
    iActiveState->CursorRight( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::CursorPressed
// Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::CursorPressed() 
    {
    iActiveState->CursorPressed( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::MoveCursorL
// Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExEngine::MoveCursorL( const TInt aNewValue )
    {
    iActiveState->MoveCursorL( this, aNewValue );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::CalculatePos
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
TInt CSIPExEngine::CalculatePos() 
    {
    return iActiveState->CalculatePos( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::IsWin
// From MSIPExGameEngine. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
TInt CSIPExEngine::IsWin( const TInt aX, const TInt aY )
    {
    return iActiveState->IsWin( this, aX, aY );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::Moves
// Getter function for the iMoves.
// -----------------------------------------------------------------------------
//
TInt CSIPExEngine::Moves()
    {
    return iMoves;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::Peer
// Getter function for the iPeer.
// -----------------------------------------------------------------------------
//
CSIPExEngine::TPeer CSIPExEngine::Peer()
    {
    return iPeer;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SetPeer
// Sets a new value to the iPeer.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SetPeer( TPeer aPeer )
    {
    iPeer = aPeer;
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::AcceptInvitationL
// Asks from game observer whether user accept the invitation or not.
// -----------------------------------------------------------------------------
//
TBool CSIPExEngine::AcceptInvitationL( const TDesC8& aFrom )
    {
    HBufC* from = HBufC::NewLC( aFrom.Length() );
    from->Des().Copy( aFrom );
    TBool retVal = iGameObserver.AcceptInvitationL( *from );
    CleanupStack::PopAndDestroy( from );
    return retVal;
    }

// ----------------------------------------------------------------------------
// From Socket Observer
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSIPExEngine::SocketState
// From MSIPExSocketEngineObserver. Redirects the call to the active state object.
// Called when the socket engine's state changes.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SocketState( TInt aNewState )
    {
    iActiveState->SocketState( this, aNewState );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SocketData
// From MSIPExSocketEngineObserver. Redirects the call to the active state object.
// Called when data has arrived from the socket.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SocketData( TDesC8& aData ) 
    {
    iActiveState->SocketData( this, aData );
    }

// ----------------------------------------------------------------------------
// From SIP Observer
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CSIPExEngine::InviteReceived
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InviteReceived( const TDesC8& aFrom, const TUint32 aIapId )
    {
    iActiveState->InviteReceived( this, aFrom, aIapId );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::InviteAcceptedByRemote
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InviteAcceptedByRemote( 
    const TInetAddr& aIPAddress, 
    const TUint32 aIapId )
    {
    iActiveState->InviteAcceptedByRemote( this, aIPAddress, aIapId );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::InviteAcceptedByUs
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InviteAcceptedByUs()
    {
    iActiveState->InviteAcceptedByUs( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::InviteDeclinedByRemote
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InviteDeclinedByRemote( const TInt aResponse )
    {
    iActiveState->InviteDeclinedByRemote( this, aResponse );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::InviteReceivedByRemote
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InviteReceivedByRemote( const TInt aResponse )
    {
    iActiveState->InviteReceivedByRemote( this, aResponse );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::EngineError
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::EngineError( TInt aError )
    {
    iActiveState->EngineError( this, aError );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::InvitationCancelled
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::InvitationCancelled()
    {
    iActiveState->InvitationCancelled( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::CancelFailed
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::CancelFailed()
    {
    iActiveState->CancelFailed( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::SessionEnded
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::SessionEnded()
    {
    iActiveState->SessionEnded( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::ConnectionLost
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::ConnectionLost()
    {
    iActiveState->ConnectionLost( this );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::ProfileEnabled
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::ProfileEnabled( TUint32 aSIPProfileId )
    {
    iActiveState->ProfileEnabled( this, aSIPProfileId );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::ProfileError
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::ProfileError( TInt aError )
    {
    iActiveState->ProfileError( this, aError );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::IMReceived
// From MSIPExSIPEngineObserver. Redirects the call to the active state object.
// -----------------------------------------------------------------------------
//
void CSIPExEngine::IMReceived( const TDesC8& aFrom,
				 const TDesC8& aMessage )
    {
    iActiveState->IMReceived( this, aFrom, aMessage );
    }

// -----------------------------------------------------------------------------
// CSIPExEngine::WriteLog
// From MSIPExSIPEngineObserver. Logs SIP engine events
// -----------------------------------------------------------------------------
//
void CSIPExEngine::WriteLog( const TDesC8& aLog )
    {
    if( iNotifier )
        {
        TRAPD( ignore, iNotifier->ShowInfoL( aLog ) );
        }
    }


// End of file
