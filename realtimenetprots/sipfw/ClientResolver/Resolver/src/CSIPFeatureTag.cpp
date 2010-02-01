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
// Name          : CSIPFeatureTag.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSIPFeatureTag.h"

// -----------------------------------------------------------------------------
// CSIPFeatureTag::CSIPFeatureTag
// -----------------------------------------------------------------------------
//
CSIPFeatureTag::CSIPFeatureTag(TType aType, RStringF aName, TBool aNegation)
    : iType(aType),
      iNegation(aNegation)
	{
	iName = aName.Copy();
	}

// -----------------------------------------------------------------------------
// CSIPFeatureTag::CSIPFeatureTag
// -----------------------------------------------------------------------------
//
CSIPFeatureTag::CSIPFeatureTag()
	{
	}

// -----------------------------------------------------------------------------
// CSIPFeatureTag::~CSIPFeatureTag
// -----------------------------------------------------------------------------
//
CSIPFeatureTag::~CSIPFeatureTag()
	{
	iName.Close();
	}

// -----------------------------------------------------------------------------
// CSIPFeatureTag::Type
// -----------------------------------------------------------------------------
//
CSIPFeatureTag::TType CSIPFeatureTag::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CSIPFeatureTag::Name
// -----------------------------------------------------------------------------
//
RStringF CSIPFeatureTag::Name() const
    {
    return iName;
    }

// -----------------------------------------------------------------------------
// CSIPFeatureTag::Negation
// -----------------------------------------------------------------------------
//
TBool CSIPFeatureTag::Negation() const
    {
    return iNegation;
    }

// -----------------------------------------------------------------------------
// CSIPFeatureTag::Match
// -----------------------------------------------------------------------------
//    
TBool CSIPFeatureTag::Match(const CSIPFeatureTag& aFeatureTag) const
    {
    TBool match = EFalse;
    if (iType == aFeatureTag.iType && iName == aFeatureTag.iName)
        {
        match = MatchValue(aFeatureTag);
        }
    return match;
    }
