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
// Name          : CSIPAuthenticationInfoHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPAuthenticationInfoHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeaderParams::NewL
// -----------------------------------------------------------------------------
//
CSIPAuthenticationInfoHeaderParams* CSIPAuthenticationInfoHeaderParams::NewL(
    const CSIPAuthenticationInfoHeaderParams& aParams)
	{
	CSIPAuthenticationInfoHeaderParams* self = 
        CSIPAuthenticationInfoHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeaderParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPAuthenticationInfoHeaderParams* CSIPAuthenticationInfoHeaderParams::NewLC(
    const CSIPAuthenticationInfoHeaderParams& aParams)
	{
	CSIPAuthenticationInfoHeaderParams* self = 
        new(ELeave)CSIPAuthenticationInfoHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeaderParams::CSIPAuthenticationInfoHeaderParams
// -----------------------------------------------------------------------------
//
CSIPAuthenticationInfoHeaderParams::CSIPAuthenticationInfoHeaderParams ()
 : CSIPParamContainerBase(',')
	{
	}

// -----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeaderParams::~CSIPAuthenticationInfoHeaderParams
// -----------------------------------------------------------------------------
//
CSIPAuthenticationInfoHeaderParams::~CSIPAuthenticationInfoHeaderParams ()
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeaderParams::AddQuotesWhenEncoding
// ----------------------------------------------------------------------------
//
TBool CSIPAuthenticationInfoHeaderParams::AddQuotesWhenEncoding(
    RStringF aParamName) const
    {
	if (aParamName == SIPStrings::StringF(SipStrConsts::ENextNonce) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::ECNonce) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::EResponseAuth))
		{
		return ETrue;
		}
	return EFalse;
    }

// ----------------------------------------------------------------------------
// CSIPAuthenticationInfoHeaderParams::CheckAndUpdateParamL
// ----------------------------------------------------------------------------
//	
void CSIPAuthenticationInfoHeaderParams::CheckAndUpdateParamL(
    RStringF aName,
    TBool /*aHasValue*/,
    TPtrC8& aValue) const
    {
	// nextnonce, cnonce
	if (aName == SIPStrings::StringF(SipStrConsts::ENextNonce) ||
	    aName == SIPStrings::StringF(SipStrConsts::ECNonce))
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::QuotedStringValue(aValue))
	        {
			User::Leave(KErrSipCodecAuthenticationInfoHeader);
	        }
		return;
		}
	// message-qop
	if (aName == SIPStrings::StringF(SipStrConsts::EQop))
		{
		RemoveQuotes(aValue);
        if (!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecAuthenticationInfoHeader);
			}
		return;
		}
	// response-auth
	if (aName == SIPStrings::StringF(SipStrConsts::EResponseAuth))
		{
		RemoveQuotes(aValue);
        if (!SIPSyntaxCheck::HexValue(aValue))
			{
			User::Leave(KErrSipCodecAuthenticationInfoHeader);
			}
		return;
		}
	// nonce-count
	if (aName == SIPStrings::StringF(SipStrConsts::ENonceCount))
		{
		const TInt KNonceCountLength = 8;
		if (!SIPSyntaxCheck::HexValue(aValue,KNonceCountLength))
			{
			User::Leave(KErrSipCodecAuthenticationInfoHeader);
			}
		return;
		}
    // The above are the only allowed parameters. No extensions allowed.
    User::Leave(KErrSipCodecAuthenticationInfoHeader);
	}
