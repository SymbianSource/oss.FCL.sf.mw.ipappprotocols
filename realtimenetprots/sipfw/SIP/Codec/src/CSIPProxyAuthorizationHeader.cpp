// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPProxyAuthorizationHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/6.0 
//



#include "sipproxyauthorizationheader.h"
#include "CSIPAuthorizationHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthorizationHeader* CSIPProxyAuthorizationHeader::DecodeL(
    const TDesC8& aValue)
	{
	CSIPProxyAuthorizationHeader* header = 
	    new(ELeave)CSIPProxyAuthorizationHeader;
    CleanupStack::PushL(header);
    header->CSIPAuthorizationHeaderBase::ConstructL();
	header->ParseL(aValue);
	CleanupStack::Pop(header);
	return header;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthorizationHeader* CSIPProxyAuthorizationHeader::NewL(
    RStringF aAuthScheme)
	{
	CSIPProxyAuthorizationHeader* self = 
		CSIPProxyAuthorizationHeader::NewLC(aAuthScheme);
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthorizationHeader* CSIPProxyAuthorizationHeader::NewLC(
    RStringF aAuthScheme)
	{
	CSIPProxyAuthorizationHeader* self = 
		new(ELeave)CSIPProxyAuthorizationHeader;
	CleanupStack::PushL(self);
	self->CSIPAuthHeaderBase::ConstructL(aAuthScheme);
    self->ConstructL();
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthorizationHeader* CSIPProxyAuthorizationHeader::NewL(
	const CSIPAuthorizationHeaderBase& aHeader)
	{
	CSIPProxyAuthorizationHeader* self = 
		CSIPProxyAuthorizationHeader::NewLC(aHeader);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthorizationHeader* CSIPProxyAuthorizationHeader::NewLC(
	const CSIPAuthorizationHeaderBase& aHeader)
	{
	CSIPProxyAuthorizationHeader* self = 
		new(ELeave)CSIPProxyAuthorizationHeader;
	CleanupStack::PushL(self);
	self->CSIPAuthorizationHeaderBase::ConstructL(aHeader);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::CSIPProxyAuthorizationHeader
// ----------------------------------------------------------------------------
//
CSIPProxyAuthorizationHeader::CSIPProxyAuthorizationHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::~CSIPProxyAuthorizationHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthorizationHeader::~CSIPProxyAuthorizationHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::ExternalizeSupported
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProxyAuthorizationHeader::ExternalizeSupported() const
	{
	return ETrue;	
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPProxyAuthorizationHeader::CloneL() const
	{
	return CSIPProxyAuthorizationHeader::NewL(*this);
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPProxyAuthorizationHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EProxyAuthorizationHeader);
	}

// -----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPProxyAuthorizationHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPProxyAuthorizationHeader* header = DecodeL(aValue);
	CleanupStack::PushL (header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPProxyAuthorizationHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPProxyAuthorizationHeader* self = 
	    new(ELeave)CSIPProxyAuthorizationHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
		
// ----------------------------------------------------------------------------
// CSIPProxyAuthorizationHeader::ExternalizeValueL
// ----------------------------------------------------------------------------
//
void 
CSIPProxyAuthorizationHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
	TPtrC8 authScheme(AuthScheme().DesC());
	aWriteStream.WriteUint32L(authScheme.Length());
	aWriteStream.WriteL(authScheme);
	iParams->ExternalizeL(aWriteStream);	
	}
