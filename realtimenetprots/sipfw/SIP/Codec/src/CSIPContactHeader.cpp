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
// Name          : CSIPContactHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipcontactheader.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "CSIPContactHeaderParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "uricontainer.h"
#include "_sipcodecdefs.h"

_LIT8 (KStar, "*");

// ----------------------------------------------------------------------------
// CSIPContactHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPContactHeader> 
CSIPContactHeader::DecodeL (const TDesC8& aValue)
	{
	RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
	CSIPHeaderBase::PushLC(&baseheaders);
	RPointerArray<CSIPContactHeader> contactHeaders;
	CleanupClosePushL(contactHeaders);
	TInt count = baseheaders.Count();
	for (TInt i=0; i<count; i++)
		{
		CSIPContactHeader* contact = 
			static_cast<CSIPContactHeader*>(baseheaders[i]);
		User::LeaveIfError(contactHeaders.Append(contact));
		}
	CleanupStack::Pop(2); // contactHeaders, baseheaders
	baseheaders.Close();
	return contactHeaders;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContactHeader* CSIPContactHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPContactHeader* self = CSIPContactHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContactHeader* CSIPContactHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPContactHeader* self = new(ELeave)CSIPContactHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aSIPAddress);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::CSIPContactHeader
// ----------------------------------------------------------------------------
//
CSIPContactHeader::CSIPContactHeader(TBool aIsStar)
 : CSIPParameterHeaderBase(';'),
   iIsStar (aIsStar)
	{
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPContactHeader::ConstructL ()
	{
	iParams = new(ELeave)CSIPContactHeaderParams;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPContactHeader::ConstructL (CSIPAddress* aSIPAddress)
	{
	ConstructL ();
	SetSIPAddressL (aSIPAddress);
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPContactHeader::ConstructL (const CSIPContactHeader& aContactHeader)
	{
	if (aContactHeader.Star())
		{
		iIsStar = ETrue;
		ConstructL ();
		}
	else
		{
		iParams = CSIPContactHeaderParams::NewL (*(aContactHeader.iParams));
		iSIPAddress = CSIPAddress::NewL (*(aContactHeader.iSIPAddress));
		}	
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::~CSIPContactHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPContactHeader::~CSIPContactHeader()
	{
	delete iParams;
	delete iSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContactHeader::CloneL () const
	{
	CSIPContactHeader* clone = new(ELeave)CSIPContactHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::operator==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool 
CSIPContactHeader::operator==(const CSIPContactHeader& aSIPContactHeader) const
	{
	if (aSIPContactHeader.Star() != iIsStar) 
        {
        return EFalse;
        }
	if (iIsStar) 
        {
        return ETrue;
        }
	if (!(*iSIPAddress == *(aSIPContactHeader.SIPAddress())))
        {
        return EFalse;
        }
	return (*iParams == *(aSIPContactHeader.iParams));
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::ExpiresParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPContactHeader::ExpiresParameter () const
	{
	if (iIsStar)
		{
		return 0;
		}
	RStringF expires = SIPStrings::StringF(SipStrConsts::EExpires);
	return iParams->IntParamValue(expires);
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::SetExpiresParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPContactHeader::SetExpiresParameterL (TInt aExpiresParam)
	{
	__ASSERT_ALWAYS (!iIsStar, User::Leave(KErrSipCodecContactSetToStar));					
	RStringF expires = SIPStrings::StringF(SipStrConsts::EExpires);
	iParams->SetParamL(expires,aExpiresParam);
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::QParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPContactHeader::QParameter () const
	{
	if (iIsStar) 
		{
		return 0;
		}
	return iParams->RealParamValue(SIPStrings::StringF(SipStrConsts::EQ));
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::SetQParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPContactHeader::SetQParameterL (TReal aQValue)
	{
	__ASSERT_ALWAYS (!iIsStar, 
				User::Leave(KErrSipCodecContactSetToStar));
	iParams->SetParamL(SIPStrings::StringF(SipStrConsts::EQ),aQValue);
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::Star
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPContactHeader::Star () const
	{
	return iIsStar;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::SIPAddress
// ----------------------------------------------------------------------------
//
EXPORT_C const CSIPAddress* CSIPContactHeader::SIPAddress() const
	{
	return iSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::SIPAddress
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPContactHeader::SIPAddress()
	{
	return iSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::SetSIPAddressL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPContactHeader::SetSIPAddressL (CSIPAddress* aSIPAddress)
	{
	__ASSERT_ALWAYS (!iIsStar, 
				User::Leave(KErrSipCodecContactSetToStar));
	__ASSERT_ALWAYS (aSIPAddress != 0, 
				User::Leave(KErrSipCodecContactAddress));

	delete iSIPAddress;
	iSIPAddress = aSIPAddress;

    if(iSIPAddress->URI().IsSIPURI())
        {
    	CSIPURI& sipURI = *(iSIPAddress->URI().SIPURI());
    	sipURI.DeleteParam(SIPStrings::StringF(SipStrConsts::EMethod));
        }
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPContactHeader::MoreThanOneAllowed () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPContactHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EContactHeader);
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::HasCompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPContactHeader::HasCompactName () const
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::CompactName
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
RStringF CSIPContactHeader::CompactName () const
	{
	return SIPStrings::StringF(SipStrConsts::EContactHeaderCompact);
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPContactHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPContactHeader* self = new(ELeave)CSIPContactHeader(EFalse);
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPContactHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	if (iIsStar)
		{
		aWriteStream.WriteUint8L(1);
		}
	else
		{
		aWriteStream.WriteUint8L(0);
		iSIPAddress->ExternalizeL(aWriteStream);
		iParams->ExternalizeL (aWriteStream);
		}
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPContactHeader::DoInternalizeValueL (RReadStream& aReadStream)
	{
	if (aReadStream.ReadUint8L() == 1) // is STAR
		{
		iIsStar = ETrue;
		iParams = new(ELeave)CSIPContactHeaderParams;
		}
	else
		{
		iIsStar = EFalse;
	    CSIPAddress* tmpAddr = CSIPAddress::InternalizeL (aReadStream);
	    CleanupStack::PushL(tmpAddr);
	    SetSIPAddressL(tmpAddr);
	    CleanupStack::Pop(tmpAddr);
		iParams = CSIPContactHeaderParams::InternalizeL (aReadStream);
		}
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPContactHeader::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleTop;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPContactHeader::BaseDecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0,
		User::Leave(KErrSipCodecContactHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',', ETrue);
	TBool star=EFalse;
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPContactHeader* contactHeader = new(ELeave)CSIPContactHeader;
		CleanupStack::PushL(contactHeader);
		contactHeader->ConstructL();
		contactHeader->ParseL (tokenizer->Tokens()[i], ETrue, ETrue);
		if (!star) star = contactHeader->iIsStar;
		if (star && i > 0) 
			{
			User::Leave(KErrSipCodecContactSetToStar); 
			}
		headers.AppendL(contactHeader);
		CleanupStack::Pop(contactHeader);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeader::ParamInsertionAllowed
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPContactHeader::ParamInsertionAllowed () const
    {
    return !(iIsStar);
    }  

// ----------------------------------------------------------------------------
// CSIPContactHeader::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPContactHeader::ToTextMandatoryPartLC () const
    {
	if (iIsStar)
        {
        return KStar().AllocLC();
        }
    return iSIPAddress->ToTextLC(ETrue);
    }

// ----------------------------------------------------------------------------
// CSIPContactHeader::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPContactHeader::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
    {
	TLex8 lex(aMandatoryPart);
	lex.SkipSpace();
	if (lex.Peek() == '*')
		{
        lex.Inc();
        lex.SkipSpace();
        if (lex.Remainder().Length() > 0)
            {
            User::Leave(KErrSipCodecContactHeader);
            }
		iIsStar = ETrue;
		}
    else
        {
	    CSIPAddress* sipAddress = CSIPAddress::DecodeL(lex.Remainder());
	    CleanupStack::PushL(sipAddress);
	    SetSIPAddressL(sipAddress);
	    CleanupStack::Pop(sipAddress);
        }
    }
    
// ----------------------------------------------------------------------------
// CSIPContactHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPContactHeader::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPContactHeader::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPContactHeader::Params ()
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPContactHeader::RemoveExpiresParam
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPContactHeader::RemoveExpiresParam()
	{
	RStringF expires = SIPStrings::StringF(SipStrConsts::EExpires);
	iParams->DeleteParam(expires);
	}
