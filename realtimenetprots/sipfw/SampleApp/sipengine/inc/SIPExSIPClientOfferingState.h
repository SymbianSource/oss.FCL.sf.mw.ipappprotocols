
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



#ifndef _SIPEXSIPCLIENTOFFERINGSTATE_H_
#define _SIPEXSIPCLIENTOFFERINGSTATE_H_


//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;


// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "ClientOffering" state.
*/
class CSIPExSIPClientOfferingState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPClientOfferingState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPClientOfferingState();

	public://new functions
		/**
		* Links the states
		* @param aIdleState The "waiting for something to happen" state.
		* @param aEstablishedState SIP Session is established.
		* @param aTerminatedState Peer has declined the invite, SIP Session is terminated.
		*/
		void LinkStates(
			CSIPExSIPStateBase& aIdleState,
			CSIPExSIPStateBase& aEstablishedState,
			CSIPExSIPStateBase& aTerminatedState );

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
		CSIPExSIPClientOfferingState();

	private://data members
		CSIPExSIPStateBase* iIdleState;
		CSIPExSIPStateBase* iEstablishedState;
		CSIPExSIPStateBase* iTerminatedState;
	};

#endif // _SIPEXSIPCLIENTOFFERINGSTATE_H_

