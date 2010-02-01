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
// Name          : CSIPEventHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipeventheader.h"
#include "CSIPEventHeaderParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

_LIT8(KDot, ".");


// ----------------------------------------------------------------------------
// CSIPEventHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPEventHeader* CSIPEventHeader::DecodeL (const TDesC8& aValue)
	{
	CSIPEventHeader* eventHeader = new(ELeave) CSIPEventHeader;
    CleanupStack::PushL(eventHeader);
    eventHeader->ConstructL();
	eventHeader->ParseL(aValue);
	CleanupStack::Pop(eventHeader);
	return eventHeader;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPEventHeader* CSIPEventHeader::NewL(const TDesC8& aEventPackage)
	{
	CSIPEventHeader* self = CSIPEventHeader::NewLC(aEventPackage);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPEventHeader* CSIPEventHeader::NewLC(const TDesC8& aEventPackage)
	{
	CSIPEventHeader* self=new(ELeave) CSIPEventHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aEventPackage);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::CSIPEventHeader
// ----------------------------------------------------------------------------
//
CSIPEventHeader::CSIPEventHeader()
:   CSIPParameterHeaderBase( ';' )
	{
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::ConstructL()
	{
	iParams = new(ELeave)CSIPEventHeaderParams;
	iEventTemplates = new(ELeave) CDesC8ArraySeg(1);
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::ConstructL (const TDesC8& aEventPackage)
	{
	ConstructL ();
	SetEventPackageL (aEventPackage);
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::ConstructL (const CSIPEventHeader& aSIPEventHeader)
	{
	iParams = CSIPEventHeaderParams::NewL (*(aSIPEventHeader.iParams));
	SetEventTemplatesL(*aSIPEventHeader.iEventTemplates);
	SetEventPackageL(aSIPEventHeader.EventPackage());
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::~CSIPEventHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPEventHeader::~CSIPEventHeader()
	{
	delete iParams;
	delete iEventPackage;
	delete iEventTemplates;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPEventHeader::CloneL () const
	{
	CSIPEventHeader* clone = new(ELeave)CSIPEventHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPEventHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::EEventHeader);
    }

// ----------------------------------------------------------------------------
// CSIPEventHeader::HasCompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPEventHeader::HasCompactName () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::CompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RStringF CSIPEventHeader::CompactName () const
	{
    return SIPStrings::StringF(SipStrConsts::EEventHeaderCompact);
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::operator==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool 
CSIPEventHeader::operator==(const CSIPEventHeader& aSIPEventHeader) const
	{
	if(iEventPackage->Compare(*(aSIPEventHeader.iEventPackage)) != 0) 
		{
		return EFalse;
		}

	if(iEventTemplates->MdcaCount() !=
		aSIPEventHeader.iEventTemplates->MdcaCount()) 
		{
		return EFalse;
		}
	for (TInt i=0; i < iEventTemplates->MdcaCount(); i++)
		{
		if(iEventTemplates->MdcaPoint(i) !=
			aSIPEventHeader.iEventTemplates->MdcaPoint(i)) 
			{
			return EFalse;
			}
		}
	return (*iParams == *(aSIPEventHeader.iParams));
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::SetEventPackageL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPEventHeader::SetEventPackageL (const TDesC8& aEventPackage)
	{
	HBufC8* tmp = aEventPackage.AllocLC();
	tmp->Des().Trim();
	if (!SIPSyntaxCheck::Token(*tmp) || (tmp->Find(KDot) != KErrNotFound))
		{
		User::Leave(KErrSipCodecEventHeader);
		}
	CleanupStack::Pop(tmp);
	delete iEventPackage;
	iEventPackage = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::EventPackage
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPEventHeader::EventPackage() const
	{
	return *iEventPackage;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::SetEventTemplatesL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPEventHeader::SetEventTemplatesL(
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
// CSIPEventHeader::EventTemplates
// ----------------------------------------------------------------------------
//
EXPORT_C const MDesC8Array& CSIPEventHeader::EventTemplates () const
	{
	return *iEventTemplates;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPEventHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPEventHeader* self = new(ELeave)CSIPEventHeader;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::CheckTemplateL
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::CheckTemplateL (const TDesC8& aParam) const
	{
	// event-template Param
	if (!SIPSyntaxCheck::Token(aParam) || aParam.Find(KDot) != KErrNotFound)
		{
		User::Leave(KErrSipCodecEventHeader);
		}
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::DoInternalizeValueL (RReadStream& aReadStream)
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

	delete iParams;
	iParams = NULL;
	iParams = CSIPEventHeaderParams::InternalizeL(aReadStream);
	}	

// ----------------------------------------------------------------------------
// CSIPEventHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iEventPackage->Length());
	if (iEventPackage->Length() > 0)
		{
		aWriteStream.WriteL (EventPackage());
		}

	aWriteStream.WriteUint32L (iEventTemplates->MdcaCount());
	for (TInt i=0; i < iEventTemplates->MdcaCount(); i++)
		{
		aWriteStream.WriteUint32L (iEventTemplates->MdcaPoint(i).Length());
		aWriteStream.WriteL (iEventTemplates->MdcaPoint(i));
		}
	iParams->ExternalizeL (aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPEventHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPEventHeader::BaseDecodeL(
    const TDesC8& aValue)
	{
	CSIPEventHeader* eventHeader = new(ELeave) CSIPEventHeader;
    CleanupStack::PushL(eventHeader);
    eventHeader->ConstructL();
	eventHeader->ParseL(aValue);
	RPointerArray<CSIPHeaderBase> headerArray;
	headerArray.AppendL(eventHeader);
	CleanupStack::Pop(eventHeader);
	return headerArray;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeader::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPEventHeader::ToTextMandatoryPartLC () const
	{
	TUint encodedLength = 0;
	RPointerArray<HBufC8> templatesAsText;
	CleanupStack::PushL (TCleanupItem(ResetAndDestroy,&templatesAsText));

	encodedLength += iEventPackage->Length(); 
	for (TInt i=0; i < iEventTemplates->MdcaCount(); i++)
		{
		HBufC8* templateAsText = (iEventTemplates->MdcaPoint(i)).AllocLC();
		encodedLength += templateAsText->Length() + 1;// param separator length
		templatesAsText.AppendL(templateAsText);
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
	CleanupStack::PushL(encodedHeader);
	return encodedHeader;
	}
	
// ----------------------------------------------------------------------------
// CSIPEventHeader::ParseMandatoryPartL
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
	{
	__ASSERT_ALWAYS (aMandatoryPart.Length() > 0, 
		User::Leave (KErrSipCodecEventHeader));
	HBufC8* tmp = aMandatoryPart.AllocLC();
	tmp->Des().Trim();

	TLex8 lex(*tmp);
	lex.SkipSpace();
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(lex.Remainder(), '.');

	TLex8 lex2(tokenizer->Tokens()[0]);
	SetEventPackageL(lex2.Remainder());
	
	for (TInt j=1; j < tokenizer->Tokens().Count(); j++)
		{
		CheckTemplateL((tokenizer->Tokens()[j]));
		iEventTemplates->AppendL((tokenizer->Tokens()[j]));
		}

	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::PopAndDestroy(tmp);
	}	

// ----------------------------------------------------------------------------
// CSIPEventHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPEventHeader::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPEventHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPEventHeader::Params ()
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPEventHeader::ResetAndDestroy
// ----------------------------------------------------------------------------
//
void CSIPEventHeader::ResetAndDestroy (TAny* anArray)
	{
	(reinterpret_cast<RPointerArray<HBufC8>*> (anArray))->ResetAndDestroy();
	}
