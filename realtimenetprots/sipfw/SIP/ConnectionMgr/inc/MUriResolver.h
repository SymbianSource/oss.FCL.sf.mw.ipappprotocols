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
* Name          : MSipUriResolver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef __MURIRESOLVER_H__
#define __MURIRESOLVER_H__

#include <e32std.h>

class CURIContainer;
class MSIPServerResolverObserver;
class MSIPResolvingResult;
class TSIPTransportParams;


class MSipUriResolver
	{
public:

    /**
     * Destructor.
	 */
	virtual ~MSipUriResolver () {}
	
    /**
     * Resolves the URI asyncronously.
     *
     * @param aURI a reference to a const CURIContainer object
     * @param aResult a reference to MSIPResolvingResults array which
     *  will be filled with resolved addresses
     * @param aParams related transport parameters
	 * @param aObserver	callback that will be used when resolving 
	 *    	  has been completed. The ownership is not transferred.
	 *        Exactly one resolving request with this observer
	 *        can be pending at a time.
     */
	virtual void GetByURIL( const CURIContainer& aURI,
	                        RPointerArray<MSIPResolvingResult>& aResult,
							const TSIPTransportParams& aParams,
		                    MSIPServerResolverObserver* aObserver ) = 0;

    /**
     * Cancels a resolving request.
     *
     * @param aObserver identifies the resolving request.
     */
	virtual void CancelGetByUri( MSIPServerResolverObserver* aObserver ) = 0;
	};

#endif // __MURIRESOLVER_H__

// End of File

