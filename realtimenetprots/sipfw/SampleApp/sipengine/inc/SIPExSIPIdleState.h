
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



#ifndef _SIPEXSIPIDLESTATE_H_
#define _SIPEXSIPIDLESTATE_H_


//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;


// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Idle" state.
*/
class CSIPExSIPIdleState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPIdleState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPIdleState();

	public://new functions
		/**
		* Links the states
		* @param aClientEstablishingState Invitation with media parameters is sent to peer.
		* @param aServerOfferingState Invitation with media parameters is received from peer.
		*/
		void LinkStates(
			CSIPExSIPStateBase& aClientEstablishingState,
			CSIPExSIPStateBase& aServerOfferingState );

	public://functions from base class
	
		/**
		* SendInviteL
		* Sends an INVITE to peer.
		* @param aEngine Reference to SIP Engine.
		* @param aSipUri uri of recipient.
		*/
		void SendInviteL( CSIPExSIPEngine& aEngine,
						  const TDesC8& aSipUri );
			
		/**
		* InviteReceivedL
		* INVITE has been received.
		* @param aEngine Reference to SIP Engine.
		* @param aTransaction Contains response elements.
		* The ownership is transferred.
		*/
		void InviteReceivedL(
			CSIPExSIPEngine& aEngine,
			CSIPServerTransaction* aTransaction );

	private:
	    /**
		* C++ default constructor.
		*/
		CSIPExSIPIdleState();

	private:	//data members
		CSIPExSIPStateBase* iClientEstablishingState;
		CSIPExSIPStateBase* iServerOfferingState;
	};

#endif // _SIPEXSIPINITSTATE_H_

