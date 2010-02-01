// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPContentTypeHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipcontenttypeheader.h"
#include "CSIPContentTypeHeaderParams.h"
#include "SIPSyntaxCheck.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPContentTypeHeader* 
CSIPContentTypeHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPContentTypeHeader* header = new(ELeave)CSIPContentTypeHeader;
	CleanupStack::PushL (header);
    header->ConstructL();
	header->ParseL (aValue);
	CleanupStack::Pop(header);
	return header;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPContentTypeHeader* CSIPContentTypeHeader::NewL(
    const TDesC8& aMediaType,
    const TDesC8& aMediaSubtype)
	{
	CSIPContentTypeHeader* self = 
		CSIPContentTypeHeader::NewLC(aMediaType,aMediaSubtype);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPContentTypeHeader* CSIPContentTypeHeader::NewLC(
    const TDesC8& aMediaType,
    const TDesC8& aMediaSubtype)
	{
	CSIPContentTypeHeader* self = new(ELeave)CSIPContentTypeHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aMediaType,aMediaSubtype);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::CSIPContentTypeHeader
// -----------------------------------------------------------------------------
//
CSIPContentTypeHeader::CSIPContentTypeHeader()
 : CSIPParameterHeaderBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeader::ConstructL ()
	{
	iParams = new(ELeave)CSIPContentTypeHeaderParams;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeader::ConstructL (const TDesC8& aMediaType,
										const TDesC8& aMediaSubtype)
	{
    ConstructL ();
	SetMediaTypeL (aMediaType);
	SetMediaSubtypeL (aMediaSubtype);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeader::ConstructL (const CSIPContentTypeHeader&
										aContentTypeHeader)
	{
	SetMediaTypeL (aContentTypeHeader.MediaType());
	SetMediaSubtypeL (aContentTypeHeader.MediaSubtype());
	iParams = CSIPContentTypeHeaderParams::NewL(*(aContentTypeHeader.iParams));
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::~CSIPContentTypeHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPContentTypeHeader::~CSIPContentTypeHeader()
	{
	delete iParams;
	delete iMediaSubtype;
	delete iMediaType;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContentTypeHeader::CloneL () const
	{
	CSIPContentTypeHeader* clone = new(ELeave)CSIPContentTypeHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::MediaType
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPContentTypeHeader::MediaType () const
	{
	return *iMediaType;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::SetMediaTypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPContentTypeHeader::SetMediaTypeL (const TDesC8& aMediaType)
	{
    SIPCodecUtils::CheckAndSetValueL(iMediaType,
                                     aMediaType,
                                     KErrSipCodecContentTypeHeader);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::MediaSubtype
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPContentTypeHeader::MediaSubtype () const
	{
	return *iMediaSubtype;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::SetMediaSubtypeL
// -----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPContentTypeHeader::SetMediaSubtypeL (const TDesC8& aMediaSubtype)
	{
    SIPCodecUtils::CheckAndSetValueL(iMediaSubtype,
                                     aMediaSubtype,
                                     KErrSipCodecContentTypeHeader);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPContentTypeHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EContentTypeHeader);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::HasCompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPContentTypeHeader::HasCompactName () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::CompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPContentTypeHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EContentTypeHeaderCompact);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContentTypeHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPContentTypeHeader* self = new(ELeave)CSIPContentTypeHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iMediaType->Length());
	aWriteStream.WriteL (*iMediaType);
	
	aWriteStream.WriteUint32L (iMediaSubtype->Length());
	aWriteStream.WriteL (*iMediaSubtype);
	
	iParams->ExternalizeL (aWriteStream);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    iMediaType = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    iMediaSubtype = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	iParams = CSIPContentTypeHeaderParams::InternalizeL (aReadStream);
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPContentTypeHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}
	
// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPContentTypeHeader::BaseDecodeL (const TDesC8& aValue)
	{
	CSIPContentTypeHeader* header = DecodeL(aValue);
	CleanupStack::PushL (header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}	

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPContentTypeHeader::ToTextMandatoryPartLC () const
    {
	TUint encodedLength = iMediaType->Length() + 1; // SLASH
	encodedLength += iMediaSubtype->Length(); 

	HBufC8* encodingResult = HBufC8::NewLC (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();

	encodingResultPtr.Append(*iMediaType);
	encodingResultPtr.Append('/'); // SLASH
	encodingResultPtr.Append(*iMediaSubtype);

	return encodingResult;
    }

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeader::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
    {
	__ASSERT_ALWAYS (aMandatoryPart.Length() > 0,
	                 User::Leave(KErrSipCodecContentTypeHeader));
	
	TLex8 lex(aMandatoryPart);
	lex.SkipSpace();
	TPtrC8 value(lex.Remainder());
	
	TInt slashPos = value.Locate('/');
	if (slashPos <= 0 || slashPos >= value.Length()-1)
		{
		User::Leave (KErrSipCodecContentTypeHeader);
		}
    
	SetMediaTypeL (value.Left(slashPos));
    SetMediaSubtypeL (value.Mid(slashPos+1));
    }
    
// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPContentTypeHeader::Params () const
    {
    return *iParams; 
    }

// -----------------------------------------------------------------------------
// CSIPContentTypeHeader::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPContentTypeHeader::Params ()
    {
    return *iParams; 
    }
