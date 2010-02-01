
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



#ifndef MSIPEXGAMEOBSERVER_H
#define MSIPEXGAMEOBSERVER_H

// INCLUDES
#include    "SIPExGameEngine.h"

// CLASS DECLARATIONS
/**
* Class for observing the game events.
*/
class MSIPExGameObserver 
	{
    public:
        /**
		* GameStateChanged
        * Called when the active state of the game is changed.
        * @param aNewState A new state of the game engine.
		*/
        virtual void GameStateChanged( CSIPExEngine::TEngineState aNewState ) = 0;

        /**
		* AcceptInvitationL
        * Called when the invitation has arrived from SIP engine.
        * @param aFrom The address of the summoner.
        * @return ETrue if the user accepts the invitation, else EFalse.
		*/
        virtual TBool AcceptInvitationL( const TDesC& aFrom ) = 0;

        /**
		* IMReceived
        * Called when the instant message has arrived from the SIP engine.
        * @param aFrom The address of the sender.
        * @param aMsg The message.
		*/
        virtual void  IMReceivedL( const TDesC8& aFrom, const TDesC8& aMsg ) = 0;
	};

#endif // MSIPEXGAMEOBSERVER_H

// End of file

