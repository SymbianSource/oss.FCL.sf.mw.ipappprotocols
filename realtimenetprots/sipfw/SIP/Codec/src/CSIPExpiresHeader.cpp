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
// Name          : CSIPExpiresHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipexpiresheader.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExpiresHeader* CSIPExpiresHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPExpiresHeader* header = new(ELeave)CSIPExpiresHeader(0);
	CleanupStack::PushL(header);
	header->ParseL(aValue,KErrSipCodecExpiresHeader);
	CleanupStack::Pop(header);
	return header;
	}

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::CSIPExpiresHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExpiresHeader::CSIPExpiresHeader (TUint aValue)
 : CSIPUnsignedIntHeaderBase(aValue)
	{
	}

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::~CSIPExpiresHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExpiresHeader::~CSIPExpiresHeader ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPExpiresHeader::CloneL () const
	{
	return new(ELeave)CSIPExpiresHeader(iValue);
	}

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPExpiresHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EExpiresHeader);
	}

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPExpiresHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPExpiresHeader* self = new(ELeave)CSIPExpiresHeader(0);
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExpiresHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPExpiresHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}
	
// -----------------------------------------------------------------------------
// CSIPExpiresHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPExpiresHeader::BaseDecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
	CSIPExpiresHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	headers.AppendL(header);
	CleanupStack::Pop(header);
	return headers;
	}
