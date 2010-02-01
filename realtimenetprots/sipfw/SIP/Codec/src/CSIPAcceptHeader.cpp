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
// Name          : CSIPAcceptHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipacceptheader.h"
#include "CSIPAcceptHeaderParams.h"
#include "SIPSyntaxCheck.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPAcceptHeader> 
CSIPAcceptHeader::DecodeL (const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPAcceptHeader> acceptHeaders;
    CleanupClosePushL(acceptHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPAcceptHeader* header = 
            static_cast<CSIPAcceptHeader*>(baseheaders[i]);
		User::LeaveIfError (acceptHeaders.Append(header));
		}
    CleanupStack::Pop(2); //acceptHeaders, baseheaders
    baseheaders.Close();
    return acceptHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptHeader* CSIPAcceptHeader::NewL (
    const TDesC8& aMediaType,
    const TDesC8& aMediaSubtype)
	{
	CSIPAcceptHeader* self = 
	    CSIPAcceptHeader::NewLC (aMediaType,aMediaSubtype);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptHeader* CSIPAcceptHeader::NewLC (
    const TDesC8& aMediaType,
    const TDesC8& aMediaSubtype)
	{
	CSIPAcceptHeader* self = new(ELeave)CSIPAcceptHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aMediaType,aMediaSubtype);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::CSIPAcceptHeader
// ----------------------------------------------------------------------------
//
CSIPAcceptHeader::CSIPAcceptHeader(TBool aEmpty)
 : CSIPParameterHeaderBase( ';' ), iEmpty(aEmpty)
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeader::ConstructL ()
	{
	iParams = new(ELeave)CSIPAcceptHeaderParams;
    iMediaType = KNullDesC8().AllocL();
	iMediaSubtype = KNullDesC8().AllocL();
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeader::ConstructL (const TDesC8& aMediaType,
                                   const TDesC8& aMediaSubtype)
	{
	iParams = new(ELeave)CSIPAcceptHeaderParams;
	SetMediaTypeL (aMediaType);
	SetMediaSubtypeL (aMediaSubtype);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeader::ConstructL (const CSIPAcceptHeader& aAcceptHeader)
	{
    if (iEmpty)
        {
        ConstructL ();
        }
    else
        {
	    SetMediaTypeL (aAcceptHeader.MediaType());
	    SetMediaSubtypeL (aAcceptHeader.MediaSubtype());
	    iParams = CSIPAcceptHeaderParams::NewL(*(aAcceptHeader.iParams));
        }
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::~CSIPAcceptHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptHeader::~CSIPAcceptHeader()
	{
	delete iParams;
	delete iMediaSubtype;
	delete iMediaType;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptHeader::CloneL () const
	{
    CSIPAcceptHeader* clone = new (ELeave) CSIPAcceptHeader(iEmpty);
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAcceptHeader::Name () const
	{
	return SIPStrings::StringF(SipStrConsts::EAcceptHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::IsEmpty
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAcceptHeader::IsEmpty () const
    {
    return iEmpty;
    }

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::MediaType
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAcceptHeader::MediaType () const
	{
	return *iMediaType;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::SetMediaTypeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAcceptHeader::SetMediaTypeL (const TDesC8& aMediaType)
	{
    __ASSERT_ALWAYS (!iEmpty, User::Leave(KErrArgument));
    SIPCodecUtils::CheckAndSetValueL(iMediaType,
                                     aMediaType,
                                     KErrSipCodecAcceptHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::MediaSubtype
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAcceptHeader::MediaSubtype () const
	{
	return *iMediaSubtype;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::SetMediaSubtypeL
// ----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPAcceptHeader::SetMediaSubtypeL (const TDesC8& aMediaSubtype)
	{
    __ASSERT_ALWAYS (!iEmpty, User::Leave(KErrArgument));
	HBufC8* tmp = aMediaSubtype.AllocLC();
	tmp->Des().Trim();
	if (!SIPSyntaxCheck::Token(*tmp)) 
		{
		User::Leave(KErrSipCodecAcceptHeader);
		}
	CleanupStack::Pop(tmp);
	delete iMediaSubtype;
	iMediaSubtype = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::QParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPAcceptHeader::QParameter () const
	{
    return iParams->RealParamValue(SIPStrings::StringF(SipStrConsts::EQ));
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::SetQParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAcceptHeader::SetQParameterL (TReal aQValue)
	{
    __ASSERT_ALWAYS (!iEmpty, User::Leave(KErrArgument));
	iParams->SetParamL(SIPStrings::StringF(SipStrConsts::EQ), aQValue);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPAcceptHeader::MoreThanOneAllowed () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptHeader::InternalizeValueL (
    RReadStream& aReadStream)
	{
	CSIPAcceptHeader* self = new(ELeave)CSIPAcceptHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
    if (!iEmpty)
        {
        aWriteStream.WriteUint8L(1);
	    aWriteStream.WriteUint32L (iMediaType->Length());
	    aWriteStream.WriteL (*iMediaType);
	    aWriteStream.WriteUint32L (iMediaSubtype->Length());
	    aWriteStream.WriteL (*iMediaSubtype);
	    iParams->ExternalizeL (aWriteStream);
        }
    else
        {
        aWriteStream.WriteUint8L(0);
        }
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    if (aReadStream.ReadUint8L()) // not empty
        {
        iMediaType = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
        iMediaSubtype = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	    iParams = CSIPAcceptHeaderParams::InternalizeL(aReadStream);
        }
    else
        {
        iEmpty = ETrue;
        ConstructL();
        }
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPAcceptHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPAcceptHeader::BaseDecodeL(const TDesC8& aValue)
    {
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 remainder(lex.Remainder());

	if (remainder.Length() == 0) 
		{
		CSIPAcceptHeader* header = new(ELeave)CSIPAcceptHeader(ETrue);
		CleanupStack::PushL(header);
		header->ConstructL();
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
    else
        {
	    CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(remainder, ',');
	    for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		    {
		    CSIPAcceptHeader* header = new(ELeave)CSIPAcceptHeader;
		    CleanupStack::PushL (header);
		    header->ConstructL();
		    header->ParseL(tokenizer->Tokens()[i]);
		    headers.AppendL(header);
		    CleanupStack::Pop(header);
		    }
        CleanupStack::PopAndDestroy(tokenizer);
        }

    CleanupStack::Pop(); // headers 
	return headers;
    }

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPAcceptHeader::ToTextMandatoryPartLC () const
	{
    HBufC8* encodingResult = 0;
    if (!iEmpty)
        {
	    TUint encodedLength = iMediaType->Length() + 1; // SLASH
	    encodedLength += iMediaSubtype->Length(); 

	    encodingResult = HBufC8::NewLC (encodedLength);
	    TPtr8 encodingResultPtr = encodingResult->Des();

	    encodingResultPtr.Append(*iMediaType);
	    encodingResultPtr.Append('/'); // SLASH
	    encodingResultPtr.Append(*iMediaSubtype);
        }
    else
        {
        encodingResult = KNullDesC8().AllocLC();
        }
	return encodingResult;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeader::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
	{
	__ASSERT_ALWAYS (aMandatoryPart.Length() > 0, 
		User::Leave(KErrSipCodecAcceptHeader));
	TLex8 lex(aMandatoryPart);
	lex.SkipSpace();
	TPtrC8 remainder(lex.Remainder());

    const TInt KMaxSlashPos = remainder.Length()-2;
	TInt slashPos = remainder.Locate('/');
	if (slashPos <= 0 || slashPos > KMaxSlashPos)
		{
		User::Leave (KErrSipCodecAcceptHeader);
		}
	
	SetMediaTypeL (remainder.Left(slashPos));
	remainder.Set(remainder.Mid(slashPos+1));
	SetMediaSubtypeL (remainder);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAcceptHeader::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAcceptHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAcceptHeader::Params () 
    {
    return *iParams; 
    }
