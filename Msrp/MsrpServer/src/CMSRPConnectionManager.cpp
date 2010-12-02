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
#include "CMSRPConnectionManager.h"
#include "MSRPCommon.h"
#include "CMSRPConnection.h"
#include "CMSRPConnListener.h"
#undef __SERIES60_
#if defined(__SERIES60_)
#include <extendedconnpref.h>
#endif

// INTERNAL INCLUDES

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPConnectionManager::CMSRPConnectionManager( const TUint32 aIapID, MMSRPConnectionManagerObserver& aServer )
                                                : iIapID( aIapID ), iObserver( aServer )
    {
    MSRPLOG( "CMSRPConnectionManager::CMSRPConnectionManager enter" )
    MSRPLOG( "CMSRPConnectionManager::CMSRPConnectionManager exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPConnectionManager* CMSRPConnectionManager::NewL
// -----------------------------------------------------------------------------
//
/*static*/CMSRPConnectionManager* CMSRPConnectionManager::NewL( const TUint32 aIapID, MMSRPConnectionManagerObserver& aServer )
    {
    MSRPLOG( "CMSRPConnectionManager::NewL enter" )

	// Perform construction.
    CMSRPConnectionManager* self = new ( ELeave ) CMSRPConnectionManager( aIapID, aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    MSRPLOG( "CMSRPConnectionManager::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPConnectionManager::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPConnectionManager::ConstructL( )
	{
    MSRPLOG( "CMSRPConnectionManager::ConstructL enter" )

	TInt status = iSocketServer.Connect();
	if (status != KErrNone)
	    {
	    //iObserver->HandleError(ESocketServerStartFailed, status , *this );//need to pass connmngr as server handles many connmngrs
	    //User::Leave(status);
	    User::Leave(MMSRPConnectionManagerObserver::ESocketServerStartFailed); //leave code instead of observer call, as in constructl itself
	    }

    status = StartInterface();
    if (status != KErrNone)
        {
        User::Leave(MMSRPConnectionManagerObserver::EInterfaceStartFailed);
        }

    MSRPLOG( "CMSRPConnectionManager::ConstructL exit" )
	}


CMSRPConnectionManager::~CMSRPConnectionManager()
	{
    MSRPLOG( "CMSRPConnectionManager::~CMSRPConnectionManager enter" )

	delete iConnListener;
	iConnectionArray.ResetAndDestroy();
	iConnectionArray.Close();
	iHostResolver.Close();
	iConnection.Close();
	iSocketServer.Close();

	MSRPLOG( "CMSRPConnectionManager::~CMSRPConnectionManager exit" )
	}


// -----------------------------------------------------------------------------
// CMSRPConnectionManager::ConnectL
// -----------------------------------------------------------------------------
//
MMSRPConnection& CMSRPConnectionManager::getConnectionL( TDesC8& aHost, TUint aPort )
	{
    MSRPLOG( "CMSRPConnectionManager::getConnectionL enter" )

    TInetAddr hostAddr;
    TRAPD(err, ResolveIPAddressL( aHost, hostAddr ));
    if(err != KErrNone)
        {
        //iObserver->HandleError( MMSRPConnectionManagerObserver::EAddressResolveError, err , *this );
        User::Leave(MMSRPConnectionManagerObserver::EAddressResolveError);
        }
    hostAddr.SetPort(aPort);

	MMSRPConnection* connection = CheckConnection( hostAddr, FALSE );

	if ( !connection )
		{
        connection = CMSRPConnection::NewL( hostAddr, *this );
        CleanupDeletePushL(connection);//as M class
        //CleanupStack::PushL( connection );
        iConnectionArray.AppendL( connection );
        CleanupStack::Pop( );
		}

    MSRPLOG( "CMSRPConnectionManager::getConnectionL exit" )

	return *connection;
	}


// -----------------------------------------------------------------------------
// CMSRPConnectionManager::StartListener
//connection calls connmngr listen,  which in turn calls listener listen
//if state not listening
// -----------------------------------------------------------------------------
//
void CMSRPConnectionManager::ListenL(MMSRPConnection* /*aConnection*/)
    {
    MSRPLOG( "CMSRPConnectionManager::ListenL enter" )
    if(!iConnListener)
        {
        iConnListener = CMSRPConnListener::NewL(*this);
        }

    TRAPD(err,iConnListener->ListenL());

    if(err != KErrNone)
        {
        //delete iConnListener;
        //iConnListener = NULL;
        //iObserver->HandleError( MMSRPConnectionManagerObserver::EListenerStartError, err , *this );//communicate to server to trigger cleanup

        /*subsession can propagate to server (based on error), which handles error*/
        User::Leave(MMSRPConnectionManagerObserver::EListenerStartError); //listenL leaves to subsession
        }

    MSRPLOG( "CMSRPConnectionManager::ListenL exit" )

    }

// -----------------------------------------------------------------------------
// CMSRPConnectionManager::ListenCancel
//connection listencancel calls mngr cancel if one registered subsession observer or state notlistening
// -----------------------------------------------------------------------------
//
void CMSRPConnectionManager::ListenCancel(MMSRPConnection* aConnection)
    {
    MSRPLOG( "CMSRPConnectionManager::ListenCancel enter" )
    if(iConnListener)
        {
        iConnListener->ListenCancel();
        }
    Remove(aConnection);

    MSRPLOG( "CMSRPConnectionManager::ListenCancel exit" )
    }


// -----------------------------------------------------------------------------
// CMSRPConnectionManager::ListenerState
// -----------------------------------------------------------------------------
//
void CMSRPConnectionManager::ListenerStateL( TInt aNewState, RSocket* aDataSocket, TInt aStatus )
    {
    MSRPLOG( "CMSRPConnectionManager::ListenerStateL enter" )
    if (aNewState == MMSRPListener::EListenAccepted)
        {
        TInetAddr remoteAddr;
        aDataSocket->RemoteName(remoteAddr);

        MMSRPConnection* connection = CheckConnection( remoteAddr, TRUE );

        if ( !connection )
            {
            iConnListener->ListenCompletedL(FALSE);
            }
        else
            {
            iConnListener->ListenCompletedL(TRUE);
            TRAPD(err, connection->ConnectionEstablishedL(MMSRPConnection::EConnected, aDataSocket, aStatus ));
            if(err!=KErrNone)
                //non leaving on error, introduce separate fn later
                connection->ConnectionEstablishedL(MMSRPConnection::EError, aDataSocket, err );
            }
        }
    else if (aNewState == MMSRPListener::ETimedOut || aNewState == MMSRPListener::ETerminate)
        {
        delete iConnListener;
        iConnListener = NULL;
        //all listening connections, auto deleted on listen timeout in conn
        }
    else if (aNewState == MMSRPListener::EListenPortError)
        {
        delete iConnListener;
        iConnListener = NULL;
        User::Leave(MMSRPConnectionManagerObserver::EListenPortFailed);
        //leave here, propagate upto msrpscheduler, in scheduler error intimate server to handle the error
        //instead could intimate observer (server) directly
        //iObserver->HandleError( MMSRPConnectionManagerObserver::EListenPortFailed, aStatus , *this );
        }
    MSRPLOG( "CMSRPConnectionManager::ListenerStateL exit" )
    }


// -----------------------------------------------------------------------------
// CMSRPConnectionManager::CheckConnection
// -----------------------------------------------------------------------------
//
MMSRPConnection* CMSRPConnectionManager::CheckConnection( TInetAddr& aHostAddress, TBool aListenOnly )
	{
	MSRPLOG( "CMSRPConnectionManager::CheckConnection enter" )
	for( TInt i = 0; i < iConnectionArray.Count(); i++ )
		{
		if ( iConnectionArray[ i ]->CheckConnection( aHostAddress, aListenOnly ) )
			{
			return iConnectionArray[ i ];
			}
		}
	MSRPLOG( "CMSRPConnectionManager::CheckConnection exit" )
	return NULL;
	}


// -----------------------------------------------------------------------------
// CMSRPConnectionManager::StartInterfaceL
// Activates PDP ctx
// -----------------------------------------------------------------------------
//
TInt CMSRPConnectionManager::StartInterface()
    {

    MSRPLOG( "CMSRPConnectionManager::StartInterface enter" )

    TInt status = KErrNone;
    //if( !iConnection.SubSessionHandle() )
        {
        status = iConnection.Open( iSocketServer );
        }

#if defined(__SERIES60_)
    if (status == KErrNone)
    {
        TConnPrefList  prefList;
        TExtendedConnPref extPrefs;

        extPrefs.SetIapId( iIapID );
        TRAP(status, prefList.AppendL(&extPrefs));
        if (status == KErrNone)
        {
        status = iConnection.Start( prefList );
        }
    }
#else
    if (status == KErrNone)
        {
        iConnectionPreferences.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        iConnectionPreferences.SetDirection( ECommDbConnectionDirectionUnknown );
        iConnectionPreferences.SetIapId( iIapID );

        status = iConnection.Start( iConnectionPreferences );
        }
#endif

    MSRPLOG( "CMSRPConnectionManager::StartInterface exit" )
    return status;

    }

// -----------------------------------------------------------------------------
// CMSRPConnectionManager::ReStartInterface
// -----------------------------------------------------------------------------
//
TInt CMSRPConnectionManager::ReStartInterface()
    {
    iConnection.Close();
    iSocketServer.Close();
    
    TInt error = iSocketServer.Connect();
    if ( !error )
        {
        error = StartInterface();
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CMSRPConnectionManager::ResolveIPAddressL
// -----------------------------------------------------------------------------
//
void CMSRPConnectionManager::ResolveIPAddressL(
    const TDesC8& aHost,
    TInetAddr& aAddress )
    {
    MSRPLOG( "CMSRPConnectionManager::ResolveIPAddressL enter" )
    //RHostResolver hostResolver;
    TNameEntry entry;
    TBuf< KMaxLengthOfUrl > hostAddr;

    if( !iHostResolver.SubSessionHandle() )
        {
        User::LeaveIfError( iHostResolver.Open( iSocketServer, KAfInet,
                                          KProtocolInetTcp, iConnection ));
        }

    //CleanupClosePushL( hostResolver );

    hostAddr.Copy( aHost );
    User::LeaveIfError( iHostResolver.GetByName( hostAddr, entry ) );
    if ( !TInetAddr::Cast( entry().iAddr ).IsWildAddr() )
         {
         aAddress = TInetAddr::Cast( entry().iAddr );
         }
    iHostResolver.Close();
    //CleanupStack::PopAndDestroy( );
    MSRPLOG( "CMSRPConnectionManager::ResolveIPAddressL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPIncomingListener::ResolveLocalIPAddressL
// Resolves the local IP address.
// -----------------------------------------------------------------------------
//
void CMSRPConnectionManager::ResolveLocalIPAddressL(TInetAddr& aLocalAddr)
    {
    MSRPLOG( "CMSRPConnectionManager::ResolveLocalIPAddressL enter" )
    if (!iLocalAddr.IsUnspecified())
        {
        aLocalAddr = iLocalAddr;
        return;
        }
    /* Open an RSocket and find its address */
    RSocket socket;
    User::LeaveIfError(socket.Open(iSocketServer, KAfInet, KSockDatagram, KProtocolInetUdp,iConnection));

    CleanupClosePushL(socket);

    TSoInetInterfaceInfo networkInfo;
    TPckg<TSoInetInterfaceInfo> opt(networkInfo);
    User::LeaveIfError(socket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl));
    TSoInetIfQuery query;
    TPckg<TSoInetIfQuery> queryBuf(query);
    TInt res = KErrNone;
    do
    {
        res = socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, opt);
        if (!res && opt().iState == EIfUp)
        {
            query.iName = opt().iName;
            res = socket.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, queryBuf);
            if (!res && query.iZone[1] == iIapID)
            {
                networkInfo = opt();
                break;
            }
        }
    }
    while (res == KErrNone);
    User::LeaveIfError(res);
    CleanupStack::PopAndDestroy(1);
    aLocalAddr = networkInfo.iAddress;
    iLocalAddr = aLocalAddr;
    MSRPLOG( "CMSRPConnectionManager::ResolveLocalIPAddressL exit" )
    }

void CMSRPConnectionManager::Remove(MMSRPConnection* aConn)
    {
    MSRPLOG( "CMSRPConnectionManager::Remove connection enter" )
    TInt index = iConnectionArray.Find(aConn);

    delete iConnectionArray[index];
    iConnectionArray.Remove(index);
    MSRPLOG( "CMSRPConnectionManager::Remove connection exit" )
    }


RSocketServ& CMSRPConnectionManager::SocketServer()
    {
    MSRPLOG( "CMSRPConnectionManager::SocketServer fetch enter" )
    MSRPLOG( "CMSRPConnectionManager::SocketServer fetch exit" )
    return iSocketServer;

    }

RConnection& CMSRPConnectionManager::SocketServerConn()
    {
    MSRPLOG( "CMSRPConnectionManager::SocketServerConn fetch enter" )
    MSRPLOG( "CMSRPConnectionManager::SocketServerConn fetch exit" )
    return iConnection;
    }

/*
TBool CMSRPConnectionManager::MatchIapId(TUint32 aIapID)
    {
    return (aIapID == iIapID);
    }
*/


// End of File
