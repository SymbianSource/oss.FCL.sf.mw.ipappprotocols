/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CProxyResolveActor.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CPROXYRESOLVEACTOR_H
#define CPROXYRESOLVEACTOR_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include "MQueryObserver.h"

// FORWARD DECLARATIONS
class CProxyResolvingQueue;
class CProxyQuery;
class MSIPProxyResolverObserver;


class CProxyResolveActor : 
        public CActive,
        public MQueryObserver
    {
public:

    static CProxyResolveActor* NewL ();
    
    static CProxyResolveActor* NewLC ();
    
    ~CProxyResolveActor ();
    
    // actual resolver function
    void ResolveProxyL ( TUint&                     aRequestId, 
                         TUint32                    aIapId, 
                         MSIPProxyResolverObserver& aObserver );
                       
    // cancel spesific query
    void CancelById ( TUint aRequestId );

public: // From CActive

    void RunL (); 
    TInt RunError (TInt aError);
    void DoCancel ();

private:

    void ConstructL ();
    
    CProxyResolveActor ();
    
    // process next query
    void ResolveNext ();
    
    // make the actual DHCP query
    void PerformDHCPQuery();
    
    // MQueryObserver callback: RConnection opened
    void ConnectionReady( CProxyQuery* aQuery);

    // MQueryObserver callback: RConnection failed
    void ErrorOccured( TInt aError, CProxyQuery* aQuery );
    

private: // Data

    CProxyResolvingQueue* iRequestQueue;    
    CProxyQuery* iCurrentQuery;
    RSocketServ   iServer;    

#ifdef CPPUNIT_TEST
    friend class CProxyResolveActorTest;
#endif
    };

#endif // CPROXYRESOLVEACTOR_H

// End of File
