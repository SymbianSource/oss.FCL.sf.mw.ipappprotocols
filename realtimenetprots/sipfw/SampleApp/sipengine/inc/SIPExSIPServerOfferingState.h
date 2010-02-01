
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



#ifndef _SIPEXSIPSERVEROFFERINGSTATE_H_
#define _SIPEXSIPSERVEROFFERINGSTATE_H_

//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "ClientOffering" state.
*/
class CSIPExSIPServerOfferingState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPServerOfferingState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPServerOfferingState();

	public://new functions
		/**
		* Links the states
		* @param aServerEstablishingState INVITE has been received and accepted, waiting for ACK.
		* @param aTerminatedState Peer has declined the invite, SIP Session is terminated.
		*/
		void LinkStates(
			CSIPExSIPStateBase& aServerEstablishingState,
			CSIPExSIPStateBase& aTerminatedState );

	public://functions from base class


        /**
		* ByeReceivedL
		* BYE has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void ByeReceivedL( CSIPExSIPEngine& aEngine,
						   CSIPServerTransaction& aTransaction );
						   
		/**
		* CancelReceivedL
		* CANCEL has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void CancelReceivedL( CSIPExSIPEngine& aEngine,
							  CSIPClientTransaction& aTransaction );
							  
		/**
		* AcceptInviteL
		* Send 200 (OK) as response to an INVITE
		* received from peer.
		* @param aEngine Reference to Engine object.
		*/
		void AcceptInviteL( CSIPExSIPEngine& aEngine );

		/**
		* DeclineInviteL
		* Send 486 (Busy Here) as response
		* to an INVITE received from peer.
		* @param aEngine Reference to Engine object.
		*/
		void DeclineInviteL( CSIPExSIPEngine& aEngine );


	private:
	    /**
		* C++ default constructor.
		*/
		CSIPExSIPServerOfferingState();

	private://data members
		CSIPExSIPStateBase* iServerEstablishingState;
		CSIPExSIPStateBase* iTerminatedState;
	};

#endif // _SIPEXSIPSERVEROFFERINGSTATE_H_

