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
* Name          : CSIPClientDiscoveryReceiver.h
* Part of       : SIP Client Resolver
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTDISCOVERYRECEIVER_H
#define CSIPCLIENTDISCOVERYRECEIVER_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "sipCRclientserver.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPClientDiscoveryObserver;
class CSIPClientDiscoverySerializer;
class MSIPClientDiscoveryITC;


class CSIPClientDiscoveryReceiver : public CActive
	{
public: // Constructors and destructor

	CSIPClientDiscoveryReceiver(MSIPClientDiscoveryITC& aITC, 
	                            MSIPClientDiscoveryObserver& aObserver,
	                            CSIPClientDiscoverySerializer& aSerializer);

	~CSIPClientDiscoveryReceiver();

protected: // From CActive:
    
    void RunL();
    TInt RunError(TInt aError);
	void DoCancel();

private: // New functions

	void HandleClientNotFoundL();
	void HandleChannelResolvedL();
	void ErrorOccured(TInt aError);
	void ReceiveNext();
    
private: // Data

	MSIPClientDiscoveryObserver& iObserver;
	CSIPClientDiscoverySerializer& iSerializer;
    MSIPClientDiscoveryITC& iITC;
    TIpcArgs iITCMsgArgs;  
    TPckgBuf<TUint32> iIdsPckg;
    TPckgBuf<TUid> iResolverUid;
    TPckgBuf<TSIPCRMessageBufSizes> iSizesPckg;
    TPckgBuf<TInt> iClientResolved;
    
private: // For testing purposes

	UNIT_TEST(CSIPClientDiscoveryReceiverTest)
	};
	
#endif // CSIPCLIENTDISCOVERYRECEIVER_H

// End of File
