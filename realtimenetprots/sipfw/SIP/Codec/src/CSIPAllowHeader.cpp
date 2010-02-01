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
// Name          : CSIPAllowHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipallowheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPAllowHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPAllowHeader> 
CSIPAllowHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPAllowHeader> allowHeaders;
	CleanupClosePushL(allowHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPAllowHeader* header = static_cast<CSIPAllowHeader*>(headers[i]);
		User::LeaveIfError(allowHeaders.Append(header));
		}
	CleanupStack::Pop(2); // allowHeaders, headers
	headers.Close();
	return allowHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAllowHeader* CSIPAllowHeader::NewL(RStringF aValue)
	{
	CSIPAllowHeader* self = CSIPAllowHeader::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAllowHeader* CSIPAllowHeader::NewLC(RStringF aValue)
	{
	CSIPAllowHeader* self = new(ELeave)CSIPAllowHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aValue,ETrue);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::CSIPAllowHeader
// ----------------------------------------------------------------------------
//
CSIPAllowHeader::CSIPAllowHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::CSIPAllowHeader
// ----------------------------------------------------------------------------
//
CSIPAllowHeader::CSIPAllowHeader(const CSIPAllowHeader& aHeader)
 : CSIPTokenHeaderBase(aHeader)
	{
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::~CSIPAllowHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAllowHeader::~CSIPAllowHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAllowHeader::CloneL() const
	{
	return new(ELeave)CSIPAllowHeader(*this);
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAllowHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EAllowHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAllowHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPAllowHeader* self = new(ELeave)CSIPAllowHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAllowHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPAllowHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}
	
// -----------------------------------------------------------------------------
// CSIPAllowHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPAllowHeader::BaseDecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	TLex8 lex(aValue);
	lex.SkipSpace();
	if (lex.Remainder().Length() > 0)
		{
		CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',',EFalse,ETrue);
		for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
			{
			CSIPAllowHeader* header = new(ELeave)CSIPAllowHeader;
			CleanupStack::PushL(header);
			header->ConstructL(tokenizer->Tokens()[i]);
			User::LeaveIfError (headers.Append(header));
			CleanupStack::Pop(header);
			}
		CleanupStack::PopAndDestroy(tokenizer);
		}
	else
		{
		CSIPAllowHeader* header = new(ELeave)CSIPAllowHeader;
		CleanupStack::PushL(header);
		header->ConstructL(KNullDesC8,ETrue);
		User::LeaveIfError (headers.Append(header));
		CleanupStack::Pop(header);
		}
	CleanupStack::Pop(); // headers 
	return headers;
	}	
