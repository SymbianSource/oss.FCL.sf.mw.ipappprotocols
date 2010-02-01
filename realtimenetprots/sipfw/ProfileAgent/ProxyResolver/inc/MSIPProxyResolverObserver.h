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
* Name          : MSIPProxyResolverObserver.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPPROXYRESOLVER_H
#define MSIPPROXYRESOLVER_H

// INCLUDES
#include <e32base.h>
#include <bamdesca.h>

class MSIPProxyResolverObserver
    {
public:

    virtual ~MSIPProxyResolverObserver() {}

    /**
     * An asynchronous proxy resolving request has been completed.
     *
     * @param aRequestId the id of the resolving request.
     * @param aProxies a list of proxy addresses.
     *        The ownership is transferred.
     */
    virtual void ProxyResolvingRequestComplete( TUint aRequestId,
                                                MDesC8Array* aProxies ) = 0;

    /**
     * An asynchronous proxy resolving request has failed.
     *
     * @param aRequestId the id of the resolving request.
     * @param aError a reason why the request failed.
     */
    virtual void ProxyResolvingRequestFailed( TUint aRequestId,
                                              TInt aError) = 0;
    };

#endif // MSIPPROXYRESOLVER_H