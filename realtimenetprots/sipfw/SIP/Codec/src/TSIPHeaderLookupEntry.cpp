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
// Name          : TSIPHeaderLookupEntry.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "TSIPHeaderLookupEntry.h"
#include "sipstrconsts.h"
#include "sipstrings.h"

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::TSIPHeaderLookupEntry 
// -----------------------------------------------------------------------------
//
TSIPHeaderLookupEntry::TSIPHeaderLookupEntry 
(RStringF aName,
 RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
 CSIPHeaderBase*(*aInternalize)(RReadStream&))

 : iName(aName.Copy()),
   iCompactName(SIPStrings::StringF(SipStrConsts::EEmpty)),
   iDecodeFunction (aDecode),
   iInternalizeFunction(aInternalize)
	{
	}

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::TSIPHeaderLookupEntry 
// -----------------------------------------------------------------------------
//
TSIPHeaderLookupEntry::TSIPHeaderLookupEntry 
(RStringF aName,
 RStringF aCompactName,
 RPointerArray<CSIPHeaderBase>(*aDecode)(const TDesC8&),
 CSIPHeaderBase*(*aInternalize)(RReadStream&))

 : iName(aName.Copy()),
   iCompactName(aCompactName.Copy()),
   iDecodeFunction (aDecode),
   iInternalizeFunction(aInternalize)
	{
	}

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::Name
// -----------------------------------------------------------------------------
//
RStringF TSIPHeaderLookupEntry::Name() const
	{
	return iName;
	}

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::CompactName
// -----------------------------------------------------------------------------
//
RStringF TSIPHeaderLookupEntry::CompactName() const
	{
	return iCompactName;
	}

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::DecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
TSIPHeaderLookupEntry::DecodeL(const TDesC8& aHeaderValue) const
	{ 
	return (*iDecodeFunction)(aHeaderValue);
	}

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPHeaderBase* 
TSIPHeaderLookupEntry::InternalizeL(RReadStream& aReadStream) const
	{
	if (iInternalizeFunction == 0)
        {
        return 0;
        }
	return (*iInternalizeFunction)(aReadStream);
	}

// -----------------------------------------------------------------------------
// TSIPHeaderLookupEntry::InternalizeSupported
// -----------------------------------------------------------------------------
//
TBool TSIPHeaderLookupEntry::InternalizeSupported() const
    {
    return (iInternalizeFunction != 0);
    }
