/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CWLANCONNUSAGEPERMISSIONMONITOR_H
#define CWLANCONNUSAGEPERMISSIONMONITOR_H

//  INCLUDES

#include <e32std.h>

// FORWARD DECLARATIONS
class MWLanConnUsagePermissionObserver;
class CRepository;

// CLASS DECLARATION

/**
 * Class for subscribing events from secure backup server.
 */
class CWLanConnUsagePermissionMonitor : public CActive
    {
	public:

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CWLanConnUsagePermissionMonitor* NewL(
			MWLanConnUsagePermissionObserver& aObserver);

		/**
		* Destructor
		*/
		~CWLanConnUsagePermissionMonitor();
		
	public: // From CActive
	
	    void RunL();
	    
	    void DoCancel();
	    
	    TInt RunError( TInt aError );

    public: // new functions
    
        TBool CurrentUsagePermissionL();
        
	private:

	    /**
        * Default constructor
        */
		CWLanConnUsagePermissionMonitor( MWLanConnUsagePermissionObserver& aObserver);

	    /**
        * Symbian 2nd phase constructor
        */
		void ConstructL ();
		
    private: 
    
        void IssueMonitoringL();
        
        TBool TranslateConnectionAllowedValue( TInt aValue );

	private:

        MWLanConnUsagePermissionObserver& iObserver;
        
        CRepository* iRepository;
        
        TBool iCurrentUsagePermission;
		
#ifdef CPPUNIT_TEST
    friend class CSIPWlanPluginTest;
#endif

    };

#endif
