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
// Name          : CSIPAuthorizationHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipauthorizationheader.h"
#include "CSIPAuthorizationHeaderParams.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeader* CSIPAuthorizationHeader::DecodeL(
    const TDesC8& aValue)
	{
	CSIPAuthorizationHeader* header = new(ELeave)CSIPAuthorizationHeader;
    CleanupStack::PushL(header);
    header->CSIPAuthorizationHeaderBase::ConstructL();
	header->ParseL(aValue);
	CleanupStack::Pop(header);
	return header;
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeader* CSIPAuthorizationHeader::NewL(
    RStringF aAuthScheme)
	{
	CSIPAuthorizationHeader* self = CSIPAuthorizationHeader::NewLC(aAuthScheme);
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeader* CSIPAuthorizationHeader::NewLC(
    RStringF aAuthScheme)
	{
	CSIPAuthorizationHeader* self = new(ELeave)CSIPAuthorizationHeader;
	CleanupStack::PushL(self);
	self->CSIPAuthHeaderBase::ConstructL(aAuthScheme);
    self->ConstructL();
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeader* CSIPAuthorizationHeader::NewL(
    const CSIPAuthorizationHeaderBase& aHeader)
	{
	CSIPAuthorizationHeader* self = CSIPAuthorizationHeader::NewLC(aHeader);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeader* 
CSIPAuthorizationHeader::NewLC (const CSIPAuthorizationHeaderBase& aHeader)
	{
	CSIPAuthorizationHeader* self = new(ELeave)CSIPAuthorizationHeader;
	CleanupStack::PushL(self);
	self->CSIPAuthorizationHeaderBase::ConstructL(aHeader);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::CSIPAuthorizationHeader
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeader::CSIPAuthorizationHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::~CSIPAuthorizationHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeader::~CSIPAuthorizationHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::ExternalizeSupported
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAuthorizationHeader::ExternalizeSupported() const
	{
	return ETrue;	
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAuthorizationHeader::CloneL() const
	{
	return CSIPAuthorizationHeader::NewL(*this);
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAuthorizationHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EAuthorizationHeader);
	}

// -----------------------------------------------------------------------------
// CSIPAuthorizationHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPAuthorizationHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPAuthorizationHeader* header = DecodeL(aValue);
	CleanupStack::PushL (header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPAuthorizationHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPAuthorizationHeader* self = new(ELeave)CSIPAuthorizationHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
		
// ----------------------------------------------------------------------------
// CSIPAuthorizationHeader::ExternalizeValueL
// ----------------------------------------------------------------------------
//
void 
CSIPAuthorizationHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
	TPtrC8 authScheme(AuthScheme().DesC());
	aWriteStream.WriteUint32L(authScheme.Length());
	aWriteStream.WriteL(authScheme);
	iParams->ExternalizeL(aWriteStream);	
	}
