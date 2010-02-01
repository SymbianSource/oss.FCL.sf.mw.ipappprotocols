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
// Name          : CSIPUnsupportedHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipunsupportedheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPUnsupportedHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPUnsupportedHeader> 
CSIPUnsupportedHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPUnsupportedHeader> unsupportedHeaders;
	CleanupClosePushL(unsupportedHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPUnsupportedHeader* header = 
			static_cast<CSIPUnsupportedHeader*>(headers[i]);
		unsupportedHeaders.AppendL(header);
		}
	CleanupStack::Pop(2); // unsupportedHeaders, headers
	headers.Close();
	return unsupportedHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPUnsupportedHeader* CSIPUnsupportedHeader::NewL(RStringF aValue)
	{
	CSIPUnsupportedHeader* self = CSIPUnsupportedHeader::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPUnsupportedHeader* CSIPUnsupportedHeader::NewLC(RStringF aValue)
	{
	CSIPUnsupportedHeader* self = new(ELeave)CSIPUnsupportedHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::CSIPUnsupportedHeader
// -----------------------------------------------------------------------------
//
CSIPUnsupportedHeader::CSIPUnsupportedHeader()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::CSIPUnsupportedHeader
// -----------------------------------------------------------------------------
//
CSIPUnsupportedHeader::CSIPUnsupportedHeader(
    const CSIPUnsupportedHeader& aHeader)
 : CSIPTokenHeaderBase(aHeader)
	{
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::~CSIPUnsupportedHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPUnsupportedHeader::~CSIPUnsupportedHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPUnsupportedHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPUnsupportedHeader::CloneL() const
	{
	return new(ELeave)CSIPUnsupportedHeader(*this);
	}

// ----------------------------------------------------------------------------
// CSIPUnsupportedHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPUnsupportedHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EUnsupportedHeader);
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPUnsupportedHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPUnsupportedHeader* self = new(ELeave)CSIPUnsupportedHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPUnsupportedHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddle;
	}

// -----------------------------------------------------------------------------
// CSIPUnsupportedHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPUnsupportedHeader::BaseDecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecUnsupportedHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPUnsupportedHeader* header = new(ELeave)CSIPUnsupportedHeader;
		CleanupStack::PushL(header);
		header->ConstructL(tokenizer->Tokens()[i]);		
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}
