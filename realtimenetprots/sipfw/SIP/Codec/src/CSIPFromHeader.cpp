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
// Name          : CSIPFromHeader.cpp
// Part of       : Codec
// Version       : SIP/4.0 
//



#include "sipfromheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPFromHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromHeader* CSIPFromHeader::DecodeL(const TDesC8& aValue)
	{
	CSIPFromHeader* from = new(ELeave)CSIPFromHeader;
    CleanupStack::PushL(from);
    from->CSIPFromToHeaderBase::ConstructL();
	from->ParseL( aValue, ETrue, ETrue );
	CleanupStack::Pop(from);
	return from;
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromHeader* CSIPFromHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPFromHeader* self = CSIPFromHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromHeader* CSIPFromHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPFromHeader* self=new(ELeave) CSIPFromHeader();
	CleanupStack::PushL(self);
	self->CSIPFromToHeaderBase::ConstructL(aSIPAddress);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromHeader* CSIPFromHeader::NewL(
    const CSIPFromToHeaderBase& aFromToHeaderBase)
	{
	CSIPFromHeader* self = CSIPFromHeader::NewLC(aFromToHeaderBase);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C 
CSIPFromHeader* CSIPFromHeader::NewLC (const CSIPFromToHeaderBase& aFromToHeaderBase)
	{
	CSIPFromHeader* self=new(ELeave) CSIPFromHeader();
	CleanupStack::PushL(self);
	self->CSIPFromToHeaderBase::ConstructL(aFromToHeaderBase);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::CSIPFromHeader
// -----------------------------------------------------------------------------
//
CSIPFromHeader::CSIPFromHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::~CSIPFromHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromHeader::~CSIPFromHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPFromHeader::CloneL () const
	{
	return CSIPFromHeader::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPFromHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EFromHeader);
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::CompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPFromHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EFromHeaderCompact);
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPFromHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPFromHeader* self = new(ELeave)CSIPFromHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//	
RPointerArray<CSIPHeaderBase> CSIPFromHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPFromHeader* from = DecodeL(aValue);
    CleanupStack::PushL(from);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(from));
	CleanupStack::Pop(from);
	return headers;	
	}
