/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipphoneregistrationmonitor.h
* Part of     : SIP Profile Agent / SIP IMS Agent
* Version     : %version: 2 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPHONEREGISTRATIONMONITOR_H
#define CSIPPHONEREGISTRATIONMONITOR_H

// INCLUDES
#include <etelmm.h>

// FORWARD DECLARATIONS
class MSipPhoneRegistrationObserver;

// CLASS DEFINITION
/**
 * Monitors the status of the phone's network registration.
 * Notifies when the status changes. 
 */
class CSipPhoneRegistrationMonitor : public CActive
	{
	public:

        /** Phone registration status */
        enum TStatus
            {
            ENotRegistered,
            ERegisteredOnHomeNetwork,
		    ERegisteredRoaming  
            };

		static CSipPhoneRegistrationMonitor* NewL( 
		    RTelServer& aTelServer,
		    const RTelServer::TPhoneInfo& aPhoneInfo,
		    MSipPhoneRegistrationObserver& aObserver );

		~CSipPhoneRegistrationMonitor();

	public: // New methods

		TStatus Status() const;

	protected: // From CActive
	
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);

	private: // Constructors

		/// Constructor
		CSipPhoneRegistrationMonitor(		 
		    MSipPhoneRegistrationObserver& aObserver );
		
		/// Default constructor, not implemented.
		CSipPhoneRegistrationMonitor();

		/// 2nd phase constructor
		void ConstructL( 
		    RTelServer& aTelServer,
		    const RTelServer::TPhoneInfo& aPhoneInfo );
		
		void MonitorStatus();

	private: // Data

		MSipPhoneRegistrationObserver& iObserver;
		RMobilePhone iPhone;
		RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;


#ifdef CPPUNIT_TEST	
	    friend class CSIPIMSProfileAgentTest;
#endif				
	};

#endif // CSIPPHONEREGISTRATIONMONITOR_H
