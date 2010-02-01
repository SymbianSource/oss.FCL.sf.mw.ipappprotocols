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
* Name        : TStateUnavailable.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TSTATEUNAVAILABLE_H__
#define __TSTATEUNAVAILABLE_H__

// INCLUDES
#include "TStateBase.h"

// CLASS DEFINITION
/**
 * TStateUnavailable implements the Unavailable state
 */
class TStateUnavailable
	: public TStateBase
	{
	public:	// Constructors and destructor
		/**
		 * Constructor.
		 * @param aOwner State model that owns us.
		 * @param aContext Reference to SIP connection context (~CSipConnection)
		 */
		TStateUnavailable( MStateModel& aOwner,
						   MSIPConnectionContext& aContext )
			: TStateBase( aOwner, aContext ) {};

	public: // From TStateBase

		void Enter( TInt aError );

	};

#endif // __TSTATEUNAVAILABLE_H__
