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
* Name          : MSIPServerResolverObserver.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPSERVERRESOLVEROBSERVER_H
#define MSIPSERVERRESOLVEROBSERVER_H

class MSIPResolvingResult;


class MSIPServerResolverObserver
	{
public:

    /**
     * Destructor.
	 */
    virtual ~MSIPServerResolverObserver() {}

    /**
     * Resolving was completed successfully. 
	 * 1..n (IP, port, transport) tuples returned.
     *
     * If this function leaves, 
     * ErrorOccured will be called with the leave code
     */
	virtual void CompletedL() = 0;

    /**
     * An error occured while resolving or no tuples found.
     *
     * @param aError the reason for the failure.
     */
	virtual void ErrorOccured( TInt aError ) = 0;
	};

#endif // MSIPSERVERRESOLVEROBSERVER_H
