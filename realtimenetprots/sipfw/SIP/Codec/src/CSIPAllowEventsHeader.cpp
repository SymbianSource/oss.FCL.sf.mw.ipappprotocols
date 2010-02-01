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
// Name          : CSIPAllowEventsHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipalloweventsheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

_LIT8(KDot, ".");


// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPAllowEventsHeader> 
CSIPAllowEventsHeader::DecodeL (const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPAllowEventsHeader> allowHeaders;
    CleanupClosePushL(allowHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPAllowEventsHeader* header = 
            static_cast<CSIPAllowEventsHeader*>(baseheaders[i]);
		User::LeaveIfError (allowHeaders.Append(header));
		}
    CleanupStack::Pop(2); //allowHeaders, baseheaders
    baseheaders.Close();
    return allowHeaders;
    }

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAllowEventsHeader* CSIPAllowEventsHeader::NewL(
    const TDesC8& aEventPackage)
	{
	CSIPAllowEventsHeader* self = CSIPAllowEventsHeader::NewLC(aEventPackage);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAllowEventsHeader* CSIPAllowEventsHeader::NewLC(
    const TDesC8& aEventPackage)
	{
	CSIPAllowEventsHeader* self=new(ELeave) CSIPAllowEventsHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aEventPackage);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::CSIPAllowEventsHeader
// ----------------------------------------------------------------------------
//
CSIPAllowEventsHeader::CSIPAllowEventsHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAllowEventsHeader::ConstructL()
	{
	iEventTemplates = new(ELeave) CDesC8ArraySeg(1);
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAllowEventsHeader::ConstructL (const TDesC8& aEventPackage)
	{
	ConstructL();
	SetEventPackageL (aEventPackage);
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ConstructL
// ----------------------------------------------------------------------------
//
void 
CSIPAllowEventsHeader::ConstructL (const CSIPAllowEventsHeader& 
										aSIPAllowEventsHeader)
	{
	SetEventTemplatesL(*aSIPAllowEventsHeader.iEventTemplates);
	SetEventPackageL(aSIPAllowEventsHeader.EventPackage());
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::~CSIPAllowEventsHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAllowEventsHeader::~CSIPAllowEventsHeader()
	{
	delete iEventTemplates;
	delete iEventPackage;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAllowEventsHeader::CloneL () const
	{
    CSIPAllowEventsHeader* clone = new (ELeave) CSIPAllowEventsHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAllowEventsHeader::Name () const
	{
    return SIPStrings::StringF(SipStrConsts::EAllowEventsHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::HasCompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPAllowEventsHeader::HasCompactName () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::CompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RStringF CSIPAllowEventsHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EAllowEventsHeaderCompact);
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::SetEventPackageL
// ----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPAllowEventsHeader::SetEventPackageL (const TDesC8& aEventPackage)
	{
	HBufC8* tmp = aEventPackage.AllocLC();
	tmp->Des().Trim();
	if (!SIPSyntaxCheck::Token(*tmp) || (tmp->Find(KDot) != KErrNotFound))
		{
		User::Leave(KErrSipCodecAllowEventsHeader);
		}
	CleanupStack::Pop(tmp);
	delete iEventPackage;
	iEventPackage = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::EventPackage
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAllowEventsHeader::EventPackage() const
	{
	return *iEventPackage;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::SetEventTemplatesL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAllowEventsHeader::SetEventTemplatesL(
    const MDesC8Array& aEventTemplates)
	{
	CDesC8ArraySeg* eventTemplates = new(ELeave) CDesC8ArraySeg(1);
	CleanupStack::PushL(eventTemplates);
	for (TInt i=0; i < aEventTemplates.MdcaCount(); i++)
		{
		CheckTemplateL(aEventTemplates.MdcaPoint(i));
		eventTemplates->AppendL(aEventTemplates.MdcaPoint(i));
		}
    CleanupStack::Pop(eventTemplates);
	delete iEventTemplates;
	iEventTemplates = eventTemplates;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::EventTemplates
// ----------------------------------------------------------------------------
//
EXPORT_C const MDesC8Array& CSIPAllowEventsHeader::EventTemplates () const
	{
	return *iEventTemplates;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPAllowEventsHeader::MoreThanOneAllowed () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::CheckTemplateL
// ----------------------------------------------------------------------------
//
void CSIPAllowEventsHeader::CheckTemplateL (const TDesC8& aParam) const
	{
	// event-template Param
	if (!SIPSyntaxCheck::Token(aParam) || 
        ((aParam).Find(KDot) != KErrNotFound))
		{
		User::Leave(KErrSipCodecAllowEventsHeader);
		}
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ToTextValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPAllowEventsHeader::ToTextValueL () const
	{
	TUint encodedLength = 0;
	RPointerArray<HBufC8> templatesAsText;
	CleanupStack::PushL (TCleanupItem(ResetAndDestroy,&templatesAsText));

	encodedLength += iEventPackage->Length(); 
	for (TInt i=0; i < iEventTemplates->MdcaCount(); i++)
		{
		HBufC8* templateAsText = (iEventTemplates->MdcaPoint(i)).AllocLC();
		encodedLength += templateAsText->Length() + 1;// param separator length
		User::LeaveIfError (templatesAsText.Append(templateAsText));
		CleanupStack::Pop(templateAsText);
		}

	HBufC8* encodedHeader = HBufC8::NewL (encodedLength);
	TPtr8 encodedHeaderPtr = encodedHeader->Des();

	encodedHeaderPtr.Append(*iEventPackage);
	
	for (TInt j=0; j < templatesAsText.Count(); j++)
		{
		encodedHeaderPtr.Append('.'); // dot
		encodedHeaderPtr.Append (*templatesAsText[j]);
		}
	
	CleanupStack::PopAndDestroy(); // templatesAsText
	return encodedHeader;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAllowEventsHeader::InternalizeValueL (
    RReadStream& aReadStream)
	{
	CSIPAllowEventsHeader* self = new(ELeave)CSIPAllowEventsHeader;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAllowEventsHeader::DoInternalizeValueL (RReadStream& aReadStream)
	{
    iEventPackage = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    
    TUint32 templateCount = aReadStream.ReadUint32L();

	for (TUint32 i=0; i < templateCount; i++)
		{
        HBufC8* value = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
        CleanupStack::PushL(value);
		iEventTemplates->AppendL(*value);
		CleanupStack::PopAndDestroy(value);
		}
	}	

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void 
CSIPAllowEventsHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iEventPackage->Length());
	if (iEventPackage->Length() > 0)
		{
		aWriteStream.WriteL (this->EventPackage());
		}
	aWriteStream.WriteUint32L (iEventTemplates->MdcaCount());
	for (TInt i=0; i < iEventTemplates->MdcaCount(); i++)
		{
		aWriteStream.WriteUint32L (iEventTemplates->MdcaPoint(i).Length());
		aWriteStream.WriteL (iEventTemplates->MdcaPoint(i));
		}
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPAllowEventsHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPAllowEventsHeader::BaseDecodeL (
    const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
		User::Leave(KErrSipCodecAllowEventsHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPAllowEventsHeader* allowEventsHeader = 
						new (ELeave) CSIPAllowEventsHeader;
        CleanupStack::PushL(allowEventsHeader);
        allowEventsHeader->ConstructL();
		allowEventsHeader->ParseL (tokenizer->Tokens()[i]);
		User::LeaveIfError (headers.Append(allowEventsHeader));
		CleanupStack::Pop(); // allowEventsHeader
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ParseL
// ----------------------------------------------------------------------------
//
void CSIPAllowEventsHeader::ParseL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
		User::Leave (KErrSipCodecAllowEventsHeader));
	HBufC8* tmp = aValue.AllocLC();
	tmp->Des().Trim();

	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(*tmp, '.');
	TLex8 lex(tokenizer->Tokens()[0]);
	lex.SkipSpace();
	this->SetEventPackageL(lex.Remainder());

	for (TInt j=1; j < tokenizer->Tokens().Count(); j++)
		{
		CheckTemplateL((tokenizer->Tokens()[j]));
		iEventTemplates->AppendL((tokenizer->Tokens()[j]));
		}

	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::PopAndDestroy(tmp);
	}

// ----------------------------------------------------------------------------
// CSIPAllowEventsHeader::ResetAndDestroy
// ----------------------------------------------------------------------------
//
void CSIPAllowEventsHeader::ResetAndDestroy (TAny* anArray)
	{
	(reinterpret_cast<RPointerArray<HBufC8>*> (anArray))->ResetAndDestroy();
	}
