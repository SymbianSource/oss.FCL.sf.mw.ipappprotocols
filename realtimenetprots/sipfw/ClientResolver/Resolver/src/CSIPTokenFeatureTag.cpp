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
// Name          : CSIPTokenFeatureTag.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPTokenFeatureTag.h"

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::NewL
// -----------------------------------------------------------------------------
//
CSIPTokenFeatureTag* CSIPTokenFeatureTag::NewL(
    RStringF aName, 
    TBool aNegation,
    const TDesC8& aValue)
	{
	CSIPTokenFeatureTag* self = 
	    CSIPTokenFeatureTag::NewLC(aName,aNegation,aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::NewLC
// -----------------------------------------------------------------------------
//
CSIPTokenFeatureTag* CSIPTokenFeatureTag::NewLC(
    RStringF aName, 
    TBool aNegation,
    const TDesC8& aValue)
	{
	CSIPTokenFeatureTag* self = 
	    new(ELeave)CSIPTokenFeatureTag(aName,aNegation);
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::CSIPTokenFeatureTag
// -----------------------------------------------------------------------------
//
CSIPTokenFeatureTag::CSIPTokenFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::CSIPTokenFeatureTag
// -----------------------------------------------------------------------------
//
CSIPTokenFeatureTag::CSIPTokenFeatureTag(
    RStringF aName, 
    TBool aNegation)
    : CSIPFeatureTag(CSIPTokenFeatureTag::ETokenFeatureTag,aName,aNegation)  
	{
	}

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::ConstructL
// -----------------------------------------------------------------------------
//   
void CSIPTokenFeatureTag::ConstructL(const TDesC8& aValue)
    {
    iValue = aValue.AllocL();
    }

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::~CSIPTokenFeatureTag
// -----------------------------------------------------------------------------
//
CSIPTokenFeatureTag::~CSIPTokenFeatureTag()
	{
	delete iValue;
	}

// -----------------------------------------------------------------------------
// CSIPTokenFeatureTag::MatchValue
// -----------------------------------------------------------------------------
//
TBool CSIPTokenFeatureTag::MatchValue(
    const CSIPFeatureTag& aFeatureTag) const
	{
	// Comparison is case insensitive
	TBool match = EFalse;
	const CSIPTokenFeatureTag& other = 
	    static_cast<const CSIPTokenFeatureTag&>(aFeatureTag);
	if (iNegation)
	    {
	    if (other.iNegation)
	        {
	        match = (iValue->CompareF(*(other.iValue)) == 0);
	        }
	    else
	        {
	        match = (iValue->CompareF(*(other.iValue)) != 0);
	        }
	    }
	else
	    {
	    if (other.iNegation)
	        {
	        match = (iValue->CompareF(*(other.iValue)) != 0);
	        }
	    else
	        {
	        match = (iValue->CompareF(*(other.iValue)) == 0);
	        }
	    }
	return match; 
	}
