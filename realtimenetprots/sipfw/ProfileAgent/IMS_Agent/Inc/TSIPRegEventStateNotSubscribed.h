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
* Name        : TSIPRegEventStateNotSubscribed.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPREGEVENTSTATENOTSUBSCRIBED_H
#define TSIPREGEVENTSTATENOTSUBSCRIBED_H

// INCLUDES
#include "TSIPRegEventSubscriptionStateBase.h"


// CLASS DECLARATION
class TSIPRegEventStateNotSubscribed : public TSIPRegEventSubscriptionStateBase
	{
	public:	// Constructors

		TSIPRegEventStateNotSubscribed(MSIPRegEventContext& aContext);

	public: // From TSIPRegEventSubscriptionStateBase
		
		void SubscribeL();

	private: // Constructors

		/// Default constructor. Not implemented.
		TSIPRegEventStateNotSubscribed();
	};

#endif // TSIPREGEVENTSTATENOTSUBSCRIBED_H
