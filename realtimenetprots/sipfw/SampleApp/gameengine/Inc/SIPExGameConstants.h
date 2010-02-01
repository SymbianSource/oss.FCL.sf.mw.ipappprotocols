
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



#ifndef SIPEXGAMECONSTANTS_H
#define SIPEXGAMECONSTANTS_H

// INCLUDES
#include    <e32base.h>

const TInt KBoxCountX( 8 );
const TInt KBoxCountY( 8 );
const TInt KMaxMoves( KBoxCountX * KBoxCountY );
const TInt KMaxSIPAddressLength( 256 );

_LIT( KGameAlreadyRunning, "Game already running..." );
_LIT( KGameStarting, "Starting" );
_LIT( KGameOver, "Game over" );
_LIT( KGameEnded, "Connection closed and session ended." );
_LIT( KYouWin, "You WIN!" );
_LIT( KYouLose, "You LOSE!" );
_LIT( KGameTie, "Game tie!" );
_LIT( KWaitingRemote, "Waiting remote" );
_LIT( KNotRegistered, "Not registered..." );
_LIT( KNotConnected, "Not connected" );
_LIT( KProfileEnabled, "Profile enabled!" );
_LIT( KProfileDisabled, "Profile disabled!" );
_LIT( KProfEnabError, "Error in profile enabling!" );
_LIT( KConnected, "Connected" );
_LIT( KTimedOut, "Timed out!" );
_LIT( KYourMove, "Your move" );
_LIT( KEnablingProfile, "Enabling profile..." );
_LIT( KSessionEnded, "Session ended!" );
_LIT( KInvitationCancelled, "Invitation cancelled" );
_LIT( KConnecting, "Connecting..." );
_LIT( KInviting, "Inviting..." );
_LIT( KWaitingRemoteConn, "Waiting remote connection..." );
_LIT( KInviteDeclined, "Invite declined!" );
_LIT( KInviteDeclinedByRemote, "Invite declined by remote: " );
_LIT( KConnectionLost, "Connection lost!" );
_LIT( KProfileError, "Error in profile operation: " );
_LIT( KConnectionDropped, "Connection dropped!" );
_LIT( KErrorInSIPEngine, "Error in SIP Engine: " );
_LIT( KProvResponse, "Response from SIP engine: " );
_LIT( KListening, "Listening..." );
_LIT( KAccepting, "Sending 200 OK..." );
_LIT( KResolving, "Resolving local address..." );
_LIT( KAckReceived, "Ack received" );
_LIT( K200Received, "200 OK received" );
_LIT( KSendingAck, "Sending Ack" );


_LIT8( KMoveFormatStr, "%d, %d" );

#endif // SIPEXGAMECONSTANTS_H

// End of file
