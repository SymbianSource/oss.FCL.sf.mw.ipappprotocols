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
// Name          : CSIPAcceptLanguageHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipacceptlanguageheader.h"
#include "CSIPAcceptHeaderParams.h"
#include "SIPSyntaxCheck.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

_LIT8(KAsterisk, "*");


// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPAcceptLanguageHeader> 
CSIPAcceptLanguageHeader::DecodeL(const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPAcceptLanguageHeader> acceptHeaders;
    CleanupClosePushL(acceptHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPAcceptLanguageHeader* header = 
            static_cast<CSIPAcceptLanguageHeader*>(baseheaders[i]);
		User::LeaveIfError (acceptHeaders.Append(header));
		}
    CleanupStack::Pop(2); //acceptHeaders, baseheaders
    baseheaders.Close();
    return acceptHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptLanguageHeader* CSIPAcceptLanguageHeader::NewL(
    const TDesC8& aLanguageRange)
	{
	CSIPAcceptLanguageHeader* self = 
		CSIPAcceptLanguageHeader::NewLC(aLanguageRange);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptLanguageHeader* CSIPAcceptLanguageHeader::NewLC(
    const TDesC8& aLanguageRange)
	{
	CSIPAcceptLanguageHeader* self = new(ELeave)CSIPAcceptLanguageHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aLanguageRange);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::CSIPAcceptLanguageHeader
// ----------------------------------------------------------------------------
//
CSIPAcceptLanguageHeader::CSIPAcceptLanguageHeader()
:   CSIPParameterHeaderBase( ';' )
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptLanguageHeader::ConstructL()
	{
	iParams = new(ELeave)CSIPAcceptHeaderParams;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptLanguageHeader::ConstructL(const TDesC8& aLanguageRange)
	{
	SetLanguageRangeL(aLanguageRange);
	ConstructL ();
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptLanguageHeader::ConstructL(
    const CSIPAcceptLanguageHeader& aAcceptLanguageHeader)
	{
	SetLanguageRangeL (aAcceptLanguageHeader.LanguageRange());
	iParams = CSIPAcceptHeaderParams::NewL(*(aAcceptLanguageHeader.iParams));
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::~CSIPAcceptLanguageHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptLanguageHeader::~CSIPAcceptLanguageHeader()
	{
	delete iParams;
	delete iLanguageRange;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptLanguageHeader::CloneL() const
	{
    CSIPAcceptLanguageHeader* clone = new(ELeave)CSIPAcceptLanguageHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAcceptLanguageHeader::Name () const
	{
    return SIPStrings::StringF(SipStrConsts::EAcceptLanguageHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPAcceptLanguageHeader::MoreThanOneAllowed() const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::LanguageRange
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAcceptLanguageHeader::LanguageRange() const
	{
	return *iLanguageRange;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::SetLanguageRangeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAcceptLanguageHeader::SetLanguageRangeL(
    const TDesC8& aLanguageRange)
	{
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aLanguageRange,'-');
	TLex8 lex(tokenizer->Tokens()[0]);
	lex.SkipSpace();
	TPtrC8 remainder(lex.Remainder());
	if (remainder.Length() > 0) 
		{
    	if (!SIPSyntaxCheck::AlphaMaxSize8(remainder)) 
    		{
    		if (tokenizer->Tokens().Count() == 1) 
    			{
    			if (remainder.CompareF(KAsterisk) != 0) 
    				{
    				User::Leave(KErrSipCodecAcceptLanguageHeader);
    				}
    			}
    		else
    			{
    			User::Leave(KErrSipCodecAcceptLanguageHeader);
    			}
    		}
    	for (TInt i=1; i < tokenizer->Tokens().Count(); i++) 
    		{
    		if(!SIPSyntaxCheck::AlphaMaxSize8(tokenizer->Tokens()[i])) 
    			{
    			User::Leave(KErrSipCodecAcceptLanguageHeader);
    			}
    		}
		}
	CleanupStack::PopAndDestroy(tokenizer);
	HBufC8* tmp = aLanguageRange.AllocL();
	tmp->Des().Trim();
	delete iLanguageRange;
	iLanguageRange = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::QParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPAcceptLanguageHeader::QParameter() const
	{
    return iParams->RealParamValue(SIPStrings::StringF(SipStrConsts::EQ));
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::SetQParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAcceptLanguageHeader::SetQParameterL(TReal aQValue)
	{
	iParams->SetParamL(SIPStrings::StringF(SipStrConsts::EQ), aQValue);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptLanguageHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPAcceptLanguageHeader* self = new(ELeave)CSIPAcceptLanguageHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAcceptLanguageHeader::ExternalizeValueL(
    RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iLanguageRange->Length());
	aWriteStream.WriteL(*iLanguageRange);
	iParams->ExternalizeL(aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAcceptLanguageHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    iLanguageRange = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	iParams = CSIPAcceptHeaderParams::InternalizeL (aReadStream);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPAcceptLanguageHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPAcceptLanguageHeader::BaseDecodeL(
    const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPAcceptLanguageHeader* header = new(ELeave)CSIPAcceptLanguageHeader;
		CleanupStack::PushL (header);
		header->ConstructL();
		header->ParseL(tokenizer->Tokens()[i]);
		if (header->LanguageRange().Length() == 0 &&
		    header->ParamCount() > 0)
		    {
		    User::Leave(KErrSipCodecAcceptEncodingHeader);
		    }		
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPAcceptLanguageHeader::ToTextMandatoryPartLC() const
	{
	return iLanguageRange->AllocLC();
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAcceptLanguageHeader::ParseMandatoryPartL(
    const TDesC8& aMandatoryPart)
	{
	SetLanguageRangeL(aMandatoryPart);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAcceptLanguageHeader::Params() const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAcceptLanguageHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAcceptLanguageHeader::Params()
    {
    return *iParams; 
    }
