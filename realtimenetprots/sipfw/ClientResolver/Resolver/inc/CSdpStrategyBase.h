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
* Name          : CSdpStrategyBase.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPSTRATEGYBASE_H
#define CSDPSTRATEGYBASE_H

#include <e32base.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

class CSIPResponse;
class CSdpDocument;
class MSipClients;


class CSdpStrategyBase : public CBase
    {
public:

    virtual ~CSdpStrategyBase();
    virtual CSIPResponse* ApplyL(CSdpDocument& aDocument,
                                 RArray<TUid>& aUids) = 0;

protected:

	CSdpStrategyBase(const MSipClients& aSipClients);
	void ConstructL();
	
protected: // Data

    const MSipClients& iSipClients;
    RStringPool iPool;
    };

#endif // CSDPSTRATEGYBASE_H

// End of File
