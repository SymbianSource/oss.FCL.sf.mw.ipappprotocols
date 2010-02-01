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
* Name        : TStateInactiveMonitoring.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TSTATEINACTIVEMONITORING_H__
#define __TSTATEINACTIVEMONITORING_H__

// INCLUDES
#include "TStateBase.h"
#include "Lwtimer.h"

// CLASS DEFINITION
/**
 * TStateInactiveMonitoring implements the Inactive:Monitoring state
 */
class TStateInactiveMonitoring
	: public TStateBase,
	public MExpirationHandler
	{
	public:	// Constructors and destructor

		/**
		 * Constructor.
		 * @param aOwner State model that owns us.
		 * @param aContext Reference to SIP connection context
		 */
		TStateInactiveMonitoring( MStateModel& aOwner,
							 	  MSIPConnectionContext& aContext,
							 	  MTimerManager& aTimer)
			: TStateBase( aOwner, aContext ),
			iTimer( aTimer ), 
			iTimerId( 0 ),
			iLessFatal( 0 ),
			iRetryCount( 0 ){};

	public: // From TStateBase

		void Enter( TInt aError );
		void Exit();
		void NetworkStateChanged( MSIPNetworkObserver::TNetworkState aState,
								  TInt aError );

	public: // From MExpirationHandler
	void TimerExpiredL( TTimerId aTimerId, TAny* aTimerParam );
	void StartRecovery(TInt aError);
	
	private: // new functions.
	void StartTimer( TInt aTimeout );
	void ResetTimer();
	
	private: // Data

	/// Reference to the global timer manager.
	MTimerManager& iTimer;
	/// Handle to our timer (when active)
	TTimerId iTimerId;
	TBool iLessFatal;
	TInt iRetryCount;
	};

#endif // __TSTATEINACTIVEMONITORING_H__
