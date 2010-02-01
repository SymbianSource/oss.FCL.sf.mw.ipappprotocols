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
* Name        : sipclientdiscoveryimplementation.h
* Part of     : SIP Client Resolver
* SIP Client Discovery API
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTDISCOVERYIMPL_H
#define CSIPCLIENTDISCOVERYIMPL_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "RSIPClientDiscovery.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPClientDiscoveryObserver;
class CSIPHeaderBase;
class CSIPContentTypeHeader;
class CSIPClientDiscoveryReceiver;
class CSIPClientDiscoverySerializer;
class CSIPRequest;


// CLASS DECLARATION
//
class CSIPClientDiscoveryImpl : public CBase
	{
public:  // Constructors and destructor
    
    static CSIPClientDiscoveryImpl* NewL (
                                MSIPClientDiscoveryObserver& aObserver, 
                                TUid aSelf);
    
    static CSIPClientDiscoveryImpl* NewLC (
                                MSIPClientDiscoveryObserver& aObserver, 
                                TUid aSelf);
    
	~CSIPClientDiscoveryImpl();

public: // New functions

	void RegisterL(TUid aChannel);

	TInt Deregister(TUid aChannel);

	TUint32 ChannelL(RStringF aRequestMethod,
                    const TDesC8& aRequestUri,
                    const RPointerArray<CSIPHeaderBase>& aHeaders,
                    const TDesC8& aContent,
                    const CSIPContentTypeHeader* aContentType=0);

	TUint32 ChannelL(TUid aResolver,
                    RStringF aRequestMethod,
                    const TDesC8& aRequestUri,
                    const RPointerArray<CSIPHeaderBase>& aHeaders,
                    const TDesC8& aContent,
                    const CSIPContentTypeHeader* aContentType=0);
		
	void Cancel(TUint32 aRequestId);
		
	void CancelAll();
		
private: // New methods

	void ConstructL (MSIPClientDiscoveryObserver& aObserver, TUid aSelf);
    
    CSIPRequest* CreateRequestLC(RStringF aRequestMethod,
                                 const TDesC8& aRequestUri,
                                 const RPointerArray<CSIPHeaderBase>& aHeaders,
                                 const CSIPContentTypeHeader* aContentType=0);
                                 
private: // Data

    RSIPClientDiscovery iSipClientDiscovery;
    CSIPClientDiscoverySerializer* iSerializer;
	CSIPClientDiscoveryReceiver* iReceiver;
	
private: // For testing purposes

	UNIT_TEST(CSIPClientDiscoveryTest)
	};

#endif // CSIPCLIENTDISCOVERYIMPL_H
