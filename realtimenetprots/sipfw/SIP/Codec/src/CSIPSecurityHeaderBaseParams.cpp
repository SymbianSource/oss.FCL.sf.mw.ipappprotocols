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
// Name          : CSIPSecurityHeaderBaseParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "CSIPSecurityHeaderBaseParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::NewL
// -----------------------------------------------------------------------------
//
CSIPSecurityHeaderBaseParams* CSIPSecurityHeaderBaseParams::NewL(
    const CSIPSecurityHeaderBaseParams& aParams)
	{
	CSIPSecurityHeaderBaseParams* self = 
        CSIPSecurityHeaderBaseParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPSecurityHeaderBaseParams* CSIPSecurityHeaderBaseParams::NewLC(
    const CSIPSecurityHeaderBaseParams& aParams)
	{
	CSIPSecurityHeaderBaseParams* self = 
        new(ELeave)CSIPSecurityHeaderBaseParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::CSIPSecurityHeaderBaseParams
// -----------------------------------------------------------------------------
//
CSIPSecurityHeaderBaseParams::CSIPSecurityHeaderBaseParams()
 : CSIPParamContainerBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::~CSIPSecurityHeaderBaseParams
// -----------------------------------------------------------------------------
//
CSIPSecurityHeaderBaseParams::~CSIPSecurityHeaderBaseParams()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPSecurityHeaderBaseParams* CSIPSecurityHeaderBaseParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPSecurityHeaderBaseParams* self = 
        new(ELeave)CSIPSecurityHeaderBaseParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::AddQuotesWhenEncoding
// ----------------------------------------------------------------------------
//
TBool CSIPSecurityHeaderBaseParams::AddQuotesWhenEncoding(
    RStringF aParamName) const
    {
    return (aParamName == SIPStrings::StringF(SipStrConsts::EDigestVerify));
    }

// ----------------------------------------------------------------------------
// CSIPSecurityHeaderBaseParams::CheckAndUpdateParamL
// ----------------------------------------------------------------------------
//	
void CSIPSecurityHeaderBaseParams::CheckAndUpdateParamL(RStringF aName,
                                                        TBool aHasValue,
                                                        TPtrC8& aValue) const
    {
    // q
    if(aName == SIPStrings::StringF(SipStrConsts::EQ))
        {
        if(!SIPSyntaxCheck::QValue(aValue))
            {
            User::Leave(KErrSipCodecSecurityHeader);
            }
        return;
        }
    // d-alg
    if(aName == SIPStrings::StringF(SipStrConsts::EDigestAlgorithm))
		{
		if(!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecSecurityHeader);
			}
		return;
		}
    // d-qop
	if (aName == SIPStrings::StringF(SipStrConsts::EDigestQop))
		{
		if (!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecSecurityHeader);
			}
		return;
		}
    //d-ver
	if (aName == SIPStrings::StringF(SipStrConsts::EDigestVerify))
		{
		RemoveQuotes(aValue);
        if (!SIPSyntaxCheck::HexValue(aValue))
			{
			User::Leave(KErrSipCodecSecurityHeader);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aName, aHasValue, aValue, KErrSipCodecSecurityHeader);
	}
