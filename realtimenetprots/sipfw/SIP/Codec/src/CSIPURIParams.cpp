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
// Name          : CSIPURIParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPURIParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "TSIPChar.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// -----------------------------------------------------------------------------
// CSIPURIParams::DecodeL
// -----------------------------------------------------------------------------
//
CSIPURIParams* CSIPURIParams::DecodeL(
    const TDesC8& aValue,
    TBool aSIPSURI)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecURIParams));

	CSIPURIParams* params = new(ELeave)CSIPURIParams(aSIPSURI);
	CleanupStack::PushL (params);
	params->ParseL(aValue);
	CleanupStack::Pop(params);
	return params;
	}

// -----------------------------------------------------------------------------
// CSIPURIParams::NewL
// -----------------------------------------------------------------------------
//
CSIPURIParams* CSIPURIParams::NewL(
    const CSIPURIParams& aParams,
    TBool aSIPSURI)
	{
	CSIPURIParams* self = CSIPURIParams::NewLC(aParams,aSIPSURI);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPURIParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPURIParams* CSIPURIParams::NewLC(
    const CSIPURIParams& aParams,
    TBool aSIPSURI)
	{
	CSIPURIParams* self = new(ELeave)CSIPURIParams(aSIPSURI);
	CleanupStack::PushL(self);
	self->ConstructL (aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPURIParams::CSIPURIParams
// -----------------------------------------------------------------------------
//
CSIPURIParams::CSIPURIParams(TBool aSIPSURI)
 : CSIPParamContainerBase(';'),
   iSIPSURI(aSIPSURI)
	{
	}

// -----------------------------------------------------------------------------
// CSIPURIParams::~CSIPURIParams
// -----------------------------------------------------------------------------
//
CSIPURIParams::~CSIPURIParams()
	{
	}

// -----------------------------------------------------------------------------
// CSIPURIParams::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPURIParams::operator==(const CSIPURIParams& aParams) const
	{
	// If a SIPURI has a "user", "ttl", "method" or "maddr" parameter 
	// the other one has to have it too and their values must match. 
	// Any other parameters must match only if they are present in both.
	for (TInt i=0; i < iParams.Count(); i++)
		{
		TInt index = aParams.FindParamIndex(iParams[i]->Name());
		if (index >= 0)
			{
			if (iParams[i]->HasValue() &&
			    !(*(aParams.iParams[index]) == *(iParams[i])))
				{
				return EFalse;
				}
			}
		else
			{
			RStringF name = iParams[i]->Name();
			if (name == SIPStrings::StringF(SipStrConsts::EUser) ||
			    name == SIPStrings::StringF(SipStrConsts::ETtl) ||
			    name == SIPStrings::StringF(SipStrConsts::EMethod) ||
			    name == SIPStrings::StringF(SipStrConsts::EMaddr))
				{
				return EFalse;
				}
			}
		}
	for (TInt j=0; j < aParams.iParams.Count(); j++)
		{
		TInt index = FindParamIndex(aParams.iParams[j]->Name());
		if (index >= 0)
			{
			if (aParams.iParams[j]->HasValue() &&
			    !(*(iParams[index]) == *(aParams.iParams[j])))
				{
				return EFalse;
				}
			}
		else
			{
			RStringF name = aParams.iParams[j]->Name();
			if (name == SIPStrings::StringF(SipStrConsts::EUser) ||
			    name == SIPStrings::StringF(SipStrConsts::ETtl) ||
			    name == SIPStrings::StringF(SipStrConsts::EMethod) ||
			    name == SIPStrings::StringF(SipStrConsts::EMaddr))
				{
				return EFalse;
				}
			}
		}
	return ETrue;
	}
	
// ----------------------------------------------------------------------------
// CSIPURIParams::CheckAndUpdateParamL
// ----------------------------------------------------------------------------
//	
void CSIPURIParams::CheckAndUpdateParamL(RStringF aName,
                                         TBool aHasValue,
                                         TPtrC8& aValue) const
    {
	// transport
	if (aName == SIPStrings::StringF(SipStrConsts::ETransport))
		{
		if (!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecURIParams);
			}
	    if (iSIPSURI && 
	        aValue.CompareF(SIPStrings::StringF(SipStrConsts::EUdp).DesC())==0)
	        {
	        User::Leave(KErrSipCodecURIParams);
	        }
		return;
		}
	// user
	if (aName == SIPStrings::StringF(SipStrConsts::EUser))
		{
		if (!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecURIParams);
			}
		return;
		}
	// method
	if (aName == SIPStrings::StringF(SipStrConsts::EMethod))
		{
		if (!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecURIParams);
			}
		return;
		}
	// ttl
	if (aName == SIPStrings::StringF(SipStrConsts::ETtl))
		{
		if (!SIPSyntaxCheck::TtlValue(aValue))
			{
			User::Leave(KErrSipCodecURIParams);
			}
		return;
		}
	// maddr
	if (aName == SIPStrings::StringF(SipStrConsts::EMaddr))
		{
		if (!SIPSyntaxCheck::Host(aValue))
			{
			User::Leave(KErrSipCodecURIParams);
			}
		return;
		}
	// Relaxed cheking for lr-parameter. If it has a value, it is ignored.
	// other URI-parameter
	if (!CheckOtherParam(aName,aHasValue,aValue)) 
        {
        User::Leave(KErrSipCodecURIParams);
        }
	}

// -----------------------------------------------------------------------------
// CSIPURIParams::CheckOtherParam
// -----------------------------------------------------------------------------
//
TBool CSIPURIParams::CheckOtherParam (RStringF aName,
                                      TBool aHasValue,
	                                  const TDesC8& aValue) const
	{
	if (!CheckSIPURIParamToken(aName.DesC())) 
		{
		return EFalse;
		}
	if (aHasValue)
		{
		return CheckSIPURIParamToken(aValue);
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPURIParams::CheckSIPURIParamToken
// -----------------------------------------------------------------------------
//
TBool CSIPURIParams::CheckSIPURIParamToken (const TDesC8& aParamToken) const
	{
	if (aParamToken.Length() == 0) 
        {
        return EFalse;
        }

	TLex8 lex(aParamToken);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		// unreserved / escaped / param-unreserved
		if (!(sipChr.IsUnreserved() || sipChr == '%' ||
			  sipChr == '[' || sipChr == ']' || sipChr == '/' || 
			  sipChr == ':' || sipChr == '&' || sipChr == '+' || 
			  sipChr == '$'))
			{			
			return EFalse;
			}
		if (sipChr == '%') // escaped: "%" HEXDIG HEXDIG
			{
			if (!lex.Get().IsHexDigit()) 
                {
                return EFalse;
                }
            if (!lex.Get().IsHexDigit()) 
                {
                return EFalse;
                }
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}
