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
// Name          : CSIPRouteHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "siprouteheaderbase.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "siphostport.h"
#include "CSIPHeaderGenericParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "uricontainer.h"
#include "_sipcodecdefs.h"

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::CSIPRouteHeaderBase
// -----------------------------------------------------------------------------
//
CSIPRouteHeaderBase::CSIPRouteHeaderBase()
 : CSIPParameterHeaderBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRouteHeaderBase::ConstructL ()
	{
	iParams = new(ELeave)CSIPHeaderGenericParams;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRouteHeaderBase::ConstructL (CSIPAddress* aSIPAddress)
	{
	ConstructL ();
	SetSIPAddressL (aSIPAddress);
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRouteHeaderBase::ConstructL (const CSIPRouteHeaderBase& aRouteHeaderBase)
	{
	iParams = CSIPHeaderGenericParams::NewL (*(aRouteHeaderBase.iParams));
	if (aRouteHeaderBase.iSIPAddress != 0)
		{	
		iSIPAddress = CSIPAddress::NewL (*(aRouteHeaderBase.iSIPAddress));
		}
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::~CSIPRouteHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPRouteHeaderBase::~CSIPRouteHeaderBase()
	{
	delete iParams;
	delete iSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::SetSIPAddressL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPRouteHeaderBase::SetSIPAddressL (CSIPAddress* aSIPAddress)
	{
	__ASSERT_ALWAYS (aSIPAddress != 0, User::Leave(KErrSipCodecRouteHeader));

    delete iSIPAddress;
	iSIPAddress = aSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::SIPAddress
// -----------------------------------------------------------------------------
//
EXPORT_C const CSIPAddress& CSIPRouteHeaderBase::SIPAddress() const
	{
	return *iSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::SIPAddress
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress& CSIPRouteHeaderBase::SIPAddress()
	{
	return *iSIPAddress;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::MoreThanOneAllowed
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPRouteHeaderBase::MoreThanOneAllowed () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPRouteHeaderBase::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::ETop;
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPRouteHeaderBase::ToTextMandatoryPartLC () const
	{
	return iSIPAddress->ToTextLC(ETrue);
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPRouteHeaderBase::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
	{
	__ASSERT_ALWAYS (aMandatoryPart.Length() > 0, 
		             User::Leave(KErrSipCodecRouteHeader));

	TLex8 lex(aMandatoryPart);
	lex.SkipSpace();
	iSIPAddress = CSIPAddress::DecodeL (lex.Remainder());
	}

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPRouteHeaderBase::Params () const
    {
    return *iParams; 
    }

// -----------------------------------------------------------------------------
// CSIPRouteHeaderBase::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPRouteHeaderBase::Params ()
    {
    return *iParams; 
    }
