
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDES
#include    "SIPExSocketEngine.h"
#include    "SIPExSocketWriter.h"
#include    "SIPExSocketReader.h"
#include    "SIPExTimeOutTimer.h"
#include    <e32svr.h>
#include    <commdbconnpref.h>

// CONSTANTS
const TInt KGamePort( 10011 );

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::NewL
// Creates the instance of class and returns it.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSocketEngine* CSIPExSocketEngine::NewL( 
    MSIPExSocketEngineObserver& aObserver )
    {
    CSIPExSocketEngine* self = CSIPExSocketEngine::NewLC( aObserver );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::NewLC
// Creates the instance of class and pushes it to the CleanupStack and return
// it.
// -----------------------------------------------------------------------------
//    
EXPORT_C CSIPExSocketEngine* CSIPExSocketEngine::NewLC( 
    MSIPExSocketEngineObserver& aObserver )
    {
    CSIPExSocketEngine* self = new (ELeave) CSIPExSocketEngine( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::CSIPExSocketEngine
// Calls base classes constructor with priority value. Add class to the 
// active sheduler.
// -----------------------------------------------------------------------------
//
CSIPExSocketEngine::CSIPExSocketEngine( MSIPExSocketEngineObserver& aObserver )
:   CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::~CSIPExSocketEngine
// Cancels any outstanding requests and deletes members.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExSocketEngine::~CSIPExSocketEngine()
    {
    Cancel();
    
	if ( iTimer )
		{
		iTimer->Cancel();
		delete iTimer;
		}

	if ( iWriter )
		{
		iWriter->Cancel();
		delete iWriter;
		}

	if ( iReader )
		{
		iReader->Cancel();
		delete iReader;
		}

    iListenSocket.Close();
    iDataSocket.Close();
    iConnection.Close();
    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::ConstructL
// The socketserver is connected and members created.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::ConstructL()
    {
    iState = ENotConnected;

    iTimer = CSIPExTimeOutTimer::NewL( *this );

    // Open channel to Socket Server
	User::LeaveIfError( iSocketServer.Connect() );

    iReader = CSIPExSocketReader::NewL( iDataSocket, *this );
    iWriter = CSIPExSocketWriter::NewL( iDataSocket, *this );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::DoCancel
// From CActive. Cancels any outstanding request according the engine state.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::DoCancel()
    {    
    iTimer->Cancel();

    // Cancel appropriate request to socket
	switch ( iState )
		{
	    case EConnecting:
		    iDataSocket.CancelConnect();
		    break;

        case EListening:
            iListenSocket.CancelAccept();
            break;
        
        default:
            break;
        }    
        
    iConnection.Close();
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::RunL
// From CActive. Handles the state changes and notifing the observer.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::RunL()
    {
    iTimer->Cancel();

    switch( iState )
        {
        case EConnecting:
            if( iStatus.Int() == KErrNone )
                {
                ChangeStateAndNotify( EConnected );
                Read();
                }
            else
                {
                ChangeStateAndNotify( ENotConnected );
                }
            break;

        case EListening:
			{
            iListenSocket.Close();
            if( iStatus.Int() == KErrNone )
                {
                ChangeStateAndNotify( EConnected );
                Read();
                }
            else
                {
                ChangeStateAndNotify( ENotConnected );
                }
			}
            break;

        case ETimedOut:
            if( iStatus.Int() == KErrNone )
                {
                iObserver.SocketState( ETimedOut );
                iState = ENotConnected;
                }
            else
                {
                ChangeStateAndNotify( ENotConnected );
                }
            break;

        default:
            iObserver.SocketState( iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::ConnectL
// Connects the socket to the remote host. Asynchronous.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExSocketEngine::ConnectL( 
    const TUint32 aIapId, 
    const TInetAddr& aIPAddress )
    {
    if( iState != ENotConnected )
        {
        return;
        }

    // Make first sure that underlying interface is active
    StartInterfaceL( aIapId );
    
	// Open a TCP socket
	User::LeaveIfError( iDataSocket.Open( 
	    iSocketServer, KAfInet, KSockStream, KProtocolInetTcp, iConnection ) );
    
    iAddress = aIPAddress;
	iAddress.SetPort( KGamePort );

    // The Connect() allocate local address automatically if 
    // not bind yet.
    iDataSocket.Connect( iAddress, iStatus );
         
    iState = EConnecting;
    SetActive();
    iTimer->After( KTimeOut );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::Write
// Starts the asynchronous writing
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExSocketEngine::Write( const TDesC8& aTxt )
    {
    if( iState == EConnected && !iWriter->IsActive() )
        {
        iWriter->Write( aTxt );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::Read
// Starts the asynchronous reading.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExSocketEngine::Read()
    {
    if( iState == EConnected && !iReader->IsActive() )
        {
        iReader->Read();
        }
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::Stop
// Cancelling any outstanding read/write request.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExSocketEngine::Stop()
    {
    switch( iState )
        {
        case EConnected:
			{
            iWriter->Cancel();
            iReader->Cancel();
            TRequestStatus status;
            iDataSocket.Shutdown( RSocket::EImmediate, status );
            User::WaitForRequest( status);
            iConnection.Close();
            ChangeStateAndNotify( ENotConnected );
			}
            break;
        case EListening:
        case EConnecting:
            Cancel();
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::StartListeningL
// Resolves the local address and starts listening game port. Asynchronous.
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr& CSIPExSocketEngine::StartListeningL( const TUint32 aIapId )
    {
    StartInterfaceL( aIapId );
    ResolveLocalIPAddressL();
    iAddress.SetPort( KGamePort );
    User::LeaveIfError( iListenSocket.Open( 
            iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
    User::LeaveIfError( iDataSocket.Open( iSocketServer ) );
    User::LeaveIfError( iListenSocket.Bind( iAddress ) );
    
    User::LeaveIfError( iListenSocket.Listen( 1 ) );

    iListenSocket.Accept( iDataSocket, iStatus );    
    iState = EListening;
    SetActive();

    return iAddress;
    }
    
// -----------------------------------------------------------------------------
// CSIPExSocketEngine::StartInterfaceL
// Activates PDP ctx
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::StartInterfaceL( const TUint32 aIapId )
    {
    if( !iConnection.SubSessionHandle() )
        { 
        User::LeaveIfError( iConnection.Open( iSocketServer ) );
        }

    TCommDbConnPref prefs;
    prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    prefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    prefs.SetIapId( aIapId );

    User::LeaveIfError( iConnection.Start( prefs ) );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::ResolveLocalIPAddressL
// Resolves the local IP address.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::ResolveLocalIPAddressL()
    {
    RHostResolver hostResolver;
    TNameEntry entry;

    User::LeaveIfError( hostResolver.Open( iSocketServer, KAfInet, 
                                          KProtocolInetTcp, iConnection ));
    CleanupClosePushL( hostResolver );
 
    User::LeaveIfError( hostResolver.GetByName( TPtrC(), entry ) );
    if ( !TInetAddr::Cast( entry().iAddr ).IsWildAddr() )
         {
         iAddress = TInetAddr::Cast( entry().iAddr );
         }
         
    CleanupStack::PopAndDestroy(); // hostResolver
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::TimerExpired
// Callback from the timeout notifier. Completes the request with error code
// KErrNone and state ETimedOut.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::TimerExpired()
    {
    Cancel();
	iState = ETimedOut;
	
    TRequestStatus* status = &iStatus;		
    SetActive();
	User::RequestComplete( status, KErrNone );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::MessageReceived
// Notifies the observer.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::MessageReceived( TDesC8& aBuffer )
    {
    iObserver.SocketData( aBuffer );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::ErrorInReading
// Completes the request with error code.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::ErrorInReading( TInt aError )
    {
    TRequestStatus* status = &iStatus;		
    SetActive();
	User::RequestComplete( status, aError );
    }

// From MWriterNotifier
// -----------------------------------------------------------------------------
// CSIPExSocketEngine::WriteDone
// 
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::WriteDone()
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::ErrorInWriting
// Completes the request with error code.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::ErrorInWriting( TInt aError )
    {
    TRequestStatus* status = &iStatus;		
    SetActive();
	User::RequestComplete( status, aError );
    }

// -----------------------------------------------------------------------------
// CSIPExSocketEngine::ChangeStateAndNotify
// Changes new state and notifies observer.
// -----------------------------------------------------------------------------
//
void CSIPExSocketEngine::ChangeStateAndNotify( TSocketEngineState aNewState )
    {
    iState = aNewState;
    iObserver.SocketState( iState );
    }

// End of file
