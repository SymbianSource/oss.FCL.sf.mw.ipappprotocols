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
* Name        : TStateActiveResolving.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/



#ifndef __TSTATEACTIVERESOLVING_H__
#define __TSTATEACTIVERESOLVING_H__

// INCLUDES
#include "TStateBase.h"
#include "Lwtimer.h"

// CONSTANTS
const TUint KInitialResolvingTimer( 1000 ); // milliseconds
const TUint KStartDoublingTimerValue( 4000 ); // milliseconds
const TUint KMaximumResolvingTimer( 32000 ); // milliseconds

// CLASS DEFINITION
/**
 * TStateActiveResolving implements the Active:Resolving state
 */
class TStateActiveResolving
	: public TStateBase,
	  public MExpirationHandler
	{
	public:	// Constructors and destructor

		/**
		 * Constructor.
		 * @param aOwner State model that owns us.
		 * @param aContext Reference to SIP connection context
		 */
		TStateActiveResolving( MStateModel& aOwner,
							   MSIPConnectionContext& aContext,
							   MTimerManager& aTimer )
			: TStateBase( aOwner, aContext ), 
			  iTimer( aTimer ),
			  iTimerId( 0 ),
			  iTimerValue( KInitialResolvingTimer ),
			  iCumulativeTimerValue( KInitialResolvingTimer ) {}

	public: // From TStateBase

		void Enter( TInt aError );
		void Exit();
		void NetworkStateChanged( MSIPNetworkObserver::TNetworkState aState,
								  TInt aError );

	public: // From MExpirationHandler

		void TimerExpiredL( TTimerId aTimerId, TAny* aTimerParam );

	private: // New methods

		/// @return ETrue if MSIPConnectionContext has a valid IP address
		TBool HasAddress();
		
		TUint CalculateTimerValue();
		void EnterActiveResolvedState();

	private: // Data

		/// Reference to the global timer manager.
		MTimerManager& iTimer;

		/// Handle to our timer (when active)
		TTimerId iTimerId;
		
		TUint iTimerValue;
		
		TUint iCumulativeTimerValue;
		
	private: // For unit testing purposes
#ifdef CPPUNIT_TEST
	    friend class CStateModel_test;
#endif
	};

#endif // __TSTATEACTIVERESOLVING_H__
