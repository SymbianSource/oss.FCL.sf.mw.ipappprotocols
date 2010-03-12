/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class aware of Device System State Changes and perform processing
 *                  related to profile registration/deregistration
 *
*/


#ifndef SIPDEVICESYSTEMSTATEAWARE_H_
#define SIPDEVICESYSTEMSTATEAWARE_H_

// INCLUDES
#include <ssm/ssmstateawaresession.h>
#include <sipsystemstatemonitor.h>
#include <e32base.h>
//Forward Declarations
class MSipSystemStateObserver;

// CLASS DECLARATION
/**
* The default implementation for SIP DeviceSystemStateAware Class.
*/
class CSipDeviceStateAware: public CActive
    {
    public:
        
         //Constructors and Destructor
        static CSipDeviceStateAware* NewL();
        
        /**
         * SIP sub-components should call this function in order
         * to get notified in case of system state changes
         */
		void AddObserverL( MSipSystemStateObserver& aObserver );
		
		/**
		 * SIP sub-components should call this function in order
         * to stop themselves getting notified for system state changes 
		 */
        void RemoveObserver( MSipSystemStateObserver& aObserver );
		
		/**
		 * In case if the profiles de-registration is completed within the 
		 * timer started, this function will be called from Observers to indicate
		 * that the  procession has been done. This function notifies to the SSM
		 * that event processing is done.
		 */
		void EventProcessingCompleted(MSipSystemStateObserver& aObserver);
		
		/**
		 * When the Notification for Offline is received, timer will be started for
		 * a given time, after which the notification of the event will be given
		 * back to the System State Manager
		 */
		static TInt TimerExpired(TAny* aSelf);
        
        //Destructor
        ~CSipDeviceStateAware();
        
    private:
        //Default Construtor
        CSipDeviceStateAware();
        //Second Phase Constructor
        void ConstructL();
        //From CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();
        
        /**
         * This function will notify all the observers when the
         * state of the phone changes
         */
        void NotifyObservers(CSipSystemStateMonitor::TSystemState aState) const;
        
        /**
         * In case if profile de-registration could not be completed by
         * observers, TimerExpiredL will call this function to indicate 
         * that event processing is complete and further notification will
         * be given to the System State Manager. 
         */
        void EventProcessingCompleted();
        
    private:
        
        //List of observers who have opted for system state changes
        RPointerArray<MSipSystemStateObserver> iObservers;
        //SystemAwareSession object used for monitoring system state
        RSsmStateAwareSession iStateAwareSession;
        //Variable depicting state of the phone
        CSipSystemStateMonitor::TSystemState iState;
		//Timer after which Acknowledgement of Notification will be given
		CPeriodic* iGuardTimer;
		//Keeps the count of observers
		TInt iCount;
    private: // For testing purposes        
        friend class CSipDeviceStateAwareTest;
    };

#endif /* SIPDEVICESYSTEMSTATEAWARE_H_ */
