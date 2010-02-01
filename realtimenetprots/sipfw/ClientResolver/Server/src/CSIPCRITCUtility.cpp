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
// Name          : CSIPCRITCUtility.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPCRITCUtility.h"
#include "SIPCRSerializer.h"
#include "sipCRclientserver.h"
#include "siprequest.h"


// ----------------------------------------------------------------------------
// CSIPCRITCUtility::NewL
// ----------------------------------------------------------------------------
//
CSIPCRITCUtility* CSIPCRITCUtility::NewL()
    {
    CSIPCRITCUtility* self = CSIPCRITCUtility::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPCRITCUtility::NewLC
// ----------------------------------------------------------------------------
//
CSIPCRITCUtility* CSIPCRITCUtility::NewLC()
    {
    CSIPCRITCUtility* self = new(ELeave)CSIPCRITCUtility;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::ConstructL ()
    {
    iSerializer = new (ELeave) SIPCRSerializer;
    }

// ----------------------------------------------------------------------------
// CSIPCRITCUtility::~CSIPCRITCUtility
// ----------------------------------------------------------------------------
//
CSIPCRITCUtility::~CSIPCRITCUtility ()
    {
    delete iSerializer;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::ReadSIPRequestL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::ReadSIPRequestL(const RMessage2& aMessage, 
                                       CSIPRequest& aRequest)
    {
    HBufC8* request = ReadLC(ESIPCRIpcArgSipRequest, aMessage);
	iSerializer->InternalizeL(*request,aRequest);
	CleanupStack::PopAndDestroy(request);
    HBufC8* content = ReadL(ESIPCRIpcArgSipMessageContent,aMessage);
    aRequest.SetContent(content);  
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::ReadResolverUidL
// ----------------------------------------------------------------------------
//
TUid CSIPCRITCUtility::ReadResolverUidL(const RMessage2& aMessage) const
    {
    TPckgBuf<TUid> resolverUidPckg;
    aMessage.ReadL(ESIPCRIpcArgResolverUid, resolverUidPckg);
    return resolverUidPckg();
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteChannelUidL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WriteChannelUidL(const RMessage2& aMessage,
                                        const TUid& aUID) const
    {
    TPckg<TUid> uidPckg(aUID);
    WriteL(ESIPCRIpcArgResolverUid, aMessage, uidPckg);
    }
  
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteSIPResponseL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WriteSIPResponseL(const RMessage2& aMessage,
                                         CBufFlat& aResponse) const
    {
    TPtr8 responseBufPtr = aResponse.Ptr(0);
    WriteL(ESIPCRIpcArgSipResponse,aMessage,responseBufPtr);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteSIPResponseContentL
// ----------------------------------------------------------------------------
//    
void CSIPCRITCUtility::WriteSIPResponseContentL(const RMessage2& aMessage,
                                                const TDesC8& aContent) const
    {
    WriteL(ESIPCRIpcArgSipMessageContent,aMessage,aContent);
    }    
    

// ----------------------------------------------------------------------------
// CSIPCRITCUtility::ReadUIDL
// ----------------------------------------------------------------------------
//
TUid CSIPCRITCUtility::ReadUIDL(const RMessage2& aMessage) const
    {
    TPckgBuf<TUid> uidPckg;
    aMessage.ReadL(ESIPCRIpcArgClientUid, uidPckg);
    return uidPckg();
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscovery::WriteUIDL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WriteUIDL(const RMessage2& aMessage,
                                 TUid aUID) const
    {
    TPckg<TUid> uidPckg(aUID);
    WriteL(ESIPCRIpcArgResolverUid, aMessage, uidPckg);
    }

// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteRequestIdL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WriteRequestIdL(const RMessage2& aMessage, 
                                       TUint32 aId) const
    {
    TPckg<TUint32> idPckg(aId);
    WriteL(ESIPCRIpcArgReqId, aMessage, idPckg);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteClientResolvedL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WriteClientResolvedL(const RMessage2& aMessage, 
                                            TSIPCRIpcResponses aResponse) const
    {
    TPckg<TInt> responsePckg(aResponse);
    WriteL(ESIPCRIpcArgClientResolved, aMessage, responsePckg);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteResponseSizeL
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WriteResponseSizeL(const RMessage2& aMessage, 
                                          TSIPCRMessageBufSizes aSizes) const
    {
    TPckgBuf<TSIPCRMessageBufSizes> sizesPckg(aSizes);
    WriteL(ESIPCRIpcArgBufSizes, aMessage, sizesPckg);
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::ReadRequestIdL
// ----------------------------------------------------------------------------
//
TUint32 CSIPCRITCUtility::ReadRequestIdL (const RMessage2& aMessage) const
    {
    TPckgBuf<TUint32> requestIdPckg;
    aMessage.ReadL(ESIPCRIpcArgReqId, requestIdPckg);
    return requestIdPckg();   
    }
    
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::Complete
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::Complete (const RMessage2& aMessage,
                                 TInt aCompletionCode) const
	{
    aMessage.Complete(aCompletionCode);
	}          	

// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WaitForMutexLC
// ----------------------------------------------------------------------------
//
void CSIPCRITCUtility::WaitForMutexLC(RMutex& aMutex)
    {
    aMutex.Wait();
    TCleanupItem mutexCleanup(SignalMutex,&aMutex);
    CleanupStack::PushL(mutexCleanup);
    }

// -----------------------------------------------------------------------------
// CSIPCRITCUtility::SignalMutex
// -----------------------------------------------------------------------------
//
void CSIPCRITCUtility::SignalMutex(TAny* aMutex)
    {
    reinterpret_cast<RMutex*>(aMutex)->Signal();
    }
	
// ----------------------------------------------------------------------------
// CSIPCRITCUtility::WriteL
// ----------------------------------------------------------------------------
//
 void CSIPCRITCUtility::WriteL (TSIPCRIpcArguments aItcArgIndex,
                                const RMessage2& aMessage,
                                const TDesC8& aDes) const
    {
    TInt length = aMessage.GetDesMaxLength(aItcArgIndex);
    if (length < 0)
        {
        User::Leave (KErrArgument);
        }
    if (length < aDes.Length())
        {
        User::Leave (KErrOverflow);
        }

    aMessage.WriteL(aItcArgIndex, aDes);               
    }

// ----------------------------------------------------------------------------
// CSIPCRITCUtility::ReadL
// ----------------------------------------------------------------------------
//
HBufC8* CSIPCRITCUtility::ReadL (TSIPCRIpcArguments aItcArgIndex,
							     const RMessage2&  aMessage) const
	{	
	HBufC8* buf = ReadLC(aItcArgIndex,aMessage);
    CleanupStack::Pop(buf);
	return buf;
	}

// -----------------------------------------------------------------------------
// CSIPCRITCUtility::ReadLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPCRITCUtility::ReadLC (TSIPCRIpcArguments aItcArgIndex,
                                  const RMessage2& aMessage) const
	{
    TInt length = aMessage.GetDesLength(aItcArgIndex);
    if (length < 0)
        {
        User::Leave(KErrBadDescriptor);
        }
    HBufC8* buf = HBufC8::NewLC (length);
	if (length > 0)
		{
		TPtr8 bufPtr(buf->Des());
		aMessage.ReadL(aItcArgIndex, bufPtr);
		}
    return buf;
	}

//  End of File  
