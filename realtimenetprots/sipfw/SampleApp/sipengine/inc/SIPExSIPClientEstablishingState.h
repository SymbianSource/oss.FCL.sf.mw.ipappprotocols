
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



#ifndef _SIPEXSIPCLIENTESTABLISHINGSTATE_H_
#define _SIPEXSIPCLIENTESTABLISHINGSTATE_H_


//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;


// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "ClientEstablishing" state.
*/
class CSIPExSIPClientEstablishingState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPClientEstablishingState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPClientEstablishingState();

	public://new functions
		/**
		* Links the states
		* @param aIdleState The "waiting for something to happen" state.
		* @param aClientOfferingState Provisional response to sent invitiation is received from peer.
		*/
		void LinkStates(
			CSIPExSIPStateBase& aIdleState,
			CSIPExSIPStateBase& aClientOfferingState,
			CSIPExSIPStateBase& aEstablishedState );

	public://functions from base class

		/**
		* CancelInviteL
		* Cancel an INVITE sent previously.
		* @param aEngine Reference to Engine object.
		*/
		void CancelInviteL( CSIPExSIPEngine& aEngine );


		/**
		* ResponseReceivedL
		* A SIP response has been received from the network.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void ResponseReceivedL( CSIPExSIPEngine& aEngine,
								CSIPClientTransaction& aTransaction );


	private:
	    /**
		* C++ default constructor.
		*/
		CSIPExSIPClientEstablishingState();

	private://data members
		CSIPExSIPStateBase* iIdleState;
		CSIPExSIPStateBase* iClientOfferingState;
		CSIPExSIPStateBase* iEstablishedState;
	};

#endif // _SIPEXSIPCLIENTESTABLISHINGSTATE_H_

