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
// Name          : CSIPNumericFeatureTag.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPNumericFeatureTag.h"
#include <e32math.h>

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::NewL
// -----------------------------------------------------------------------------
//
CSIPNumericFeatureTag* CSIPNumericFeatureTag::NewL(
    RStringF aName, 
    TBool aNegation,
    const TDesC8& aValueNoHash)
	{
	CSIPNumericFeatureTag* self = 
	    CSIPNumericFeatureTag::NewLC(aName,aNegation,aValueNoHash);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::NewLC
// -----------------------------------------------------------------------------
//
CSIPNumericFeatureTag* CSIPNumericFeatureTag::NewLC(
    RStringF aName, 
    TBool aNegation,
    const TDesC8& aValueNoHash)
	{
	CSIPNumericFeatureTag* self = 
	    new(ELeave)CSIPNumericFeatureTag(aName,aNegation);
	CleanupStack::PushL(self);
	self->ConstructL(aValueNoHash);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::CSIPNumericFeatureTag
// -----------------------------------------------------------------------------
//
CSIPNumericFeatureTag::CSIPNumericFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::CSIPNumericFeatureTag
// -----------------------------------------------------------------------------
//
CSIPNumericFeatureTag::CSIPNumericFeatureTag(
    RStringF aName, 
    TBool aNegation)
    : CSIPFeatureTag(CSIPFeatureTag::ENumericFeatureTag,aName,aNegation),
      iLowerBound(-KMaxTReal),
      iUpperBound(KMaxTReal)  
	{
	}

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::ConstructL
// -----------------------------------------------------------------------------
//   
void CSIPNumericFeatureTag::ConstructL(const TDesC8& aValueNoHash)
    {
    TLex8 lexer(aValueNoHash);
    TRelation relation;
    User::LeaveIfError(ParseRelation(lexer,relation));
    TPtrC8 remainder(lexer.Remainder());
    switch(relation)
        {
        case ERange:
            User::LeaveIfError(ParseRange(remainder,iLowerBound,iUpperBound));
            break;
        case EGreaterThanOrEqual:
            User::LeaveIfError(ParseNumber(remainder,iLowerBound));
            break;
        case ELessThanOrEqual:
            User::LeaveIfError(ParseNumber(remainder,iUpperBound));
            break;
        default:
            {
            User::LeaveIfError(ParseNumber(remainder,iLowerBound));
            iUpperBound = iLowerBound;
            }
            break;    
        }
    }

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::~CSIPNumericFeatureTag
// -----------------------------------------------------------------------------
//
CSIPNumericFeatureTag::~CSIPNumericFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::MatchValue
// -----------------------------------------------------------------------------
//
TBool CSIPNumericFeatureTag::MatchValue(const CSIPFeatureTag& aFeatureTag) const
	{
	const CSIPNumericFeatureTag& other = 
	    static_cast<const CSIPNumericFeatureTag&>(aFeatureTag);
	return IsWithinBounds(other.iLowerBound,other.iUpperBound,
	                      other.iNegation);
	}

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::ParseRelation
// -----------------------------------------------------------------------------
//	
TInt CSIPNumericFeatureTag::ParseRelation(
    TLex8& aLexer,
    TRelation& aRelation)
    {
    if (aLexer.Remainder().Length() == 0)
        {
        return KErrCorrupt;
        }
    switch(aLexer.Get())
        {
        case '>':
            {
            if (aLexer.Get() != '=')
                {
                return KErrCorrupt;
                }
            aRelation = EGreaterThanOrEqual;
            }
            break;
        case '<':
            {
            if (aLexer.Get() != '=')
                {
                return KErrCorrupt;
                }
            aRelation = ELessThanOrEqual;     
            }
            break;
        case '=':
            {
            aRelation = EEqual;
            }
            break;
        default:
            {
            aRelation = ERange;
            aLexer.UnGet();            
            }
            break;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::ParseRange
// -----------------------------------------------------------------------------
//
TInt CSIPNumericFeatureTag::ParseRange(
    const TDesC8& aDes,
    TReal& aLowerBound,
    TReal& aUpperBound)
    {
    TInt colonPos = aDes.Locate(':');
    if (colonPos <= 0 || colonPos == aDes.Length()-1)
        {
        return KErrCorrupt;
        }
    TInt err = ParseNumber(aDes.Left(colonPos),aLowerBound);
    if (err == KErrNone)
        {
        err = ParseNumber(aDes.Mid(colonPos+1),aUpperBound);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::ParseNumber
// -----------------------------------------------------------------------------
//
TInt CSIPNumericFeatureTag::ParseNumber(
    const TDesC8& aDes,
    TReal& aValue)
    {
    if (aDes.Length() == 0)
        {
        return KErrCorrupt;
        }
    TLex8 lexer(aDes);
    TChar chr = lexer.Get();
    TReal multiplier = 1;
    if (chr == '-')
        {
        multiplier = -1;
        }
    else
        {
        if (chr.IsDigit())
            {
            lexer.UnGet();
            }
        else
            {
            if (chr != '+')
                {
                return KErrCorrupt;
                }
            }
        }
    const TChar KDot = '.';    
    TInt err = lexer.Val(aValue,KDot);
    if (err == KErrNone)
        {
        if (lexer.Remainder().Length() > 0)
            {
            err = KErrCorrupt;
            }
        else
            {
            aValue *= multiplier;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPNumericFeatureTag::IsWithinBounds
// -----------------------------------------------------------------------------
//
TBool CSIPNumericFeatureTag::IsWithinBounds(
    TReal aLowerBound,
    TReal aUpperBound,
    TBool aNegation) const
    {
    TBool withinBounds = EFalse;
    if (iNegation)
        {
        if (aNegation)
            {
            withinBounds = 
                ((aLowerBound == iLowerBound && aUpperBound == iUpperBound) ||
                 (aLowerBound < iLowerBound || aUpperBound > iUpperBound));
            }
        else
            {
            withinBounds = (aUpperBound < iLowerBound || 
                            aLowerBound > iUpperBound);
            }        
        }
    else
        {
        if (!aNegation)
            {
            withinBounds = (aLowerBound >= iLowerBound && 
                            aUpperBound <= iUpperBound);
            }
        // If the compared feature tag has a negation, it is never within bounds
        }    
    return withinBounds;
    }
