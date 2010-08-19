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
// Name          : CSIPAcceptEncodingHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipacceptencodingheader.h"
#include "CSIPAcceptHeaderParams.h"
#include "SIPSyntaxCheck.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"
#include <featdiscovery.h>


// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPAcceptEncodingHeader> 
CSIPAcceptEncodingHeader::DecodeL(const TDesC8& aValue)
	{
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPAcceptEncodingHeader> acceptHeaders;
    CleanupClosePushL(acceptHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPAcceptEncodingHeader* header = 
            static_cast<CSIPAcceptEncodingHeader*>(baseheaders[i]);
		User::LeaveIfError (acceptHeaders.Append(header));
		}
    CleanupStack::Pop(); // acceptHeaders
    CleanupStack::Pop(); // baseheaders
    baseheaders.Close();
    return acceptHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptEncodingHeader* CSIPAcceptEncodingHeader::NewL(
    const TDesC8& aCodings)
	{
	CSIPAcceptEncodingHeader* self = 
		CSIPAcceptEncodingHeader::NewLC(aCodings);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptEncodingHeader* CSIPAcceptEncodingHeader::NewLC(
    const TDesC8& aCodings)
	{
	CSIPAcceptEncodingHeader* self = new(ELeave)CSIPAcceptEncodingHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aCodings);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::CSIPAcceptEncodingHeader
// ----------------------------------------------------------------------------
//
CSIPAcceptEncodingHeader::CSIPAcceptEncodingHeader()
:   CSIPParameterHeaderBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptEncodingHeader::ConstructL()
	{
	iParams = new(ELeave)CSIPAcceptHeaderParams;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptEncodingHeader::ConstructL(const TDesC8& aCodings)
	{
	SetCodingsL(aCodings);
	ConstructL();
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAcceptEncodingHeader::ConstructL(
    const CSIPAcceptEncodingHeader& aAcceptEncodingHeader)
	{
	SetCodingsL(aAcceptEncodingHeader.Codings());
	iParams = 
		CSIPAcceptHeaderParams::NewL(*(aAcceptEncodingHeader.iParams));
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::~CSIPAcceptEncodingHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAcceptEncodingHeader::~CSIPAcceptEncodingHeader()
	{
	delete iParams;
	delete iCodings;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptEncodingHeader::CloneL() const
	{
    CSIPAcceptEncodingHeader* clone = new(ELeave)CSIPAcceptEncodingHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::Codings
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAcceptEncodingHeader::Codings () const
	{
	return *iCodings;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::SetCodingsL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAcceptEncodingHeader::SetCodingsL(const TDesC8& aCodings)
	{
	HBufC8* tmp = aCodings.AllocLC();
	tmp->Des().Trim();
	//Allowing '/' token for interoperability issues
	RArray<TSIPChar> array;
	if(CFeatureDiscovery::IsFeatureSupportedL(TUid::Uid(KFeatureIdFfSipNonstandardInteropfix)))
	    {
        array.Append('/');
	    }
	
	if (tmp->Length() > 0 && !SIPSyntaxCheck::Token(*tmp,&array))
        {
        array.Close();
        User::Leave(KErrSipCodecAcceptEncodingHeader);
	    }
	array.Close();
	CleanupStack::Pop(tmp);
	delete iCodings;
	iCodings = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::QParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPAcceptEncodingHeader::QParameter() const
	{
	return iParams->RealParamValue(SIPStrings::StringF(SipStrConsts::EQ));
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::SetQParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAcceptEncodingHeader::SetQParameterL(TReal aQValue)
	{
	iParams->SetParamL(SIPStrings::StringF(SipStrConsts::EQ), aQValue);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAcceptEncodingHeader::Params() const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAcceptEncodingHeader::Params()
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPAcceptEncodingHeader::MoreThanOneAllowed() const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAcceptEncodingHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EAcceptEncodingHeader);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPAcceptEncodingHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPAcceptEncodingHeader* self = new(ELeave)CSIPAcceptEncodingHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAcceptEncodingHeader::ExternalizeValueL(
    RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iCodings->Length());
	aWriteStream.WriteL (*iCodings);
	iParams->ExternalizeL (aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAcceptEncodingHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    iCodings = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	iParams = CSIPAcceptHeaderParams::InternalizeL (aReadStream);
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::ToTextMandatoryPartLC
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPAcceptEncodingHeader::ToTextMandatoryPartLC() const
	{
	return iCodings->AllocLC();
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPAcceptEncodingHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EBottom;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptEncodingHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPAcceptEncodingHeader::BaseDecodeL(
    const TDesC8& aValue)
    {
	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPAcceptEncodingHeader* header = 
			new(ELeave)CSIPAcceptEncodingHeader;
		CleanupStack::PushL(header);
		header->ConstructL();
		header->ParseL(tokenizer->Tokens()[i]);
		if (header->Codings().Length() == 0 &&
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
// CSIPAcceptEncodingHeader::ParseMandatoryPartL
// ----------------------------------------------------------------------------
//
void CSIPAcceptEncodingHeader::ParseMandatoryPartL(
    const TDesC8& aMandatoryPart)
	{
	SetCodingsL(aMandatoryPart);
	}
