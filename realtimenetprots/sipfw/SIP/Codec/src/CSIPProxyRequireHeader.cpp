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
// Name          : CSIPProxyRequireHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipproxyrequireheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPProxyRequireHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPProxyRequireHeader> 
CSIPProxyRequireHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPProxyRequireHeader> proxyRequireHeaders;
	CleanupClosePushL(proxyRequireHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPProxyRequireHeader* header = 
			static_cast<CSIPProxyRequireHeader*>(headers[i]);
		User::LeaveIfError(proxyRequireHeaders.Append(header));
		}
	CleanupStack::Pop(2); // proxyRequireHeaders, headers
	headers.Close();
	return proxyRequireHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyRequireHeader* CSIPProxyRequireHeader::NewL(RStringF aValue)
	{
	CSIPProxyRequireHeader* self = CSIPProxyRequireHeader::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyRequireHeader* CSIPProxyRequireHeader::NewLC(RStringF aValue)
	{
	CSIPProxyRequireHeader* self = new(ELeave)CSIPProxyRequireHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::CSIPProxyRequireHeader
// -----------------------------------------------------------------------------
//
CSIPProxyRequireHeader::CSIPProxyRequireHeader()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::CSIPProxyRequireHeader
// -----------------------------------------------------------------------------
//
CSIPProxyRequireHeader::CSIPProxyRequireHeader(
    const CSIPProxyRequireHeader& aHeader)
  : CSIPTokenHeaderBase(aHeader)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::~CSIPProxyRequireHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyRequireHeader::~CSIPProxyRequireHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPProxyRequireHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPProxyRequireHeader::CloneL() const
	{
    return new(ELeave)CSIPProxyRequireHeader(*this);
	}

// ----------------------------------------------------------------------------
// CSIPProxyRequireHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPProxyRequireHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EProxyRequireHeader);
	}

// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPProxyRequireHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPProxyRequireHeader* self = new(ELeave)CSIPProxyRequireHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPProxyRequireHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleTop;
	}
	
// -----------------------------------------------------------------------------
// CSIPProxyRequireHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPProxyRequireHeader::BaseDecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecProxyRequireHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPProxyRequireHeader* header = new(ELeave)CSIPProxyRequireHeader;
		CleanupStack::PushL(header);
		header->ConstructL(tokenizer->Tokens()[i]);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}


