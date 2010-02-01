// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : CSIPClientDiscovery.cpp
// Part of       : SIP Client Resolver
// SIP Client Discovery API
// Version       : 1.0
//



// INCLUDES
#include "sipclientdiscovery.h"
#include "CSIPClientDiscoveryImpl.h"

// -----------------------------------------------------------------------------
// CSIPClientDiscovery::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientDiscovery* CSIPClientDiscovery::NewL (
                                    MSIPClientDiscoveryObserver& aObserver,
                                    TUid aSelf)	
    {
    CSIPClientDiscovery* self = CSIPClientDiscovery::NewLC (aObserver, aSelf);
    CleanupStack::Pop (self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscovery::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientDiscovery* CSIPClientDiscovery::NewLC (
                                    MSIPClientDiscoveryObserver& aObserver,
                                    TUid aSelf)
    {
    CSIPClientDiscovery* self = new(ELeave)CSIPClientDiscovery;
    CleanupStack::PushL (self);
    self->ConstructL (aObserver, aSelf);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSIPClientDiscovery::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPClientDiscovery::ConstructL (
                                MSIPClientDiscoveryObserver& aObserver,
                                TUid aSelf)
    {
    iCSIPClientDiscoveryImpl = CSIPClientDiscoveryImpl::NewL (aObserver,aSelf);
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscovery::~CSIPClientDiscovery
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPClientDiscovery::~CSIPClientDiscovery()
    {
    delete iCSIPClientDiscoveryImpl;
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscovery::RegisterL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPClientDiscovery::RegisterL(TUid aChannel)
    {
    iCSIPClientDiscoveryImpl->RegisterL(aChannel);
    }
        
// -----------------------------------------------------------------------------
// CSIPClientDiscovery::Deregister
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPClientDiscovery::Deregister(TUid aChannel)
    {
    return iCSIPClientDiscoveryImpl->Deregister(aChannel);
    }
        
// -----------------------------------------------------------------------------
// CSIPClientDiscovery::ChannelL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CSIPClientDiscovery::ChannelL(RStringF aRequestMethod,
                                const TDesC8& aRequestUri,
                                const RPointerArray<CSIPHeaderBase>& aHeaders,
                                const TDesC8& aContent,
                                const CSIPContentTypeHeader* aContentType)
    {
    return iCSIPClientDiscoveryImpl->ChannelL (aRequestMethod, 
                                               aRequestUri, 
                                               aHeaders,
                                               aContent,
                                               aContentType);
    }
        
// -----------------------------------------------------------------------------
// CSIPClientDiscovery::ChannelL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CSIPClientDiscovery::ChannelL(TUid aResolver,
                                RStringF aRequestMethod,
                                const TDesC8& aRequestUri,
                                const RPointerArray<CSIPHeaderBase>& aHeaders,
                                const TDesC8& aContent,
                                const CSIPContentTypeHeader* aContentType)
    {
    return iCSIPClientDiscoveryImpl->ChannelL(aResolver,
                                              aRequestMethod,
                                              aRequestUri, 
                                              aHeaders,
                                              aContent,
                                              aContentType);
    }

// -----------------------------------------------------------------------------
// CSIPClientDiscovery::Cancel
// -----------------------------------------------------------------------------
//
EXPORT_C  void CSIPClientDiscovery::Cancel(TUint32 aRequestId)
    {
    iCSIPClientDiscoveryImpl->Cancel (aRequestId);
    }
        
// -----------------------------------------------------------------------------
// CSIPClientDiscovery::CancelAll
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPClientDiscovery::CancelAll()
    {
    iCSIPClientDiscoveryImpl->CancelAll();
    }

//  End of File  
