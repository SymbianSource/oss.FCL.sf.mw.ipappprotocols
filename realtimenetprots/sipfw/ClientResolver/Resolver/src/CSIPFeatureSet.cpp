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
// Name          : CSIPFeatureSet.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPFeatureSet.h"
#include "CSIPBooleanFeatureTag.h"
#include "CSIPNumericFeatureTag.h"
#include "CSIPStringFeatureTag.h"
#include "CSIPTokenFeatureTag.h"
#include "sipacceptcontactheader.h"
#include "TTagValueListParser.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

_LIT8(KTrue, "TRUE");
_LIT8(KFalse, "FALSE");

// -----------------------------------------------------------------------------
// CSIPFeatureSet::NewL
// -----------------------------------------------------------------------------
//
CSIPFeatureSet* CSIPFeatureSet::NewL()
	{
	CSIPFeatureSet* self = CSIPFeatureSet::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::NewLC
// -----------------------------------------------------------------------------
//
CSIPFeatureSet* CSIPFeatureSet::NewLC()
	{
	CSIPFeatureSet* self = new(ELeave)CSIPFeatureSet;
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::NewL
// -----------------------------------------------------------------------------
//
CSIPFeatureSet* CSIPFeatureSet::NewL(
    const CSIPAcceptContactHeader& aAcceptContactHeader)
	{
	CSIPFeatureSet* self = CSIPFeatureSet::NewLC(aAcceptContactHeader);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::NewLC
// -----------------------------------------------------------------------------
//
CSIPFeatureSet* CSIPFeatureSet::NewLC(
    const CSIPAcceptContactHeader& aAcceptContactHeader)
	{
	CSIPFeatureSet* self = new(ELeave)CSIPFeatureSet;
	CleanupStack::PushL(self);
	self->ConstructL(aAcceptContactHeader);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::CSIPFeatureSet
// -----------------------------------------------------------------------------
//
CSIPFeatureSet::CSIPFeatureSet()
	{
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPFeatureSet::ConstructL(
    const CSIPAcceptContactHeader& aAcceptContactHeader)
	{
    AddFeaturesL(aAcceptContactHeader);
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::~CSIPFeatureSet
// -----------------------------------------------------------------------------
//
CSIPFeatureSet::~CSIPFeatureSet()
	{
	iFeatureTags.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::AddFeaturesL
// -----------------------------------------------------------------------------
//
void CSIPFeatureSet::AddFeaturesL(
    const CSIPAcceptContactHeader& aAcceptContactHeader)
    {
	TInt paramCount = aAcceptContactHeader.ParamCount();
	for(TInt i = 0; i < paramCount; i++)
	    {
	    RStringF name;
	    User::LeaveIfError(aAcceptContactHeader.Param(i,name));
	    CleanupClosePushL(name);
	    if (IsFeatureParamName(name))
	        {
    	    RStringF value = aAcceptContactHeader.ParamValue(name);
    	    AddFeatureTagsL(name,value.DesC());
	        }
	    CleanupStack::PopAndDestroy(1); // name
	    }
    }

// -----------------------------------------------------------------------------
// CSIPFeatureSet::Require
// -----------------------------------------------------------------------------
//
TBool CSIPFeatureSet::Require() const
	{
	return iRequire;
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::CalculateScore
// -----------------------------------------------------------------------------
//
TInt CSIPFeatureSet::CalculateScore(const CSIPFeatureSet& aFeatureSet) const
	{
	TInt score = 0;
	for(TInt i = 0; i < aFeatureSet.iFeatureTags.Count(); i++)
	    {
	    const CSIPFeatureTag* otherTag = aFeatureSet.iFeatureTags[i];
        for(TInt j = 0; j < iFeatureTags.Count(); j++)
            {
            const CSIPFeatureTag* myTag = iFeatureTags[j];
            if (myTag->Match(*otherTag))
    	        {
    	        score++;
    	        }
    	    }
	    }
	if (aFeatureSet.Require() && score < aFeatureSet.iFeatureTags.Count())
	    {
	    // All feature tags should match if require-tag is present
	    score = 0;
	    }
	return score;
	}

// -----------------------------------------------------------------------------
// CSIPFeatureSet::AddFeatureTagsL
// -----------------------------------------------------------------------------
//	
void CSIPFeatureSet::AddFeatureTagsL(
    RStringF aName,
    const TDesC8& aValue)
    {
    if (aValue.Length() > 0)
        {
        TPtrC8 value;
        RemoveQuotesL(aValue,value);
        if (value[0] == '<')
            {
            CSIPFeatureTag* fTag = CSIPStringFeatureTag::NewLC(aName,value);
            iFeatureTags.AppendL(fTag);
            CleanupStack::Pop(fTag);            
            }
        else
            {
            TTagValueListParser parser;
            parser.Parse(value);
            while (!parser.Eos())
                {
                TPtrC8 segment;
                User::LeaveIfError(parser.GetNext(segment));
                AddFeatureTagL(aName,segment);
                }            
            }
        }
    else
        {
        AddBooleanFeatureTagL(aName,EFalse,ETrue);    
        }
    }

// -----------------------------------------------------------------------------
// CSIPFeatureSet::AddFeatureTagL
// -----------------------------------------------------------------------------
//
void CSIPFeatureSet::AddFeatureTagL(
    RStringF aName,
    const TDesC8& aValue)
    {
    TBool negation = EFalse;
    TLex8 lex(aValue);
    TChar chr = lex.Get();
    if (chr == '!')
        {
        negation = ETrue;
        chr = lex.Get();
        }
    if (chr == '#')
        {
        CSIPFeatureTag* fTag = 
            CSIPNumericFeatureTag::NewLC(aName,negation,lex.Remainder());
        iFeatureTags.AppendL(fTag);
        CleanupStack::Pop(fTag);
        }
    else
        {
        lex.UnGet();
        TPtrC8 value(lex.Remainder());
        if (value.CompareF(KTrue) == 0 || 
            value.CompareF(KFalse) == 0 ||
            value.Length() == 0)
            {
            TBool boolVal = !(value.CompareF(KFalse) == 0);
            AddBooleanFeatureTagL(aName,negation,boolVal);
            }
        else
            {
            CSIPFeatureTag* fTag = 
                CSIPTokenFeatureTag::NewLC(aName,negation,value);
            iFeatureTags.AppendL(fTag);
            CleanupStack::Pop(fTag);            
            }
        }    
    }

// -----------------------------------------------------------------------------
// CSIPFeatureSet::AddBooleanFeatureTagL
// -----------------------------------------------------------------------------
//
void CSIPFeatureSet::AddBooleanFeatureTagL(
    RStringF aName,
    TBool aNegation,
    TBool aValue)
    {
    if (aName == SIPStrings::StringF(SipStrConsts::ERequire))
        {
        iRequire = ETrue;
        }
    else if (aName == SIPStrings::StringF(SipStrConsts::EExplicit))
        {
        // explicit-parameter not handled. 
        // It must not be added to feature set
        }
    else
        {
        CSIPFeatureTag* fTag = 
            CSIPBooleanFeatureTag::NewLC(aName,aNegation,aValue);
        iFeatureTags.AppendL(fTag);
        CleanupStack::Pop(fTag);
        }   
    } 		

// -----------------------------------------------------------------------------
// CSIPFeatureSet::RemoveQuotesL
// -----------------------------------------------------------------------------
//    
void CSIPFeatureSet::RemoveQuotesL(
    const TDesC8& aValue,
    TPtrC8& aWithoutQuotes) const
    {
    __ASSERT_ALWAYS(aValue.Length() > 0, User::Leave(KErrCorrupt));
    TLex8 lex(aValue);
    lex.SkipSpace();
    TPtrC8 remainder(lex.Remainder());
    if (StartsAndEndsWithQuotes(remainder))
        {
        lex.Get(); // Skip the opening quote
        remainder.Set(lex.Remainder());
        aWithoutQuotes.Set(remainder.Left(remainder.Length()-1));
        }
    else
        {
        aWithoutQuotes.Set(aValue);
        }
    }

// -----------------------------------------------------------------------------
// CSIPFeatureSet::StartsAndEndsWithQuotes
// -----------------------------------------------------------------------------
//
TBool CSIPFeatureSet::StartsAndEndsWithQuotes(const TDesC8& aValue) const
    {
	// trim and check quotes
	TLex8 lex(aValue);
	lex.SkipSpace();
	TPtrC8 trimmedVal(lex.Remainder());
	const TInt KTwoQuotesLength = 2;
	if (trimmedVal.Length() < KTwoQuotesLength) 
		{
		return EFalse;
		}
	if (trimmedVal.Locate('"') != 0) 
		{
		return EFalse;
		}
	if (trimmedVal.LocateReverse('"') != trimmedVal.Length()-1) 
		{
		return EFalse;
		}
	return ETrue; 
    }

// -----------------------------------------------------------------------------
// CSIPFeatureSet::IsFeatureParamName
// -----------------------------------------------------------------------------
//
TBool CSIPFeatureSet::IsFeatureParamName(RStringF aName) const
    {
    if (aName == SIPStrings::StringF(SipStrConsts::EAudio) ||
        aName == SIPStrings::StringF(SipStrConsts::EAutomata) ||
		aName == SIPStrings::StringF(SipStrConsts::EClass) ||
		aName == SIPStrings::StringF(SipStrConsts::EDuplex) ||
		aName == SIPStrings::StringF(SipStrConsts::EData) ||
		aName == SIPStrings::StringF(SipStrConsts::EControl) ||
		aName == SIPStrings::StringF(SipStrConsts::EMobility) ||
		aName == SIPStrings::StringF(SipStrConsts::EDescription) ||
		aName == SIPStrings::StringF(SipStrConsts::EEvents) ||
		aName == SIPStrings::StringF(SipStrConsts::EPriority) ||
		aName == SIPStrings::StringF(SipStrConsts::EMethods) ||
		aName == SIPStrings::StringF(SipStrConsts::ESchemes) ||
		aName == SIPStrings::StringF(SipStrConsts::EApplication) ||
		aName == SIPStrings::StringF(SipStrConsts::EVideo) ||
		aName == SIPStrings::StringF(SipStrConsts::ELanguage) ||
		aName == SIPStrings::StringF(SipStrConsts::EType) ||
		aName == SIPStrings::StringF(SipStrConsts::EIsfocus) ||
		aName == SIPStrings::StringF(SipStrConsts::EActor) ||
		aName == SIPStrings::StringF(SipStrConsts::EText) ||
		aName == SIPStrings::StringF(SipStrConsts::EExtensions) ||
		aName == SIPStrings::StringF(SipStrConsts::ERequire) ||
		aName == SIPStrings::StringF(SipStrConsts::EExplicit))
        {
        return ETrue;
        }
    TLex8 lex(aName.DesC());
    lex.SkipSpace();
    return (lex.Peek() == '+');
    }
