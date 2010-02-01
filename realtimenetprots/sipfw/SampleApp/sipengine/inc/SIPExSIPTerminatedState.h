
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



#ifndef _SIPEXSIPTERMINATEDSTATE_H_
#define _SIPEXSIPTERMINATEDSTATE_H_

//  INCLUDES
#include "SIPExSIPStateBase.h"

// FORWARD DECLARATIONS
class CSIPExSIPEngine;

// CLASS DECLARATION
/**
* Reacts to events that are possible in 
* "Established" state.
*/
class SIPExSIPTerminatedState: public CSIPExSIPStateBase
	{
	public:// Constructors and destructor
        /**
        * Two-phased constructor.
        */
		IMPORT_C static SIPExSIPTerminatedState* NewL();

		/**
		* Destructor
		*/
		IMPORT_C virtual ~SIPExSIPTerminatedState();

	public://new functions
		/**
		* Links the states
		*/
		IMPORT_C void LinkStates();

	public://functions from base class


	private:
	    /**
		* C++ default constructor.
		*/
		SIPExSIPTerminatedState();

	private://data members

	};

#endif // _SIPEXSIPTERMINATEDSTATE_H_

