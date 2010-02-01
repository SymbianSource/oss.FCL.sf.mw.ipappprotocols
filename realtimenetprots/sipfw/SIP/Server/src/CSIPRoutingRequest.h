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
* Name          : CSIPRoutingRequest.h
* Part of       : SIP Server Core
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPROUTINGREQUEST_H
#define CSIPROUTINGREQUEST_H

#include <e32base.h>
#include "_sipcodecdefs.h"

class MSIPRequestRouterObserver;


class CSIPRoutingRequest : public CBase
    {
public: // Constructors and destructor

    static CSIPRoutingRequest* NewL (TUint32 aIapId,
                                     MSIPRequestRouterObserver& aObserver);
                                     
    static CSIPRoutingRequest* NewLC (TUint32 aIapId,
                                      MSIPRequestRouterObserver& aObserver);

    ~CSIPRoutingRequest ();
    
public: // New functions

	static TInt LinkOffset();
	void SetRequestId(TUint32 aId);
    TUint32 RequestId() const;
    TUint32 IapId() const;
    MSIPRequestRouterObserver& Observer();
    
private: // Second phase constructors

    CSIPRoutingRequest (TUint32 aIapId, MSIPRequestRouterObserver& aObserver);

private: // Data
    
   	TSglQueLink iLink;    
    TUint32 iRequestId;
    TUint32 iIapId;
    MSIPRequestRouterObserver& iObserver;

private: // For testing purposes

	UNIT_TEST(CSIPRoutingRequestTest)
    };

#endif // CSIPROUTINGREQUEST_H

// End of File
