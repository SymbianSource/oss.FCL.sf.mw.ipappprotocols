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
* Name          : sipclientresolverconfig.h
* Part of       : SIP / SIP Client Resolver
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTRESOLVERCONFIG_H
#define CSIPCLIENTRESOLVERCONFIG_H

//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* A class for retrieving SIP Client Resolver plug-in UIDs for a username.
* @lib sipresolver.lib
*/
NONSHARABLE_CLASS(CSipClientResolverConfig) : public CBase
    {
    public:  // Constructors and destructor

        static CSipClientResolverConfig* NewL();
        
        static CSipClientResolverConfig* NewLC();        

        ~CSipClientResolverConfig();

    public: // New functions
    
        void GetUIDsForUserNameL(
            const TDesC8& aUserName,
            RArray<TUid>& aUids);

    private: // Second phase constructors
	
        CSipClientResolverConfig();
        void ConstructL();	    
	    
    private: // Data

        CRepository* iCenRep;
        
    private: // For testing pruposes
#ifdef CPPUNIT_TEST
        friend class CSipClientResolverConfigTest;
#endif
    };

#endif // CSIPCLIENTRESOLVERCONFIG_H

// End of File
