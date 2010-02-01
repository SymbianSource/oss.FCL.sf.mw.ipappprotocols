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
// Name          : CSIPAuthenticateHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/6.0 
//



#include "CSIPAuthenticateHeaderParams.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SIPSyntaxCheck.h"
#include "uri8.h"
#include "CSIPTokenizer.h"
#include "sipcodecutils.h"

// Yahoo uses this invalid character in its authentication:
const TInt KExtraYahooChr = 2; 

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderParams* CSIPAuthenticateHeaderParams::NewL(
    const CSIPAuthenticateHeaderParams& aParams)
	{
	CSIPAuthenticateHeaderParams* self = 
	    CSIPAuthenticateHeaderParams::NewLC (aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderParams* 
CSIPAuthenticateHeaderParams::NewLC (const CSIPAuthenticateHeaderParams& 
									  aParams)
	{
	CSIPAuthenticateHeaderParams* self = 
        new(ELeave)CSIPAuthenticateHeaderParams;
	CleanupStack::PushL(self);
	self->CSIPParamContainerBase::ConstructL (aParams);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::CSIPAuthenticateHeaderParams
// ----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderParams::CSIPAuthenticateHeaderParams ()
 : CSIPParamContainerBase(',')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::~CSIPAuthenticateHeaderParams
// ----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderParams::~CSIPAuthenticateHeaderParams ()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderParams* CSIPAuthenticateHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPAuthenticateHeaderParams* self = 
	    new(ELeave)CSIPAuthenticateHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::AddQuotesWhenEncoding
// ----------------------------------------------------------------------------
//	
TBool CSIPAuthenticateHeaderParams::AddQuotesWhenEncoding(
    RStringF aParamName) const
    {
	if (aParamName == SIPStrings::StringF(SipStrConsts::ERealm) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::ENonce) ||
	    aParamName == SIPStrings::StringF(SipStrConsts::EOpaque))
	    {
	    return ETrue;
	    }
	return EFalse;
    }
	
// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderParams::CheckAndUpdateParamL
// ----------------------------------------------------------------------------
//	
void CSIPAuthenticateHeaderParams::CheckAndUpdateParamL(RStringF aName,
                                                        TBool aHasValue,
                                                        TPtrC8& aValue) const
    {	
	// realm, nonce, opaque
	if (aName == SIPStrings::StringF(SipStrConsts::ERealm) ||
	    aName == SIPStrings::StringF(SipStrConsts::EOpaque))
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::QuotedStringValue(aValue))
	        {
			User::Leave(KErrSipCodecAuthenticationHeader);
	        }
		return;
		}
	// nonce, cnonce
	if (aName == SIPStrings::StringF(SipStrConsts::ENonce))	
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::QuotedStringValue(aValue,KExtraYahooChr))
	        {
			User::Leave(KErrSipCodecAuthorizationHeader);
	        }
		return;
		}		
	// algorithm, stale
	if (aName == SIPStrings::StringF(SipStrConsts::EAlgorithm) ||
	    aName == SIPStrings::StringF(SipStrConsts::EStale))
		{
		RemoveQuotes(aValue);
	    if (!SIPSyntaxCheck::Token(aValue))
	        {
			User::Leave(KErrSipCodecAuthenticationHeader);
	        }
		return;
		}		
	// qop
	if (aName == SIPStrings::StringF(SipStrConsts::EQop))
		{
        if (!SIPSyntaxCheck::QuotedTokenWithComma(aValue))
			{
			User::Leave(KErrSipCodecAuthenticationHeader);
			}
		return;
		}
	// domain
	if (aName == SIPStrings::StringF(SipStrConsts::EDomain))
		{
        CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ' ');
        for(TInt i=0; i<tokenizer->Tokens().Count(); i++)
            {
		    TUriParser8 parser;
            TPtrC8 valueWithoutQuotes;
            TInt error = 0;
            if(SIPCodecUtils::ValueWithoutQuotes
                                (tokenizer->Tokens()[i], valueWithoutQuotes) 
                                != KErrNone)
                {
                User::Leave(KErrSipCodecAuthenticationHeader);
                }
            error = parser.Parse(valueWithoutQuotes);
            if ( error == KErrNoMemory )
			    {
			    User::Leave(KErrNoMemory);
			    }
            if ( error != KErrNone)
			    {
			    User::Leave(KErrSipCodecAuthenticationHeader);
			    }
            }
        CleanupStack::PopAndDestroy(tokenizer);
		return;
		}
	// other
	if (!SIPSyntaxCheck::Token(aName.DesC()))
		{
		User::Leave(KErrSipCodecAuthenticationHeader);
		}
    if (aHasValue &&
        !SIPSyntaxCheck::Token(aValue) &&
		!SIPSyntaxCheck::QuotedString(aValue))
		{
		User::Leave(KErrSipCodecAuthenticationHeader);
		} 
    }	
