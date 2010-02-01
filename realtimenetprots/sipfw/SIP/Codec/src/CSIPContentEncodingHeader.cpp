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
// Name          : CSIPContentEncodingHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipcontentencodingheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPContentEncodingHeader> 
CSIPContentEncodingHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPContentEncodingHeader> contentEncodingHeaders;
	CleanupClosePushL(contentEncodingHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPContentEncodingHeader* contentEncoding = 
			static_cast<CSIPContentEncodingHeader*>(headers[i]);
		User::LeaveIfError(contentEncodingHeaders.Append(contentEncoding));
		}
	CleanupStack::Pop(2); // contentEncodingHeaders, headers
	headers.Close();
	return contentEncodingHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentEncodingHeader* CSIPContentEncodingHeader::NewL(
    RStringF aValue)
	{
	CSIPContentEncodingHeader* self = CSIPContentEncodingHeader::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentEncodingHeader* 
CSIPContentEncodingHeader::NewLC(RStringF aValue)
	{
	CSIPContentEncodingHeader* self = new(ELeave)CSIPContentEncodingHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::CSIPContentEncodingHeader
// ----------------------------------------------------------------------------
//
CSIPContentEncodingHeader::CSIPContentEncodingHeader()
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::CSIPContentEncodingHeader
// ----------------------------------------------------------------------------
//
CSIPContentEncodingHeader::CSIPContentEncodingHeader(
    const CSIPContentEncodingHeader& aHeader)
 : CSIPTokenHeaderBase(aHeader)
	{
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::~CSIPContentEncodingHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentEncodingHeader::~CSIPContentEncodingHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContentEncodingHeader::CloneL() const
	{
	return new(ELeave)CSIPContentEncodingHeader(*this);
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPContentEncodingHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EContentEncodingHeader);
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::HasCompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPContentEncodingHeader::HasCompactName() const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::CompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RStringF CSIPContentEncodingHeader::CompactName() const
	{
	return SIPStrings::StringF(SipStrConsts::EContentEncodingHeaderCompact);
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContentEncodingHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPContentEncodingHeader* self = new(ELeave)CSIPContentEncodingHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContentEncodingHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPContentEncodingHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}
	
// -----------------------------------------------------------------------------
// CSIPContentEncodingHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPContentEncodingHeader::BaseDecodeL(
    const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecContentEncodingHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPContentEncodingHeader* header = 
			new(ELeave)CSIPContentEncodingHeader;
		CleanupStack::PushL(header);
		header->ConstructL(tokenizer->Tokens()[i]);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}
