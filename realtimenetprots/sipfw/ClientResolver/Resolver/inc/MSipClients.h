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
* Name        : MSipClients.h
* Part of     : SIP Client Resolver
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef MSIPCLIENTS_H
#define MSIPCLIENTS_H

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MSipClient;

// CLASS DEFINITION
/**
 *
 */
class MSipClients
	{
	public:	// Abstract methods

		virtual MSipClient* GetByUID (const TUid& aUid) const = 0;
		};

#endif // MSIPCLIENTS_H
