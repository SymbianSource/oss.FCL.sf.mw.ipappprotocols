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


#ifndef CSYSTEMSTATECONNUSAGEPERMISSIONMONITOR_H
#define CSYSTEMSTATECONNUSAGEPERMISSIONMONITOR_H

//  INCLUDES

#include <e32std.h>

// FORWARD DECLARATIONS
class MSystemStateConnUsagePermissionObserver;
class CRepository;

// CLASS DECLARATION

/**
 * Class for subscribing events from secure backup server.
 */
class CSystemStateConnUsagePermissionMonitor : public CActive
    {
	public:

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSystemStateConnUsagePermissionMonitor* NewL();

		/**
		* Destructor
		*/
		~CSystemStateConnUsagePermissionMonitor();
		
	    /**
        * Add obsever
        */
	    void AddObserverL( MSystemStateConnUsagePermissionObserver& aObserver );
		
	    /**
        * Remove observer
        */
		void RemoveObserver( MSystemStateConnUsagePermissionObserver& aObserver );

	public: // From CActive
	
	    void RunL();
	    
	    void DoCancel();
	    
	    TInt RunError( TInt aError );

    public: // new functions
    
        TInt CurrentUsagePermission();
        
	private:

		CSystemStateConnUsagePermissionMonitor();
		void ConstructL ();
		
    private: 
    
        void IssueMonitoringL();
        TBool TranslateConnectionAllowedValue( TInt aValue );

	private:

        TInt iCurrentUsagePermission;
        RPointerArray<MSystemStateConnUsagePermissionObserver> iObservers;  
        CRepository* iRepository;
		
#ifdef CPPUNIT_TEST
    friend class CSipSystemStateMonitorImplTest;
#endif

    };

#endif
