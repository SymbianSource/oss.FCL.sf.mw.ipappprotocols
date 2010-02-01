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
* Name        : CConnectionMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CCONNECTIONMONITOR_H__
#define __CCONNECTIONMONITOR_H__

// INCLUDES
#include <es_sock.h>

// FORWARD DECLARATIONS
class MSIPNetworkObserver;

// CLASS DEFINITION
/**
 * CConnectionMonitor implements an active object for observing RConnection
 * progress notifications and reporting them to the MBearerOwner interface.
 */
class CConnectionMonitor
	: public CActive
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @param aObserver Reference to observer
		 * @param aConnection Reference to observed connection
		 * @return An initialized instance of this class.
		 */
		static CConnectionMonitor* NewL( MSIPNetworkObserver& aObserver,
								   		 RConnection& aConnection );

		/// Destructor
		~CConnectionMonitor();

	public: // New methods

		void StartMonitoring();
		void StopMonitoring();
		void MonitorConnectionClosure();

	protected: // From CActive

		void DoCancel();
		void RunL();

	private: // New methods

		/**
		 * Translates state information from TNifProgress to
		 * a boolean value.
		 *
		 * @param aProgress Reference Nif progress information
		 * @return ETrue if the progress stage is KConnectionOpen
		 *		   or KLinkLayerOpen, EFalse otherwise.
		 */
		TBool TranslateStage( const TNifProgress& aProgress );
		
		void StartMonitor();

	private: // Constructors

		/**
		 * Constructor.
		 *
		 * @param aObserver Reference to observer
		 * @param aConnection Reference to observed connection
		 */
		CConnectionMonitor( MSIPNetworkObserver& aObserver,
					 		RConnection& aConnection );

		/// Default constructor. Not implemented.
		CConnectionMonitor();

	private: // Data

		/// Reference to our observer. Not owned.
		MSIPNetworkObserver& iObserver;

		/// Reference to the observed connection. Not owned.
		RConnection& iConnection;

		/// Network interface progress state.
		TNifProgressBuf iProgress;
		
		TBool iMonitorTotalClosure;
	};

#endif // __CCONNECTIONMONITOR_H__
