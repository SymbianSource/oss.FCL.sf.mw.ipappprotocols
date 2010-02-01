
/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __CSIPEXENGINE_H__
#define __CSIPEXENGINE_H__

// INCLUDES
#include    "SIPExGameConstants.h"
#include    "SIPExSocketEngineObserver.h"
#include    "SIPExSIPEngineObserver.h"


// Remove imports in unit test build
#ifdef CPPUNIT_TEST
#undef IMPORT_C
#define IMPORT_C
#endif

// DATA TYPES

// FORWARD DECLARATIONS
class MSIPExStateViewNotifier;
class TSIPExState;
class MSIPExGameObserver;
class CSIPExSIPEngine;
class CSIPExSocketEngine;
class TInetAddr;

// CLASS DECLARATIONS
/**
* The engine class for SIP Example application.
* the networking and SIP messaging.
*/
class CSIPExEngine 
:   public CBase, 
    public MSIPExSIPEngineObserver,
    public MSIPExSocketEngineObserver
    {
    public:

		/**
		* Create new instance of GameEngine.
		* @param aGameObserver Reference to observing class.
		* @returns Pointer to new CSIPExEngine instance.
		*/
        IMPORT_C static CSIPExEngine* NewL( MSIPExGameObserver& aGameObserver );

		/**
		* Create new instance of GameEngine.
        * The GameEngine pointer is left to the CleanupStack 
        * returned.
		* @param aGameObserver Reference to observing class.
		* @returns Pointer to new CSIPExEngine instance.
		*/
        IMPORT_C static CSIPExEngine* NewLC( MSIPExGameObserver& aGameObserver );

		/**
		* Destructor.
		*/
        IMPORT_C ~CSIPExEngine();

    public: // public data

		/**
		* Enumerations for the application role.
        * The inviting peer acts as a client and invited peer as a server.
		*/        
        enum TPeer 
            {
            EUnknown,
            EClient,
            EServer
            };

        /**
        * Game engine states for UI.
        */
        enum TEngineState
            {
            EIdle,
            EEnabled,
            EActivating,
            EActive
            };

    public: // New functions

        /**
        * The instant message is send to the remote user.
		* Redirects the call to the active State object.
        * @param aAddress The address of the recipient.
        * @param aMsg The message.
		*/
        IMPORT_C void SendInstantMsgL( const TDesC& aAddress, const TDesC& aMsg );

        /**
		* Player invites the remote peer to the game.
        * Redirectes the call to the active State object.
        * @param aAddress The address of the invited peer.
		*/
        IMPORT_C void InviteL( const TDesC& aAddress );

        /**
		* The SIP profile is enabled.
        * Redirectes the call to the active State object.
		*/
        IMPORT_C void EnableProfileL();

        /**
		* The SIP profile is disabled.
        * Redirectes the call to the active State object.
		*/
        IMPORT_C void DisableProfileL();

        /**
		* The user ends the game.
        * Redirectes the call to the active State object.
		*/
        IMPORT_C void EndGameL();

        /**
        * Resolves whether we should draw cursor or not.
        * @return Returns ETrue if the game is in state where we 
        * should draw cursor (if it is our turn). Otherwise
        * EFalse is returned.
        */
        IMPORT_C TBool DrawCursor();

        /**
        * Resolves whether we should draw board or not.
        * @return Returns ETrue if the game is in state where we 
        * should draw board (if profile is enabled). Otherwise
        * EFalse is returned.
        */
        IMPORT_C TBool DrawBoard();

        /**
        * Updates the game state. User has pressed left key.
        */
        IMPORT_C void CursorLeft();

        /**
        * Updates the game state. User has pressed right key.
        */
        IMPORT_C void CursorRight();

        /**
        * Updates the game state. User has pressed enter key.
        */
        IMPORT_C void CursorPressed();

        /**
        * Updatas the game state. User moves the cursor with pointer.
        * @param aNewCursorPosition A new cursor column position.
        */
        IMPORT_C void MoveCursorL( const TInt aNewCursorPosition );

        /**
        * Returns value in specified place in the board.
        * @param aX The place's x coordinate.
        * @param aY The place's y coordinate.
        * @return The value in specified place.
        */
        IMPORT_C TInt BoardValue( TInt aX, TInt aY );

        /**
        * Returns the cursor position.
        * @return The cursor's position
        */
        IMPORT_C TInt Cursor();

        /**
        * Sets the iNotifier.
        * @param aNotifier Reference to the view notifier.
        */
        IMPORT_C void SetViewNotifier( MSIPExStateViewNotifier& aNotifier );

    public: // From socket observer

        /**
		* Callback from socket observer.
        * Called when the state changes in the socket engine.
        * @param aNewState A new state of the socket engine.
        */
        void SocketState( TInt aNewState );

        /**
		* Callback from socket observer.
        * Called when the data is received from the socket.
        * @param aData The data received from the socket.
        */
        void SocketData( TDesC8& aData );
        
    private: // From SIP observer. See SIPExObserver.h
        void InviteReceived( const TDesC8& aFrom, const TUint32 aIapId );
        void InviteReceivedByRemote( const TInt aResponse );
        void InviteDeclinedByRemote( const TInt aResponse );
        void InviteAcceptedByRemote( const TInetAddr& aIPAddress, const TUint32 aIapId );
        void InviteAcceptedByUs();
        void InvitationCancelled();
        void EngineError( TInt aError );
        void CancelFailed();
        void SessionEnded();
        void ConnectionLost();
        void ProfileEnabled( TUint32 aSIPProfileId );
        void ProfileError( TInt aError );
        void IMReceived( const TDesC8& aFrom,
						 const TDesC8& aMessage );
        void WriteLog( const TDesC8& aLog );

    private: // New functions

        /**
        * Initilizes the game state.
        */
        void ResetGame();

        /**
        * Destroys the iSocketEngine.
        */
        void DestroySocketEngine();

        /**
        * Changes the active state.
        * @param aNewState A reference the to new active state.
        */
        void ChangeState( TSIPExState& aNewState );

        /**
        * Send the move to the remote peer.
        * @param aX The x coordinate of the move.
        * @param aY The y coordinate of the move.
        */
        void SendMessage( const TInt aX, const TInt aY );

        /**
        * Shows the text in the status info area. The call is
        * redirected to the view notifier.
        * @param aTxt The text shown in the status info area.
        */
        void StatusInfoL( const TDesC& aTxt );

        /**
        * Shows the text in the info area. The call is redirected 
        * to the view notifier.
        * @param aTxt The text shown in the info area.
        */
        void InfoL( const TDesC& aInfoTxt );

        /**
        * Calculates the next free place in cursor's column.
        * @return The next free position on the board.
        */
        TInt CalculatePos();

        /**
        * Checks if the move is win move.
        * @param aX The x coordinate of the move.
        * @param aY The y coordinate of the move.
        * @return 1 if you won, 
        *         2 if the remote player has won,
        *         -1 if not win move.
        */
        TInt IsWin( const TInt aX, const TInt aY );

        /**
        * Returns the count of moves in this game.
        * @return The count of moves.
        */
        TInt Moves();

        /**
        * Sets the specified value in to the specified place in
        * the board.
        * @param aX The x coordinate value.
        * @param aY The y coordinate value.
        * @param aValue The value to be set to the (x, y) position.
        */
        void SetBoard( const TInt aX, const TInt aY, const TInt aValue );

        /**
        * Increases the moves value by specified amount.
        * @param aAmount The amount of the increased moves.
        */
        void IncreaseMovesBy( const TInt aAmount );

        /**
        * Set cursor the specified position.
        * @param aNewValue The new position value for the cursor.
        */
        void SetCursor( const TInt aNewValue );
        
        /**
        * Returns the peer value.
        * @return The iPeer value.
        */
        TPeer Peer();
        
        /**
        * Sets the iPeer's value
        * @param aPeer A new value for the iPeer.
        */
        void SetPeer( TPeer aPeer );
        
        /**
        * Sets remote peer's move to the board.
        * @param aX The x coordinate value.
        * @param aY The y coordinate value.
        */
        void SetRemote( const TInt aX, const TInt aY );

        /**
		* The acceptance is asked from the user
        * Redirectes the call to the game observer.
        * @param aFrom The summoner's address
        * @return Whether we accept the invitation or not.
		*/
        TBool AcceptInvitationL( const TDesC8& aFrom );

        /**
        * Returns the SIP Engine pointer.
        * @return The pointer to the CSIPExSIPEngine. The ownership
        *         is NOT transferred.
        */
        CSIPExSIPEngine*    SIPEngine();

        /**
        * Returns the socket engine. If the socket engine (iSocketEngine) is 
        * NULL it will be created.
        * @return The pointer to the CSIPExSocketEngine. The ownership
        *         is NOT transferred.
        */        
        CSIPExSocketEngine* SocketEngineL();

        /**
        * Returns the game observer reference.
        * @return The reference to the MSIPExGameObserver.
        */        
        MSIPExGameObserver& GameObserver();

    private:

        /**
        * Constructor
        * @param aGameObserver The observer reference.
        */        
        CSIPExEngine( MSIPExGameObserver& aGameObserver );

        /**
        * 2nd phase constructor
        */        
        void ConstructL();

    private: // Member variables
        // Owned: States
        TSIPExState*    iStateIdle;
        TSIPExState*    iStateRegistering;
        TSIPExState*    iStateRegistered;
        TSIPExState*    iStateInviting;
        TSIPExState*    iStateConnecting;
        TSIPExState*    iStateLocal;
        TSIPExState*    iStateRemote;
        TSIPExState*    iStateAcceptingSIP;
        
        // Not owned: ui notifier
        MSIPExStateViewNotifier*     iNotifier;

        // Owned: The networking engine
        CSIPExSocketEngine*     iSocketEngine;

        // Owned: The SIP signaling engine.
        CSIPExSIPEngine*        iSIPEngine;
        
        // Observer for game events
        MSIPExGameObserver&     iGameObserver;

        // Game data
        TInt iMoves;
	    TInt iBoard[ KBoxCountX ][ KBoxCountY ];
	    TInt iCursor;
        TPeer iPeer;

        // Not owned: Reference to active state
        TSIPExState*    iActiveState;

       // State classes are friend classes because they need access 
        // to the engine.
        
        friend class TSIPExState;
        friend class TSIPExStateAcceptingSIP;
        friend class TSIPExStateConnecting;
        friend class TSIPExStateIdle;
        friend class TSIPExStateInviting;
        friend class TSIPExStateLocal;
        friend class TSIPExStateRegistered;
        friend class TSIPExStateRegistering;
        friend class TSIPExStateRemote;
        
        // In unit tests following friend class definitions are needed
        #ifdef CPPUNIT_TEST
        friend class CGameEngineTest;
        friend class CBaseStateTest;
        friend class CStateAcceptingSIPTest;
        #endif
    };

#endif // __CSIPEXENGINE_H__

