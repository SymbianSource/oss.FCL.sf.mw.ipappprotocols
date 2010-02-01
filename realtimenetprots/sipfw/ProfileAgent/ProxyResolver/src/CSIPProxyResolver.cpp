// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CSIPProxyResolver.cpp
// Part of     : ProxyResolver
// Version     : SIP/4.0
//



// INCLUDES
#include "CSIPProxyResolver.h"
#include "CProxyResolveActor.h"


#ifdef CPPUNIT_TEST
#undef EXPORT_C
#define EXPORT_C
#endif
    
// -----------------------------------------------------------------------------
// CSIPProxyResolver::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyResolver* CSIPProxyResolver::NewL()
    {
    CSIPProxyResolver* self = CSIPProxyResolver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProxyResolver::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyResolver* CSIPProxyResolver::NewLC()
    {
    CSIPProxyResolver* self = new( ELeave )CSIPProxyResolver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProxyResolver::~CSIPProxyResolver
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyResolver::~CSIPProxyResolver ()
    {
    delete iResolver;
    }

// -----------------------------------------------------------------------------
// CSIPProxyResolver::CSIPProxyResolver
// -----------------------------------------------------------------------------
//  
CSIPProxyResolver::CSIPProxyResolver ()
    {
    }
    
// -----------------------------------------------------------------------------
// CSIPProxyResolver::ConstructL
// -----------------------------------------------------------------------------
//  
void CSIPProxyResolver::ConstructL ()
    {
    iResolver = CProxyResolveActor::NewL();
    }
    
// -----------------------------------------------------------------------------
// CSIPProxyResolver::ResolveProxyL
// -----------------------------------------------------------------------------
//  
EXPORT_C void CSIPProxyResolver::ResolveProxyL( 
    TUint& aRequestId,
    TUint32 aIapId,
    MSIPProxyResolverObserver& aObserver )
    {
    iResolver->ResolveProxyL( aRequestId, aIapId, aObserver);
    }

// -----------------------------------------------------------------------------
// CSIPProxyResolver::Cancel
// -----------------------------------------------------------------------------
//  
EXPORT_C void CSIPProxyResolver::Cancel( TUint aRequestId )
    {
    iResolver->CancelById( aRequestId );
    }


