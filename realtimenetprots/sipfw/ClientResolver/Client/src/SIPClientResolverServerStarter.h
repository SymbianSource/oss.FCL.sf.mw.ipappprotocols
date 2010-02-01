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
* Name         : SIPClientResolverServerStarter.h
* Part of      : SIP Client Resolver
* Version      : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPCLIENTRESOLVERSERVERSTARTER_H
#define SIPCLIENTRESOLVERSERVERSTARTER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* This class starts client-server server part of Client Resolver.
* 
* @lib siprsvclient.dll
*/

class SIPClientResolverServerStarter
    {
public: // New functions

    static TInt Start();

private:

    static TInt CreateServerProcess(RSemaphore& aSemaphore);
    };

#endif // SIPCLIENTRESOLVERSERVERSTARTER_H

// End of File
