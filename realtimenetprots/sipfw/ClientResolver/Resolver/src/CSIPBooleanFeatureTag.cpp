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
// Name          : CSIPBooleanFeatureTag.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPBooleanFeatureTag.h"

// -----------------------------------------------------------------------------
// CSIPBooleanFeatureTag::NewL
// -----------------------------------------------------------------------------
//
CSIPBooleanFeatureTag* CSIPBooleanFeatureTag::NewL(
    RStringF aName,
    TBool aNegation, 
    TBool aValue)
	{
	CSIPBooleanFeatureTag* self = 
	    CSIPBooleanFeatureTag::NewLC(aName,aNegation,aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPBooleanFeatureTag::NewLC
// -----------------------------------------------------------------------------
//
CSIPBooleanFeatureTag* CSIPBooleanFeatureTag::NewLC(
    RStringF aName,
    TBool aNegation, 
    TBool aValue)
	{
	CSIPBooleanFeatureTag* self = 
	    new(ELeave)CSIPBooleanFeatureTag(aName,aNegation,aValue);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPBooleanFeatureTag::CSIPBooleanFeatureTag
// -----------------------------------------------------------------------------
//
CSIPBooleanFeatureTag::CSIPBooleanFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPBooleanFeatureTag::CSIPBooleanFeatureTag
// -----------------------------------------------------------------------------
//
CSIPBooleanFeatureTag::CSIPBooleanFeatureTag(
    RStringF aName,
    TBool aNegation, 
    TBool aValue)
    : CSIPFeatureTag(CSIPFeatureTag::EBooleanFeatureTag,aName,aNegation),
      iValue(aValue)    
	{
	if (aNegation)
	    {
	    iValue = !aValue;
	    }
	}

// -----------------------------------------------------------------------------
// CSIPBooleanFeatureTag::~CSIPBooleanFeatureTag
// -----------------------------------------------------------------------------
//
CSIPBooleanFeatureTag::~CSIPBooleanFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPBooleanFeatureTag::MatchValue
// -----------------------------------------------------------------------------
//
TBool CSIPBooleanFeatureTag::MatchValue(const CSIPFeatureTag& aFeatureTag) const
	{
	const CSIPBooleanFeatureTag& other = 
	    static_cast<const CSIPBooleanFeatureTag&>(aFeatureTag);
	return (iValue == other.iValue);
	}
