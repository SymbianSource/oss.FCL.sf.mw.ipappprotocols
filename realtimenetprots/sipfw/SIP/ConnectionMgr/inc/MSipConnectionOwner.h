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
* Name          : MSipConnectionOwner.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MSIPCONNECTIONOWNER_H__
#define __MSIPCONNECTIONOWNER_H__

#include <e32std.h>

class MSipConnectionOwner
	{
public:

    virtual TBool IsSystemStarted() const = 0;

	virtual void Destroy ( TUint32 aIapId ) = 0;

	virtual TBool IsCellularDataUsageAllowed() const = 0;
	};

#endif // end of __MSIPCONNECTIONOWNER_H__

// End of File
