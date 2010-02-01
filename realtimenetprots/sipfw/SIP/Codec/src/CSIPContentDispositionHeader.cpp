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
// Name          : CSIPContentDispositionHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipcontentdispositionheader.h"
#include "CSIPHeaderGenericParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentDispositionHeader* 
CSIPContentDispositionHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPContentDispositionHeader* contentDispositionHeader = 
		new(ELeave) CSIPContentDispositionHeader;
    CleanupStack::PushL(contentDispositionHeader);
    contentDispositionHeader->ConstructL();
	contentDispositionHeader->ParseL(aValue);
	CleanupStack::Pop(contentDispositionHeader);
	return contentDispositionHeader;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentDispositionHeader* CSIPContentDispositionHeader::NewL(
    const TDesC8& aDispType)
	{
	CSIPContentDispositionHeader* self = 
		CSIPContentDispositionHeader::NewLC(aDispType);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentDispositionHeader* 
CSIPContentDispositionHeader::NewLC(const TDesC8& aDispType)
	{
	CSIPContentDispositionHeader* self=
		new(ELeave) CSIPContentDispositionHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aDispType);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::CSIPContentDispositionHeader
// ----------------------------------------------------------------------------
//
CSIPContentDispositionHeader::CSIPContentDispositionHeader()
:   CSIPParameterHeaderBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPContentDispositionHeader::ConstructL()
	{
	iParams = new(ELeave)CSIPHeaderGenericParams;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPContentDispositionHeader::ConstructL (const TDesC8& aDispType)
	{
	ConstructL ();
	SetDispTypeL (aDispType);
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::ConstructL
// ----------------------------------------------------------------------------
//
void 
CSIPContentDispositionHeader::ConstructL (const TDesC8& aDispType, 
										  const TDesC8& aHandlingParam)
	{
	ConstructL ();
	SetDispTypeL(aDispType);
    iParams->SetParamL(
        SIPStrings::StringF(SipStrConsts::EHandling),aHandlingParam);
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::ConstructL
// ----------------------------------------------------------------------------
//
void 
CSIPContentDispositionHeader::ConstructL (const CSIPContentDispositionHeader& 
										  aSIPContentDispositionHeader)
	{
	iParams =
		CSIPHeaderGenericParams::NewL(
									*(aSIPContentDispositionHeader.iParams));
	SetDispTypeL(aSIPContentDispositionHeader.DispType());
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::~CSIPContentDispositionHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContentDispositionHeader::~CSIPContentDispositionHeader()
	{
	delete iParams;
	delete iDispType;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContentDispositionHeader::CloneL () const
	{
    CSIPContentDispositionHeader* clone = 
        new (ELeave) CSIPContentDispositionHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPContentDispositionHeader::Name () const
	{
    return SIPStrings::StringF(SipStrConsts::EContentDispositionHeader);
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::SetDispTypeL
// ----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPContentDispositionHeader::SetDispTypeL (const TDesC8& aDispType)
	{
    SIPCodecUtils::CheckAndSetValueL(iDispType,
                                     aDispType,
                                     KErrSipCodecContentDispositionHeader);
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::DispType
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPContentDispositionHeader::DispType() const
	{
	if (iDispType != 0) 
		{
		return *iDispType;
		}
	return KNullDesC8;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeaderr::ToTextMandatoryPartLC
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPContentDispositionHeader::ToTextMandatoryPartLC () const
	{
	TUint encodedLength = 0;
	if(iDispType != 0)
		{
		encodedLength += iDispType->Length(); 
		}

	HBufC8* encodedHeader = HBufC8::NewLC (encodedLength);
	TPtr8 encodedHeaderPtr = encodedHeader->Des();

	encodedHeaderPtr.Append(*iDispType);
	return encodedHeader;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContentDispositionHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPContentDispositionHeader* self = 
		new(ELeave)CSIPContentDispositionHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPContentDispositionHeader::DoInternalizeValueL(
    RReadStream& aReadStream)
	{
    iDispType = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	iParams = CSIPHeaderGenericParams::InternalizeL (aReadStream);
	}	

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPContentDispositionHeader::ExternalizeValueL(
    RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iDispType->Length());
	if (iDispType->Length() > 0)
		{
		aWriteStream.WriteL (DispType());
		}
	iParams->ExternalizeL (aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPContentDispositionHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::BaseDecodeL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPContentDispositionHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPContentDispositionHeader* contentDispositionHeader = 
		new(ELeave) CSIPContentDispositionHeader;
    CleanupStack::PushL(contentDispositionHeader);
    contentDispositionHeader->ConstructL();
	contentDispositionHeader->ParseL (aValue);
	RPointerArray<CSIPHeaderBase> headerArray;
	User::LeaveIfError (headerArray.Append(contentDispositionHeader));
	CleanupStack::Pop(); // contentDispositionHeader
	return headerArray;
    }

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPContentDispositionHeader::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPContentDispositionHeader::Params ()
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPContentDispositionHeader::ParseMandatoryPartL
// ----------------------------------------------------------------------------
//
void 
CSIPContentDispositionHeader::ParseMandatoryPartL(const TDesC8& aMandatoryPart)
	{
	__ASSERT_ALWAYS (aMandatoryPart.Length() > 0, 
		User::Leave (KErrSipCodecContentDispositionHeader));
	TLex8 lex(aMandatoryPart);
	lex.SkipSpace();
	SetDispTypeL(lex.Remainder());
	}
