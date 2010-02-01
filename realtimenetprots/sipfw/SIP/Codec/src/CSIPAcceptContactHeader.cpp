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
// Name          : CSIPAcceptContactHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipacceptcontactheader.h"
#include "CSIPAcceptContactHeaderParams.h"
#include "SIPSyntaxCheck.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

const TInt KAsterisk(42);

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPAcceptContactHeader> 
CSIPAcceptContactHeader::DecodeL(const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPAcceptContactHeader> acceptHeaders;
    CleanupClosePushL(acceptHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPAcceptContactHeader* header = 
            static_cast<CSIPAcceptContactHeader*>(baseheaders[i]);
		User::LeaveIfError (acceptHeaders.Append(header));
		}
    CleanupStack::Pop(); // acceptHeaders
    CleanupStack::Pop(); // baseheaders
    baseheaders.Close();
    return acceptHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptContactHeader* CSIPAcceptContactHeader::NewL()
	{
    CSIPAcceptContactHeader* self = CSIPAcceptContactHeader::NewLC();
    CleanupStack::Pop(self);
    return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptContactHeader* CSIPAcceptContactHeader::NewLC()
	{
    CSIPAcceptContactHeader* self = new(ELeave) CSIPAcceptContactHeader;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::CSIPAcceptContactHeader
// ----------------------------------------------------------------------------
//
CSIPAcceptContactHeader::CSIPAcceptContactHeader()
: CSIPParameterHeaderBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptContactHeader::ConstructL()
	{
    iParams = new (ELeave) CSIPAcceptContactHeaderParams;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptContactHeader::ConstructL(
                        const CSIPAcceptContactHeader& aAcceptContactHeader)
	{
    iParams = CSIPAcceptContactHeaderParams::NewL(
                                            *(aAcceptContactHeader.iParams));
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::~CSIPAcceptContactHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptContactHeader::~CSIPAcceptContactHeader()
	{
    delete iParams;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::operator==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAcceptContactHeader::operator==(
                                  const CSIPAcceptContactHeader& aHeader) const
    {
    return (*iParams == *(aHeader.iParams));
    }

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
CSIPAcceptContactHeader::InternalizeValueL(RReadStream& aReadStream)
	{
    CSIPAcceptContactHeader* header = new (ELeave)CSIPAcceptContactHeader;
    CleanupStack::PushL(header);
    header->DoInternalizeValueL(aReadStream);
    CleanupStack::Pop(header);
    return header;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptContactHeader::CloneL() const
	{
    CSIPAcceptContactHeader* header = new (ELeave)CSIPAcceptContactHeader;
    CleanupStack::PushL(header);
    header->ConstructL(*this);
    CleanupStack::Pop(header);
    return header;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::Name
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAcceptContactHeader::Name() const
	{
    return SIPStrings::StringF(SipStrConsts::EAcceptContactHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::HasCompactName
// ----------------------------------------------------------------------------
//
TBool CSIPAcceptContactHeader::HasCompactName() const
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::CompactName
// ----------------------------------------------------------------------------
//
RStringF CSIPAcceptContactHeader::CompactName() const
    {
    return SIPStrings::StringF(SipStrConsts::EAcceptContactHeaderCompact);
    }

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::MoreThanOneAllowed
// ----------------------------------------------------------------------------
//
TBool CSIPAcceptContactHeader::MoreThanOneAllowed() const
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::PreferredPlaceInMessage
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPAcceptContactHeader::PreferredPlaceInMessage() const
	{
    return CSIPHeaderBase::EMiddle;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPAcceptContactHeader::BaseDecodeL(const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	TLex8 lex(aValue);
	lex.SkipSpace();
	if (lex.Remainder().Length() > 0)
		{
		CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
		for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
			{
			CSIPAcceptContactHeader* header = 
                new(ELeave)CSIPAcceptContactHeader;
			CleanupStack::PushL(header);
			header->ConstructL();
            header->ParseL(tokenizer->Tokens()[i]);
			User::LeaveIfError (headers.Append(header));
			CleanupStack::Pop(header);
			}
		CleanupStack::PopAndDestroy(tokenizer);
		}
	else
		{
        User::Leave(KErrSipCodecAcceptContactHeader);
		}
	CleanupStack::Pop(); // headers 
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::ExternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAcceptContactHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
    iParams->ExternalizeL(aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::ToTextMandatoryPartLC
// ----------------------------------------------------------------------------
//
HBufC8* CSIPAcceptContactHeader::ToTextMandatoryPartLC() const
	{
    HBufC8* mandatorypart = HBufC8::NewLC(1);
    mandatorypart->Des().Append(KAsterisk);
    return mandatorypart;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::ParseMandatoryPartL
// ----------------------------------------------------------------------------
//
void CSIPAcceptContactHeader::ParseMandatoryPartL(const TDesC8& aMandatoryPart)
	{
    TLex8 lex(aMandatoryPart);
    lex.SkipSpace();
    if(lex.Get() != KAsterisk) // Should be asterisk '*'
        {
        User::Leave(KErrSipCodecAcceptContactHeader);
        }
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::Params
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAcceptContactHeader::Params() const
	{
    return *iParams;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::Params
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAcceptContactHeader::Params()
	{
    return *iParams;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptContactHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAcceptContactHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    iParams = CSIPAcceptContactHeaderParams::InternalizeL(aReadStream);
	}
