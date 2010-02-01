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
// Name          : CSIPSecurityVerifyHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipsecurityverifyheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPSecurityVerifyHeader> 
CSIPSecurityVerifyHeader::DecodeL (const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPSecurityVerifyHeader> verifyHeaders;
    CleanupClosePushL(verifyHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPSecurityVerifyHeader* header = 
            static_cast<CSIPSecurityVerifyHeader*>(baseheaders[i]);
		verifyHeaders.AppendL(header);
		}
    CleanupStack::Pop(2); //verifyHeaders, baseheaders
    baseheaders.Close();
    return verifyHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityVerifyHeader* CSIPSecurityVerifyHeader::NewL(
    const TDesC8& aMechanismName)
	{
	CSIPSecurityVerifyHeader* self = 
        CSIPSecurityVerifyHeader::NewLC(aMechanismName);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityVerifyHeader* CSIPSecurityVerifyHeader::NewLC(
    const TDesC8& aMechanismName)
	{
	CSIPSecurityVerifyHeader* self = new(ELeave)CSIPSecurityVerifyHeader;
	CleanupStack::PushL(self);
	self->CSIPSecurityHeaderBase::ConstructL(aMechanismName);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityVerifyHeader* CSIPSecurityVerifyHeader::NewL(
    const CSIPSecurityHeaderBase& aHeader)
    {
    CSIPSecurityVerifyHeader* self = CSIPSecurityVerifyHeader::NewLC(aHeader);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityVerifyHeader* CSIPSecurityVerifyHeader::NewLC(const
            CSIPSecurityHeaderBase& aHeader)
    {
    CSIPSecurityVerifyHeader* self = new(ELeave) CSIPSecurityVerifyHeader;
    CleanupStack::PushL(self);
    self->ConstructL(aHeader);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::CSIPSecurityVerifyHeader
// -----------------------------------------------------------------------------
//
CSIPSecurityVerifyHeader::CSIPSecurityVerifyHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::~CSIPSecurityVerifyHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityVerifyHeader::~CSIPSecurityVerifyHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSecurityVerifyHeader::CloneL () const
	{
    CSIPSecurityVerifyHeader* clone = new(ELeave) CSIPSecurityVerifyHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPSecurityVerifyHeader::Name () const
	{
    return SIPStrings::StringF(SipStrConsts::ESecurityVerifyHeader);
	}

// -----------------------------------------------------------------------------
// CSIPSecurityVerifyHeader::BaseDecodeL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPSecurityVerifyHeader::BaseDecodeL(const TDesC8& aValue)
    {
	__ASSERT_ALWAYS (aValue.Length() > 0,
                     User::Leave(KErrSipCodecSecurityHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPSecurityVerifyHeader* header = new(ELeave)CSIPSecurityVerifyHeader;
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




