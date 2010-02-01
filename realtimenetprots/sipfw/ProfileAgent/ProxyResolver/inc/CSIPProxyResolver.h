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
* Name          : CSIPProxyResolver.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROXYRESOLVER_H
#define CSIPPROXYRESOLVER_H

// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MSIPProxyResolverObserver;
class CProxyResolveActor;

#ifdef CPPUNIT_TEST
#undef IMPORT_C
#define IMPORT_C
#endif

class CSIPProxyResolver : public CBase
    {
public:

    /**
     * Creates a new CSIPProxyResolver instance
     * @returns a new instance of CSIPProxyResolver
     */
    IMPORT_C static CSIPProxyResolver* NewL ();

    /**
     * Creates a new CSIPProxyResolver instance and pushes it to CleanupStack
     * @returns a new instance of CSIPProxyResolver
     */
    IMPORT_C static CSIPProxyResolver* NewLC ();

    /**
     * Destructor
     */
    IMPORT_C ~CSIPProxyResolver ();

    /**
     * Resolves a list of outbound proxies asynchronously.
     * Multiple resolving request can be outstanding for the same observer.
     * This is why request ids are needed.
     * CSIPProxyResolver implements a queue of the requests.
     * Once a request completes the first is taken from the queue
     * and resolved. If the queue is empty, new requests are waited.
     *
     * @param aRequestId on return contains a unique id 
     *        of the resolving request
     * @param aIapId the IAP id to be used
     * @param aObserver a callback to be called when resolving 
     *        completes succefully or an error occures.
     */
    IMPORT_C void ResolveProxyL (TUint& aRequestId,
                                 TUint32 aIapId,
                                 MSIPProxyResolverObserver& aObserver);

    /**
     * Cancels an outstanding resolving request.
     * 
     * @param aRequestId the id of the resolving request to be cancelled
     *        issued by calling ResolveProxyL.
     */
    IMPORT_C void Cancel (TUint aRequestId);

private:

    CSIPProxyResolver ();
    void ConstructL ();
    
private: // data
    
    // active object handles the resolving request asynchronously
    CProxyResolveActor* iResolver;
    };

#endif // CSIPPROXYRESOLVER_H

// End of File
