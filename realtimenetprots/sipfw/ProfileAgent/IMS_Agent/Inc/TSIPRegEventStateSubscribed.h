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
* Name        : TSIPRegEventStateSubscribed.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPREGEVENTSTATESUBSCRIBED_H
#define TSIPREGEVENTSTATESUBSCRIBED_H

// INCLUDES
#include "TSIPRegEventSubscriptionStateBase.h"


// CLASS DECLARATION
class TSIPRegEventStateSubscribed : public TSIPRegEventSubscriptionStateBase
	{
	public:	// Constructors

		TSIPRegEventStateSubscribed(MSIPRegEventContext& aContext);

	public: // From TSIPRegEventSubscriptionStateBase
		
		void RefreshL();
		
        void ResponseReceivedL(TUint aStatusCode, TUint aRetryAfter);
        
        void ErrorOccuredL();

	private: // Constructors

		/// Default constructor. Not implemented.
		TSIPRegEventStateSubscribed();
	};

#endif // TSIPREGEVENTSTATESUBSCRIBED_H
