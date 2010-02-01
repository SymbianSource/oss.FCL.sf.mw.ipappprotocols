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
// Name          : CSIPCallIDHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipcallidheader.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipcodecutils.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPCallIDHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPCallIDHeader* CSIPCallIDHeader::DecodeL (const TDesC8& aValue)
	{
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 value(lex.Remainder());
	__ASSERT_ALWAYS(value.Length() > 0, User::Leave(KErrSipCodecCallIdHeader));
	
	CSIPCallIDHeader* self = new(ELeave)CSIPCallIDHeader;
	CleanupStack::PushL(self);		
	self->ParseL (value);
	self->iCallID = value.AllocL();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::CSIPCallIDHeader
// ----------------------------------------------------------------------------
//
CSIPCallIDHeader::CSIPCallIDHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::~CSIPCallIDHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPCallIDHeader::~CSIPCallIDHeader()
	{
	delete iCallID;
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPCallIDHeader::CloneL () const
	{
	CSIPCallIDHeader* clone = new(ELeave)CSIPCallIDHeader;
	CleanupStack::PushL(clone);
	clone->iCallID = iCallID->AllocL();
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::operator==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool 
CSIPCallIDHeader::operator==(const CSIPCallIDHeader& aCallIDHeader)
	{
	return (iCallID->Compare(*(aCallIDHeader.iCallID)) == 0);
	}
	
// -----------------------------------------------------------------------------
// CSIPCSeqHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
CSIPCallIDHeader::InternalizeValueL (RReadStream& aReadStream)
	{
	CSIPCallIDHeader* self = new(ELeave)CSIPCallIDHeader;
	CleanupStack::PushL(self);
	self->iCallID = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPCallIDHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::ECallIDHeader);
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::HasCompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPCallIDHeader::HasCompactName () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::CompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RStringF CSIPCallIDHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::ECallIDHeaderCompact);
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::ToTextValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPCallIDHeader::ToTextValueL () const
	{
	return iCallID->AllocL();
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPCallIDHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddle;
	}
	
// ----------------------------------------------------------------------------
// CSIPCallIDHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPCallIDHeader::BaseDecodeL (const TDesC8& aValue)
	{
	CSIPCallIDHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError (headers.Append(header));
	CleanupStack::Pop(header);
	return headers;	
	}	

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPCallIDHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iCallID->Length());
	aWriteStream.WriteL(*iCallID);
	}

// ----------------------------------------------------------------------------
// CSIPCallIDHeader::ParseL
// ----------------------------------------------------------------------------
//
void CSIPCallIDHeader::ParseL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
		User::Leave(KErrSipCodecCallIdHeader));
    
    TLex8 lex(aValue);
    TPtrC8 value(lex.NextToken());
    lex.SkipSpace();
    if (lex.Remainder().Length() > 0) 
		{
		User::Leave(KErrSipCodecCallIdHeader);
		}

	TInt atPos = value.Locate('@');
	if (atPos == 0 || atPos == value.Length()-1)
		{
		User::Leave(KErrSipCodecCallIdHeader);
		}

	if (atPos < 0) // word
		{
		if (!SIPSyntaxCheck::Word(value))
			{
			User::Leave(KErrSipCodecCallIdHeader);
			}
		}
	else // word@word
		{
		if (!SIPSyntaxCheck::Word(value.Left(atPos)))
			{
			User::Leave(KErrSipCodecCallIdHeader);
			}
		if (!SIPSyntaxCheck::Word(value.Mid(atPos+1)))
			{
			User::Leave(KErrSipCodecCallIdHeader);
			}
		}
	}
