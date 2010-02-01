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
// Name          : CSIPRecordRouteHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siprecordrouteheader.h"
#include "sipaddress.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPRecordRouteHeader> 
CSIPRecordRouteHeader::DecodeL(const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> baseHeaders = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&baseHeaders);
	RPointerArray<CSIPRecordRouteHeader> headers;
	CleanupClosePushL(headers);
	TInt count = baseHeaders.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPRecordRouteHeader* route = 
			static_cast<CSIPRecordRouteHeader*>(baseHeaders[i]);
		User::LeaveIfError(headers.Append(route));
		}
	CleanupStack::Pop(2); // headers, baseHeaders
	baseHeaders.Close();
	return headers;
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRecordRouteHeader* CSIPRecordRouteHeader::NewL(
    CSIPAddress* aSIPAddress)
	{
	CSIPRecordRouteHeader* self = CSIPRecordRouteHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRecordRouteHeader* CSIPRecordRouteHeader::NewLC(
    CSIPAddress* aSIPAddress)
	{
	CSIPRecordRouteHeader* self = new(ELeave)CSIPRecordRouteHeader;
	CleanupStack::PushL(self);
	self->CSIPRouteHeaderBase::ConstructL(aSIPAddress);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::NewLC
// -----------------------------------------------------------------------------
//
CSIPRecordRouteHeader* CSIPRecordRouteHeader::NewL(
    const CSIPRouteHeaderBase& aHeader)
	{
	CSIPRecordRouteHeader* self = new(ELeave)CSIPRecordRouteHeader;
	CleanupStack::PushL(self);
	self->CSIPRouteHeaderBase::ConstructL(aHeader);
	CleanupStack::Pop(self);	
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::CSIPRecordRouteHeader
// -----------------------------------------------------------------------------
//
CSIPRecordRouteHeader::CSIPRecordRouteHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::~CSIPRecordRouteHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRecordRouteHeader::~CSIPRecordRouteHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRecordRouteHeader::CloneL() const
	{
	return CSIPRecordRouteHeader::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRecordRouteHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::ERecordRouteHeader);
	}

// ----------------------------------------------------------------------------
// CSIPRecordRouteHeader::ExternalizeSupported
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPRecordRouteHeader::ExternalizeSupported() const
	{
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CSIPRecordRouteHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPRecordRouteHeader::BaseDecodeL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecRouteHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',', ETrue);
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
	    CSIPRecordRouteHeader* header = new(ELeave)CSIPRecordRouteHeader;
	    CleanupStack::PushL(header);
	    header->CSIPRouteHeaderBase::ConstructL();
		header->ParseL (tokenizer->Tokens()[i], ETrue, ETrue); 
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers
	return headers;
	}		
