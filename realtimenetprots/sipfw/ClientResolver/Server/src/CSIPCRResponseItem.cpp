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
// Name          : CSIPCRResponseItem.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPCRResponseItem.h"
#include "sipresponse.h"


// ----------------------------------------------------------------------------
// CSIPCRResponseItem::CSIPCRResponseItem
// ----------------------------------------------------------------------------
//
CSIPCRResponseItem::CSIPCRResponseItem(TUint32 aRequestId,
                                       TUid aChannel)
 : iError (KErrNone),
   iRequestId(aRequestId),
   iChannel(aChannel),
   iResponse(NULL),
   iContent(NULL)
	{
	}

// ----------------------------------------------------------------------------
// CSIPCRResponseItem::CSIPCRResponseItem
// ----------------------------------------------------------------------------
//
CSIPCRResponseItem::CSIPCRResponseItem(TUint32 aRequestId, 
                                       CBufFlat* aResponse,
                                       HBufC8* aContent)
 : iError (KErrNone),
   iRequestId(aRequestId),
   iResponse(aResponse),
   iContent(aContent)
	{
	}

// ----------------------------------------------------------------------------
// CSIPCRResponseItem::CSIPCRResponseItem
// ----------------------------------------------------------------------------
//
CSIPCRResponseItem::CSIPCRResponseItem(TUint32 aRequestId, 
                                       TInt aCompletionCode)
 : iError (aCompletionCode),
   iRequestId(aRequestId),
   iResponse(NULL),
   iContent(NULL)
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPCRResponseItem::~CSIPCRResponseItem
// ----------------------------------------------------------------------------
//
CSIPCRResponseItem::~CSIPCRResponseItem ()
	{
    delete iResponse;
    delete iContent;
	}	

// ----------------------------------------------------------------------------
// CSIPCRResponseItem::RequestId
// ----------------------------------------------------------------------------
//
TUint32 CSIPCRResponseItem::RequestId() const
    {
    return iRequestId;
    }

// ----------------------------------------------------------------------------
// CSIPCRResponseItem::SIPResponse
// ----------------------------------------------------------------------------
//
CBufFlat* CSIPCRResponseItem::SIPResponse()
    {
    return iResponse;
    }

// ----------------------------------------------------------------------------
// CSIPCRResponseItem::SIPResponseContent
// ----------------------------------------------------------------------------
//
HBufC8* CSIPCRResponseItem::SIPResponseContent()
    {
    return iContent;
    }
    
// -----------------------------------------------------------------------------
// CSIPCRResponseItem::BufSizes
// -----------------------------------------------------------------------------
//
TSIPCRMessageBufSizes CSIPCRResponseItem::BufSizes () const
    {
    TSIPCRMessageBufSizes sizes;
    if (iResponse) 
        {
        sizes.iHeaderBufSize = iResponse->Ptr(0).Length();
        }
    if (iContent)
        {
        sizes.iContentBufSize = iContent->Length();
        }
    return sizes;
    }    

// ----------------------------------------------------------------------------
// CSIPCRResponseItem::Channel
// ----------------------------------------------------------------------------
//
TUid CSIPCRResponseItem::Channel() const
    {
    return iChannel;
    }
// ----------------------------------------------------------------------------
// CSIPCRResponseItem::Error
// ----------------------------------------------------------------------------
//
TInt CSIPCRResponseItem::Error () const
	{
    return iError;
	}

// End of File


