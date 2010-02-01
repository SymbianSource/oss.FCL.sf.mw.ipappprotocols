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
// Name          : CSIPAddressHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipaddressheaderbase.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "CSIPHeaderGenericParams.h"
#include "CSIPTokenizer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecerr.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::CSIPAddressHeaderBase
// ----------------------------------------------------------------------------
//
CSIPAddressHeaderBase::CSIPAddressHeaderBase()
 : CSIPParameterHeaderBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddressHeaderBase::ConstructL()
	{
	iParams = new(ELeave)CSIPHeaderGenericParams;
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddressHeaderBase::ConstructL(CSIPAddress* aSIPAddress)
	{
	ConstructL();
	SetSIPAddressL(aSIPAddress);
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddressHeaderBase::ConstructL(const CSIPAddressHeaderBase& aHeader)
	{
	iParams = CSIPHeaderGenericParams::NewL(*(aHeader.iParams));
	iSIPAddress = CSIPAddress::NewL(*(aHeader.iSIPAddress));	
	}	

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::~CSIPAddressHeaderBase
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddressHeaderBase::~CSIPAddressHeaderBase()
	{
	delete iParams;
	delete iSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::SetSIPAddressL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAddressHeaderBase::SetSIPAddressL(CSIPAddress* aSIPAddress)
	{
	__ASSERT_ALWAYS(aSIPAddress != 0, User::Leave(KErrSipCodecReplyToHeader));
	delete iSIPAddress;
	iSIPAddress = aSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::SIPAddress
// ----------------------------------------------------------------------------
//
EXPORT_C const CSIPAddress& CSIPAddressHeaderBase::SIPAddress() const
	{
	return *iSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::SIPAddress
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress& CSIPAddressHeaderBase::SIPAddress()
	{
	return *iSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAddressHeaderBase::DoInternalizeValueL(RReadStream& aReadStream)
	{
	iSIPAddress = CSIPAddress::InternalizeL(aReadStream);
	iParams = CSIPHeaderGenericParams::InternalizeL(aReadStream);
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::ExternalizeValueL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAddressHeaderBase::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
	iSIPAddress->ExternalizeL(aWriteStream);
	iParams->ExternalizeL(aWriteStream);
	}

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
HBufC8* CSIPAddressHeaderBase::ToTextMandatoryPartLC() const
    {
    return iSIPAddress->ToTextLC(ETrue);
    }

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
void CSIPAddressHeaderBase::ParseMandatoryPartL(const TDesC8& aMandatoryPart)
    {
	CSIPAddress* sipAddress = CSIPAddress::DecodeL(aMandatoryPart);
	CleanupStack::PushL(sipAddress);
	SetSIPAddressL(sipAddress);
	CleanupStack::Pop(sipAddress);
    }   
    
// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAddressHeaderBase::Params() const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAddressHeaderBase::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAddressHeaderBase::Params()
    {
    return *iParams; 
    }

