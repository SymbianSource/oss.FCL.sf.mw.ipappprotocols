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
// Name          : CSIPCSSerializer.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//




#include "CSIPCSSerializer.h"
#include "sipmessage.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipheaderbase.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "siprouteheader.h"
#include "uricontainer.h"
#include "siphostport.h"
#include "SIPHeaderLookup.h"
#include "TSIPRemoteTargetAndProxy.h"
#include "sipstrings.h"


const TInt KBufExpandSize=100;
// The maximum value for RHeap Alloc operations is KMaxTInt/2-1
const TInt KMaxLength=(KMaxTInt/2)-1;

// -----------------------------------------------------------------------------
// CSIPCSSerializer::NewL
// -----------------------------------------------------------------------------
//
CSIPCSSerializer* CSIPCSSerializer::NewL()
	{
    CSIPCSSerializer* self = CSIPCSSerializer::NewLC();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::NewLC
// -----------------------------------------------------------------------------
//
CSIPCSSerializer* CSIPCSSerializer::NewLC ()
	{
    CSIPCSSerializer* self = new (ELeave) CSIPCSSerializer;
    CleanupStack::PushL (self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::CSIPCSSerializer
// -----------------------------------------------------------------------------
//
CSIPCSSerializer::CSIPCSSerializer()
	{
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPCSSerializer::ConstructL()
	{
	SIPHeaderLookup::OpenL();
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::~CSIPCSSerializer
// -----------------------------------------------------------------------------
//
CSIPCSSerializer::~CSIPCSSerializer()
	{
	SIPHeaderLookup::Close();
	}
	
// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeCredentialsL
// -----------------------------------------------------------------------------
//
CDesC8Array* CSIPCSSerializer::InternalizeCredentialsL(const TDesC8& aDes) const
    {
    CDesC8ArraySeg* settings = new(ELeave)CDesC8ArraySeg(1);
    CleanupStack::PushL(settings);
    RDesReadStream readStream(aDes);
    readStream.PushL();
    TUint32 settingLength = readStream.ReadUint32L();
    while (settingLength > 0)
        {
        if (settingLength > KMaxLength)
            {
            User::Leave(KErrOverflow);
            }
        HBufC8* setting = HBufC8::NewLC (settingLength);
        TPtr8 settingPtr(setting->Des());
        readStream.ReadL (settingPtr,settingLength);
        settings->AppendL(settingPtr);
        CleanupStack::PopAndDestroy(setting);
        settingLength = readStream.ReadUint32L();
        }
    readStream.Pop();
    readStream.Close();
    CleanupStack::Pop(settings);
    return settings;
    }	

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ExternalizeLC
// -----------------------------------------------------------------------------
//
CBufFlat* CSIPCSSerializer::ExternalizeLC(
    const MDesC8Array& aSecurityMechanisms) const
    {
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
    for (TInt i=0; i < aSecurityMechanisms.MdcaCount(); i++)
        {
        TPtrC8 mechanism(aSecurityMechanisms.MdcaPoint(i));
        writeStream.WriteUint32L(mechanism.Length());
        writeStream.WriteL(mechanism);
        }
    writeStream.WriteUint32L(0); // no more mechanisms in the stream
    writeStream.Pop();
	writeStream.Close();
	return buf;
    }

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ExternalizeValueLC
// -----------------------------------------------------------------------------
//
CBufFlat* CSIPCSSerializer::ExternalizeValueLC(
    const CSIPHeaderBase& aHeader) const
	{
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	aHeader.ExternalizeL(writeStream,EFalse);
	writeStream.Pop();
	writeStream.Close();
	return buf;
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ExternalizeLC
// -----------------------------------------------------------------------------
//
CBufFlat* CSIPCSSerializer::ExternalizeLC (CSIPMessage& aMessage) const
	{
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	if (aMessage.IsRequest())
		{
		writeStream.WriteUint8L(1); // request flag
		CSIPRequest& request = static_cast<CSIPRequest&>(aMessage);
		ExternalizeL(request,writeStream);
		}
	else
		{
		writeStream.WriteUint8L(0); // response flag
		CSIPResponse& response = static_cast<CSIPResponse&>(aMessage);
		ExternalizeL(response,writeStream);
		}
	writeStream.Pop();
	writeStream.Close();
	return buf;
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPCSSerializer::ExternalizeL (CSIPRequest& aRequest,
									 RWriteStream& aWriteStream) const
	{
	ExternalizeL(aRequest.Method(),aWriteStream);
    if (aRequest.RequestURI())
        {
        aWriteStream.WriteUint8L(1);
        aRequest.RequestURI()->ExternalizeL(aWriteStream);
        }
    else
        {
        aWriteStream.WriteUint8L(0);
        }
	aRequest.ExternalizeHeadersL(aWriteStream);
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPCSSerializer::ExternalizeL (CSIPResponse& aResponse,
									 RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint16L(aResponse.ResponseCode());
	ExternalizeL(aResponse.ReasonPhrase(),aWriteStream);
	aResponse.ExternalizeHeadersL(aWriteStream);
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeL
// -----------------------------------------------------------------------------
//
void CSIPCSSerializer::InternalizeL (const TDesC8& aDes,
                                     CSIPRequest& aRequest,
                                     TBool aMethodExpected) const
    {
	RDesReadStream readStream(aDes);
    CleanupClosePushL (readStream);
    if (aMethodExpected)
        {
	    InternalizeMethodL (aRequest,readStream);
        }
	aRequest.InternalizeHeadersL(readStream);
    readStream.Pop();
    readStream.Close();
    }

// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeL
// -----------------------------------------------------------------------------
//    
void CSIPCSSerializer::InternalizeL (const TDesC8& aDes,
                                     CSIPRequest& aRequest,
                                     TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy,
                                     TBool aMethodExpected,
                                     TBool aProxyExpected) const
    {
	RDesReadStream readStream(aDes);
	CleanupClosePushL(readStream);
    if (aMethodExpected)
        {
	    InternalizeMethodL(aRequest,readStream);
        }
    CURIContainer* remoteTarget = NULL;
    if (readStream.ReadUint8L()) // remote target present
        {
        remoteTarget = CURIContainer::InternalizeL(readStream);
        CleanupStack::PushL(remoteTarget);
        }
    CSIPRouteHeader* proxy = NULL;
    if (aProxyExpected)
        {
        proxy = static_cast<CSIPRouteHeader*>(
            CSIPRouteHeader::InternalizeValueL(readStream));
        CleanupStack::PushL(proxy);
        }
	aRequest.InternalizeHeadersL(readStream);
	if (proxy)
	    {
	    CleanupStack::Pop(proxy);
	    }
	if (remoteTarget)
	    {
	    CleanupStack::Pop(remoteTarget);
	    }
    readStream.Pop();
    readStream.Close();
    aRemoteTargetAndProxy.iProxy = proxy;
    aRemoteTargetAndProxy.iRemoteTarget = remoteTarget;
    }

// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPResponse* CSIPCSSerializer::InternalizeL (const TDesC8& aDes) const
	{
	RDesReadStream readStream(aDes);
	readStream.PushL();
	TUint statusCode = readStream.ReadUint16L();
	RStringF reasonPhrase = ReadFromStreamL(readStream);
	CleanupClosePushL(reasonPhrase);
	CSIPResponse* response = CSIPResponse::NewL(statusCode,reasonPhrase);
	CleanupStack::PopAndDestroy(); // reasonPhrase
	CleanupStack::PushL(response);
	response->InternalizeHeadersL(readStream);
	CleanupStack::Pop(response);
    readStream.Pop();
    readStream.Close();
	return response;
	}

// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeHeadersL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPCSSerializer::InternalizeHeadersL(
    const TDesC8& aDes) const
    {
    RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	RDesReadStream readStream(aDes);
	CleanupClosePushL (readStream);
	TUint8 moreHeaders = readStream.ReadUint8L();
    CSIPHeaderBase* header = NULL;
	while (moreHeaders)
		{
		RStringF headerName = ReadFromStreamL(readStream);
		CleanupClosePushL(headerName);
		header = SIPHeaderLookup::InternalizeL(headerName,readStream);
		CleanupStack::PopAndDestroy(); // headerName
		if (header)
			{
			CleanupStack::PushL(header);
            headers.AppendL(header);
			CleanupStack::Pop(header);
			}
		moreHeaders = readStream.ReadUint8L();
		}
    readStream.Pop();
    readStream.Close();
    CleanupStack::Pop(); // headers
    return headers;
    }
    
// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeProxyL
// -----------------------------------------------------------------------------
//
CSIPRouteHeader* CSIPCSSerializer::InternalizeProxyL (const TDesC8& aDes) const
    {
	RDesReadStream readStream(aDes);
	CleanupClosePushL(readStream);
    CSIPRouteHeader* proxy = 
        static_cast<CSIPRouteHeader*>(
            CSIPRouteHeader::InternalizeValueL(readStream));
    readStream.Pop();
    readStream.Close();
    return proxy;              
    }

// -----------------------------------------------------------------------------
// CSIPCSSerializer::InternalizeMethodL
// -----------------------------------------------------------------------------
//
void CSIPCSSerializer::InternalizeMethodL (CSIPRequest& aRequest,
                                           RReadStream& aReadStream) const
    {
    RStringF method = ReadFromStreamL(aReadStream);
	CleanupClosePushL(method);    
    aRequest.SetMethodL(method);
    CleanupStack::PopAndDestroy(); // method
    }

// -----------------------------------------------------------------------------
// CSIPCSSerializer::ReadFromStreamLC
// -----------------------------------------------------------------------------
//
RStringF CSIPCSSerializer::ReadFromStreamL (RReadStream& aReadStream) const
	{
	TUint32 bufLength = aReadStream.ReadUint32L();
	HBufC8* buf = HBufC8::NewLC (bufLength);
	TPtr8 bufPtr(buf->Des());	
	if (bufLength > 0)
		{
		aReadStream.ReadL (bufPtr,bufLength);
		}
	RStringF str = SIPStrings::Pool().OpenFStringL(bufPtr);
	CleanupStack::PopAndDestroy(buf);	
	return str;
	}
	
// -----------------------------------------------------------------------------
// CSIPCSSerializer::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPCSSerializer::ExternalizeL (RStringF aStr,
                                     RWriteStream& aWriteStream) const
    {
    TPtrC8 des(aStr.DesC());
	aWriteStream.WriteUint32L(des.Length());
	aWriteStream.WriteL(des);    
    }
