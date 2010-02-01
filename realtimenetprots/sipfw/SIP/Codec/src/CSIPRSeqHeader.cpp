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
// Name          : CSIPRSeqHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "siprseqheader.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRSeqHeader* CSIPRSeqHeader::DecodeL(const TDesC8& aValue)
    {
    CSIPRSeqHeader* header = new (ELeave) CSIPRSeqHeader(0);
    CleanupStack::PushL(header);
    header->ParseL(aValue, KErrSipCodecRSeqHeader);
	CleanupStack::Pop(header);
    return header;
    }

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::CSIPRSeqHeader
// ----------------------------------------------------------------------------
//    
EXPORT_C CSIPRSeqHeader::CSIPRSeqHeader(TUint aValue)
 : CSIPUnsignedIntHeaderBase(aValue)
    {
    }
    
// ----------------------------------------------------------------------------
// CSIPRSeqHeader::~CSIPRSeqHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRSeqHeader::~CSIPRSeqHeader()
    {
    }

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRSeqHeader::InternalizeValueL(
    RReadStream& aReadStream)
    {
	CSIPRSeqHeader* self = new(ELeave)CSIPRSeqHeader(0);
	CleanupStack::PushL(self);
	self->DoInternalizeValueL (aReadStream);
	CleanupStack::Pop(self);
	return self;
    }

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRSeqHeader::CloneL() const
    {
    return new(ELeave)CSIPRSeqHeader(iValue);
    }

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::Name
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRSeqHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::ERSeqHeader);
    }

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::PreferredPlaceInMessage
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace CSIPRSeqHeader::PreferredPlaceInMessage() const
    {
    return CSIPHeaderBase::EMiddle;
    }

// ----------------------------------------------------------------------------
// CSIPRSeqHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPRSeqHeader::BaseDecodeL(const TDesC8& aValue)
    {
	RPointerArray<CSIPHeaderBase> headers;
	CSIPRSeqHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	headers.AppendL(header);
	CleanupStack::Pop(header);
	return headers;
    }
