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

// CLASS HEADER
#include "CMSRPServer.h"

// INTERNAL INCLUDE FILES
#include "MSRPCommon.h"
#include "MSRPServerCommon.h"
#include "CMSRPServerSession.h"
#include "CMSRPConnectionManager.h"
#include "TStateFactory.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMSRPServer::CMSRPServer
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPServer::CMSRPServer() :
CPolicyServer( EPriorityStandard, MSRPServerPolicy, ESharableSessions )
    {
    MSRPLOG( "MSRPServer: CMSRPServer::CMSRPServer enter" )
    MSRPLOG( "MSRPServer: CMSRPServer::CMSRPServer exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPServer::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMSRPServer::ConstructL()
    {
    MSRPLOG( "MSRPServer: CMSRPServer::ConstructL enter" )

    //const TInt err( Start( KMSRPServerName ) );
    //__ASSERT_ALWAYS( err == KErrNone, PanicServer( EMSRPServerStartError ) );
    StartL( KMSRPServerName );

    MSRPLOG( "MSRPServer: CMSRPServer::ConstructL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMSRPServer* CMSRPServer::NewLC()
    {
    MSRPLOG( "MSRPServer::NewL enter" )

	CMSRPServer* self = new( ELeave ) CMSRPServer();

    CleanupStack::PushL( self );
    self->ConstructL();

    MSRPLOG( "CMSRPServer::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPServer::~CMSRPServer
// destructor
// -----------------------------------------------------------------------------
//
CMSRPServer::~CMSRPServer()
    {
    MSRPLOG( "CMSRPServer::~CMSRPServer enter" )

    if( iStateFactory )
        {
        delete iStateFactory;
        iStateFactory = NULL;
        }
    
	delete iConnectionManager;
    MSRPLOG( "CMSRPServer::~CMSRPServer exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPServer::NewSessionL
// Creates new session if the clients version number matches with server
// version number
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2* CMSRPServer::NewSessionL(
    const TVersion &aVersion, // Clients version of the server
    const RMessage2& /*aMessage*/ ) const // Message
    {
    MSRPLOG( "CMSRPServer::NewSessionL enter" )

    // Check we're the right version.
    TVersion versionInfo( KMSRPServerMajorVersionNumber,
        KMSRPServerMinorVersionNumber,
		KMSRPServerBuildVersionNumber );

    if ( !User::QueryVersionSupported( versionInfo, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

	// let's create the session
	CMSRPServer* self = const_cast< CMSRPServer* >( this );
	CMSRPServerSession* session =
		CMSRPServerSession::NewL( *self );

    MSRPLOG( "CMSRPServer::NewSessionL exit" )
	return session;
    }

// ----------------------------------------------------------------------------
// CMSRPServer::ConnectionManager
// ----------------------------------------------------------------------------
//
MMSRPConnectionManager& CMSRPServer::ConnectionManager() const
    {
    return *iConnectionManager;
    }

CStateFactory& CMSRPServer::StateFactory() const
    {
    return *iStateFactory;
    }


// ----------------------------------------------------------------------------
// CMSRPServer::SessionCreated
// ----------------------------------------------------------------------------
//
void CMSRPServer::SessionCreated()
    {
    MSRPLOG2( "CMSRPServer::SessionCreated enter, count = %d", iSessionCount  )
	iSessionCount++;
    MSRPLOG2( "CMSRPServer::SessionCreated exit, count = %d", iSessionCount  )
    }


// ----------------------------------------------------------------------------
// CMSRPServer::SessionCreated
// ----------------------------------------------------------------------------
//
void CMSRPServer::CreatingSubSessionL( const TUint aIapID )
    {
    MSRPLOG( "CMSRPServer::CreatingSubSessionL ")
    if ( !iConnectionManager )
        {
        iConnectionManager = CMSRPConnectionManager::NewL( aIapID,*this );
        }

    if ( !iStateFactory )
        {
        iStateFactory = CStateFactory::NewL( );        
        }    
    MSRPLOG( "CMSRPServer::SubSessionCreated exit")
    }


// ----------------------------------------------------------------------------
// CMSRPServer::SessionDeleted
// ----------------------------------------------------------------------------
//
void CMSRPServer::SessionDeleted()
    {
    iSessionCount--;
    if( !iSessionCount )
        {
        delete iConnectionManager;
        iConnectionManager = NULL;
        CActiveScheduler::Stop();
        }
    }

void CMSRPServer::HandleError(TInt /*aError*/, TInt /*aStatus*/, MMSRPConnection& /*aConnection*/)
    {

    // Really no use of the observer. Connection errors should be informed via Connection!!
    
    }


//  End of File
