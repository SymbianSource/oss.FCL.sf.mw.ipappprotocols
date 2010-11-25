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

// INTERNAL INCLUDES
#include "CMSRPConnector.h"
#include "MSRPCommon.h"

// -----------------------------------------------------------------------------
// CMSRPConnListener::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
/*static*/ MMSRPConnector* CMSRPConnector::NewL(MMSRPConnectionManager& aConnMngr, MMSRPConnectorObserver& aConnection)
    {
    MSRPLOG( "CMSRPConnector::NewL enter" )
    CMSRPConnector* self = new (ELeave) CMSRPConnector( aConnMngr, aConnection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    MSRPLOG( "CMSRPConnector::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::CMSRPConnector
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPConnector::CMSRPConnector(MMSRPConnectionManager& aConnMngr, MMSRPConnectorObserver& aConnection)        
    : CActive(CActive::EPriorityStandard), iConnMngr(aConnMngr), iConnection( aConnection )
    {  
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::~CMSRPConnector
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPConnector::~CMSRPConnector()
    {
    MSRPLOG( "CMSRPConnector::~CMSRPConnector enter" )
    delete iTimer;
    Cancel();    
    if(iSocket)
        {
        iSocket->Close();
        delete iSocket;
        }
    MSRPLOG( "CMSRPConnector::~CMSRPConnector exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMSRPConnector::ConstructL()
    {
    MSRPLOG( "-> CMSRPConnector::ConstructL" )
    //iState = ENotConnected;
    iTimer = CMSRPTimeOutTimer::NewL( *this );
    //create socket

    iSocket = new (ELeave) RSocket();
    RSocketServ& socketServ = iConnMngr.SocketServer();
    RConnection& connection = iConnMngr.SocketServerConn();
    TInt error = iSocket->Open( socketServ, KAfInet, KSockStream, KProtocolInetTcp, connection );
    MSRPLOG2( "CMSRPConnector::ConstructL = open error = %d", error )
    if ( error )
        {
        // if open fails, let's try to restart connection
        MSRPLOG( "CMSRPConnector::ConstructL restarting interface" )
        iConnMngr.ReStartInterface();
        error = iSocket->Open( socketServ, KAfInet, KSockStream, KProtocolInetTcp, connection );
        }
    User::LeaveIfError( error );
    error = iSocket->SetOpt( KSoReuseAddr, KSolInetIp, ETrue );
    MSRPLOG2( "CMSRPConnector::ConstructL = opt error = %d", error )
    User::LeaveIfError( error );
    iSocket->SetLocalPort( KMsrpPort );
    MSRPLOG( "<- CMSRPConnector::ConstructL" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::DoCancel
// Cancels outstanding request.
// -----------------------------------------------------------------------------
//
void CMSRPConnector::DoCancel()
    {
    MSRPLOG( "CMSRPConnector::DoCancel enter" )
    iSocket->CancelConnect();
    MSRPLOG( "CMSRPConnector::DoCancel exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::Connect
// -----------------------------------------------------------------------------
//
void CMSRPConnector::Connect(TInetAddr& aRemoteAddr)
    {
    MSRPLOG( "CMSRPConnector::Connect enter" )
    iSocket->Connect( aRemoteAddr, iStatus );
    //iState = EConnecting;
    SetActive();
    iTimer->After( KTimeOutInSeconds * KSecondinMicroseconds );
    MSRPLOG( "CMSRPConnector::Connect exit" )
    }


// -----------------------------------------------------------------------------
// CMSRPConnector::TimerExpired
// -----------------------------------------------------------------------------
//
void CMSRPConnector::TimerExpiredL()   
    {
    MSRPLOG( "CMSRPConnector::TimerExpired enter" )
    iConnection.ConnectionEstablishedL(MMSRPConnection::EConnectTimedOut, iSocket, KErrTimedOut);
    MSRPLOG( "CMSRPConnector::TimerExpired exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::RunL
// -----------------------------------------------------------------------------
//
void CMSRPConnector::RunL()
    {
    MSRPLOG( "CMSRPConnector::RunL enter" )
    iTimer->Cancel();
    
    if(iStatus.Int() != KErrNone)
        {
        MSRPLOG2( "CMSRPConnector::RunL Connection Error %d", iStatus.Int() );
        iConnection.ConnectionEstablishedL(MMSRPConnection::EError, iSocket, iStatus.Int());
        return;
        }
    
    iConnection.ConnectionEstablishedL(MMSRPConnection::EConnected, iSocket, KErrNone);
    MSRPLOG( "CMSRPConnector::RunL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnector::RunError
// -----------------------------------------------------------------------------
//
TInt CMSRPConnector::RunError(TInt aError)
    {
    //handle error delete connector on error
    //make sure ConnectorStateL does not leave on EError
    MSRPLOG( "CMSRPConnector::RunError enter" )
    TRAPD(err, iConnection.ConnectionEstablishedL(MMSRPConnection::EError, iSocket, aError));
    MSRPLOG( "CMSRPConnector::RunError exit" )
    return err;
    }
    
// -----------------------------------------------------------------------------
// CMSRPConnector::ConnectComplete
// -----------------------------------------------------------------------------
//
void CMSRPConnector::ConnectComplete()
    {
    MSRPLOG( "CMSRPConnector::ConnectComplete enter" )
    iSocket = NULL;
    MSRPLOG( "CMSRPConnector::ConnectComplete exit" )
    }


// End of File
