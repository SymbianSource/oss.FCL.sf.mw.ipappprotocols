
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



#ifndef _SIPEXSIPENGINEOBSERVER_H_
#define _SIPEXSIPENGINEOBSERVER_H_

// INCLUDES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The observer class for monitoring the SIP Engine.
*  Class must be implemented by the user in order to 
*  observe changes in the SIP Engine.
*/

class MSIPExSIPEngineObserver 
	{

    public: // New functions
    
		// Session control
		//
		// INVITE
		
		/**
		* InviteReceived
		* Called when an INVITE is received by the SIP Client.
		* @param aFrom The sender of the INVITE
		* @param aIapId The access point id of used conection
		*/
    	virtual void InviteReceived( const TDesC8& aFrom, const TUint32 aIapId ) = 0;

    	/**
    	* InviteReceivedByRemote
    	* Called when a provisional response (1xx, not 100 (Trying))
    	* is received after sending INVITE.
    	*/
    	virtual void InviteReceivedByRemote( const TInt aResponse ) = 0;

    	/**
    	* InviteAcceptedByRemote
    	* Called when the response 200 (OK) is received
    	* after sending INVITE.
    	* @param aIPAddr The IP Address of the remote party.
    	* @param aIapId iap id used.
    	*/
    	virtual void InviteAcceptedByRemote( const TInetAddr& aIPAddr, const TUint32 aIapId ) = 0;
    	
    	/**
    	* InviteDeclinedByRemote
    	* Called when a 4xx, 5xx, or 6xx response is received
    	* after sending INVITE.
    	* As the session isn't established, the engine should be
    	* deleted.
    	* @param aResponse Value of the received response.
    	*/
    	virtual void InviteDeclinedByRemote( const TInt aResponse ) = 0;
    	
    	/**
    	* InviteAcceptedByUs
    	* Called when the response ACK is received after
    	* responding with 200 (OK) to an invitation.
    	*/
    	virtual void InviteAcceptedByUs() = 0;


		// CANCEL
    	
    	/**
    	* InvitationCancelled
    	* Called when an INVITE has been successfully cancelled.
    	*/
    	virtual void InvitationCancelled() = 0;
    	
    	/**
    	* CancelFailed
    	* Called when the cancellation of an INVITE fails.
    	*/
    	virtual void CancelFailed() = 0;


		// BYE
    	
    	/**
    	* SessionEnded
    	* Called when the session has been closed, ie. when the
    	* the response 200 (OK) is received after sending BYE,
    	* or after 200 (OK) has been sent after receiving BYE.
    	* Or we have declined a received INVITE.
    	* The observer should delete the engine.
    	*/
    	virtual void SessionEnded() = 0;
    	
    	
    	// Connection lost
    	
    	/**
    	* ConnectionLost
    	* Called when the connection is lost because of an
    	* error. The observer should delete the engine.
    	*/
    	virtual void ConnectionLost() = 0;
    	
    	
    	// SIP Engine Error
    	
    	/**
    	* EngineError
    	* Called when an error occurs in the SIP Example
    	* SIP Engine.
    	* @param aError Error number
    	*/
    	virtual void EngineError( TInt aError ) = 0;



		// Profile control

		/**
		* ProfileEnabled
		* Called when the EnableProfile method has
		* completed successfully.
		* @param aSIPProfileId ID of the enabled profile
		*/
		virtual void ProfileEnabled( TUint32 aSIPProfileId ) = 0;

		/**
		* ProfileError
		* An error has occurred during a profile operation
		* @param aError Error number
		*/
		virtual void ProfileError( TInt aError ) = 0;
		
		// Instant Message control
		
		/**
		* IMReceived
		* Called when an Instant Message is received.
		* @param aFrom The sender of the IM.
		* @param aMessage The message received.
		*/
		virtual void IMReceived( const TDesC8& aFrom,
								 const TDesC8& aMessage ) = 0;
		/**
		* WriteLog
		* @param aLog, log entry.
		*/
		virtual void WriteLog( const TDesC8& aLog ) = 0;
	};

#endif	// _SIPEXSIPENGINEOBSERVER_H_
