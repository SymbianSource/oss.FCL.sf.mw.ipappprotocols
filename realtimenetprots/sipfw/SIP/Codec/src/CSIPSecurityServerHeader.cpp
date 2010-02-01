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
// Name          : CSIPSecurityServerHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipsecurityserverheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPSecurityServerHeader> 
CSIPSecurityServerHeader::DecodeL (const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPSecurityServerHeader> serverHeaders;
    CleanupClosePushL(serverHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPSecurityServerHeader* header = 
            static_cast<CSIPSecurityServerHeader*>(baseheaders[i]);
		User::LeaveIfError (serverHeaders.Append(header));
		}
    CleanupStack::Pop(2); //serverHeaders, baseheaders
    baseheaders.Close();
    return serverHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityServerHeader* CSIPSecurityServerHeader::NewL(
    const TDesC8& aMechanism)
    {
	CSIPSecurityServerHeader* self = 
        CSIPSecurityServerHeader::NewLC(aMechanism);
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityServerHeader* CSIPSecurityServerHeader::NewLC(
    const TDesC8& aMechanism)
    {
	CSIPSecurityServerHeader* self = new(ELeave)CSIPSecurityServerHeader;
	CleanupStack::PushL(self);
    self->CSIPSecurityHeaderBase::ConstructL(aMechanism);
	return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::CSIPSecurityServerHeader
// -----------------------------------------------------------------------------
//
CSIPSecurityServerHeader::CSIPSecurityServerHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::~CSIPSecurityServerHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityServerHeader::~CSIPSecurityServerHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSecurityServerHeader::CloneL () const
	{
    CSIPSecurityServerHeader* clone = new(ELeave) CSIPSecurityServerHeader;
    CleanupStack::PushL( clone );
    clone->ConstructL( *this );
    CleanupStack::Pop( clone );
	return clone;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//		
EXPORT_C RStringF CSIPSecurityServerHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::ESecurityServerHeader);
    }

// -----------------------------------------------------------------------------
// CSIPSecurityServerHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPSecurityServerHeader::BaseDecodeL(const TDesC8& aValue)
    {
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecSecurityHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
        CSIPSecurityServerHeader* header = 
            new(ELeave) CSIPSecurityServerHeader;
        CleanupStack::PushL(header);
        header->ConstructL();
		header->ParseL (tokenizer->Tokens()[i]);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
    }
