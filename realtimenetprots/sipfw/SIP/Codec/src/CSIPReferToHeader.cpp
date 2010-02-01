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
// Name          : CSIPReferToHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "siprefertoheader.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "CSIPHeaderGenericParams.h"
#include "CSIPTokenizer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecerr.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPReferToHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReferToHeader* CSIPReferToHeader::DecodeL(const TDesC8& aValue)
	{	
	CSIPReferToHeader* header = new(ELeave)CSIPReferToHeader;
    CleanupStack::PushL(header);
    header->ConstructL();
	header->ParseL( aValue, ETrue, ETrue );
	CleanupStack::Pop(header);
	return header;	
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReferToHeader* CSIPReferToHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPReferToHeader* self = CSIPReferToHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReferToHeader* CSIPReferToHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPReferToHeader* self = new(ELeave)CSIPReferToHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aSIPAddress);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPReferToHeader::CSIPReferToHeader
// ----------------------------------------------------------------------------
//
CSIPReferToHeader::CSIPReferToHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::~CSIPReferToHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPReferToHeader::~CSIPReferToHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPReferToHeader::CloneL() const
	{
	CSIPReferToHeader* clone = new(ELeave)CSIPReferToHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);	
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPReferToHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EReferToHeader);
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::HasCompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPReferToHeader::HasCompactName() const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::CompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RStringF CSIPReferToHeader::CompactName() const
	{
	return SIPStrings::StringF(SipStrConsts::EReferToHeaderCompact);
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPReferToHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPReferToHeader* self = new(ELeave)CSIPReferToHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPReferToHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPReferToHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddle;
	}
	
// -----------------------------------------------------------------------------
// CSIPReferToHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//	
RPointerArray<CSIPHeaderBase> 
CSIPReferToHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPReferToHeader* header = DecodeL(aValue);
    CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;	
	} 	
