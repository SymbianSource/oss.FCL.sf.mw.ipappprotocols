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

#include "CMSRPConnListener.h"
#include "MSRPCommon.h"
// -----------------------------------------------------------------------------
// CMSRPConnListener::NewL
// Static constructor
// -----------------------------------------------------------------------------
//

/*static*/ MMSRPListener* CMSRPConnListener::NewL(CMSRPConnectionManager& aConnMngr)
    {
    MSRPLOG( "CMSRPConnListener::NewL enter" )
    CMSRPConnListener* self = new (ELeave) CMSRPConnListener( aConnMngr );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    MSRPLOG( "CMSRPConnListener::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::CMSRPConnListener
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPConnListener::CMSRPConnListener(CMSRPConnectionManager& aConnMngr )        
    : CActive(CActive::EPriorityStandard),
    iConnMngr( aConnMngr )
    {  
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::~CMSRPConnListener
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPConnListener::~CMSRPConnListener()
    {
    MSRPLOG( "CMSRPConnListener::~CMSRPConnListener enter" )
    delete iTimer;
    Cancel(); 
    iListenSocket.Close();    
/*    if(iDataSocket)
        {
        iDataSocket->Close();
        delete iDataSocket;
        iDataSocket = NULL;
        }*/
    MSRPLOG( "CMSRPConnListener::~CMSRPConnListener exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::ConstructL()
    {
    iState = ENotListening;
    iTimer = CMSRPTimeOutTimer::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::DoCancel
// Cancels outstanding request.
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::DoCancel()
    {
    MSRPLOG( "CMSRPConnListener::DoCancel enter" )
    iListenSocket.CancelAccept();
    //if(iDataSocket)
        {
        iDataSocket->Close();
        delete iDataSocket;
        iDataSocket = NULL;
        }
    MSRPLOG( "CMSRPConnListener::DoCancel exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::ListenL
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::ListenL()
    {
    MSRPLOG( "CMSRPConnListener::ListenL enter" )
    iTimer->Cancel();
    iListenCount++;    
    if(iState == ENotAccepting || iState == ENotListening)
        ActivateL();
    MSRPLOG( "CMSRPConnListener::ListenL exit" )
        
    }
  
// -----------------------------------------------------------------------------
// CMSRPConnListener::ListenCancel
// ListenCancel will get invoked for connections that time out
// So listener will not keep running waiting for clients that failed to connect
// -----------------------------------------------------------------------------
//

void CMSRPConnListener::ListenCancel()
    {
    MSRPLOG( "CMSRPConnListener::ListenCancel enter" )
    if(iListenCount > 0)
        iListenCount--;
    
    if(iListenCount == 0)
        {
        Cancel();    
        iState = ENotAccepting;
       iTimer->After( KListenTimeoutInSeconds * KSecondinMicroseconds );
        }
    MSRPLOG( "CMSRPConnListener::ListenCancel exit" )    
    }

// -----------------------------------------------------------------------------
// called by observer on Notify, reactivates listener and updates count
// -----------------------------------------------------------------------------
void CMSRPConnListener::ListenCompletedL(TBool aValid)
    {
    MSRPLOG( "CMSRPConnListener::ListenCompletedL enter" )
    if(!aValid)
        {
        /*ownership not transferred*/
        iDataSocket->Close();
        delete iDataSocket;        
        }
    else if ( iListenCount > 0)
        {
        iListenCount--;
        }
    iDataSocket = NULL;    
    if(iListenCount > 0)
        {
        ActivateL();
        }
    else
        {
        iState = ENotAccepting;
        iTimer->After( KListenTimeoutInSeconds * KSecondinMicroseconds );
        }
    MSRPLOG( "CMSRPConnListener::ListenCompletedL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::StartListeningL
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::ActivateL()
    {
    MSRPLOG( "CMSRPConnListener::ActivateL enter" )
    if(iState == ENotListening)
        {
        TInetAddr localAddr;
        iConnMngr.ResolveLocalIPAddressL( localAddr );
        localAddr.SetPort( KMsrpPort );
        User::LeaveIfError( iListenSocket.Open(
                iConnMngr.SocketServer(), KAfInet, KSockStream, KProtocolInetTcp, iConnMngr.SocketServerConn() ) );
        User::LeaveIfError( iListenSocket.SetOpt( KSoReuseAddr, KSolInetIp, ETrue ) );
        
        User::LeaveIfError( iListenSocket.Bind( localAddr ) );
    
        User::LeaveIfError( iListenSocket.Listen( KListenQueueSize ) );

        }
    
    iDataSocket = new (ELeave) RSocket;
    User::LeaveIfError( iDataSocket->Open( iConnMngr.SocketServer() ) );
    iListenSocket.Accept( *iDataSocket, iStatus );
    iState = EAccepting;
    SetActive();

    MSRPLOG( "CMSRPConnListener::ActivateL exit" )
    }


// -----------------------------------------------------------------------------
// CMSRPConnListener::RunL
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::RunL()
    {    
    MSRPLOG2( "CMSRPConnListener::RunL status = %d", iStatus.Int() );
 
    //switch( iState )
        {            
        if( iStatus.Int() == KErrNone )
            {              
            //if(iListenCount) 
            NotifyL(EListenAccepted);
            }
        else
            {
            //if state not stopping NotifyL(EListenPortError); else NotifyL(EStopped);
            NotifyL(EListenPortError);
            }      
         }

    MSRPLOG( "CMSRPConnListener::RunL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::RunError
// -----------------------------------------------------------------------------
//
TInt CMSRPConnListener::RunError(TInt aError)
    {    
    MSRPLOG( "CMSRPConnListener::RunError enter" );
 
    //leave on other error, terminate 
    //TRAPD(err, NotifyL(ETerminate);
    aError = KErrNone;
    MSRPLOG( "CMSRPConnListener::RunError exit" );
    return aError;
    }


// -----------------------------------------------------------------------------
// CMSRPConnListener::ChangeStateAndNotify
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::NotifyL(TListenerState aNewState)
    {
    MSRPLOG( "CMSRPConnListener::NotifyL enter" )
    iState = aNewState;
    iConnMngr.ListenerStateL( iState, iDataSocket, iStatus.Int());
    MSRPLOG( "CMSRPConnListener::NotifyL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::TimerExpired
// Callback from the timeout notifier.
// -----------------------------------------------------------------------------
//
void CMSRPConnListener::TimerExpiredL()
    {
    MSRPLOG( "CMSRPConnListener::TimerExpired enter" )
    //non leaving, no issue here as notifyl does not leave on timeout, 
    //TRAPD(err, NotifyL(ETimedOut));
    NotifyL(ETimedOut);
    MSRPLOG( "CMSRPConnListener::TimerExpired exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnListener::Stop
// Cancelling any outstanding read/write request.
// -----------------------------------------------------------------------------
//
/*
void CMSRPConnListener::StopListeningL()
    {
    iListenSocket.CancelAccept();
    NotifyL(EStopping);
    }
*/


// End of file
