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
// Name          : CSIPCRRequestItem.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0 
//



// INCLUDES
#include <e32base.h>
#include "CSIPCRRequestItem.h"
#include "TSipClient.h"

// ----------------------------------------------------------------------------
// CSIPCRRequestItem::NewL
// ----------------------------------------------------------------------------
//
CSIPCRRequestItem* CSIPCRRequestItem::NewL (TUint32 aRequestId,
                                            const TSipClient& aClient,
                                            const TUid& aChannelUid,
                                            const TUid& aResolverUid)
    {
    CSIPCRRequestItem* self = CSIPCRRequestItem::NewLC(aRequestId,
                                                       aClient,
                                                       aChannelUid,
                                                       aResolverUid);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPCRRequestItem::NewLC
// ----------------------------------------------------------------------------
//
CSIPCRRequestItem* CSIPCRRequestItem::NewLC (TUint32 aRequestId,
                                             const TSipClient& aClient,
                                             const TUid& aChannelUid,
                                             const TUid& aResolverUid)
    {
    CSIPCRRequestItem* self = new(ELeave)CSIPCRRequestItem(aRequestId,
                                                           aClient,
                                                           aChannelUid,
                                                           aResolverUid);
    CleanupStack::PushL(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPCRRequestItem::~CSIPCRRequestItem
// ----------------------------------------------------------------------------
//
CSIPCRRequestItem::~CSIPCRRequestItem ()
    {
    }
    
// ----------------------------------------------------------------------------
// CSIPCRRequestItem::CSIPCRRequestItem
// ----------------------------------------------------------------------------
//
CSIPCRRequestItem::CSIPCRRequestItem ( TUint32 aRequestId,
                                      const TSipClient& aClient,
                                      const TUid& aChannelUid,
                                      const TUid& aResolverUid )
: iRequestId( aRequestId ),
  iClient( aClient.Uid(),
  			  aClient.AllowStarting(),
  			  aClient.RomBased() ),
  iChannelUid( aChannelUid ),
  iResolverUid( aResolverUid ),
  iConnectCalled( EFalse )
    {
    iClient.SetPluginType( aClient.PluginType() );
    }

// ----------------------------------------------------------------------------
// CSIPCRRequestItem::RequestId
// ----------------------------------------------------------------------------
//
TUint32 CSIPCRRequestItem::RequestId() const
    {
    return iRequestId;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRRequestItem::Client
// ----------------------------------------------------------------------------
//    
const TSipClient& CSIPCRRequestItem::Client() const
    {
    return iClient;
    }    
    
// ----------------------------------------------------------------------------
// CSIPCRRequestItem::ChannelUid
// ----------------------------------------------------------------------------
//    
const TUid& CSIPCRRequestItem::ChannelUid() const
    {
    return iChannelUid;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRRequestItem::ResolverUid
// ----------------------------------------------------------------------------
//    
const TUid& CSIPCRRequestItem::ResolverUid() const
    {
    return iResolverUid;
    }    
    
// ----------------------------------------------------------------------------
// CSIPCRRequestItem::SetConnectCalled
// ----------------------------------------------------------------------------
//     
void CSIPCRRequestItem::SetConnectCalled()
    {
    iConnectCalled = ETrue;
    }
	
// ----------------------------------------------------------------------------
// CSIPCRRequestItem::ConnectCalled
// ----------------------------------------------------------------------------
// 	
TBool CSIPCRRequestItem::ConnectCalled() const
    {
    return iConnectCalled;
    }

// End of File
