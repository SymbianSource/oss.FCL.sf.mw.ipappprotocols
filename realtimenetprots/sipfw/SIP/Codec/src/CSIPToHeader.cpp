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
// Name          : CSIPToHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siptoheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPToHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* CSIPToHeader::DecodeL(const TDesC8& aValue)
	{
	CSIPToHeader* to = new(ELeave)CSIPToHeader;
    CleanupStack::PushL(to);
    to->ConstructL();
	to->ParseL( aValue, ETrue, ETrue);
	CleanupStack::Pop(to);
	return to;
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* CSIPToHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPToHeader* self = CSIPToHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* CSIPToHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPToHeader* self = new(ELeave)CSIPToHeader;
	CleanupStack::PushL(self);
	self->CSIPFromToHeaderBase::ConstructL(aSIPAddress);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* CSIPToHeader::NewL(
    const CSIPFromToHeaderBase& aFromToHeaderBase)
	{
	CSIPToHeader* self = CSIPToHeader::NewLC(aFromToHeaderBase);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* CSIPToHeader::NewLC(
    const CSIPFromToHeaderBase& aFromToHeaderBase)
	{
	CSIPToHeader* self = new(ELeave)CSIPToHeader;
	CleanupStack::PushL(self);
	self->CSIPFromToHeaderBase::ConstructL(aFromToHeaderBase);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::CSIPToHeader
// -----------------------------------------------------------------------------
//
CSIPToHeader::CSIPToHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::~CSIPToHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader::~CSIPToHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPToHeader::CloneL () const
	{
	return CSIPToHeader::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPToHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EToHeader);
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::CompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPToHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EToHeaderCompact);
	}

// -----------------------------------------------------------------------------
// CSIPToHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPToHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPToHeader* self = new(ELeave)CSIPToHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPToHeader::DecodeL
// -----------------------------------------------------------------------------
//	
RPointerArray<CSIPHeaderBase> CSIPToHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPToHeader* to = DecodeL(aValue);
    CleanupStack::PushL(to);
	RPointerArray<CSIPHeaderBase> headers;
	headers.AppendL(to);
	CleanupStack::Pop(to);
	return headers;	
	}	
