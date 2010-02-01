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
// Name          : CSIPStringFeatureTag.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPStringFeatureTag.h"

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::NewL
// -----------------------------------------------------------------------------
//
CSIPStringFeatureTag* CSIPStringFeatureTag::NewL(
    RStringF aName, 
    const TDesC8& aValueWithBrackets)
	{
	CSIPStringFeatureTag* self = 
	    CSIPStringFeatureTag::NewLC(aName,aValueWithBrackets);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::NewLC
// -----------------------------------------------------------------------------
//
CSIPStringFeatureTag* CSIPStringFeatureTag::NewLC(
    RStringF aName, 
    const TDesC8& aValueWithBrackets)
	{
	CSIPStringFeatureTag* self = 
	    new(ELeave)CSIPStringFeatureTag(aName);
	CleanupStack::PushL(self);
	self->ConstructL(aValueWithBrackets);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::CSIPStringFeatureTag
// -----------------------------------------------------------------------------
//
CSIPStringFeatureTag::CSIPStringFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::CSIPStringFeatureTag
// -----------------------------------------------------------------------------
//
CSIPStringFeatureTag::CSIPStringFeatureTag(
    RStringF aName)
    : CSIPFeatureTag(CSIPFeatureTag::EStringFeatureTag,aName,EFalse)  
	{
	}

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::ConstructL
// -----------------------------------------------------------------------------
//   
void CSIPStringFeatureTag::ConstructL(const TDesC8& aValueWithBrackets)
    {
    __ASSERT_ALWAYS(aValueWithBrackets.Length() >= 3, 
                    User::Leave(KErrCorrupt));
    
    __ASSERT_ALWAYS(aValueWithBrackets.Locate('<') == 0, 
                    User::Leave(KErrCorrupt));
    
    __ASSERT_ALWAYS(aValueWithBrackets.Locate('>') == 
                    aValueWithBrackets.Length()-1, 
                    User::Leave(KErrCorrupt));
    
	TPtrC8 includesLastBracket(aValueWithBrackets.Mid(1));
	TPtrC8 noBrackets(includesLastBracket.Left(includesLastBracket.Length()-1));
    iValue = noBrackets.AllocL();
    }

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::~CSIPStringFeatureTag
// -----------------------------------------------------------------------------
//
CSIPStringFeatureTag::~CSIPStringFeatureTag()
	{
	delete iValue;
	}

// -----------------------------------------------------------------------------
// CSIPStringFeatureTag::MatchValue
// -----------------------------------------------------------------------------
//
TBool CSIPStringFeatureTag::MatchValue(const CSIPFeatureTag& aFeatureTag) const
	{
	const CSIPStringFeatureTag& other = 
	    static_cast<const CSIPStringFeatureTag&>(aFeatureTag);
	return (iValue->Compare(*(other.iValue)) == 0); // case sensitive
	}
