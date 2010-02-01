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
// Name          : CSIPWWWAuthenticateHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/5.0 
//



#include "sipwwwauthenticateheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPWWWAuthenticateHeader* 
CSIPWWWAuthenticateHeader::DecodeL(const TDesC8& aValue)
	{
	CSIPWWWAuthenticateHeader* header = 
		new(ELeave)CSIPWWWAuthenticateHeader;
    CleanupStack::PushL(header);
    header->CSIPAuthenticateHeaderBase::ConstructL();
	header->ParseL(aValue);
	CleanupStack::Pop(header);
	return header;
	}

// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPWWWAuthenticateHeader* CSIPWWWAuthenticateHeader::NewL(
    RStringF aAuthScheme)
	{
	CSIPWWWAuthenticateHeader* self = 
		CSIPWWWAuthenticateHeader::NewLC(aAuthScheme);
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPWWWAuthenticateHeader* CSIPWWWAuthenticateHeader::NewLC(
    RStringF aAuthScheme)
	{
	CSIPWWWAuthenticateHeader* self = new(ELeave)CSIPWWWAuthenticateHeader;
	CleanupStack::PushL(self);
	self->CSIPAuthHeaderBase::ConstructL(aAuthScheme);
    self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::CSIPWWWAuthenticateHeader
// ----------------------------------------------------------------------------
//
CSIPWWWAuthenticateHeader::CSIPWWWAuthenticateHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::~CSIPWWWAuthenticateHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPWWWAuthenticateHeader::~CSIPWWWAuthenticateHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPWWWAuthenticateHeader::CloneL() const
	{
	CSIPWWWAuthenticateHeader* clone = new(ELeave)CSIPWWWAuthenticateHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPWWWAuthenticateHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EWWWAuthenticateHeader);
	}

// -----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPWWWAuthenticateHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPWWWAuthenticateHeader* header = DecodeL(aValue);
	CleanupStack::PushL (header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPWWWAuthenticateHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
CSIPHeaderBase* CSIPWWWAuthenticateHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPWWWAuthenticateHeader* self = new(ELeave)CSIPWWWAuthenticateHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
