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
* Name        : TStateActiveResolved.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TSTATEACTIVERESOLVED_H__
#define __TSTATEACTIVERESOLVED_H__

// INCLUDES
#include "TStateBase.h"

// CLASS DEFINITION
/**
 * TStateActiveResolved implements the Active:Resolved state
 */
class TStateActiveResolved
	: public TStateBase
	{
	public:	// Constructors and destructor

		/**
		 * Constructor.
		 * @param aOwner State model that owns us.
		 * @param aContext Reference to SIP connection context
		 */
		TStateActiveResolved( MStateModel& aOwner,
							  MSIPConnectionContext& aContext )
			: TStateBase( aOwner, aContext ) {};

	public: // From TStateBase

		void SendL( const TSIPTransportParams& aParams,
		            CSIPRequest& aRequest,
		            TBool aForceUDP,
		            const CUri8* aOutboundProxy,
					const TInetAddr& aRemoteAddr,
					TRequestStatus& aStatus );

 		void SendL( const TSIPTransportParams& aParams,
 		            CSIPResponse& aResponse,
                   	TTransactionId aId,
					TRequestStatus& aStatus );

		void NetworkStateChanged( MSIPNetworkObserver::TNetworkState aState,
								  TInt aError );

	};

#endif // __TSTATEACTIVERESOLVED_H__
