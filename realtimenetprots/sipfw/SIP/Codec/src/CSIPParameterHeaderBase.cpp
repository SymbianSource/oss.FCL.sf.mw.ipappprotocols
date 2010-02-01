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
// Name          : CSIPParameterHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipparameterheaderbase.h"
#include "CSIPParamContainerBase.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::CSIPParameterHeaderBase
// -----------------------------------------------------------------------------
//
CSIPParameterHeaderBase::CSIPParameterHeaderBase()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::CSIPParameterHeaderBase
// -----------------------------------------------------------------------------
//
CSIPParameterHeaderBase::CSIPParameterHeaderBase(const TChar& aSeparator)
 : iSeparator(aSeparator)
	{
	}	

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::~CSIPParameterHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPParameterHeaderBase::~CSIPParameterHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::HasParam
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPParameterHeaderBase::HasParam(RStringF aName) const
	{
	return Params().HasParam(aName);
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::ParamValue
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPParameterHeaderBase::ParamValue(RStringF aName) const
	{
	return Params().ParamValue(aName);
	}		

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::SetParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPParameterHeaderBase::SetParamL (RStringF aName,
                                                  RStringF aValue)
	{
    __ASSERT_ALWAYS(ParamInsertionAllowed(),
                    User::Leave(KErrSipCodecNotAllowed));
	Params().SetParamL(aName,aValue);
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::SetParamL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPParameterHeaderBase::SetParamL (RStringF aName)
	{
    __ASSERT_ALWAYS(ParamInsertionAllowed(),
                    User::Leave(KErrSipCodecNotAllowed));
    Params().SetParamL(aName);
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::DeleteParam
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPParameterHeaderBase::DeleteParam(RStringF aName)
	{
	Params().DeleteParam(aName);
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::ParamCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPParameterHeaderBase::ParamCount() const
    {
    return Params().ParamCount();
    }

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::Param
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPParameterHeaderBase::Param(TInt aIndex, RStringF& aName) const
    {
    return Params().ParamName(aIndex,aName);
    }

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPParameterHeaderBase::ToTextValueL() const
	{
	HBufC8* encodedMandatoryPart = ToTextMandatoryPartLC();
	TUint encodedLength = encodedMandatoryPart->Length(); 

	HBufC8* encodedParams = Params().ToTextLC();
	if (Params().ParamCount() > 0)
		{
		encodedLength += 1; // separator		
		encodedLength += encodedParams->Length();
		}

	HBufC8* encodingResult = HBufC8::NewL (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();

	encodingResultPtr.Append(*encodedMandatoryPart);

	if (Params().ParamCount() > 0)
		{
		encodingResultPtr.Append(iSeparator);
		encodingResultPtr.Append(*encodedParams);
		}

	CleanupStack::PopAndDestroy(encodedParams);
	CleanupStack::PopAndDestroy(encodedMandatoryPart);
	return encodingResult;
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::ParseL
// -----------------------------------------------------------------------------
//
void CSIPParameterHeaderBase::ParseL(const TDesC8& aValue, 
                                     TBool aIgnoreParenthesis,
                                     TBool aSkipSeparatorsBeforeAt)
	{
	CSIPTokenizer* tokenizer = 
        CSIPTokenizer::NewLC(aValue,
        					 iSeparator, 
        					 aIgnoreParenthesis, 
        					 EFalse, 
        					 aSkipSeparatorsBeforeAt);
    ParseMandatoryPartL (tokenizer->Tokens()[0]);
    if (tokenizer->Tokens().Count() > 1 && !ParamInsertionAllowed())
        {
        User::Leave(KErrSipCodecAnyParam);
        }
	for (TInt i=1; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPParam* param = CSIPParam::DecodeL(tokenizer->Tokens()[i]);
		CleanupStack::PushL (param);
		//here the check for Duplicate Parameter was removed to solve Inter-op Issues.
		Params().SetParamL (param);
		CleanupStack::Pop(param);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	}

// -----------------------------------------------------------------------------
// CSIPParameterHeaderBase::ParamInsertionAllowed
// -----------------------------------------------------------------------------
//
TBool CSIPParameterHeaderBase::ParamInsertionAllowed() const
    {
    // default implementation
    return ETrue;
    }   
