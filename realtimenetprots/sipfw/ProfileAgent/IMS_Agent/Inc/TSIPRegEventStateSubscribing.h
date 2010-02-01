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
* Name        : TSIPRegEventStateSubscribing.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPREGEVENTSTATESUBSCRIBING_H
#define TSIPREGEVENTSTATESUBSCRIBING_H

// INCLUDES
#include "TSIPRegEventSubscriptionStateBase.h"


// CLASS DECLARATION
class TSIPRegEventStateSubscribing : public TSIPRegEventSubscriptionStateBase
	{
	public:	// Constructors

		TSIPRegEventStateSubscribing(MSIPRegEventContext& aContext);

	public: // From TSIPRegEventSubscriptionStateBase
		
        void ResponseReceivedL(TUint aStatusCode, TUint aRetryAfter);

	private: // Constructors

		/// Default constructor. Not implemented.
		TSIPRegEventStateSubscribing();
	};

#endif // TSIPREGEVENTSTATESUBSCRIBING_H
