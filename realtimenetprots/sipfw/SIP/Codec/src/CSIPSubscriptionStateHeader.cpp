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
// Name          : CSIPSubscriptionStateHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipsubscriptionstateheader.h"
#include "CSIPSubscriptionStateHeaderParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscriptionStateHeader* 
CSIPSubscriptionStateHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPSubscriptionStateHeader* header = 
	    new(ELeave)CSIPSubscriptionStateHeader;
    CleanupStack::PushL(header);
    header->ConstructL();
	header->ParseL(aValue);
	CleanupStack::Pop(header);
	return header;	
    }

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscriptionStateHeader* CSIPSubscriptionStateHeader::NewL(
    const TDesC8& aSubStateValue)
	{
	CSIPSubscriptionStateHeader* self = 
		CSIPSubscriptionStateHeader::NewLC(aSubStateValue);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscriptionStateHeader* CSIPSubscriptionStateHeader::NewLC(
    const TDesC8& aSubStateValue)
	{
	CSIPSubscriptionStateHeader* self = 
        new(ELeave) CSIPSubscriptionStateHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aSubStateValue);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::CSIPSubscriptionStateHeader
// ----------------------------------------------------------------------------
//
CSIPSubscriptionStateHeader::CSIPSubscriptionStateHeader()
:   CSIPParameterHeaderBase( ';' )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSubscriptionStateHeader::ConstructL()
	{
	iParams = new(ELeave)CSIPSubscriptionStateHeaderParams;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSubscriptionStateHeader::ConstructL (const TDesC8& aSubStateValue)
	{
	ConstructL ();
	SetSubStateValueL (aSubStateValue);
	}

// -----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSubscriptionStateHeader::ConstructL (const CSIPSubscriptionStateHeader&
										aSubscriptionStateHeader)
	{
    SetSubStateValueL(aSubscriptionStateHeader.SubStateValue());
    iParams = CSIPSubscriptionStateHeaderParams::NewL(
        *(aSubscriptionStateHeader.iParams));
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::~CSIPSubscriptionStateHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPSubscriptionStateHeader::~CSIPSubscriptionStateHeader()
	{
	delete iParams;
	delete iSubStateValue;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSubscriptionStateHeader::CloneL () const
	{
	CSIPSubscriptionStateHeader* clone = 
        new(ELeave)CSIPSubscriptionStateHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPSubscriptionStateHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::ESubscriptionStateHeader);
    }

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::SetSubStateValueL
// ----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPSubscriptionStateHeader::SetSubStateValueL (const TDesC8& aSubStateValue)
	{
    SIPCodecUtils::CheckAndSetValueL(iSubStateValue,
                                     aSubStateValue,
                                     KErrSipCodecSubscriptionStateHeader);
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::SubStateValue
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPSubscriptionStateHeader::SubStateValue() const
	{
    return *iSubStateValue;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::ExpiresParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPSubscriptionStateHeader::ExpiresParameter () const
	{
	return iParams->IntParamValue(SIPStrings::StringF(SipStrConsts::EExpires));
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::SetExpiresParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPSubscriptionStateHeader::SetExpiresParameterL (TInt aExpiresParam)
	{
	iParams->SetParamL (
        SIPStrings::StringF(SipStrConsts::EExpires), 
        aExpiresParam);
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::RetryAfterParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPSubscriptionStateHeader::RetryAfterParameter () const
	{
	return iParams->IntParamValue(
        SIPStrings::StringF(SipStrConsts::ERetryAfter));
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::SetRetryAfterParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPSubscriptionStateHeader::SetRetryAfterParameterL(
    TInt aRetryAfterParam)
	{
	iParams->SetParamL (
        SIPStrings::StringF(SipStrConsts::ERetryAfter), 
        aRetryAfterParam);
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPSubscriptionStateHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPSubscriptionStateHeader* self = new(ELeave)CSIPSubscriptionStateHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPSubscriptionStateHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    iSubStateValue = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	iParams = CSIPSubscriptionStateHeaderParams::InternalizeL (aReadStream);
	}	

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPSubscriptionStateHeader::ExternalizeValueL(
    RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iSubStateValue->Length());
	if (iSubStateValue->Length() > 0)
		{
		aWriteStream.WriteL (SubStateValue());
		}
	iParams->ExternalizeL (aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPSubscriptionStateHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPSubscriptionStateHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPSubscriptionStateHeader* header = DecodeL(aValue);
    CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headerArray;
	User::LeaveIfError (headerArray.Append(header));
	CleanupStack::Pop(header);
	return headerArray;
    }

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderr::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPSubscriptionStateHeader::ToTextMandatoryPartLC () const
	{
	return iSubStateValue->AllocLC();
	}
	
// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
void 
CSIPSubscriptionStateHeader::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
	{
	SetSubStateValueL(aMandatoryPart);
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPSubscriptionStateHeader::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPSubscriptionStateHeader::Params ()
    {
    return *iParams; 
    }
