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
// Name          : CSIPParamContainerBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPParamContainerBase.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "SIPSyntaxCheck.h"
#include "sipcodecutils.h"
#include "sipstrconsts.h"

const TInt KMaxNumericValueAsTextLength = 80;
_LIT8(KTIntFormat, "%d");

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::CSIPParamContainerBase
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase::CSIPParamContainerBase(const TChar& aSeparator)
 : iParamSeparator (aSeparator)
	{
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ConstructL
// -----------------------------------------------------------------------------
//
void 
CSIPParamContainerBase::ConstructL(const CSIPParamContainerBase& aContainer)
	{
	for (TInt i=0; i < aContainer.iParams.Count(); i++)
		{
		CSIPParam* param = CSIPParam::NewLC(*aContainer.iParams[i]);
		User::LeaveIfError(iParams.Append(param));
		CleanupStack::Pop(param);
		}
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::~CSIPParamContainerBase
// -----------------------------------------------------------------------------
//
CSIPParamContainerBase::~CSIPParamContainerBase()
	{
	iParams.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetParamL(RStringF aName)
	{
	CSIPParam* param = CSIPParam::NewLC(aName);
	SetParamL(param);
	CleanupStack::Pop(param);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetParamL(RStringF aName, RStringF aValue)
	{
	CSIPParam* param = CSIPParam::NewLC(aName,aValue);
	SetParamL(param);
	CleanupStack::Pop(param);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetParamL(RStringF aName, const TDesC8& aValue)
	{
	CSIPParam* param = CSIPParam::NewLC(aName,aValue);
	SetParamL(param);
	CleanupStack::Pop(param);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetDesParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetDesParamL(RStringF aName, const TDesC8& aValue)
    {
    CSIPParam* param = CSIPParam::NewLC(aName,aValue);
    SetParamL(param);
    CleanupStack::Pop(param);
    param->SetValueL(aValue,ETrue);
    }

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetParamL(RStringF aName, TInt aValue)
	{
	__ASSERT_ALWAYS(aValue >= 0, User::Leave(KErrSipCodecAnyParam));
	TBuf8<KMaxNumericValueAsTextLength> valueAsText;
	valueAsText.Format(KTIntFormat,aValue);
	SetParamL(aName,valueAsText);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetParamL(RStringF aName, TReal aValue)
	{
	TBuf8<KMaxNumericValueAsTextLength> valueAsText;
	TRealFormat format;
	const TChar KDotChr = '.';
	format.iPoint = KDotChr; // SIP uses always dot as a decimal point
	User::LeaveIfError(valueAsText.Num(aValue,format));
	SetParamL(aName, valueAsText);
	}	

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::SetParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::SetParamL (CSIPParam* aParam)
	{
	__ASSERT_ALWAYS (aParam != NULL, User::Leave(KErrSipCodecAnyParam));
	
	CheckParamL(*aParam);

	TInt index = FindParamIndex (aParam->Name());
	User::LeaveIfError (iParams.Append(aParam));
	if (index >= 0) // remove the old param
		{
		CSIPParam* oldParam = iParams[index];
		iParams.Remove(index);
		delete oldParam;
		}	
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::HasParam
// -----------------------------------------------------------------------------
//
TBool CSIPParamContainerBase::HasParam (RStringF aParamName) const
	{
	return (FindParamIndex(aParamName) >= 0);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ParamValue
// -----------------------------------------------------------------------------
//
RStringF CSIPParamContainerBase::ParamValue (RStringF aParamName) const
	{
	TInt index = FindParamIndex(aParamName);
	if (index >= 0 && iParams[index]->HasValue())
		{
		return iParams[index]->Value();
		}
	return SIPStrings::StringF(SipStrConsts::EEmpty);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::DesParamValue
// -----------------------------------------------------------------------------
//	
const TDesC8& CSIPParamContainerBase::DesParamValue(RStringF aParamName) const
    {
	TInt index = FindParamIndex(aParamName);
	if (index >= 0)
		{
		return iParams[index]->DesValue();
		}
	return KNullDesC8;    
    }	

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::IntParamValue
// -----------------------------------------------------------------------------
//
TInt CSIPParamContainerBase::IntParamValue (RStringF aParamName) const
	{
	TPtrC8 des(ParamValue(aParamName).DesC());
	TInt value=KErrNotFound;
	if (des.Length() > 0)
		{
		TLex8 lex(des);
		TInt err = lex.Val(value);
		if (value < 0)
			{
			value = KErrSipCodecAnyParamValue;
			}	
		if (err != KErrNone)
			{
			value = err;
			}
		}
	return value;
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::RealParamValue
// -----------------------------------------------------------------------------
//
TReal CSIPParamContainerBase::RealParamValue (RStringF aParamName) const
	{
	TLex8 lex(ParamValue(aParamName).DesC());
	TReal value = 0;
	const TChar KDotChr = '.';
	lex.Val(value,KDotChr); // SIP uses always dot as a decimal point
	return value;
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::DeleteParam
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::DeleteParam (RStringF aParamName)
	{
	TInt index = FindParamIndex (aParamName);
	if (index >= 0) 
		{
		CSIPParam* param = iParams[index];
		iParams.Remove(index);
		delete param;
		}
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ParamCount
// -----------------------------------------------------------------------------
//
TInt CSIPParamContainerBase::ParamCount() const
	{
	return iParams.Count();
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ParamName
// -----------------------------------------------------------------------------
//	
TInt CSIPParamContainerBase::ParamName(TInt aIndex, RStringF& aParamName) const
    {
    TInt err = KErrNotFound;
    if (aIndex >= 0 && aIndex < iParams.Count())
        {
        aParamName = iParams[aIndex]->Name().Copy();
        err = KErrNone;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ToTextLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPParamContainerBase::ToTextLC () const
	{
	TUint encodedLength = 0;

	RPointerArray<HBufC8> paramsAsText;
	CleanupStack::PushL (TCleanupItem(ResetAndDestroy,&paramsAsText));

	for (TInt i=0; i < iParams.Count(); i++)
		{
		HBufC8* paramAsText = iParams[i]->ToTextLC();
		encodedLength += paramAsText->Length();
		if (i < iParams.Count()-1) 
		    {
		    encodedLength += 1; // param separator
		    }
		paramsAsText.AppendL(paramAsText);
		CleanupStack::Pop(paramAsText);
		}
	
	HBufC8* encodedParams = HBufC8::NewL (encodedLength);
	TPtr8 encodedParamsPtr = encodedParams->Des();

	for (TInt j=0; j < paramsAsText.Count(); j++)
		{
		encodedParamsPtr.Append (*paramsAsText[j]);
		if (j < paramsAsText.Count()-1)
			{
			encodedParamsPtr.Append(iParamSeparator);
			}
		}

	CleanupStack::PopAndDestroy(1); // paramsAsText
	CleanupStack::PushL(encodedParams);
	return encodedParams;
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::ExternalizeL (RWriteStream& aWriteStream)
	{
	aWriteStream.WriteUint32L (iParams.Count());
	for (TInt i=0; i < iParams.Count(); i++)
		{
		iParams[i]->ExternalizeL(aWriteStream);
		}
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::DoInternalizeL (RReadStream& aReadStream)
	{
	TUint32 parameterCount = aReadStream.ReadUint32L();
	for (TUint32 i=0; i < parameterCount; i++)
		{
		CSIPParam* param = CSIPParam::InternalizeL(aReadStream);
		CleanupStack::PushL (param);
		SetParamL(param);
		CleanupStack::Pop(param);
		}
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ParseL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::ParseL (const TDesC8& aValue)
	{
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC (aValue,iParamSeparator);
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		TPtrC8 pname;
		TPtrC8 pvalue(KNullDesC8);
		TBool hasValue = CSIPParam::ParseL(tokenizer->Tokens()[i],pname,pvalue);
		CSIPParam* param = CSIPParam::NewLC(pname);
		RStringF pnameStr = param->Name();
		if (HasParam(pnameStr)) 
			{
			User::Leave(KErrSipCodecDuplicateParam);
			}
		CheckAndUpdateParamL(pnameStr,hasValue,pvalue);
		if (hasValue)
		    {
		    param->SetValueL(pvalue,AddQuotesWhenEncoding(pnameStr));
		    }
        User::LeaveIfError(iParams.Append(param));
		CleanupStack::Pop(param);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::FindParamIndex
// -----------------------------------------------------------------------------
//
TInt CSIPParamContainerBase::FindParamIndex (RStringF aParamName) const
	{
	for (TInt i=0; i < iParams.Count(); i++)
		{
		if (aParamName == iParams[i]->Name()) 
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::ResetAndDestroy
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::ResetAndDestroy (TAny* anArray)
	{
	(reinterpret_cast<RPointerArray<HBufC8>*> (anArray))->ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::CheckGenericParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::CheckGenericParamL(CSIPParam& aParam,
                                                TInt aLeaveCode) const
    {
    TPtrC8 value(aParam.Value().DesC());
    CheckGenericParamL(aParam.Name(),aParam.HasValue(),value,aLeaveCode);
    } 

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::CheckGenericParamL
// -----------------------------------------------------------------------------
//
void CSIPParamContainerBase::CheckGenericParamL(RStringF aName,
	                                            TBool aHasValue,
	                                            const TDesC8& aValue, 
                                                TInt aLeaveCode) const
    {
	if (!SIPSyntaxCheck::Token(aName.DesC()))
		{
		User::Leave(aLeaveCode);
		}
	if (!aHasValue) 
		{
		return;
		}
	if (!SIPSyntaxCheck::GenericParamValue(aValue))
		{
		User::Leave(aLeaveCode);
		}
    }

// -----------------------------------------------------------------------------
// CSIPParamContainerBase::RemoveQuotes
// -----------------------------------------------------------------------------
//  
void CSIPParamContainerBase::RemoveQuotes(TPtrC8& aParamValue) const
    {
    if (SIPSyntaxCheck::StartsAndEndsWithQuotes(aParamValue))
        {
		TPtrC8 valueWithoutQuotes;
        SIPCodecUtils::ValueWithoutQuotes(aParamValue, valueWithoutQuotes);
        aParamValue.Set(valueWithoutQuotes);
		}   
    }
    
// ----------------------------------------------------------------------------
// CSIPParamContainerBase::AddQuotesWhenEncoding
// ----------------------------------------------------------------------------
//    
TBool CSIPParamContainerBase::AddQuotesWhenEncoding(
    RStringF /*aParamName*/) const
    {
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CSIPParamContainerBase::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPParamContainerBase::CheckParamL(CSIPParam& aParam) const
	{
	RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC();
	if (aParam.DesValue().Length() > 0)
	    {
	    value.Set(aParam.DesValue());
	    }	
	CheckAndUpdateParamL(name,aParam.HasValue(),value);
	if (AddQuotesWhenEncoding(name))
	    {
	    // Quotes may have been removed. Set the updated value.
	    aParam.SetValueL(value,ETrue);     
	    }
	}    
    
// -----------------------------------------------------------------------------
// CSIPParamContainerBase::CheckAndUpdateParamL
// -----------------------------------------------------------------------------
//      
void CSIPParamContainerBase::CheckAndUpdateParamL(RStringF /*aName*/,
                                                  TBool /*aHasValue*/,
                                                  TPtrC8& /*aValue*/) const
    {
    // By default NOP.
    }
