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
* Name        : CStateModel.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSTATEMODEL_H__
#define __CSTATEMODEL_H__

// INCLUDES
#include <e32base.h>
#include "MStateModel.h"

// FORWARD DECLARATIONS
class MSIPConnectionContext;
class MTimerManager;
class TStateBase;

// CLASS DEFINITION
/**
 * CStateModel implements a state model for CSipConnection
 */
class CStateModel
	: public CBase,
	  public MStateModel
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @param aContext Reference to SIP Connection Context (~CSipConnection)
		 * @param aTimer Reference to the global timer manager.
		 * @return An initialized instance of this class.
		 */
		static CStateModel* NewL( MSIPConnectionContext& aContext,
								  MTimerManager& aTimer );

		/// Destructor
		~CStateModel();

	public: // From MStateModel

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

		MStateModel::TState StateValue() const;

		void EnterState( MStateModel::TState aState, TInt aError );
		
		TInt Error() const;

	public: // From MSIPNetworkObserver

		void NetworkStateChanged( MSIPNetworkObserver::TNetworkState aState,
								  TInt aError );

	private: // New methods

		/// @return Reference to the active state instance.
		inline TStateBase& State() const;

	private: // Constructors

		/// Default constructor. Not implemented.
		CStateModel();

		/**
		 * Constructor.
		 *
		 * @param aContext Reference to SIP Connection Context (~CSipConnection)
		 * @param aTimer Reference to the global timer manager.
		 */
		CStateModel( MSIPConnectionContext& aContext,
					 MTimerManager& aTimer );

		/// 2nd phase constructor
		void ConstructL();

	private: // Data

		/// Reference to SIP Connection Context (~CSipConnection)
		MSIPConnectionContext& iContext;

		/// Reference to the global timer manager.
		MTimerManager& iTimer;

		/// State instances. Owned.
		CArrayVarFlat< TStateBase > iStates;

		/// Current state as enumeration value
		MStateModel::TState iStateValue;
		
		/// Stores connection error
		TInt iError;
	private: //For testing purposes
#ifdef CPPUNIT_TEST	
		friend class CStateModel_test;
#endif
	};

#endif // __CSTATEMODEL_H__
