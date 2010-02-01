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
// Name          : CSIPSecurityClientHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipsecurityclientheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPSecurityClientHeader> 
CSIPSecurityClientHeader::DecodeL (const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPSecurityClientHeader> clientHeaders;
    CleanupClosePushL(clientHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPSecurityClientHeader* header = 
            static_cast<CSIPSecurityClientHeader*>(baseheaders[i]);
		User::LeaveIfError (clientHeaders.Append(header));
		}
    CleanupStack::Pop(2); //clientHeaders, baseheaders
    baseheaders.Close();
    return clientHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityClientHeader* CSIPSecurityClientHeader::NewL(
    const TDesC8& aMechanismName)
	{
	CSIPSecurityClientHeader* self = 
        CSIPSecurityClientHeader::NewLC(aMechanismName);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityClientHeader* CSIPSecurityClientHeader::NewLC(
    const TDesC8& aMechanismName)
	{
	CSIPSecurityClientHeader* self = new(ELeave)CSIPSecurityClientHeader;
	CleanupStack::PushL(self);
    self->CSIPSecurityHeaderBase::ConstructL(aMechanismName);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::CSIPSecurityClientHeader
// -----------------------------------------------------------------------------
//
CSIPSecurityClientHeader::CSIPSecurityClientHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::~CSIPSecurityClientHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityClientHeader::~CSIPSecurityClientHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSecurityClientHeader::CloneL () const
	{
    CSIPSecurityClientHeader* clone = new(ELeave) CSIPSecurityClientHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// ----------------------------------------------------------------------------
// CSIPSecurityClientHeader::Name
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPSecurityClientHeader::Name () const
    {
    return SIPStrings::StringF(SipStrConsts::ESecurityClientHeader);
    }

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::BaseDecodeL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPSecurityClientHeader::BaseDecodeL(const TDesC8& aValue)
    {
	__ASSERT_ALWAYS (aValue.Length() > 0,
                     User::Leave(KErrSipCodecSecurityHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPSecurityClientHeader* header = 
            new(ELeave) CSIPSecurityClientHeader;
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

// -----------------------------------------------------------------------------
// CSIPSecurityClientHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSecurityClientHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPSecurityClientHeader* self = new(ELeave)CSIPSecurityClientHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

