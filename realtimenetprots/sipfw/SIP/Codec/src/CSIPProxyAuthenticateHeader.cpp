// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPProxyAuthenticateHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/5.0 
//



#include "sipproxyauthenticateheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthenticateHeader* CSIPProxyAuthenticateHeader::DecodeL(
    const TDesC8& aValue)
	{
	CSIPProxyAuthenticateHeader* header = 
		new(ELeave)CSIPProxyAuthenticateHeader;
    CleanupStack::PushL(header);
    header->CSIPAuthenticateHeaderBase::ConstructL();
	header->ParseL(aValue);
	CleanupStack::Pop(header);
	return header;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthenticateHeader* CSIPProxyAuthenticateHeader::NewL(
    RStringF aAuthScheme)
	{
	CSIPProxyAuthenticateHeader* self = 
		CSIPProxyAuthenticateHeader::NewLC(aAuthScheme);
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthenticateHeader* CSIPProxyAuthenticateHeader::NewLC(
    RStringF aAuthScheme)
	{
	CSIPProxyAuthenticateHeader* self = new(ELeave)CSIPProxyAuthenticateHeader;
	CleanupStack::PushL(self);
	self->CSIPAuthHeaderBase::ConstructL(aAuthScheme);
    self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::CSIPProxyAuthenticateHeader
// ----------------------------------------------------------------------------
//
CSIPProxyAuthenticateHeader::CSIPProxyAuthenticateHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::~CSIPProxyAuthenticateHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPProxyAuthenticateHeader::~CSIPProxyAuthenticateHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPProxyAuthenticateHeader::CloneL() const
	{
	CSIPProxyAuthenticateHeader* clone = new(ELeave)CSIPProxyAuthenticateHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPProxyAuthenticateHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EProxyAuthenticateHeader);
	}

// -----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPProxyAuthenticateHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPProxyAuthenticateHeader* header = DecodeL(aValue);
	CleanupStack::PushL (header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPProxyAuthenticateHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPProxyAuthenticateHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPProxyAuthenticateHeader* self = new(ELeave)CSIPProxyAuthenticateHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
