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
// Name          : CSIPSecurityHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipsecurityheaderbase.h"
#include "CSIPSecurityHeaderBaseParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::CSIPSecurityHeaderBase
// -----------------------------------------------------------------------------
//
CSIPSecurityHeaderBase::CSIPSecurityHeaderBase()
:   CSIPParameterHeaderBase( ';' )
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecurityHeaderBase::ConstructL ()
	{
	iParams = new(ELeave)CSIPSecurityHeaderBaseParams;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecurityHeaderBase::ConstructL (const TDesC8& aMechanismName)
	{
	ConstructL ();
	SetMechanismNameL (aMechanismName);
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecurityHeaderBase::ConstructL (
                             const CSIPSecurityHeaderBase& aSecurityHeaderBase)
	{
	iParams = 
        CSIPSecurityHeaderBaseParams::NewL (*(aSecurityHeaderBase.iParams));
	SetMechanismNameL(aSecurityHeaderBase.MechanismName());
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::~CSIPSecurityHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPSecurityHeaderBase::~CSIPSecurityHeaderBase()
	{
	delete iParams;
	delete iMechanismName;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::PreferenceParam
// -----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPSecurityHeaderBase::PreferenceParam () const
	{
	return iParams->RealParamValue(SIPStrings::StringF(SipStrConsts::EQ));
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::SetPreferenceParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPSecurityHeaderBase::SetPreferenceParamL (TReal aPreferenceValue)
	{
	iParams->SetParamL(SIPStrings::StringF(SipStrConsts::EQ), aPreferenceValue);
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::MechanismName
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPSecurityHeaderBase::MechanismName() const
	{
	if (iMechanismName != 0) 
        {
        return *iMechanismName;
        }
	return KNullDesC8;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::SetMechanismNameL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPSecurityHeaderBase::SetMechanismNameL(
    const TDesC8& aMechanismName)
	{
	HBufC8* tmp = aMechanismName.AllocLC();
	tmp->Des().Trim();
	if (!SIPSyntaxCheck::Token(*tmp))
		{
		User::Leave(KErrSipCodecSecurityHeader);
		}
	CleanupStack::Pop(tmp);
	delete iMechanismName;
	iMechanismName = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::MoreThanOneAllowed
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPSecurityHeaderBase::MoreThanOneAllowed () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPSecurityHeaderBase::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::ETop;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPSecurityHeaderBase::DoInternalizeValueL (RReadStream& aReadStream)
	{
    iMechanismName = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	iParams = CSIPSecurityHeaderBaseParams::InternalizeL (aReadStream);
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPSecurityHeaderBase::ExternalizeValueL (
                             RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iMechanismName->Length());
	if (iMechanismName->Length() > 0)
		{
		aWriteStream.WriteL (this->MechanismName());
		}
	iParams->ExternalizeL (aWriteStream);
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::ToTextMandatoryPartLC
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPSecurityHeaderBase::ToTextMandatoryPartLC () const
    {
    return iMechanismName->AllocLC();
    }

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::ParseMandatoryPartL
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPSecurityHeaderBase::ParseMandatoryPartL (const TDesC8& aMandatoryPart)
    {
    SetMechanismNameL(aMandatoryPart);
    }
    
// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPSecurityHeaderBase::Params () const
    {
    return *iParams; 
    }

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBase::Params
// From CSIPParameterHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPSecurityHeaderBase::Params ()
    {
    return *iParams; 
    }
