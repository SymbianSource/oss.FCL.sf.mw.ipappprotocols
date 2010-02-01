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
* Name          : CProxyQuery.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CPROXYQUERY_H
#define CPROXYQUERY_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <comms-infras/es_config.h>
#include <badesca.h>
#include "MConnObserver.h"
#include "CProxyConnectionContainer.h"

// FROWARD DECLARATIONS
class MQueryObserver;
class MSIPProxyResolverObserver;
class CProxyConnectionContainer;

class CProxyQuery : 
        public CBase,
        public MConnObserver
    {
public:

    static CProxyQuery* NewL(  TUint                      aRequestId, 
                               TUint32                    aIAPId,
                               MSIPProxyResolverObserver& aResolverObserver,
                               MQueryObserver&            aQueryObserver,
                               RSocketServ&               aServer );

    static CProxyQuery* NewLC( TUint                      aRequestId,
                               TUint32                    aIAPId,
                               MSIPProxyResolverObserver& aResolverObserver,
                               MQueryObserver&            aQueryObserver,
                               RSocketServ&               aServer );
    // destructor
    ~CProxyQuery ();
    
    // get the RConnection object
    RConnection & Connection();
    
    // flag for opening status
    TBool ConnectionOpened();

    // ioctl queries passed ok
    void ResolvingRequestComplete();
    
    // ioctl queries failed
    void ResolvingRequestFailed( TInt aError );

    //get the request id
    TUint RequestId() const;

    // get the IAP id
    TUint32 IAPId() const;

    // callback fron CProxyConnectionContainer indicating failure
    void ErrorOccured( TInt aError );
    
    // callback fron CProxyConnectionContainer: RConnection is opened
    void ConnectionReady();
            
    //Get the current query type
    TUint CurrentQueryType();
        
    // access to the current buffer (TSipServerAddrBuf/TSipServerDomainBuf)
    TDes8*  GetBuf();

    //Increase the index with one
    void SetNextIndex();
    
    // Set current query type
    void SetQueryType(const TUint aNewType);
    
    //Add results of ioctl query to result array
    void AddResultsL();
        
public: // Data
    
    static const TInt iOffset;
    TSglQueLink iLink;  

private: // Private methods

    void ConstructL();
    CProxyQuery( TUint                      aRequestId, 
                 TUint32                    aIAPId,
                 MSIPProxyResolverObserver& aResolverObserver,
                 MQueryObserver&            aQueryObserver,
                 RSocketServ&               aServer );

private: // Data

    TUint                       iRequestId;             // request id 
    TUint32                     iIAPId;                 // IAP id
    MSIPProxyResolverObserver&  iResolverObserver;      // observer (user)
    MQueryObserver&             iQueryObserver;         // observer (CProxyResolveActor)
    CProxyConnectionContainer*  iConnCont;              // RConnection class
    TBool                       iConnectionOpened;      // flag for RConnection status
    RSocketServ&                iServer;                // socket server
    TUint                       iCurrentQueryType;      // current query type
    TSipServerAddrBuf           iSipServerAddrBuf;      //Address buffer        
    TSipServerDomainBuf         iSipServerDomainBuf;    //Domain name buffer
    CDesC8ArrayFlat*            iResultsArray;          // result array
       
private:

#ifdef CPPUNIT_TEST
    friend class CProxyQueryTest;    
    friend class CProxyResolvingQueueTest;
    friend class CProxyResolveActorTest;    
#endif
    };
    
#endif // CPROXYQUERY_H

// End of File
