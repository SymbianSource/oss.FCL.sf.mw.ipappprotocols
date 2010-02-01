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
// Name          : CSIPReplyToHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipreplytoheader.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "CSIPHeaderGenericParams.h"
#include "CSIPTokenizer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecerr.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPReplyToHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReplyToHeader* CSIPReplyToHeader::DecodeL(const TDesC8& aValue)
	{
    __ASSERT_ALWAYS(aValue.Length() > 0,
                    User::Leave(KErrSipCodecReplyToHeader));

	CSIPReplyToHeader* header = new(ELeave)CSIPReplyToHeader;
    CleanupStack::PushL(header);
    header->ConstructL();
	header->ParseL(aValue, ETrue, ETrue);
	CleanupStack::Pop(header);
	return header;	
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReplyToHeader* CSIPReplyToHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPReplyToHeader* self = CSIPReplyToHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReplyToHeader* CSIPReplyToHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPReplyToHeader* self = new(ELeave)CSIPReplyToHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aSIPAddress);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPReplyToHeader::CSIPReplyToHeader
// ----------------------------------------------------------------------------
//
CSIPReplyToHeader::CSIPReplyToHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::~CSIPReplyToHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReplyToHeader::~CSIPReplyToHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPReplyToHeader::CloneL() const
	{
	CSIPReplyToHeader* clone = new(ELeave)CSIPReplyToHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);	
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPReplyToHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EReplyToHeader);
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPReplyToHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPReplyToHeader* self = new(ELeave)CSIPReplyToHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPReplyToHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPReplyToHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddle;
	}
	
// -----------------------------------------------------------------------------
// CSIPReplyToHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//	
RPointerArray<CSIPHeaderBase> 
CSIPReplyToHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPReplyToHeader* header = DecodeL(aValue);
    CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;	
	} 	
