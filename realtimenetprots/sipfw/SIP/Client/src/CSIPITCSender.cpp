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
// Name          : CSIPITCSender.cpp
// Part of       : SIPClient
// Version       : SIP/4.0 
//



#include "CSIPITCSender.h"
#include "MSIPITC.h"
#include "sipmessageelements.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "uricontainer.h"
#include "siprouteheader.h"
#include "sipcallidheader.h"
#include "SIPHeaderLookup.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

const TInt KMaxHeaderSizeInReadStream = 300;
const TInt KMaxTagLength = 100;
const TInt KExternalizeBufferExpandSize = 100;


// -----------------------------------------------------------------------------
// CSIPITCSender::NewL
// -----------------------------------------------------------------------------
//
CSIPITCSender* CSIPITCSender::NewL (MSIPITC& aITC)
	{
    CSIPITCSender* self = CSIPITCSender::NewLC(aITC);
    CleanupStack::Pop (self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::NewLC
// -----------------------------------------------------------------------------
//
CSIPITCSender* CSIPITCSender::NewLC (MSIPITC& aITC)
	{
	CSIPITCSender* self = new(ELeave)CSIPITCSender(aITC);
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::CSIPITCSender
// -----------------------------------------------------------------------------
//	
CSIPITCSender::CSIPITCSender (MSIPITC& aITC)
 : iITC (aITC),
   iLookupOpened (EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::ConstructL ()
	{
    SIPHeaderLookup::OpenL();
    iLookupOpened = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::~CSIPITCSender
// -----------------------------------------------------------------------------
//
CSIPITCSender::~CSIPITCSender ()
    {
    if (iLookupOpened)
        {
        SIPHeaderLookup::Close();
        }
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::Send
// -----------------------------------------------------------------------------
//
TInt CSIPITCSender::Send (TSIPIds& aIds, TSipItcFunctions aITCFunction)
	{
    TPckgBuf<TSIPIds> sipIdsPckg(aIds);
    iITCMsgArgs.Set (ESipItcArgIds, &sipIdsPckg);

	TInt err = iITC.Send(aITCFunction,iITCMsgArgs);
	aIds = sipIdsPckg();
    return err;
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::SendL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SendL (TSIPIds& aIds, TSipItcFunctions aITCFunction)
	{
    TInt err = Send(aIds,aITCFunction);
    if (err != KErrNone)
        {
        User::Leave(err);
        }
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::SendL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SendL (TSIPIds& aIds,
						   TSipItcFunctions aITCFunction,
						   const CSIPRequestElements& aElements)
	{
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
    CBufFlat* buf = ExternalizeLC (aElements);
    TPtr8 externalizedHeaders = buf->Ptr(0);
    iITCMsgArgs.Set (ESipItcArgMessageHeaders, &externalizedHeaders);

    SetContentToSend (aElements.MessageElements());
    SendL (aIds,aITCFunction);
    CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::SendL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SendL (TSIPIds& aIds,
                           TSipItcFunctions aITCFunction,
                           RStringF aMethod,
                           const CUri8* aRemoteUri,
                           const CSIPFromHeader* aFrom,
                           const CSIPToHeader* aTo,
                           const CSIPMessageElements* aElements,
                           const CSIPRouteHeader* aOutboundProxy)
    {
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
    CBufFlat* buf = CBufFlat::NewL(KExternalizeBufferExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
    writeStream.PushL();
    ExternalizeL(aMethod,writeStream);
    if (aRemoteUri)
        {
        writeStream.WriteUint8L(1); // remote uri present
        TPtrC8 uri(aRemoteUri->Uri().UriDes());
        writeStream.WriteUint32L(uri.Length());
        writeStream.WriteL(uri);
        }
    else
        {
        writeStream.WriteUint8L(0); // remote not uri present
        }
    if (aOutboundProxy)
        {
        aOutboundProxy->ExternalizeL(writeStream,EFalse);
        }
    if (aFrom)
        {
        ExternalizeL(aFrom,writeStream);
        }
    if (aTo)
        {
        ExternalizeL(aTo,writeStream);
        }
    ExternalizeL(aElements,writeStream,EFalse);
	writeStream.Pop();
    writeStream.Close();
    TPtr8 externalizedHeaders = buf->Ptr(0);
    iITCMsgArgs.Set(ESipItcArgMessageHeaders,&externalizedHeaders);

    SetContentToSend (aElements);
    SendL (aIds,aITCFunction);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::SendL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SendL (TSIPIds& aIds,
						   TSipItcFunctions aITCFunction,
						   const CSIPMessageElements* aElements)
	{
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
    SetContentToSend (aElements);
	if (aElements != 0)
		{
        CBufFlat* buf = ExternalizeLC (*aElements);
        TPtr8 externalizedHeaders = buf->Ptr(0);
        iITCMsgArgs.Set (ESipItcArgMessageHeaders, &externalizedHeaders);
        SendL(aIds,aITCFunction);
        CleanupStack::PopAndDestroy(buf);
        }
    else
		{
        TBuf8<1> emptyBuf; 
        iITCMsgArgs.Set (ESipItcArgMessageHeaders, &emptyBuf);
		SendL(aIds,aITCFunction);
		}
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::SendL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SendL (TSIPIds& aIds,
						   TSipItcFunctions aITCFunction,
                           RStringF aMethod,
						   const CSIPMessageElements* aElements)
	{
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
    SetContentToSend (aElements);
	CBufFlat* buf = CBufFlat::NewL(KExternalizeBufferExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	ExternalizeL(aMethod,writeStream);
    ExternalizeL (aElements,writeStream,EFalse);
    TPtr8 bufPtr = buf->Ptr(0);
    iITCMsgArgs.Set (ESipItcArgMessageHeaders, &bufPtr);
    SendL(aIds,aITCFunction);
    writeStream.Pop();
    writeStream.Close();
    CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::SendL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SendL (TSIPIds& aIds,
						   TSipItcFunctions aITCFunction,
						   const CSIPResponseElements& aElements)
	{
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
    CBufFlat* buf = ExternalizeLC (aElements);
	TPtr8 externalizedHeaders = buf->Ptr(0);
    iITCMsgArgs.Set (ESipItcArgMessageHeaders, &externalizedHeaders);

    SetContentToSend (aElements.MessageElements());
    SendL (aIds,aITCFunction);
    CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::OutboundProxyL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SetOutboundProxyL (TSIPIds& aIds,
                                       const CSIPRouteHeader& aOutboundProxy)
    {
	CBufFlat* buf = CBufFlat::NewL(KExternalizeBufferExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	aOutboundProxy.ExternalizeL(writeStream,EFalse);
	writeStream.Pop();
	writeStream.Close();
	TPtr8 externalizedProxy = buf->Ptr(0);
	iITCMsgArgs.Set (ESipItcArgMessageHeaders, &externalizedProxy);
    SendL (aIds,ESipItcSetOutboundProxy);
    CleanupStack::PopAndDestroy(buf);
    }
    
// -----------------------------------------------------------------------------
// CSIPITCSender::ReadSIPHeaderL
// -----------------------------------------------------------------------------
//    
CSIPHeaderBase* CSIPITCSender::ReadSIPHeaderL (TSIPIds& aIds,
                                               TSipItcFunctions aITCFunction,
                                               RStringF aHeaderName)
    {
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
	HBufC8* headerBuf = HBufC8::NewLC(KMaxHeaderSizeInReadStream);
	TPtr8 headerBufPtr = headerBuf->Des();
    iITCMsgArgs.Set (ESipItcArgMessageHeaders, &headerBufPtr);
    SendL (aIds,aITCFunction);
    CSIPHeaderBase* header = InternalizeL(aHeaderName, headerBufPtr);
    CleanupStack::PopAndDestroy(headerBuf);
    return header;  
    }
    
// -----------------------------------------------------------------------------
// CSIPITCSender::ReadCallIDHeaderL
// -----------------------------------------------------------------------------
//     
CSIPCallIDHeader* CSIPITCSender::ReadCallIDHeaderL(TSIPIds& aIds)
	{
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
	HBufC8* headerBuf = HBufC8::NewLC(KMaxHeaderSizeInReadStream);
	TPtr8 headerBufPtr = headerBuf->Des();
    iITCMsgArgs.Set (ESipItcArgMessageHeaders, &headerBufPtr);
    SendL (aIds,ESipItcGetCallIDHeader);
	RDesReadStream readStream(headerBufPtr);
	readStream.PushL();          
    CSIPCallIDHeader* callID = NULL;
    if (headerBufPtr.Length() > 0)
    	{
    	callID = static_cast<CSIPCallIDHeader*>(
    				CSIPCallIDHeader::InternalizeValueL(readStream));
		}
    readStream.Pop();
    readStream.Close();
    CleanupStack::PopAndDestroy(headerBuf);
    return callID;	
	}
	
// -----------------------------------------------------------------------------
// CSIPITCSender::ReadLocalTagL
// -----------------------------------------------------------------------------
//	
RStringF CSIPITCSender::ReadLocalTagL(TSIPIds& aIds)
    {
    Mem::FillZ(&iITCMsgArgs, sizeof(iITCMsgArgs));
	HBufC8* localTagBuf = HBufC8::NewLC(KMaxTagLength);
	TPtr8 localTagBufPtr = localTagBuf->Des();
    iITCMsgArgs.Set(ESipItcArgLocalTag, &localTagBufPtr);
    SendL(aIds,ESipItcGetLocalTag);
    RStringF localTag = SIPStrings::Pool().OpenFStringL(localTagBufPtr); 
    CleanupStack::PopAndDestroy(localTagBuf);
    return localTag;
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::SetRefreshIntervalL
// -----------------------------------------------------------------------------
//    
void CSIPITCSender::SetRefreshIntervalL (TSIPIds& aIds, TInt aInterval)
    {
	iITCMsgArgs.Set(ESipItcArgRefreshInterval, aInterval);
    SendL(aIds,ESipItcSetRefreshInterval); 
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::GetRefreshIntervalL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::GetRefreshIntervalL (TSIPIds& aIds, TInt& aInterval)
    {    
    TPckgBuf<TInt> intervalPckg(0);
    iITCMsgArgs.Set(ESipItcArgRefreshInterval, &intervalPckg);
    SendL (aIds,ESipItcGetRefreshInterval);
    aInterval = intervalPckg();    
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::SetContentToSend
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SetContentToSend (const CSIPMessageElements* aElements)
    {
    if (aElements != 0)
        {
        SetContentToSend(*aElements);
        }
    else
        {
        iITCMsgArgs.Set (ESipItcArgMessageContent, &iEmptyContent);
        }
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::SetContentToSend
// -----------------------------------------------------------------------------
//
void CSIPITCSender::SetContentToSend (const CSIPMessageElements& aElements)
    {
    iITCMsgArgs.Set (ESipItcArgMessageContent, &(aElements.Content()));
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::ExternalizeLC 
// -----------------------------------------------------------------------------
//
template<class T> CBufFlat* CSIPITCSender::ExternalizeLC (const T& aElements)
	{
	CBufFlat* buf = CBufFlat::NewL(KExternalizeBufferExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	aElements.ExternalizeL(writeStream);
	writeStream.Pop();
	writeStream.Close();
    return buf;
	}

// -----------------------------------------------------------------------------
// CSIPITCSender::ExternalizeL
// -----------------------------------------------------------------------------
//
template<class T> void CSIPITCSender::ExternalizeL (const T* aElement, 
                                                    RWriteStream& aWriteStream,
                                                    TBool aAddExpectedFlag)
    {
    if (aElement)
        {
        if (aAddExpectedFlag)
            {
            aWriteStream.WriteUint8L(1); // element expected flag
            }
        aElement->ExternalizeL(aWriteStream);
        }
    else
        {
        aWriteStream.WriteUint8L(0); // element not expected flag
        }
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPITCSender::ExternalizeL (RStringF aStr, RWriteStream& aWriteStream)
    {
    TPtrC8 des(aStr.DesC());
    if (des.Length() > 0)
        {
        aWriteStream.WriteUint32L(des.Length());
	    aWriteStream.WriteL(des);
        }    
    }

// -----------------------------------------------------------------------------
// CSIPITCSender::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPITCSender::InternalizeL (RStringF aName, TDes8& aDes)
    {
    CSIPHeaderBase* header = NULL;
	if (aDes.Length() > 0)
		{
	    RDesReadStream readStream(aDes);
	    readStream.PushL();
        header = SIPHeaderLookup::InternalizeL(aName,readStream);
	    readStream.Pop();
	    readStream.Close();
        }
    return header;
    }
