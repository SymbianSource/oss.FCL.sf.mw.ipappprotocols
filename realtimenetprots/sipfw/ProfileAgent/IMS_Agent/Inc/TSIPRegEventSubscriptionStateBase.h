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
* Name        : TSIPRegEventSubscriptionStateBase.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPREGEVENTSUBSCRIPTIONSTATEBASE_H
#define TSIPREGEVENTSUBSCRIPTIONSTATEBASE_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MSIPRegEventContext;
class CSIPResponseElements;


// CLASS DECLARATION
class TSIPRegEventSubscriptionStateBase
	{
	protected:	// Constructors and destructor

		TSIPRegEventSubscriptionStateBase(MSIPRegEventContext& aContext);
		virtual ~TSIPRegEventSubscriptionStateBase();

	public: // New methods

		virtual void SubscribeL();
		
		virtual void RefreshL();
		
		virtual void ResponseReceivedL(TUint aStatusCode, TUint aRetryAfter);
		
		virtual void ErrorOccuredL();

	private: // Constructors

		/// Default constructor. Not implemented.
		TSIPRegEventSubscriptionStateBase();

	protected: // Data

		MSIPRegEventContext& iContext;
	};

#endif // TSIPREGEVENTSUBSCRIPTIONSTATEBASE_H
