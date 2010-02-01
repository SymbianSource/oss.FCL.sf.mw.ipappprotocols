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
* Name        : CRegistrationMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CREGISTRATIONMONITOR_H__
#define __CREGISTRATIONMONITOR_H__

// INCLUDES
#include <etelmm.h>

// FORWARD DECLARATIONS
class MSIPBearerMonitorObserver;

// CLASS DEFINITION
/**
 *
 */
class CRegistrationMonitor
	: public CActive
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CRegistrationMonitor* NewL( 
		    MSIPBearerMonitorObserver& aObserver );

		/// Destructor
		~CRegistrationMonitor();

	public: // New methods

		TBool IsRegistered() const;

	protected: // From CActive
	
		void DoCancel();
		void RunL();

	private: // Constructors

		/// Constructor
		CRegistrationMonitor( MSIPBearerMonitorObserver& aObserver );
		
		/// Default constructor, not implemented.
		CRegistrationMonitor();

		/// 2nd phase constructor
		void ConstructL();
		
		void MonitorStatus();

	private: // Data

		MSIPBearerMonitorObserver& iObserver;
		RTelServer iTelServer;
		RMobilePhone iPhone;
		RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;
		TBool iIsRegistered;

	};

#endif // __CREGISTRATIONMONITOR_H__
