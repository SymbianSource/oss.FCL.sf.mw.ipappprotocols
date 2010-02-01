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
* Name          : MSIPQueryData.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPQUERYDATA_H
#define MSIPQUERYDATA_H

#include <e32std.h>
#include <in_sock.h>

class MSIPQueryData
	{
public:

    /**
     * Destructor.
	 */
    virtual ~MSIPQueryData () {}

    /**
     * Gets the resolved address.
     *
     * @returns the resolved address
     */
	virtual TInetAddr& Address() = 0;

    /**
     * Gets the resolved protocol.
     *
     * @returns the resolved protocol
     */
	virtual TUint Protocol() = 0;
	};

#endif // MSIPQUERYDATA_H
