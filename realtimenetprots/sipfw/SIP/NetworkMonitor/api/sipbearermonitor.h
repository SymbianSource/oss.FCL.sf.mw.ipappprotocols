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
* Name        : sipbearermonitor.h
* Part of     : SIP NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/



#ifndef CSIPBEARERMONITOR_H
#define CSIPBEARERMONITOR_H

// INCLUDES
#include <ecom/ecom.h>
#include "sipbearermonitorobserver.h"
#include "sipnetworkobserver.h"

// FORWARD DECLARATIONS
class MSIPBearerOwner;
class MSIPNetworkInfoObserver;

/**
* @publishedPartner
* @released
*
* Construction parameters for CSIPBearerMonitor are packaged into 
* TSIPBearerParams
*/
class TSIPBearerParams
    {
    public:
        inline TSIPBearerParams( MSIPBearerOwner& aOwner,
        					     TInt aIapId,
        					     TInt aError,
        					     MSIPNetworkInfoObserver& aNetworkInfoObserver );

    private:
        TSIPBearerParams();
        
    public:
        MSIPBearerOwner& iOwner;
        TInt iIapId;
        TInt iError;
        MSIPNetworkInfoObserver& iNetworkInfoObserver;
    };

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Interface for ECom Bearer Monitor plugins. Implementations must implement the
* abstract functions defined in MSIPBearerMonitorObserver.
*/
class CSIPBearerMonitor :
	public CObject,
	public MSIPBearerMonitorObserver
	{
	public:	// Constructors and destructor

		/**
		 * Destructor.
		 * Informs ECom framework that this specific instance
		 * of the interface has been destroyed.
		 */
		inline ~CSIPBearerMonitor();

	public: // New methods

		/**
		 * @return Current bearer monitor state
		 */
		inline MSIPNetworkObserver::TNetworkState State() const;
		
	    /**
	     * Inherited monitor may override this to get refresh requests from observer
	     */
	    virtual inline void RefreshL( TInt aError );
	    
	    /**
	     * Inherited monitor may override this to inform whether monitoring should be
	     * continued after aError occured
	     *
	     * @return Default implementation returns always EFalse
	     */
	    virtual inline TBool ContinueMonitoring( TInt aError );

	protected: // Constructors

		/**
		 * Constructor.
		 *
		 * @param aParams ECom instantiation parameters, basically
		 *				  this is a MSIPBearerOwner*.
		 */
		inline CSIPBearerMonitor( TAny* aParams );

	private: // Constructors

		/**
		 * Default constructor. Not implemented.
		 */
		CSIPBearerMonitor();

	public: // Data

		// Unique key for implementations of this interface.
		TUid iInstanceKey;

	protected: // Data

		// Reference to our parent
		MSIPBearerOwner& iParent;

		// Network state.
		MSIPNetworkObserver::TNetworkState iState;
		
		// IapId which created this monitor, monitor plugins which are shared
		// by multiple accesspoints (e.g. gprs monitor) don't really need this
		// information
		TInt iIapId;
		
		// Error which caused plugins loading, may be also KErrNone
		TInt iError;
		
		// Observer listening for access network information changes
		MSIPNetworkInfoObserver& iNetworkInfoObserver;
	};
	
#include "sipbearermonitor.inl"

#endif // CSIPBEARERMONITOR_H
