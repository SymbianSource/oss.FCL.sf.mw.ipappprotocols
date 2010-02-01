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
* Description:  
*
*/


#ifndef CSIPSYSTEMSTATEMONITORAO_H
#define CSIPSYSTEMSTATEMONITORAO_H

//  INCLUDES
#include <sipsystemstateobserver.h>
#include <sipsystemstatemonitor.h>
#include <e32property.h>
#include <e32base.h>


// CLASS DECLARATION
class CSipSystemStateMonitorAo : public CActive
    {
	public: // Constructors and destructor

        static CSipSystemStateMonitorAo* NewL();
            
		~CSipSystemStateMonitorAo();
		
    public: // New functions
    
        CSipSystemStateMonitor::TSystemState State() const;
        
        void AddObserverL( MSipSystemStateObserver& aObserver );
        void RemoveObserver( MSipSystemStateObserver& aObserver );
        
	private: // Constructors
	
        CSipSystemStateMonitorAo();
		void ConstructL ();
	
	private: // From CActive
	
		void RunL();
        TInt RunError( TInt aError );
		void DoCancel();
		
    private: // New functions
    
        void NotifyObservers();
        TBool IsSystemReady( TInt aSystemState ) const;
        
	private: // Data

		RProperty iProperty;
		// Observers not owned
		RPointerArray<MSipSystemStateObserver> iObservers;
		CSipSystemStateMonitor::TSystemState iState;

    private: // For testing purposes
    
        friend class CSipSystemStateMonitorImplTest;
        friend class CSipSystemStateMonitorAoTest;	
    };

#endif //CSIPSYSTEMSTATEMONITORAO_H

