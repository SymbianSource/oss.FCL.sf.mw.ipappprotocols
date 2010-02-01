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
* Name          : MSIPITC.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPITC_H
#define MSIPITC_H

#include <e32std.h>
#include "sipclientserver.h"


class MSIPITC
	{
public:

    virtual ~MSIPITC () { }

    virtual TInt Send (TSipItcFunctions aFunction, TIpcArgs& aArgs) const = 0;

    virtual void Receive (TIpcArgs& aArgs, TRequestStatus& aStatus) = 0;

    virtual TInt Receive (TIpcArgs& aArgs) = 0;
    
    virtual void CancelReceive () = 0;
	};

#endif // MSIPITC_H

// End of File
