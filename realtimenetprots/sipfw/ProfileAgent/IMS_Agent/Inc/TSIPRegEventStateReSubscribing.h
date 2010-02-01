/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TSIPRegEventStateReSubscribing.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPREGEVENTSTATERESUBSCRIBING_H
#define TSIPREGEVENTSTATERESUBSCRIBING_H

// INCLUDES
#include "TSIPRegEventSubscriptionStateBase.h"


// CLASS DECLARATION
class TSIPRegEventStateReSubscribing : public TSIPRegEventSubscriptionStateBase
	{
	public:	// Constructors

		TSIPRegEventStateReSubscribing(MSIPRegEventContext& aContext);

	public: // From TSIPRegEventSubscriptionStateBase
		
        void ResponseReceivedL(TUint aStatusCode, TUint aRetryAfter);

	private: // Constructors

		/// Default constructor. Not implemented.
		TSIPRegEventStateReSubscribing();
	};

#endif // TSIPREGEVENTSTATERESUBSCRIBING_H
