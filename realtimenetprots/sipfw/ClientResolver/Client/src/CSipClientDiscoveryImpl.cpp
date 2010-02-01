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
// Name          : CSIPClientDiscoveryImpl.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include <s32mem.h>
#include "CSIPClientDiscoveryImpl.h"
#include "CSipCRServer.h"
#include "CSIPClientDiscoverySerializer.h"
#include "CSIPClientDiscoveryReceiver.h"
#include "sipclientdiscoveryobserver.h"
#include "sipstrings.h"
#include "siprequest.h"
#include "uricontainer.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::NewL
// ----------------------------------------------------------------------------
//
CSIPClientDiscoveryImpl* CSIPClientDiscoveryImpl::NewL (
                                    MSIPClientDiscoveryObserver& aObserver,
                                    TUid aSelf)	
    {
    CSIPClientDiscoveryImpl* self = CSIPClientDiscoveryImpl::NewLC (aObserver, 
                                                                    aSelf);
    CleanupStack::Pop (self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::NewLC
// ----------------------------------------------------------------------------
//
CSIPClientDiscoveryImpl* CSIPClientDiscoveryImpl::NewLC (
                                    MSIPClientDiscoveryObserver& aObserver,
                                    TUid aSelf)
    {
    CSIPClientDiscoveryImpl* self = new(ELeave)CSIPClientDiscoveryImpl;
    CleanupStack::PushL (self);
    self->ConstructL (aObserver, aSelf);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPClientDiscoveryImpl::ConstructL (
                                    MSIPClientDiscoveryObserver& aObserver,
                                    TUid aSelf)
    {
    SIPStrings::OpenL();

    User::LeaveIfError(iSipClientDiscovery.Connect(aSelf));
    
    iSerializer = CSIPClientDiscoverySerializer::NewL();
    iReceiver = new(ELeave)CSIPClientDiscoveryReceiver(iSipClientDiscovery, 
                                                       aObserver, 
                                                       *iSerializer);
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscovery::~CSIPClientDiscovery
// ----------------------------------------------------------------------------
//
CSIPClientDiscoveryImpl::~CSIPClientDiscoveryImpl()
    {
    delete iReceiver;
    iSipClientDiscovery.Close();
    delete iSerializer;
    SIPStrings::Close();
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::RegisterL
// ----------------------------------------------------------------------------
//
void CSIPClientDiscoveryImpl::RegisterL(TUid aChannel)
    {
    TPckgBuf<TUid> clientUidPckg(aChannel);
    TIpcArgs ipcArgs(TIpcArgs::ENothing);
    ipcArgs.Set (ESIPCRIpcArgClientUid, &clientUidPckg);
    User::LeaveIfError(iSipClientDiscovery.Send(ESIPCRIpcRegister,ipcArgs));
    }
        
// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::DeregisterClientL
// ----------------------------------------------------------------------------
//
TInt CSIPClientDiscoveryImpl::Deregister(TUid aChannel)
    {
    TPckgBuf<TUid> clientUidPckg(aChannel);
    TIpcArgs ipcArgs(TIpcArgs::ENothing);
    ipcArgs.Set (ESIPCRIpcArgClientUid, &clientUidPckg);
    return iSipClientDiscovery.Send(ESIPCRIpcDeregister,ipcArgs);
    }
        
// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::ChannelL
// ----------------------------------------------------------------------------
//
TUint32 CSIPClientDiscoveryImpl::ChannelL(RStringF aRequestMethod,
                                const TDesC8& aRequestUri,
                                const RPointerArray<CSIPHeaderBase>& aHeaders,
                                const TDesC8& aContent,
                                const CSIPContentTypeHeader* aContentType)
    {
    TUid uid(TUid::Null());
    return ChannelL (uid, aRequestMethod, aRequestUri, aHeaders, 
                     aContent, aContentType);
    }
        
// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::ChannelL
// ----------------------------------------------------------------------------
//
TUint32 CSIPClientDiscoveryImpl::ChannelL(TUid aResolver,
                                RStringF aRequestMethod,
                                const TDesC8& aRequestUri,
                                const RPointerArray<CSIPHeaderBase>& aHeaders,
                                const TDesC8& aContent,
                                const CSIPContentTypeHeader* aContentType)
    {
    CSIPRequest* request = 
        CreateRequestLC(aRequestMethod, aRequestUri, aHeaders, aContentType);
    CBufFlat* buf = iSerializer->ExternalizeSIPRequestL(*request);    
    CleanupStack::PopAndDestroy(request);
    CleanupStack::PushL(buf);
                                                    
    TPtr8 sipRequest(buf->Ptr(0));
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESIPCRIpcArgSipRequest, &sipRequest);
    args.Set(ESIPCRIpcArgSipMessageContent, &aContent);
    
    TPckgBuf<TUint32> reqIdPckg;
    args.Set(ESIPCRIpcArgReqId, &reqIdPckg);
    
    TInt err = KErrNone;
    if (aResolver.iUid)
        {
        TPckgBuf<TUid> resolverUidPckg(aResolver);
        args.Set(ESIPCRIpcArgResolverUid, &resolverUidPckg);
        err = iSipClientDiscovery.SendReceive(ESIPCRIpcChannelWithResolver,
                                              args);
        }
    else
        {
        err = iSipClientDiscovery.SendReceive(ESIPCRIpcChannel, args);
        }        
    if (err != KErrNone) 
        {
        User::Leave(err);
        }
                
    CleanupStack::PopAndDestroy(buf);
    return reqIdPckg();
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::Cancel
// ----------------------------------------------------------------------------
//
void CSIPClientDiscoveryImpl::Cancel(TUint32 aRequestId)
    {
    TPckgBuf<TUint32> requestIdPckg(aRequestId);
    TIpcArgs ipcArgs;
    ipcArgs.Set (ESIPCRIpcArgReqId, &requestIdPckg);
    iSipClientDiscovery.Send(ESIPCRIpcCancelRequest, ipcArgs);
    }
        
// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::CancelAll
// ----------------------------------------------------------------------------
//
void CSIPClientDiscoveryImpl::CancelAll()
    {
    TIpcArgs ipcArgs(TIpcArgs::ENothing);
    iSipClientDiscovery.Send(ESIPCRIpcCancelAllRequests,ipcArgs);
    }
    
// ----------------------------------------------------------------------------
// CSIPClientDiscoveryImpl::CreateRequestLC
// ----------------------------------------------------------------------------
//
CSIPRequest* CSIPClientDiscoveryImpl::CreateRequestLC(RStringF aRequestMethod,
                                const TDesC8& aRequestUri,
                                const RPointerArray<CSIPHeaderBase>& aHeaders,
                                const CSIPContentTypeHeader* aContentType)
    {
    CSIPRequest* request =  CSIPRequest::NewLC(aRequestMethod);
    CURIContainer* uriContainer = CURIContainer::DecodeL(aRequestUri);
    CleanupStack::PushL(uriContainer);
    request->SetRequestURIL(uriContainer);
    CleanupStack::Pop(uriContainer);
    
    for (TInt i = 0; i<aHeaders.Count(); i++)
        {
        request->AddHeaderL(*(aHeaders[i]));
        }
   
    if (aContentType)
        {
        request->AddHeaderL((const CSIPHeaderBase&)*aContentType);
        }
        
    return request;
    }

//  End of File  
