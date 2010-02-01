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
// Name          : CSIPClientDiscoverySerializer.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPClientDiscoverySerializer.h"
#include "sipstrings.h"
#include "sipcontenttypeheader.h"
#include "siprequest.h"
#include "uricontainer.h"
#include "sipresponse.h"
#include "sipstrconsts.h"


// CONSTANTS
const TInt KBufExpandSize=100;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::NewL
// ----------------------------------------------------------------------------
//
CSIPClientDiscoverySerializer* CSIPClientDiscoverySerializer::NewL()
    {
    CSIPClientDiscoverySerializer* self = 
                                    CSIPClientDiscoverySerializer::NewLC ();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::NewL
// ----------------------------------------------------------------------------
//
CSIPClientDiscoverySerializer* CSIPClientDiscoverySerializer::NewLC()
    {
    CSIPClientDiscoverySerializer* self = 
                                    new (ELeave) CSIPClientDiscoverySerializer;
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::CSIPClientDiscoverySerializer
// ----------------------------------------------------------------------------
//
CSIPClientDiscoverySerializer::CSIPClientDiscoverySerializer ()
	{
	}

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPClientDiscoverySerializer::ConstructL ()
	{
	SIPStrings::OpenL();
	}

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::~CSIPCSSerializer
// ----------------------------------------------------------------------------
//
CSIPClientDiscoverySerializer::~CSIPClientDiscoverySerializer ()
	{
	SIPStrings::Close();
	}

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::ExternalizeSIPRequestL
// ----------------------------------------------------------------------------
//
CBufFlat* CSIPClientDiscoverySerializer::ExternalizeSIPRequestL(
    CSIPRequest& aRequest)
	{
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	ExternalizeRequestL(aRequest, writeStream);
	writeStream.Pop();
	writeStream.Close();
	CleanupStack::Pop(buf);
	return buf;
	}

// ----------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::InternalizeSIPResponseL
// ----------------------------------------------------------------------------
//
CSIPResponse* CSIPClientDiscoverySerializer::InternalizeSIPResponseL(
    const TDesC8& aResponse)
    {
    RDesReadStream readStream(aResponse);
    CleanupClosePushL(readStream);
    
    TUint responseCode = readStream.ReadUint16L();
    TUint32 reasonPhraseLength = readStream.ReadUint32L();
    HBufC8* reasonPhraseBuf = HBufC8::NewLC(reasonPhraseLength);
    TPtr8 reasonPhrasePtr(reasonPhraseBuf->Des());
    readStream.ReadL(reasonPhrasePtr,reasonPhraseLength);
    RStringF reasonPhrase = SIPStrings::Pool().OpenFStringL(reasonPhrasePtr);
    CleanupStack::PopAndDestroy(reasonPhraseBuf);
    CleanupClosePushL(reasonPhrase);
    CSIPResponse* response = CSIPResponse::NewL(responseCode, reasonPhrase);
    CleanupStack::PopAndDestroy(1); //reasonPhrase 
    CleanupStack::PushL(response);
    response->InternalizeHeadersL(readStream);
    CleanupStack::Pop(response);
    
    readStream.Pop();
    readStream.Close();
 
    return response;
    }

// ---------------------------------------------------------------------------
// CSIPClientDiscoverySerializer::ExternalizeRequestL
// ---------------------------------------------------------------------------
//
void CSIPClientDiscoverySerializer::ExternalizeRequestL(
    CSIPRequest& aRequest, 
    RWriteStream& aWriteStream)
    {
    aWriteStream.WriteUint16L(aRequest.Method().DesC().Length());
    aWriteStream.WriteL (aRequest.Method().DesC());
    CURIContainer* uriContainer = aRequest.RequestURI();
	uriContainer->ExternalizeL(aWriteStream);
	aRequest.ExternalizeHeadersL(aWriteStream);
    }

//  End of File  
