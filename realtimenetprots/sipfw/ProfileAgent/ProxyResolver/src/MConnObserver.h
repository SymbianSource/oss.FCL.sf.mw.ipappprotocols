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
* Name          : MConnObserver.h
* Part of       : ProxyResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MCONNOBSERVER_H__
#define __MCONNOBSERVER_H__

#include <e32std.h>

class MConnObserver
    {
public:

    virtual ~MConnObserver() {}

    /**
     * A RConnection has been succesfully started and is ready to be used. 
     */
    virtual void ConnectionReady() = 0;

    /**
     * Starting a RConnection failed.
     *
     * @param aError a reason why starting failed
     */
    virtual void ErrorOccured( TInt aError ) = 0;
    };

#endif // __MCONNOBSERVER_H__
