// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPAuthenticateHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/5.0 
//



#include "sipauthenticateheaderbase.h"
#include "CSIPAuthenticateHeaderParams.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::~CSIPAuthenticateHeaderBase
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthenticateHeaderBase::~CSIPAuthenticateHeaderBase()
	{
	delete iParams;
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::CSIPAuthenticateHeaderBase
// ----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderBase::CSIPAuthenticateHeaderBase()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAuthenticateHeaderBase::ConstructL()
	{
	iParams = new(ELeave)CSIPAuthenticateHeaderParams;
	}	
	
// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAuthenticateHeaderBase::ConstructL(
    const CSIPAuthenticateHeaderBase& aHeader)
	{
	CSIPAuthHeaderBase::ConstructL(aHeader.AuthScheme());
	iParams = CSIPAuthenticateHeaderParams::NewL(*(aHeader.iParams));
	}	
    
// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAuthenticateHeaderBase::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAuthenticateHeaderBase::Params ()
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::HasQopValue
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAuthenticateHeaderBase::HasQopValueL(
    const TDesC8& aValue) const
    { 
	TBool found = EFalse;
	RStringF qop = SIPStrings::StringF(SipStrConsts::EQop);
    TPtrC8 qopWithoutQuotes;
    SIPCodecUtils::ValueWithoutQuotes(
        iParams->ParamValue(qop).DesC(), qopWithoutQuotes);		
	if(qopWithoutQuotes.Length() > 0 && aValue.Length() > 0)
		{
		CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(qopWithoutQuotes, ',');
		for (TInt i=0; i < tokenizer->Tokens().Count() && !found; i++)
			{
			HBufC8* trimmedToken = tokenizer->Tokens()[i].AllocLC();
			trimmedToken->Des().Trim();
			if(aValue == *trimmedToken)
				{
				found = ETrue;
				}
			CleanupStack::PopAndDestroy(trimmedToken);
			}
		CleanupStack::PopAndDestroy(tokenizer);
		}
	return found;
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::ExternalizeSupported
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAuthenticateHeaderBase::ExternalizeSupported() const
	{
	return ETrue;
	}
		
// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::ExternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAuthenticateHeaderBase::ExternalizeValueL(
    RWriteStream& aWriteStream) const
	{
	TPtrC8 authScheme(AuthScheme().DesC());
	aWriteStream.WriteUint32L(authScheme.Length());
	aWriteStream.WriteL(authScheme);
	iParams->ExternalizeL(aWriteStream);	
	}

// ----------------------------------------------------------------------------
// CSIPAuthenticateHeaderBase::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAuthenticateHeaderBase::DoInternalizeValueL(RReadStream& aReadStream)
	{
    HBufC8* authScheme = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    CleanupStack::PushL(authScheme);
    SetAuthSchemeL(*authScheme);
    CleanupStack::PopAndDestroy(authScheme);
	iParams = CSIPAuthenticateHeaderParams::InternalizeL (aReadStream);	
	}
