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
// Name          : CSIPSupportedHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipsupportedheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPSupportedHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPSupportedHeader> 
CSIPSupportedHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPSupportedHeader> supportedHeaders;
	CleanupClosePushL(supportedHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPSupportedHeader* header = 
			static_cast<CSIPSupportedHeader*>(headers[i]);
		User::LeaveIfError(supportedHeaders.Append(header));
		}
	CleanupStack::Pop(2); // supportedHeaders, headers
	headers.Close();
	return supportedHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSupportedHeader* CSIPSupportedHeader::NewL(RStringF aValue)
	{
	CSIPSupportedHeader* self = CSIPSupportedHeader::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSupportedHeader* CSIPSupportedHeader::NewLC(RStringF aValue)
	{
	CSIPSupportedHeader* self = new(ELeave)CSIPSupportedHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aValue,ETrue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::CSIPSupportedHeader
// -----------------------------------------------------------------------------
//
CSIPSupportedHeader::CSIPSupportedHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::CSIPSupportedHeader
// -----------------------------------------------------------------------------
//
CSIPSupportedHeader::CSIPSupportedHeader(const CSIPSupportedHeader& aHeader)
 : CSIPTokenHeaderBase(aHeader)
	{
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::~CSIPSupportedHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSupportedHeader::~CSIPSupportedHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPSupportedHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSupportedHeader::CloneL() const
	{
	return new(ELeave)CSIPSupportedHeader(*this);
	}

// ----------------------------------------------------------------------------
// CSIPSupportedHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPSupportedHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::ESupportedHeader);
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::HasCompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPSupportedHeader::HasCompactName() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::CompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPSupportedHeader::CompactName() const
	{
	return SIPStrings::StringF(SipStrConsts::ESupportedHeaderCompact);
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSupportedHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPSupportedHeader* self = new(ELeave)CSIPSupportedHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSupportedHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPSupportedHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddle;
	}
	
// -----------------------------------------------------------------------------
// CSIPSupportedHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPSupportedHeader::BaseDecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	TLex8 lex(aValue);
	lex.SkipSpace();
	if (lex.Remainder().Length() > 0)
		{
		CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
		for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
			{
			CSIPSupportedHeader* header = new(ELeave)CSIPSupportedHeader;
			CleanupStack::PushL(header);
			header->ConstructL(tokenizer->Tokens()[i]);
			User::LeaveIfError (headers.Append(header));
			CleanupStack::Pop(header);
			}
		CleanupStack::PopAndDestroy(tokenizer);
		}
	else
		{
		CSIPSupportedHeader* header = new(ELeave)CSIPSupportedHeader;
		CleanupStack::PushL(header);
		header->ConstructL(KNullDesC8,ETrue);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::Pop(); // headers 
	return headers;
	}	
