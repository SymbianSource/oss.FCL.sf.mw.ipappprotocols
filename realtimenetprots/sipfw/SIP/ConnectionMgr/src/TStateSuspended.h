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
* Name        : TStateSuspended.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TSTATESUSPENDED_H__
#define __TSTATESUSPENDED_H__

// INCLUDES
#include "TStateBase.h"

// CLASS DEFINITION
/**
 * TStateSuspended implements the Suspended state
 */
class TStateSuspended
	: public TStateBase
	{
	public:	// Constructors and destructor

		/**
		 * Constructor.
		 * @param aOwner State model that owns us.
		 * @param aContext Reference to SIP connection context
		 */
		TStateSuspended( MStateModel& aOwner,
						 MSIPConnectionContext& aContext )
			: TStateBase( aOwner, aContext ) {};

	public: // From TStateBase

		void NetworkStateChanged( MSIPNetworkObserver::TNetworkState aState,
								  TInt aError );

	};

#endif // __TSTATESUSPENDED_H__
