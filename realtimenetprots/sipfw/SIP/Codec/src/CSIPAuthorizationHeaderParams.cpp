// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPAuthorizationHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/6.0 
//



#include <uri8.h>
#include "CSIPAuthorizationHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipcodecutils.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// Yahoo uses this invalid character in its authentication:
const TInt KExtraYahooChr = 2; 

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderParams* CSIPAuthorizationHeaderParams::NewL(
    const CSIPAuthorizationHeaderParams& aParams)
	{
	CSIPAuthorizationHeaderParams* self = 
        CSIPAuthorizationHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderParams* CSIPAuthorizationHeaderParams::NewLC(
    const CSIPAuthorizationHeaderParams& aParams)
	{
	CSIPAuthorizationHeaderParams* self = 
        new(ELeave)CSIPAuthorizationHeaderParams;
	CleanupStack::PushL(self);
	self->CSIPParamContainerBase::ConstructL(aParams);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::CSIPAuthorizationHeaderParams
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderParams::CSIPAuthorizationHeaderParams ()
 : CSIPParamContainerBase(',')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::~CSIPAuthorizationHeaderParams
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderParams::~CSIPAuthorizationHeaderParams ()
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderParams* CSIPAuthorizationHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPAuthorizationHeaderParams* self = 
	    new(ELeave)CSIPAuthorizationHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::AddQuotesWhenEncoding
// ----------------------------------------------------------------------------
//	
TBool CSIPAuthorizationHeaderParams::AddQuotesWhenEncoding(
    RStringF aParamName) const
    {
	if (aParamName == SIPStrings::StringF(SipStrConsts::EUserName) ||
		aParamName == SIPStrings::StringF(SipStrConsts::ERealm) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::ENonce) ||		
	    aParamName == SIPStrings::StringF(SipStrConsts::ECNonce) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::EOpaque) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::EResponse) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::EUri) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::EAuts))	
		{
		return ETrue;
		}
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderParams::CheckAndUpdateParamL
// ----------------------------------------------------------------------------
//	
void CSIPAuthorizationHeaderParams::CheckAndUpdateParamL(RStringF aName,
                                                         TBool aHasValue,
                                                         TPtrC8& aValue) const
    {
	// username, realm, nonce, cnonce, opaque
	if (aName == SIPStrings::StringF(SipStrConsts::EUserName) ||
		aName == SIPStrings::StringF(SipStrConsts::ERealm) ||
	    aName == SIPStrings::StringF(SipStrConsts::EOpaque))	
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::QuotedStringValue(aValue))
	        {
			User::Leave(KErrSipCodecAuthorizationHeader);
	        }
		return;
		}
	// nonce, cnonce
	if (aName == SIPStrings::StringF(SipStrConsts::ENonce) ||		
	    aName == SIPStrings::StringF(SipStrConsts::ECNonce))	
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::QuotedStringValue(aValue,KExtraYahooChr))
	        {
			User::Leave(KErrSipCodecAuthorizationHeader);
	        }
		return;
		}	
	//algorithm, message-qop
	if (aName == SIPStrings::StringF(SipStrConsts::EAlgorithm) ||
		aName == SIPStrings::StringF(SipStrConsts::EQop))
		{
		RemoveQuotes(aValue);
        if (!SIPSyntaxCheck::Token(aValue))
			{
			User::Leave(KErrSipCodecAuthorizationHeader);
			}
		return;
		}
	// nonce-count
	if (aName == SIPStrings::StringF(SipStrConsts::ENonceCount))
		{
		const TInt KNonceCountLength = 8;
		if (!SIPSyntaxCheck::HexValue(aValue,KNonceCountLength))
			{
			User::Leave(KErrSipCodecAuthorizationHeader);
			}
		return;
		}
	// dresponse
	if (aName == SIPStrings::StringF(SipStrConsts::EResponse))
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::HexValue(aValue))
	        {
			User::Leave(KErrSipCodecAuthorizationHeader);
			}
		return;
		}
	// uri
	if (aName == SIPStrings::StringF(SipStrConsts::EUri))
		{
		RemoveQuotes(aValue);
		TUriParser8 parser;
		TInt error = 0;
		error = parser.Parse(aValue);
		if ( error == KErrNoMemory )
			{
			User::Leave(KErrNoMemory);
			}
        if ( error != KErrNone)
			{
			User::Leave(KErrSipCodecAuthorizationHeader);
			}
		return;
		}
	// auts
	if (aName == SIPStrings::StringF(SipStrConsts::EAuts))
		{
		RemoveQuotes(aValue);
        if (!SIPSyntaxCheck::Base64Encoded(aValue))
			{
			User::Leave(KErrSipCodecAuthorizationHeader);
			}
		return;
		}
	// extension
	if (!SIPSyntaxCheck::Token(aName.DesC()))
		{
		User::Leave(KErrSipCodecAuthorizationHeader);
		}
    if (aHasValue &&
        !SIPSyntaxCheck::Token(aValue) &&
		!SIPSyntaxCheck::QuotedString(aValue))
		{
		User::Leave(KErrSipCodecAuthorizationHeader);
		}
	}
