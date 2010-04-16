/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SIPRFSMONITORAO_H
#define SIPRFSMONITORAO_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include <e32cmn.h>
#include <sipsystemstatemonitor.h>

class CSipRfsMonitorAo : public CActive
    {
    public: // Constructors and destructor
        static CSipRfsMonitorAo* NewL();    
        ~CSipRfsMonitorAo();
        
    private: // Constructors
        CSipRfsMonitorAo();
        void ConstructL ();
        
    public: // New Functions
        CSipSystemStateMonitor::TRfsState State() const;
        void AddObserverL( MSipSystemStateObserver& aObserver );
        void RemoveObserver( MSipSystemStateObserver& aObserver );
        void EventProcessingCompleted(MSipSystemStateObserver& aObserver);
        TBool MappedState(TInt aState);
        
    private:
        void NotifyObservers();
        
    private: // From CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();

    private: // Data
        // Observers not owned
        TInt                                        iCount;
        RPointerArray<MSipSystemStateObserver>      iObservers;
        RProperty                                   iProperty;
        CSipSystemStateMonitor::TRfsState           iState;
        
    private: // For testing purposes  
        friend class CSipRfsMonitorAoTestApp;
    };
#endif /* SIPRFSMONITORAO_H */
