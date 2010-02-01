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
* Name        : CNetworkManager.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CNETWORKMANAGER_H__
#define __CNETWORKMANAGER_H__

// INCLUDES
#include <e32base.h>
#include <commsdattypesv1_1.h>
#include "sipnetworkobserver.h"
#include "sipbearerowner.h"
#include "sipnetworkinfoobserver.h"
#include "sipaccessnetworkinfo.h"
    
using namespace CommsDat;

// FORWARD DECLARATIONS
class CSIPBearerMonitor;
class MSIPNetworkInfoObserver;

// CLASS DEFINITION
/**
 * CNetworkManager implements a
 */
class CNetworkManager :
	public CBase,
	public MSIPBearerOwner,
	public MSIPNetworkInfoObserver
	{
	private: // Nested classes

		// CLASS DEFINITION
		/**
		 * Simple container class for observer registry entries.
		 */
		class TObserverEntry
			{
			public: // Data

				/// Pointer to an observer. Not owned.
				MSIPNetworkObserver* iObserver;
				/// Pointer to a bearer monitor instance. Not owned.
				CSIPBearerMonitor* iBearer;
				/// State reported when observer was notified the last time
				MSIPNetworkObserver::TNetworkState iReportedState;
			};

	public:	// Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CNetworkManager* NewL();

		/// Destructor
		~CNetworkManager();

		/**
		* Refresh the connection on the IAP ID.
		*@param aIapId IAP ID on which the connection has to be refreshed.
		*/		
		void RefreshConnectionL(TUint32 aIapId);

	public: // New methods

		/**
		 * Register a new network state observer.
		 *
		 * @param aObserver Reference to a network state observer.
		 * @param aIapId Internet Access Point Id for the observer's connection.
		 * @param aError error which caused observers registration, may be also
		 * KErrNone since all plugins don't need error information
		 */
		void RegisterObserverL( MSIPNetworkObserver& aObserver,
								TInt aIapId,
								TInt aError );

		/**
		 * Deregister a registered observer. Does nothing if
		 * the specified observer has not been registered.
		 *
		 * @param aObserver Reference to a network state observer.
		 */
		void Deregister( MSIPNetworkObserver& aObserver );
		
		/**
		 * Ask from corresponding monitor plugin whether monitoring should be
	     * continued after aError has occured while re-establishing the
	     * connection.
		 *
		 * @param aObserver Reference to a network state observer.
		 * @param aError Error code which occured while re-establishing the
		 *				 connection
		 */
		TBool ContinueMonitoring( MSIPNetworkObserver& aObserver, 
                                  TInt aError );

		/// @return Current network state.
		MSIPNetworkObserver::TNetworkState State( 
		    MSIPNetworkObserver& aObserver ) const;
		
		/**
		 * Register an observer to listen for access network information
		 * changes.
		 *
		 * @param aObserver Observer which starts to listen information
		 * @return const TSIPAccessNetworkInfo& Current information
		 */
		const TSIPAccessNetworkInfo&
			RegisterAccessNetworkObserver( MSIPNetworkInfoObserver& aObserver );
	
		/**
		 * Deregister the observer that listens for access network information
		 * changes.		 		 
		 */
		void DeregisterAccessNetworkObserver();

	public: // From MSIPBearerOwner

		void BearerStateChanged( CSIPBearerMonitor* aBearer,
								 TInt aError );

	public: // From MSIPNetworkInfoObserver

		void InfoChanged( const TSIPAccessNetworkInfo& aInfo );
								  
	private: // New methods

		/**
		 * Finds an observer registry entry matching aObserver.
		 *
		 * @return Registry entry index or KErrNotFound if a match was
		 *		   not found.
		 */
		TInt FindRegistryEntry( MSIPNetworkObserver& aObserver ) const;

		/**
		 * Tries to find an existing BearerMonitor instance matching the
		 * IAPServiceType defined for the specified IAP. If none is
		 * found, creates a new instance using ECom.
		 *
		 * @param aIapID CommDb IAP ID
		 * @return BearerMonitor instance. Ownership not transferred.
		 *         NOTE: Can return also NULL if IAP is LANService
		 *               IAP which is not used for WLAN or unit testing
		 */
		CSIPBearerMonitor* FindBearerL( TInt aIapId, TInt aError );

		/**
		 * Gets the service type ("IAP\IAPServiceType") from CommDB for
		 * the specified access point.
		 *
		 * @param aIapID CommDb IAP ID
		 * @param aServiceType On return contains the service type string.
		 * @param aObjectName On return contains the service type string + 
		 *        possibly iapId for easing possiblity of having several 
		 *        monitor plugins open for single bearer type
		 * @return Real IapId (with VPN IAP, aIapId is different
		 *         than returned id)
		 */
		TInt GetServiceTypeAndObjectNameL( TInt aIapId,
		                                   TDes& aServiceType, 
		                                   TDes& aObjectName );
		
		/**
         * Finds out really used IAP of VPN IAP.
         *
         * @param aCommsDat
         * @param CCDIAPRecord IAP record for VPN
         * @param aServiceType On return contains the service type
         *        of really used IAP
         * @return Real IapId used by VPN IAP
         */	
		TInt HandleVPNServiceL( CMDBSession& aCommsDat, 
		                        CCDIAPRecord& aIapRecord, 
		                        TDes& aServiceType );
        
        /**
         * Gets service type of IAP.
         *
         */
        void GetServiceTypeL( CCDIAPRecord& aIapRecord, TDes& aServiceType );
        
        /**
         * Gets dummy AN values.
         *
         * @return dummy access network values
         */	                                     
		TSIPAccessNetworkInfo DummyAccessNetworkValues() const;
		

	private: // Constructors and destructor

		/// Default constructor.
		CNetworkManager();

		/// 2nd phase constructor
		void ConstructL();

	private: // Data

		/// Object container index used to create the CObjectCon instance.
		CObjectConIx* iContainerIndex;

		/// Object container for Bearer and Monitor instances. Not owned.
		CObjectCon* iContainer;

		/// Observer registry. Owned.
		CArrayFixFlat< TObserverEntry > iObservers;
		
		
		/// Observer listening for access network information. Not owned.
		MSIPNetworkInfoObserver* iNetworkInfoObserver;
		
		/// Current info
		TSIPAccessNetworkInfo iAccessNetworkInfo;
	
		
#ifdef CPPUNIT_TEST
        friend class CNetworkManagerStub;
#endif
	};

#endif // __CNETWORKMANAGER_H__
