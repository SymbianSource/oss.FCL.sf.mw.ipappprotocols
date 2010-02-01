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
// Name          : CSIPContentLengthHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPContentLengthHeader.h"
#include "SIPSyntaxCheck.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPContentLengthHeader::BaseDecodeL(
    const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
	CSIPContentLengthHeader* header = new(ELeave)CSIPContentLengthHeader;
	CleanupStack::PushL (header);
	header->ParseL(aValue,KErrSipCodecContentLengthHeader);
	headers.AppendL(header);
	CleanupStack::Pop(header);
	return headers;
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::CSIPContentLengthHeader
// -----------------------------------------------------------------------------
//
CSIPContentLengthHeader::CSIPContentLengthHeader (TUint aValue)
 : CSIPUnsignedIntHeaderBase(aValue)
	{
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::~CSIPContentLengthHeader
// -----------------------------------------------------------------------------
//
CSIPContentLengthHeader::~CSIPContentLengthHeader ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPContentLengthHeader::CloneL () const
	{
	return new(ELeave)CSIPContentLengthHeader(iValue);
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPContentLengthHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EContentLengthHeader);
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::HasCompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPContentLengthHeader::HasCompactName () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::CompactName
// From CSIPHeaderBase:	
// -----------------------------------------------------------------------------
//
RStringF CSIPContentLengthHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EContentLengthHeaderCompact);
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::ExternalizeSupported
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPContentLengthHeader::ExternalizeSupported () const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPContentLengthHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPContentLengthHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}
