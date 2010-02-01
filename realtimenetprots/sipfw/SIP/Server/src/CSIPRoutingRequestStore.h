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
* Name          : CSIPRoutingRequestStore.h
* Part of       : SIP Server Core
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPROUTINGREQUESTSTORE_H
#define CSIPROUTINGREQUESTSTORE_H

#include <e32base.h>
#include "MSIPRequestRouter.h"
#include "SipRequestHandlerObserver.h"
#include "_sipcodecdefs.h"

class CSipServerCore;
class CSIPRoutingRequest; 


class CSIPRoutingRequestStore : public CBase,
                                public MSIPRequestRouter,
                                public MSIPRequestHandlerObserver
    {
public: // Contstructors and destructor

    static CSIPRoutingRequestStore* NewL(CSipServerCore& aServerCore);
    static CSIPRoutingRequestStore* NewLC(CSipServerCore& aServerCore);

    ~CSIPRoutingRequestStore ();

public: // From MSIPRequestRouter

	TUint32 TransactionOwnerL(CSIPRequest& aRequest,
	                          TUint32 aIapId,
	                          MSIPRequestRouterObserver& aObs);

   
	void Cancel(TUint32 aRequestId);
	

public: // From MSIPRequestHandlerObserver

	void ClientFoundL(TUint32 aRequestId, TUid aUid);

	void ClientNotFoundL(TUint32 aRequestId, CSIPResponse* aSIPResponse);

	void ErrorOccurred(TUint32 aRequestId, TInt aError);		
	

private: // New functions

    CSIPRoutingRequest* Find(TUint32 aRequestId);
    void RemoveAndDelete(TUint32 aRequestId);

private: // Second phase constructors

    CSIPRoutingRequestStore (CSipServerCore& aServerCore);

private: // Data

	CSipServerCore& iServerCore;
    TSglQue<CSIPRoutingRequest> iList;

private: // For testing purposes

	UNIT_TEST(CSIPRoutingRequestStoreTest)
    };

#endif // CSIPROUTINGREQUESTSTORE_H

// End of File
