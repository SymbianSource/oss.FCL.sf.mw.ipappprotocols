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
// Name          : SIPCRSerializer.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "SIPCRSerializer.h"
#include "sipstrings.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "uricontainer.h"


const TInt KBufExpandSize=100;

// ----------------------------------------------------------------------------
// SIPCRSerializer::ExternalizeL
// ----------------------------------------------------------------------------
//
CBufFlat* SIPCRSerializer::ExternalizeL(CSIPRequest& aRequest)
	{
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
    writeStream.WriteUint16L(aRequest.Method().DesC().Length());
    writeStream.WriteL (aRequest.Method().DesC());
    CURIContainer* uriContainer = aRequest.RequestURI();
    if (uriContainer)
        {
        uriContainer->ExternalizeL(writeStream);
        }
	aRequest.ExternalizeHeadersL(writeStream);	
	writeStream.Pop();
	writeStream.Close();
	CleanupStack::Pop(buf);
	return buf;
	}

// -----------------------------------------------------------------------------
// SIPCRSerializer::InternalizeL
// -----------------------------------------------------------------------------
//
void SIPCRSerializer::InternalizeL(const TDesC8& aDes, CSIPRequest& aRequest)
    {
    RDesReadStream readStream(aDes);
    CleanupClosePushL (readStream);
    
    InternalizeMethodL (aRequest,readStream);
    
    CURIContainer* uriContainer = CURIContainer::InternalizeL(readStream);
    CleanupStack::PushL(uriContainer);
    aRequest.SetRequestURIL(uriContainer);
    CleanupStack::Pop(uriContainer);
    
    aRequest.InternalizeHeadersL(readStream);

    readStream.Pop();
    readStream.Close();
    }

// -----------------------------------------------------------------------------
// SIPCRSerializer::ExternalizeL
// -----------------------------------------------------------------------------
//
CBufFlat* SIPCRSerializer::ExternalizeL(CSIPResponse& aResponse)
    {
    CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	
	writeStream.WriteUint16L(aResponse.ResponseCode());
	
	TPtrC8 des(aResponse.ReasonPhrase().DesC());
	writeStream.WriteUint32L(des.Length());
	writeStream.WriteL(des);
	
	aResponse.ExternalizeHeadersL(writeStream);
	
	writeStream.Pop();
	writeStream.Close();
    CleanupStack::Pop(buf);
	return buf;
    }
    
// -----------------------------------------------------------------------------
// SIPCRSerializer::InternalizeMethodL
// -----------------------------------------------------------------------------
//
void SIPCRSerializer::InternalizeMethodL(CSIPRequest& aRequest,
                                         RReadStream& aReadStream)
    {
    RStringF method = ReadFromStreamL(aReadStream);
    CleanupClosePushL(method);    
    aRequest.SetMethodL(method);
    CleanupStack::PopAndDestroy(1); // method
    }
  
  // -----------------------------------------------------------------------------
// SIPCRSerializer::ReadFromStreamLC
// -----------------------------------------------------------------------------
//
RStringF SIPCRSerializer::ReadFromStreamL(RReadStream& aReadStream)
	{
	TUint16 bufLength = aReadStream.ReadUint16L();
	HBufC8* buf = HBufC8::NewLC (bufLength);
	TPtr8 bufPtr(buf->Des());	
	if (bufLength > 0)
		{
		aReadStream.ReadL (bufPtr,bufLength);
		}
	else 
	    {
	    }
	RStringF str = SIPStrings::Pool().OpenFStringL(bufPtr);
	CleanupStack::PopAndDestroy(buf);	
	return str;
	}
  
// End of File
