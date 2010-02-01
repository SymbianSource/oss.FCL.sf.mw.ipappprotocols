
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



#ifndef _SIPEXSIPTERMINATINGSTATE_H_
#define _SIPEXSIPTERMINATINGSTATE_H_

//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Established" state.
*/
class CSIPExSIPTerminatingState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSIPExSIPTerminatingState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~CSIPExSIPTerminatingState();

	public://new functions
		/**
		* Links the states
		* @param aTerminatedState SIP Session has been ended.
		*/
		void LinkStates( CSIPExSIPStateBase& aIdleState );

	public://functions from base class

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
		CSIPExSIPTerminatingState();

	private://data members
		CSIPExSIPStateBase* iIdleState;
	};

#endif // _SIPEXSIPTERMINATINGSTATE_H_

