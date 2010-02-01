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
* Name        : TStateBase.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TSTATEBASE_H__
#define __TSTATEBASE_H__

// INCLUDES
#include "MStateModel.h"

class MSIPConnectionContext;
class CUri8;

// CLASS DEFINITION
/**
 * TStateBase implements a state base class
 */
class TStateBase
	{
	protected:	// Constructors and destructor

		/**
		 * Constructor.
		 *
		 * @param aOwner Reference to state model owning this state.
		 * @param aContext Reference to SIP Connection Context (~CSipConnection)
		 */
		TStateBase( MStateModel& aOwner,
					MSIPConnectionContext& aContext );

		/// Virtual destructor (PC-Lint requires this also for a T-class)
		virtual ~TStateBase() {};

	public: // New methods

		virtual void SendL( const TSIPTransportParams& aParams,
		                    CSIPRequest& aRequest,
		                    TBool aForceUDP,
		                    const CUri8* aOutboundProxy,
							const TInetAddr& aRemoteAddr,
							TRequestStatus& aStatus );

 		virtual void SendL( const TSIPTransportParams& aParams,
 		                    CSIPResponse& aResponse,
                   			TTransactionId aId,
							TRequestStatus& aStatus );

		/** 
		* Called by state model when a state is entered
		*
		* @param aError
		*/
		virtual void Enter( TInt aError );

		/// Called by state model when a state is left
		virtual void Exit();

		/*
		 * Called by state model when it receveis a network monitoring
		 * state change notification.
		 *
		 * @param aState
		 * @param aError
		 */
		virtual void NetworkStateChanged(
							MSIPNetworkObserver::TNetworkState aState,
							TInt aError );

	protected: // New methods

		/**
		 *
		 */
		void EnterState( MStateModel::TState aState,
						 TInt aError = KErrNone );

	private: // Constructors

		/// Default constructor. Not implemented.
		TStateBase();

	protected: // Data

		/// Reference to state model owning this state.
		MStateModel& iOwner;

		/// Reference to SIP Connection Context (~CSipConnection)
		MSIPConnectionContext& iContext;

	};

#endif // __TSTATEBASE_H__
