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
// Name          : CSIPRouteHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siprouteheader.h"
#include "sipaddress.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "CSIPHeaderGenericParams.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPRouteHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPRouteHeader> 
CSIPRouteHeader::DecodeL(const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&headers);
	RPointerArray<CSIPRouteHeader> routeHeaders;
	CleanupClosePushL(routeHeaders);
	TInt count = headers.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPRouteHeader* route = static_cast<CSIPRouteHeader*>(headers[i]);
		User::LeaveIfError(routeHeaders.Append(route));
		}
	CleanupStack::Pop(2); // routeHeaders, headers
	headers.Close();
	return routeHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRouteHeader* CSIPRouteHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPRouteHeader* self = CSIPRouteHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRouteHeader* CSIPRouteHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPRouteHeader* self = new(ELeave)CSIPRouteHeader;
	CleanupStack::PushL(self);
	self->CSIPRouteHeaderBase::ConstructL(aSIPAddress);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::NewLC
// -----------------------------------------------------------------------------
//
CSIPRouteHeader* CSIPRouteHeader::NewL(const CSIPRouteHeaderBase& aHeader)
	{
	CSIPRouteHeader* self = new(ELeave)CSIPRouteHeader;
	CleanupStack::PushL(self);
	self->CSIPRouteHeaderBase::ConstructL(aHeader);
	CleanupStack::Pop(self);	
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::CSIPRouteHeader
// -----------------------------------------------------------------------------
//
CSIPRouteHeader::CSIPRouteHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::~CSIPRouteHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRouteHeader::~CSIPRouteHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRouteHeader::CloneL() const
	{
	return CSIPRouteHeader::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRouteHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::ERouteHeader);
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRouteHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPRouteHeader* self = new(ELeave)CSIPRouteHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPRouteHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
	iSIPAddress = CSIPAddress::InternalizeL (aReadStream);
	iParams = CSIPHeaderGenericParams::InternalizeL (aReadStream);
	}	

// -----------------------------------------------------------------------------
// CSIPRouteHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void 
CSIPRouteHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
	iSIPAddress->ExternalizeL (aWriteStream);
	iParams->ExternalizeL (aWriteStream);
	}
	
// -----------------------------------------------------------------------------
// CSIPRouteHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPRouteHeader::BaseDecodeL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
                     User::Leave(KErrSipCodecRouteHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',', ETrue);
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
	    CSIPRouteHeader* header = new(ELeave)CSIPRouteHeader;
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

