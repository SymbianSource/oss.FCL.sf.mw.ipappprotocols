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
// Name          : CSIPMaxForwardsHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipmaxforwardsheader.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMaxForwardsHeader* 
CSIPMaxForwardsHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPMaxForwardsHeader* header = new(ELeave)CSIPMaxForwardsHeader(0);
	CleanupStack::PushL(header);
	header->ParseL(aValue,KErrSipCodecMaxForwardsHeader);
	CleanupStack::Pop(header);
	return header;
	}

// -----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::CSIPMaxForwardsHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMaxForwardsHeader::CSIPMaxForwardsHeader (TUint aValue)
 : CSIPUnsignedIntHeaderBase(aValue)
	{
	}

// -----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::~CSIPMaxForwardsHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMaxForwardsHeader::~CSIPMaxForwardsHeader ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPMaxForwardsHeader::CloneL () const
	{
	return new(ELeave)CSIPMaxForwardsHeader(iValue);
	}

// -----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPMaxForwardsHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EMaxForwardsHeader);
	}

// ----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::ExternalizeSupported
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPMaxForwardsHeader::ExternalizeSupported () const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPMaxForwardsHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPMaxForwardsHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}
	
// -----------------------------------------------------------------------------
// CSIPExpiresHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPMaxForwardsHeader::BaseDecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
	CSIPMaxForwardsHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}	
