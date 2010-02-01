
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



#ifndef _SIPEXSIPSERVERESTABLISHINGSTATE_H_
#define _SIPEXSIPSERVERESTABLISHINGSTATE_H_

//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "ClientOffering" state.
*/
class CSIPExSIPServerEstablishingState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPServerEstablishingState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPServerEstablishingState();

	public://new functions
		/**
		* Links the states
		* @param aEstablishedState SIP Session is established.
		* @param aTerminatedState Peer has declined the invite, SIP Session is terminated.
		*/
		void LinkStates(
			CSIPExSIPStateBase& aEstablishedState,
			CSIPExSIPStateBase& aTerminatedState );

	public://functions from base class

		/**
		* AckReceivedL
		* ACK has been received.
		* @param aEngine Reference to Engine object.
		* @param aTransaction Contains response elements.
		*/
		void AckReceivedL( CSIPExSIPEngine& aEngine,
						   CSIPServerTransaction& aTransaction );


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


	private:
	    /**
		* C++ default constructor.
		*/
		CSIPExSIPServerEstablishingState();

	private://data members
		CSIPExSIPStateBase* iEstablishedState;
		CSIPExSIPStateBase* iTerminatedState;
	};

#endif // _SIPEXSIPSERVERESTABLISHINGSTATE_H_

