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
// Name          : CSIPAuthenticationInfoHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipauthenticationinfoheader.h"
#include "CSIPAuthenticationInfoHeaderParams.h"
#include "sipcodecerr.h"
#include "CSIPTokenizer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthenticationInfoHeader*
CSIPAuthenticationInfoHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPAuthenticationInfoHeader* header = 
        new(ELeave)CSIPAuthenticationInfoHeader;
	CleanupStack::PushL (header);
    header->ConstructL(aValue);
	CleanupStack::Pop(header);
	return header;
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::CSIPAuthenticationInfoHeader
// -----------------------------------------------------------------------------
//
CSIPAuthenticationInfoHeader::CSIPAuthenticationInfoHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPAuthenticationInfoHeader::ConstructL(const TDesC8& aValue)
	{
    iParams = new(ELeave)CSIPAuthenticationInfoHeaderParams;
    iParams->ParseL(aValue);
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPAuthenticationInfoHeader::ConstructL (
    const CSIPAuthenticationInfoHeader& aHeader)
	{
    iParams = CSIPAuthenticationInfoHeaderParams::NewL(*(aHeader.iParams));
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::~CSIPAuthenticationInfoHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthenticationInfoHeader::~CSIPAuthenticationInfoHeader()
	{
	delete iParams;
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::Value
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAuthenticationInfoHeader::Value(RStringF aParam) const
    {
    return iParams->ParamValue(aParam);
    }

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::DesValue
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAuthenticationInfoHeader::DesValue(
    RStringF aParam) const
    {
    return iParams->DesParamValue(aParam);
    }

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAuthenticationInfoHeader::CloneL () const
	{
	CSIPAuthenticationInfoHeader* clone = 
        new (ELeave) CSIPAuthenticationInfoHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAuthenticationInfoHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::EAuthenticationInfoHeader);
    }

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::ExternalizeSupported
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAuthenticationInfoHeader::ExternalizeSupported () const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPAuthenticationInfoHeader::ToTextValueL () const
	{
    HBufC8* value = iParams->ToTextLC();
    CleanupStack::Pop(value);
	return value;
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPAuthenticationInfoHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPAuthenticationInfoHeader* header = 
        new(ELeave)CSIPAuthenticationInfoHeader;
	CleanupStack::PushL(header);
	header->ConstructL(aValue);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError (headers.Append(header));
	CleanupStack::Pop(header);
	return headers;    
    }

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::EncodeMultipleToOneLine
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPAuthenticationInfoHeader::EncodeMultipleToOneLine() const
	{
	return EFalse;		
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPAuthenticationInfoHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}
