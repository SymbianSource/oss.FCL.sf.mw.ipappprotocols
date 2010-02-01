/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipalrhandler.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/



#ifndef CSIPALRHANDLER_H
#define CSIPALRHANDLER_H

/**
*@internalComponent
*/

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPProfileCacheItem;
class MSIPExtendedConcreteProfileObserver;
class MSipProfileErrorHandler;
class CSipAlrMonitor;
class CSipSystemStateMonitor;

// CLASS DECLARATION
/**
* The default implementation for SIP ALR Monitor.
*
* @lib sipsrofilesrv.exe
*/
class CSipAlrHandler : public CBase
    {
	public: // Constructors and destructor

    	static CSipAlrHandler* NewL(
    	    MSipProfileErrorHandler& aErrorHandler,
    	    CSipSystemStateMonitor* aSystemStateMonitor);

    	~CSipAlrHandler();
    		
    public: // New functions

        void EnableProfileL( 
            CSIPProfileCacheItem& aProfile,
            const MSIPExtendedConcreteProfileObserver& aObserver);

        void DisableProfileL( 
            CSIPProfileCacheItem& aProfile,
            const MSIPExtendedConcreteProfileObserver& aObserver);

        void AllowMigrationL(
            CSIPProfileCacheItem& aProfile,
            TUint32 aIapId,
            const MSIPExtendedConcreteProfileObserver& aObserver);

        void DisallowMigrationL(
            CSIPProfileCacheItem& aProfile,
            TUint32 aIapId,
            const MSIPExtendedConcreteProfileObserver& aObserver);

        void RefreshIapAvailabilityL(
            CSIPProfileCacheItem& aProfile);

		/**
        * Loads the plug-in if not yet loaded
		* @return ALR monitor plug-in, ownership is not transferred.
        */
        CSipAlrMonitor& AlrMonitorL();

	private: // Constructors
	
	    CSipAlrHandler(
	        MSipProfileErrorHandler& aErrorHandler,
	        CSipSystemStateMonitor* aSystemStateMonitor);

    private: // New functions

		/**
        * Handles possible errors during disable or enable
		* @param aItem profile item to be handled
		* @param aErr error code to be handled
		* @param aOwnError internal error code
        */
		void HandleErrorL(CSIPProfileCacheItem& aItem,
						  TInt aErr,
						  TInt aOwnError);

    private: // Data

        // ALR monitor plug-in, owned.
        CSipAlrMonitor* iAlrMonitor;
        MSipProfileErrorHandler& iErrorHandler;
        CSipSystemStateMonitor* iSystemStateMonitor; // Not owned
       
    private: // For testing purposes		
#ifdef CPPUNIT_TEST 
        friend class CSipAlrHandlerTest;
        friend class CSIPProfileStateRegInProgTest;
        friend class CSIPProfileStateWaitForPermissionTest;
        friend class CSIPProfileStateMigratingToNewIAPTest;
#endif
    };

#endif // CSIPALRHANDLER_H
