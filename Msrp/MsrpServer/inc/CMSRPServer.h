/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/


#ifndef CMSRPSERVER_H
#define CMSRPSERVER_H

//  INCLUDES
#include <e32base.h>

#include "MsrpCommon.h"

// FORWARD DECLARATIONS
class MMSRPConnectionManager;
class CMSRPConnectionManager;
class MMSRPConnection;
class CStateFactory;

#include "MMSRPConnectionManagerObserver.h"

// CLASS DECLARATION
/**
*  Manages sessions
*/
class CMSRPServer : public CPolicyServer, MMSRPConnectionManagerObserver
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMSRPServer* NewLC();

        /**
        * Destructor.
        */
        virtual ~CMSRPServer();

    public: // New functions

        /**
        * Access an instance of connection manager
        */
        MMSRPConnectionManager& ConnectionManager() const;
		CStateFactory& StateFactory() const;

        /**
        * Session informs server when created
        */
        void SessionCreated();
        
        void CreatingSubSessionL( const TUint aIapID );

        /**
        * Session informs server when deleted.
        */
        void SessionDeleted();

		// From the MMSRPConnectionManagerObserver

	 void HandleError(TInt aError, TInt aStatus, MMSRPConnection& aConnection);
        
        
    private: // Functions from base classes

        /**
        * From CServer2, creates a new server session.
        * @param aVersion Client interface version.
        * @param aMessage 'connect' message from the client.
        * @return Pointer to the created server session.
        */
        CSession2* NewSessionL(
            const TVersion &aVersion,
            const RMessage2& aMessage ) const;

    private:

        /**
        * C++ default constructor.
        */
        CMSRPServer();

        /**
        * private 2nd phase constructor
        */
        void ConstructL();

	private: // variables

		// pointer to connection manager instance, owned
		CMSRPConnectionManager* iConnectionManager;
		CStateFactory* iStateFactory;

		// Counter for session instance
	    TInt iSessionCount;
	};

// ----------------------------------------------------------------------------------------
// Server policy
// ----------------------------------------------------------------------------------------

// Total number of ranges
const TUint KTotalNumberOfRanges = 2;

// Definition of the ranges of IPC numbers
const TInt MSRPServerRanges[ KTotalNumberOfRanges ] =
        {
        EMSRPCreateSubSession,
        EMSRPReserved,
        };

// Policy to implement for each of the above ranges
const TUint8 MSRPServerElementsIndex[ KTotalNumberOfRanges ] =
        {
        CPolicyServer::EAlwaysPass, // 0
        CPolicyServer::ENotSupported, // 1
        };

// Specific capability checks
// not used currently
const CPolicyServer::TPolicyElement MSRPServerElements[] =
    {
	//lint -e{1924}
    {_INIT_SECURITY_POLICY_C1( ECapabilityReadDeviceData ), CPolicyServer::EFailClient}
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy MSRPServerPolicy =
        {
        CPolicyServer::EAlwaysPass, // all applications can access
        KTotalNumberOfRanges, // number of ranges
        MSRPServerRanges,    // ranges array
        MSRPServerElementsIndex, // elements<->ranges index
		MSRPServerElements
        };

#endif      // CMSRPSERVER_H

// End of File
