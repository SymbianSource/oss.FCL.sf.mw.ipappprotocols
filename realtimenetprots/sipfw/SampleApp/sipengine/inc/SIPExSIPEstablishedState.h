
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



#ifndef _SIPEXSIPESTABLISHEDSTATE_H_
#define _SIPEXSIPESTABLISHEDSTATE_H_


//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;


// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Established" state.
*/
class CSIPExSIPEstablishedState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPEstablishedState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPEstablishedState();

	public://new functions
		/**
		* Links the states
		* @param aTerminatingState BYE sent, waiting for response.
		* @param aTerminatedState SIP Session has been ended.
		*/
		void LinkStates(
			CSIPExSIPStateBase& aTerminatingState,
			CSIPExSIPStateBase& aTerminatedState );

	public://functions from base class

		/**
		* EndSessionL
		* Send BYE in an established session.
		* @param aEngine Reference to Engine object.
		*/
		void EndSessionL( CSIPExSIPEngine& aEngine );


		/**
		* ByeReceivedL
		* BYE has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void ByeReceivedL( CSIPExSIPEngine& aEngine,
						   CSIPServerTransaction& aTransaction );

		/**
		* ResponseReceivedL
		* A SIP response has been received from the network.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void ResponseReceivedL( CSIPExSIPEngine& aEngine,
								CSIPClientTransaction& aTransaction );


		/**
		* AckReceivedL
		* ACK has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void AckReceivedL(
						   CSIPExSIPEngine& aEngine,
						   CSIPServerTransaction& aTransaction );

	private:
	    /**
		* C++ default constructor.
		*/
		CSIPExSIPEstablishedState();

	private://data members
		CSIPExSIPStateBase* iTerminatingState;
		CSIPExSIPStateBase* iTerminatedState;
	};

#endif // _SIPEXSIPESTABLISHEDSTATE_H_

