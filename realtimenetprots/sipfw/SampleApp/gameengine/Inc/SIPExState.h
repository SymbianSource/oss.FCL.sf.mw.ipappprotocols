
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



#ifndef __TSIPEXSTATE_H__
#define __TSIPEXSTATE_H__

// INCLUDES
#include    "SIPExSocketEngine.h"
#include    "SIPExGameEngine.h"
#include    <e32base.h>
#include    <in_sock.h>

// FORWARD DECLARATIONS

// CLASS DECLARATIONS
/**
* The abstract base class for all states.
*/
class TSIPExState 
    {
    public: // New functions

        /**
        * ChangeState
        * Changing the active state.
        * @param aContext The pointer to the game engine.
        * @param aNewState The new active state.
        */
        void ChangeState( CSIPExEngine* aContext, TSIPExState* aNewState );

        /**
        * Info
        * Non leaving function for setting info text.
        * @param aContext The pointer to the game engine.
        * @param aText A text to set to the info
        */
        void Info( CSIPExEngine* aContext, const TDesC& aText );

        /**
        * StatusInfo
        * Non leaving function for setting status info text.
        * @param aContext The pointer to the game engine.
        * @param aText A text to set to the info
        */
        void StatusInfo( CSIPExEngine* aContext, const TDesC& aText );

        /**
        * CombineAndShowInfoL
        * Combines a descriptor and a number value as one descriptor.
        * The result descriptor is set to info view.
        * The max length of the new desc is 64.
        * @param aContext The pointer to the game engine.
        * @param aText A text part.
        * @param aNum A number part.
        */
        void CombineAndShowInfoL( CSIPExEngine* aContext, const TDesC& aText, TInt aNum );

    public:
        // ----------------------------------------------------------------------------
        // From SIP Observer
        // ----------------------------------------------------------------------------

        /**
        * InviteReceived
        * From MSIPExSIPEngineObserver.
        */
        virtual void InviteReceived( CSIPExEngine* aContext, 
                const TDesC8& aFrom, const TUint32 aIapId );
            
        /**
        * InviteAcceptedByRemote
        * From MSIPExSIPEngineObserver.
        */
        virtual void InviteAcceptedByRemote( CSIPExEngine* aContext, const TInetAddr& aIPAddress, const TUint32 aIapId );
            
        /**
        * InviteDeclinedByRemote
        * From MSIPExSIPEngineObserver.
        */
        virtual void InviteDeclinedByRemote( CSIPExEngine* aContext, const TInt aResponse );

        /**
        * InviteReceivedByRemote
        * From MSIPExSIPEngineObserver.
        */
        virtual void InviteReceivedByRemote( CSIPExEngine* aContext, const TInt aResponse );

        /**
        * InviteAcceptedByUs
        * From MSIPExSIPEngineObserver.
        */
        virtual void InviteAcceptedByUs( CSIPExEngine* aContext );
            
        /**
        * InvitationCancelled
        * From MSIPExSIPEngineObserver.
        */
        virtual void InvitationCancelled( CSIPExEngine* aContext);
            
        /**
        * CancelFailed
        * From MSIPExSIPEngineObserver.
        */
        virtual void CancelFailed( CSIPExEngine* aContext );

        /**
        * ProfileEnabled
        * From MSIPExSIPEngineObserver.
        */
        virtual void ProfileEnabled( CSIPExEngine* aContext, TUint32 aSIPProfileId );

        /**
        * IMReceived
        * From MSIPExSIPEngineObserver.
        */
        virtual void IMReceived( CSIPExEngine* aContext, const TDesC8& aFrom,
            const TDesC8& aMessage );

        /**
        * SessionEnded
        * From MSIPExSIPEngineObserver.
        */
        virtual void SessionEnded( CSIPExEngine* aContext );
            
        /**
        * ConnectionLost
        * From MSIPExSIPEngineObserver.
        */
        virtual void ConnectionLost( CSIPExEngine* aContext );
            
        /**
        * ProfileError
        * From MSIPExSIPEngineObserver.
        */
        virtual void ProfileError( CSIPExEngine* aContext, TInt aError );

        /**
        * EngineError
        * From MSIPExSIPEngineObserver.
        */
        void EngineError( CSIPExEngine* aContext, TInt aError );
            
        // ----------------------------------------------------------------------------
        // From Socket engine
        // ----------------------------------------------------------------------------

        /**
        * SocketState
        * From MSIPExSocketEngineObserver.
        */
        virtual void SocketState( CSIPExEngine* aContext, TInt aNewState );

        /**
        * SocketData
        * From MSIPExSocketEngineObserver.
        */
        virtual void SocketData( CSIPExEngine* aContext, TDesC8& aData );
            
        // ----------------------------------------------------------------------------
        // From Game Engine
        // ----------------------------------------------------------------------------
        
        /**
        * InviteL
        * Called when user invites the remote user to the game.
        */
        virtual void InviteL( CSIPExEngine* aContext, const TDesC& aAddress );

        /**
        * DrawCursor
        * Tells whether we should draw the cursor or not.
        */
        virtual TBool DrawCursor();

        /**
        * DrawBoard
        * Tells whether we should draw the board or not.
        */
        virtual TBool DrawBoard();

        /**
        * CalculatePos
        * Calculates the next free place in cursor's column.
        */
        virtual TInt CalculatePos( CSIPExEngine* aContext );

        /**
        * IsWin
        * Checks the if the move is winning move.
        */
        virtual TInt IsWin( CSIPExEngine* aContext, const TInt aX, const TInt aY );

        /**
        * EnableProfile
        * Virtual function.
        */
        virtual void EnableProfileL( CSIPExEngine* aContext );

        /**
        * DisableProfile
        * Virtual function.
        */
        virtual void DisableProfileL( CSIPExEngine* aContext );

        /**
        * CursorPressed
        * Virtual function.
        */
        virtual void CursorPressed( CSIPExEngine* aContext );
            
        /**
        * CursorRight
        * Virtual function.
        */
        virtual void CursorRight( CSIPExEngine* aContext );

        /**
        * CursorLeft
        * Virtual function.
        */
        virtual void CursorLeft(  CSIPExEngine* aContext );

        /**
        * MoveCursorL
        * Virtual function
        */
        virtual void MoveCursorL( CSIPExEngine* aContext, const TInt aNewPosition );

        /**
        * SendInstantMsgL
        * Virtual function.
        */
        virtual void SendInstantMsgL( CSIPExEngine* aContext, 
            const TDesC& aAddress, const TDesC& aMsg );

        /**
        * EndGameL
        * Virtual function.
        */
        virtual void EndGameL( CSIPExEngine* aContext );
    };

#endif // __TSIPEXSTATE_H__

// End of file
