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
// Name          : CSIPServiceRouteHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipservicerouteheader.h"
#include "sipaddress.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPServiceRouteHeader> 
CSIPServiceRouteHeader::DecodeL(const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> baseHeaders = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&baseHeaders);
	RPointerArray<CSIPServiceRouteHeader> headers;
	CleanupClosePushL(headers);
	TInt count = baseHeaders.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPServiceRouteHeader* route = 
			static_cast<CSIPServiceRouteHeader*>(baseHeaders[i]);
		headers.AppendL(route);
		}
	CleanupStack::Pop(2); // headers, baseHeaders
	baseHeaders.Close();
	return headers;
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPServiceRouteHeader* CSIPServiceRouteHeader::NewL(
    CSIPAddress* aSIPAddress)
	{
	CSIPServiceRouteHeader* self = CSIPServiceRouteHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPServiceRouteHeader* CSIPServiceRouteHeader::NewLC(
    CSIPAddress* aSIPAddress)
	{
	CSIPServiceRouteHeader* self = new(ELeave)CSIPServiceRouteHeader;
	CleanupStack::PushL(self);
	self->CSIPRouteHeaderBase::ConstructL(aSIPAddress);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::CSIPServiceRouteHeader
// -----------------------------------------------------------------------------
//
CSIPServiceRouteHeader::CSIPServiceRouteHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::~CSIPServiceRouteHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPServiceRouteHeader::~CSIPServiceRouteHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPServiceRouteHeader::CloneL() const
	{
	CSIPServiceRouteHeader* self = new(ELeave)CSIPServiceRouteHeader;
	CleanupStack::PushL(self);
	self->CSIPRouteHeaderBase::ConstructL(*this);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPServiceRouteHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EServiceRouteHeader);
	}

// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::ExternalizeSupported
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPServiceRouteHeader::ExternalizeSupported() const
	{
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// CSIPServiceRouteHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPServiceRouteHeader::BaseDecodeL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecRouteHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',', ETrue);
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
	    CSIPServiceRouteHeader* header = new(ELeave)CSIPServiceRouteHeader;
	    CleanupStack::PushL(header);
	    header->CSIPRouteHeaderBase::ConstructL();
		header->ParseL(tokenizer->Tokens()[i], ETrue, ETrue); 
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}	
