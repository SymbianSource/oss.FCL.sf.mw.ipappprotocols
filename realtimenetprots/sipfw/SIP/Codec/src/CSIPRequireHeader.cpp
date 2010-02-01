// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPRequireHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siprequireheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPRequireHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPRequireHeader> 
CSIPRequireHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPRequireHeader> requireHeaders;
	CleanupClosePushL(requireHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPRequireHeader* header = static_cast<CSIPRequireHeader*>(headers[i]);
		User::LeaveIfError(requireHeaders.Append(header));
		}
	CleanupStack::Pop(2); // requireHeaders, headers
	headers.Close();
	return requireHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPRequireHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequireHeader* CSIPRequireHeader::NewL(RStringF aValue)
	{
	CSIPRequireHeader* self = CSIPRequireHeader::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequireHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequireHeader* CSIPRequireHeader::NewLC(RStringF aValue)
	{
	CSIPRequireHeader* self = new(ELeave)CSIPRequireHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequireHeader::CSIPRequireHeader
// -----------------------------------------------------------------------------
//
CSIPRequireHeader::CSIPRequireHeader()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPRequireHeader::CSIPRequireHeader
// -----------------------------------------------------------------------------
//
CSIPRequireHeader::CSIPRequireHeader(const CSIPRequireHeader& aHeader)
  : CSIPTokenHeaderBase(aHeader)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRequireHeader::~CSIPRequireHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRequireHeader::~CSIPRequireHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPRequireHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRequireHeader::CloneL() const
	{
	return new(ELeave)CSIPRequireHeader(*this);
	}

// ----------------------------------------------------------------------------
// CSIPRequireHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRequireHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::ERequireHeader);
	}

// -----------------------------------------------------------------------------
// CSIPRequireHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRequireHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPRequireHeader* self = new(ELeave)CSIPRequireHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRequireHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPRequireHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddleTop;
	}
	
// -----------------------------------------------------------------------------
// CSIPRequireHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPRequireHeader::BaseDecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecRequireHeader));
                     	
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPRequireHeader* header = new(ELeave)CSIPRequireHeader;
		CleanupStack::PushL(header);
		header->ConstructL(tokenizer->Tokens()[i]);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}
