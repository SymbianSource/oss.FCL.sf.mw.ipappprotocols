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
// Name          : CSIPAuthHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipauthheaderbase.h"
#include "CSIPParamContainerBase.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::CSIPAuthHeaderBase
// ----------------------------------------------------------------------------
//
CSIPAuthHeaderBase::CSIPAuthHeaderBase()
:iSeparator(',')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAuthHeaderBase::ConstructL(RStringF aAuthScheme)
	{
	SetAuthSchemeL(aAuthScheme.DesC());
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::~CSIPAuthHeaderBase
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthHeaderBase::~CSIPAuthHeaderBase()
	{
	iAuthScheme.Close();
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::AuthScheme
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAuthHeaderBase::AuthScheme() const
	{
	return iAuthScheme;
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::SetParamAndAddQuotesL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAuthHeaderBase::SetParamAndAddQuotesL(RStringF aName,
                                                        const TDesC8& aValue)
	{
    const TInt KQuotesLength = 2;
	HBufC8* tmp = HBufC8::NewLC(aValue.Length()+KQuotesLength);
	TPtr8 tmpPtr(tmp->Des());
	tmpPtr.Append('"');
	tmpPtr.Append(aValue);
	tmpPtr.Append('"');	
	Params().SetParamL(aName,tmpPtr);
	CleanupStack::PopAndDestroy(tmp);
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::ExternalizeSupported
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAuthHeaderBase::ExternalizeSupported() const
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::EncodeMultipleToOneLine
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPAuthHeaderBase::EncodeMultipleToOneLine() const
	{
	return EFalse;	
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//		
TBool CSIPAuthHeaderBase::MoreThanOneAllowed() const
	{
	return ETrue;	
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPAuthHeaderBase::PreferredPlaceInMessage () const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
HBufC8* CSIPAuthHeaderBase::ToTextValueL() const
	{
	TUint encodedLength = iAuthScheme.DesC().Length(); 

	HBufC8* encodedParams = Params().ToTextLC();
	if (Params().ParamCount() > 0)
		{
		encodedLength += 1; // separator		
		encodedLength += encodedParams->Length();
		}

	HBufC8* encodingResult = HBufC8::NewL (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();

	encodingResultPtr.Append(iAuthScheme.DesC());

	if (Params().ParamCount() > 0)
		{
		encodingResultPtr.Append(' '); // Separate Scheme from other values
		encodingResultPtr.Append(*encodedParams);
		}

	CleanupStack::PopAndDestroy(encodedParams);
	return encodingResult;
	}

// ----------------------------------------------------------------------------
// CSIPAuthHeaderBase::SetAuthSchemeL
// ----------------------------------------------------------------------------
//
void CSIPAuthHeaderBase::SetAuthSchemeL(const TDesC8& aAuthScheme)
	{		
    RStringF tmp = 
        SIPCodecUtils::CheckAndCreateTokenL(
            aAuthScheme,KErrSipCodecAuthScheme);
	iAuthScheme.Close();
    iAuthScheme = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::ParseL
// -----------------------------------------------------------------------------
//
void CSIPAuthHeaderBase::ParseL(const TDesC8& aValue)
	{
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 mandatoryPart(lex.NextToken());
	if (mandatoryPart.Length() < 1) 
		{
		User::Leave (KErrSipCodecAuthScheme);
		}
    SetAuthSchemeL(mandatoryPart);
    lex.SkipSpace();
    Params().ParseL(lex.Remainder());
	}

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::HasParam
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAuthHeaderBase::HasParam(RStringF aName) const
	{
	return Params().HasParam(aName);
	}

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::ParamValue
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPAuthHeaderBase::ParamValue(RStringF aName) const
	{
	return Params().ParamValue(aName);
	}

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::DesParamValue
// -----------------------------------------------------------------------------
//	
EXPORT_C const TDesC8& CSIPAuthHeaderBase::DesParamValue(RStringF aName) const
    {
    return Params().DesParamValue(aName);
    }		

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::SetParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPAuthHeaderBase::SetParamL(
    RStringF aName,
    RStringF aValue)
	{
	Params().SetParamL(aName,aValue);
	}

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::SetDesParamL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CSIPAuthHeaderBase::SetDesParamL(
    RStringF aName,
    const TDesC8& aValue)
    {
    Params().SetDesParamL(aName,aValue);
    }

// -----------------------------------------------------------------------------
// CSIPAuthHeaderBase::DeleteParam
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPAuthHeaderBase::DeleteParam(RStringF aName)
	{
	Params().DeleteParam(aName);
	}
