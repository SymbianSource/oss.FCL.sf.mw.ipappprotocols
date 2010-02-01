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
// Name          : CSIPFromToHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipfromtoheaderbase.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "siphostport.h"
#include "CSIPFromToHeaderParams.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecerr.h"
#include "uricontainer.h"
#include "_sipcodecdefs.h"

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::CSIPFromToHeaderBase
// -----------------------------------------------------------------------------
//
CSIPFromToHeaderBase::CSIPFromToHeaderBase()
 : CSIPParameterHeaderBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderBase::ConstructL()
	{
	iParams = new(ELeave)CSIPFromToHeaderParams;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderBase::ConstructL (CSIPAddress* aSIPAddress)
	{
	ConstructL ();
	SetSIPAddressL (aSIPAddress);
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderBase::ConstructL (const CSIPFromToHeaderBase& aHeader)
	{
	iParams = CSIPFromToHeaderParams::NewL (*(aHeader.iParams));
	if (aHeader.iSIPAddress != 0)
		{
		iSIPAddress = CSIPAddress::NewL (*(aHeader.iSIPAddress));
		}
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::~CSIPFromToHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromToHeaderBase::~CSIPFromToHeaderBase()
	{
	delete iParams;
	delete iSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::SetSIPAddressL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPFromToHeaderBase::SetSIPAddressL (CSIPAddress* aSIPAddress)
	{
	__ASSERT_ALWAYS (aSIPAddress != 0,
                     User::Leave(KErrSipCodecFromOrToAddress));

	delete iSIPAddress;
	iSIPAddress = aSIPAddress;

    if(iSIPAddress->URI().IsSIPURI())
        {
	    CSIPURI& sipURI = *(iSIPAddress->URI().SIPURI());
	    sipURI.HostPort().DeletePort();
	    sipURI.DeleteParam(SIPStrings::StringF(SipStrConsts::EMethod));
	    sipURI.DeleteParam(SIPStrings::StringF(SipStrConsts::EMaddr));
	    sipURI.DeleteParam(SIPStrings::StringF(SipStrConsts::ETtl));
	    sipURI.DeleteParam(SIPStrings::StringF(SipStrConsts::ETransport));
	    sipURI.DeleteParam(SIPStrings::StringF(SipStrConsts::ELr));
	    sipURI.DeleteAllURIHeaders();
        }
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::SIPAddress
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPAddress& CSIPFromToHeaderBase::SIPAddress() const
	{
	return *iSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::SIPAddress
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress& CSIPFromToHeaderBase::SIPAddress()
	{
	return *iSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool 
CSIPFromToHeaderBase::operator==(const CSIPFromToHeaderBase& aHeader) const
	{
	if (!(iSIPAddress->URI() == aHeader.SIPAddress().URI()))
		{
		return EFalse;
		}
	return (*iParams == *(aHeader.iParams));
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::HasCompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPFromToHeaderBase::HasCompactName () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderBase::DoInternalizeValueL (RReadStream& aReadStream)
    {
	CSIPAddress* tmpAddr = CSIPAddress::InternalizeL (aReadStream);
	CleanupStack::PushL(tmpAddr);
	SetSIPAddressL(tmpAddr);
	CleanupStack::Pop(tmpAddr);
	iParams = CSIPFromToHeaderParams::InternalizeL (aReadStream);
	}	

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderBase::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	iSIPAddress->ExternalizeL(aWriteStream);
	iParams->ExternalizeL (aWriteStream);
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPFromToHeaderBase::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleTop;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPFromToHeaderBase::ToTextMandatoryPartLC () const
    {
    return iSIPAddress->ToTextLC(ETrue);
    }

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderBase::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
    {
	CSIPAddress* sipAddress = CSIPAddress::DecodeL(aMandatoryPart);
	CleanupStack::PushL(sipAddress);
	SetSIPAddressL(sipAddress);
	CleanupStack::Pop(sipAddress);
    }
    
// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPFromToHeaderBase::Params () const
    {
    return *iParams; 
    }

// -----------------------------------------------------------------------------
// CSIPFromToHeaderBase::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPFromToHeaderBase::Params ()
    {
    return *iParams; 
    }

