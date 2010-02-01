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
// Name          : CSIPMinExpiresHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipminexpiresheader.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMinExpiresHeader* 
CSIPMinExpiresHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPMinExpiresHeader* header = new(ELeave)CSIPMinExpiresHeader(0);
	CleanupStack::PushL(header);
	header->ParseL(aValue,KErrSipCodecMinExpiresHeader);
	CleanupStack::Pop(header);
	return header;
	}

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::CSIPMinExpiresHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMinExpiresHeader::CSIPMinExpiresHeader (TUint aValue)
 : CSIPUnsignedIntHeaderBase(aValue)
	{
	}

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::~CSIPMinExpiresHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMinExpiresHeader::~CSIPMinExpiresHeader ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPMinExpiresHeader::CloneL () const
	{
	return new(ELeave)CSIPMinExpiresHeader(iValue);
	}

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPMinExpiresHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EMinExpiresHeader);
	}

// ----------------------------------------------------------------------------
// CSIPMinExpiresHeader::ExternalizeSupported
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPMinExpiresHeader::ExternalizeSupported () const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPMinExpiresHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}

// -----------------------------------------------------------------------------
// CSIPMinExpiresHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPMinExpiresHeader::BaseDecodeL(
    const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
	CSIPMinExpiresHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	headers.AppendL(header);
	CleanupStack::Pop(header);
	return headers;
	}	
