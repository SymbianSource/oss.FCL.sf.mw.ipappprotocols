/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MQueryObserver.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MQUERYOBSERVER_H__
#define __MQUERYOBSERVER_H__

#include <e32std.h>
class CProxyQuery;


class MQueryObserver
    {
public:

    virtual ~MQueryObserver() {}

    /**
     * A RConnection has been succesfully started and
     * the query is ready for resolving.
     *
     * @param aQuery identifies which query is ready to be resolved
     */
    virtual void ConnectionReady( CProxyQuery* aQuery) = 0;

    /**
     * Opening a RConnection has failed and
     * therefore the query cannot be resolved.
     *
     * @param aError a reason for failure
     */
    virtual void ErrorOccured( TInt aError, CProxyQuery* aQuery ) = 0;
    };

#endif // __MQUERYOBSERVER_H__
