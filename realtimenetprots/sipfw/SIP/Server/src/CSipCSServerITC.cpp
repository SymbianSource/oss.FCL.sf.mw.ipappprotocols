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
// Name          : CSipCSServerITC.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//




#include "CSipCSServerITC.h"
#include "CSIPCSSerializer.h"
#include "SipCSServer.pan"
#include "siprequest.h"
#include "sipresponse.h"
#include "siprouteheader.h"
#include "sipfromheader.h"
#include "TSIPRemoteTargetAndProxy.h"
#include "uricontainer.h"
#include "SipLogs.h"


// -----------------------------------------------------------------------------
// CSipCSServerITC::NewL
// -----------------------------------------------------------------------------
//
CSipCSServerITC* CSipCSServerITC::NewL ()
	{
    CSipCSServerITC* self = CSipCSServerITC::NewLC ();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::NewLC
// -----------------------------------------------------------------------------
//
CSipCSServerITC* CSipCSServerITC::NewLC ()
	{
    CSipCSServerITC* self = new (ELeave) CSipCSServerITC;
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::CSipCSServerITC
// -----------------------------------------------------------------------------
//
CSipCSServerITC::CSipCSServerITC ()
	{
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::ConstructL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::ConstructL ()
	{
    iSerializer = CSIPCSSerializer::NewL();
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::~CSipCSServerITC
// -----------------------------------------------------------------------------
//
CSipCSServerITC::~CSipCSServerITC ()
	{
    delete iSerializer;
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::Serializer
// -----------------------------------------------------------------------------
//
CSIPCSSerializer& CSipCSServerITC::Serializer ()
    {
    return *iSerializer;
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              CBufFlat& aSipHeaders) const
    {
    TPtr8 headerBufPtr = aSipHeaders.Ptr(0);
    WriteL(ESipItcArgMessageHeaders,aMessage,headerBufPtr);
    }
    
// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              const TDesC8& aDes,
                              TSipItcArguments aItcArgIndex) const
    {
    WriteL(aItcArgIndex,aMessage,aDes);
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              const TSIPIds& aIds) const
	{
    TPckgBuf<TSIPIds> sipIdsPckg(aIds);
    WriteL(ESipItcArgIds, aMessage, sipIdsPckg);
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              const TSIPMessageBufSizes aSizes) const
    {
    TPckgBuf<TSIPMessageBufSizes> bufSizesPckg(aSizes);
    WriteL(ESipItcArgBufSizes, aMessage, bufSizesPckg);
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
							  const CSIPHeaderBase& aHeader) const
	{
	CBufFlat* headerBuf = iSerializer->ExternalizeValueLC(aHeader);
	TPtr8 headerBufPtr(headerBuf->Ptr(0));
	WriteL(ESipItcArgMessageHeaders,aMessage,headerBufPtr);
	CleanupStack::PopAndDestroy(headerBuf);
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              TInt aValue,
                              TSipItcArguments aItcArgIndex) const
    {
    TPckg<TInt> handlePckg(aValue);
    WriteL(aItcArgIndex, aMessage, handlePckg);
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              CSIPConnection::TState aState) const
    {
    TPckg<CSIPConnection::TState> statePckg(aState);
    WriteL(ESipItcArgConnectionState, aMessage, statePckg);
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              MDesC8Array& aSecurityMechanisms) const
    {
	CBufFlat* buf = iSerializer->ExternalizeLC(aSecurityMechanisms);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(ESipItcArgAuthenticationMechanism, aMessage, bufPtr);
	CleanupStack::PopAndDestroy(buf);
    }
 
// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//    
void CSipCSServerITC::WriteL (const RMessage2& aMessage,
                              const TInetAddr& aInetAddr) const
    {
    TPckgBuf<TInetAddr> addrPckg(aInetAddr);
    WriteL(ESipItcArgLocalAddr, aMessage, addrPckg);    
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadAppUidL
// -----------------------------------------------------------------------------
//
TUid CSipCSServerITC::ReadAppUidL (const RMessage2& aMessage) const
	{
    TPckgBuf<TUid> appUidPckg;
    aMessage.ReadL(ESipItcArgAppUid, appUidPckg);
    return appUidPckg();
	}
    
// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadRequestL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::ReadRequestL (const RMessage2& aMessage,
                                    CSIPRequest& aRequest,
                                    TBool aMethodExpected) const
    {
    HBufC8* headers = ReadLC(aMessage,ESipItcArgMessageHeaders);
	if (headers->Length() > 0)
		{
		iSerializer->InternalizeL(*headers,aRequest,aMethodExpected);
		}
	CleanupStack::PopAndDestroy(headers);	
    HBufC8* content = ReadL(aMessage,ESipItcArgMessageContent);
    aRequest.SetContent(content);
    __SIP_MESSAGE_LOG("ServerCore",aRequest)
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadRequestL
// -----------------------------------------------------------------------------
//
void 
CSipCSServerITC::ReadRequestL (const RMessage2& aMessage,
                               CSIPRequest& aRequest,
                               TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy,
                               TBool aMethodExpected,
                               TBool aProxyExpected) const
    {
    HBufC8* headers = ReadLC(aMessage,ESipItcArgMessageHeaders);
	if (headers->Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    iSerializer->InternalizeL(*headers,aRequest,aRemoteTargetAndProxy,
                              aMethodExpected,aProxyExpected);
    CleanupStack::PopAndDestroy(headers);
    CleanupStack::PushL(aRemoteTargetAndProxy.iRemoteTarget);
    CleanupStack::PushL(aRemoteTargetAndProxy.iProxy);
    HBufC8* content = ReadL(aMessage,ESipItcArgMessageContent);
    aRequest.SetContent(content);
    CleanupStack::Pop(aRemoteTargetAndProxy.iProxy);
    CleanupStack::Pop(aRemoteTargetAndProxy.iRemoteTarget);
    __SIP_MESSAGE_LOG("ServerCore",aRequest)
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadResponseLC
// -----------------------------------------------------------------------------
//
CSIPResponse* CSipCSServerITC::ReadResponseLC (const RMessage2& aMessage) const
    {
    HBufC8* headers = ReadLC(aMessage,ESipItcArgMessageHeaders);
	if (headers->Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    CSIPResponse* response = iSerializer->InternalizeL(*headers);
    CleanupStack::PopAndDestroy(headers);
    CleanupStack::PushL(response);
    HBufC8* content = ReadL(aMessage,ESipItcArgMessageContent);
    response->SetContent(content);
    __SIP_MESSAGE_LOG("ServerCore",*response)
    return response;
    }
	
// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadProxyLC
// -----------------------------------------------------------------------------
//	
CSIPRouteHeader* CSipCSServerITC::ReadProxyLC (const RMessage2& aMessage) const
    {
    HBufC8* proxyBuf = ReadLC(aMessage,ESipItcArgMessageHeaders);
	if (proxyBuf->Length() == 0)
        {
        User::Leave(KErrArgument);
        }
    CSIPRouteHeader* proxy = iSerializer->InternalizeProxyL(*proxyBuf);
	CleanupStack::PopAndDestroy(proxyBuf);
    CleanupStack::PushL(proxy);
    return proxy;       
    }

// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadSipIdsL
// -----------------------------------------------------------------------------
//
TSIPIds CSipCSServerITC::ReadSipIdsL (const RMessage2& aMessage) const
	{
    TPckgBuf<TSIPIds> sipIdsPckg;
    aMessage.ReadL(ESipItcArgIds, sipIdsPckg);
    return sipIdsPckg();
	}
	
// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadSIPSockOptL
// -----------------------------------------------------------------------------
//
TSIPSockOpt CSipCSServerITC::ReadSIPSockOptL (const RMessage2& aMessage) const
	{
    TPckgBuf<TSIPSockOpt> sipSockOptPckg;
    aMessage.ReadL(ESipItcArgSockOpt, sipSockOptPckg);
    return sipSockOptPckg();
	}	

// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::ReadL (const RMessage2& aMessage,
                             TInt& aValue,
                             TSipItcArguments aItcArgIndex) const
    {
    aValue = ITCArgInt(aItcArgIndex,aMessage);
    }
    
// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadLC
// -----------------------------------------------------------------------------
//
HBufC8* CSipCSServerITC::ReadLC (const RMessage2&  aMessage,
                                 TSipItcArguments aItcArgIndex) const
	{
    TInt length = aMessage.GetDesLength (aItcArgIndex);
    if (length < 0)
        {
        User::Leave (KErrBadDescriptor);
        }
    HBufC8* buf = HBufC8::NewLC (length);
	if (length > 0)
		{
		TPtr8 bufPtr(buf->Des());
		aMessage.ReadL (aItcArgIndex, bufPtr);
		}
    return buf;
	}
	
// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadCredentialsLC
// -----------------------------------------------------------------------------
//
CDesC8Array* 
CSipCSServerITC::ReadCredentialsLC (const RMessage2& aMessage) const
    {
    HBufC8* credentialsBuf = ReadLC(aMessage,ESipItcArgCredentials);
    CDesC8Array* credentials = 
        iSerializer->InternalizeCredentialsL(*credentialsBuf);
    CleanupStack::Pop(credentialsBuf);
    CleanupStack::PushL(credentials);
    return credentials;
    }	    

// -----------------------------------------------------------------------------
// CSipCSServerITC::Complete
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::Complete (const RMessage2& aMessage,
                                TInt aCompletionCode) const
	{
    aMessage.Complete (aCompletionCode);
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::PanicClient
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::PanicClient(const RMessage2& aMessage, TInt aPanic) const
	{
    RThread client;
    aMessage.Client(client);
    client.Panic(KSipCSServerPanic, aPanic);
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::ITCArgPtr
// -----------------------------------------------------------------------------
//
const TAny* CSipCSServerITC::ITCArgPtr (TSipItcArguments aItcArgIndex,
                                        const RMessage2&  aMessage) const
	{
    const TAny* ptr;
    switch (static_cast<TInt>(aItcArgIndex))
		{
        case 0: ptr = aMessage.Ptr0(); break;
        case 1: ptr = aMessage.Ptr1(); break;
        case 2: ptr = aMessage.Ptr2(); break;
        case 3: ptr = aMessage.Ptr3(); break;
        default: ptr = 0; break;
		}
    return ptr; 
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::ITCArgInt
// -----------------------------------------------------------------------------
//
TInt CSipCSServerITC::ITCArgInt (TSipItcArguments aItcArgIndex,
                                 const RMessage2&  aMessage) const
	{
    TInt integer;
    switch (static_cast<TInt>(aItcArgIndex))
		{
        case 0: integer = aMessage.Int0(); break;
        case 1: integer = aMessage.Int1(); break;
        case 2: integer = aMessage.Int2(); break;
        case 3: integer = aMessage.Int3(); break;
        default: integer = KErrArgument; break;
		}
    return integer; 
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::WriteL
// -----------------------------------------------------------------------------
//
void CSipCSServerITC::WriteL (TSipItcArguments aItcArgIndex,
							  const RMessage2&  aMessage,
							  const TDesC8&    aDes) const
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
    aMessage.WriteL (aItcArgIndex, aDes);
	}

// -----------------------------------------------------------------------------
// CSipCSServerITC::ReadL
// -----------------------------------------------------------------------------
//
HBufC8* CSipCSServerITC::ReadL (const RMessage2&  aMessage,
                                TSipItcArguments aItcArgIndex) const
	{
	HBufC8* buf = ReadLC(aMessage,aItcArgIndex);
    CleanupStack::Pop(buf);
	return buf;
	}
