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
// Name          : CSIPUnsignedIntHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipunsignedintheaderbase.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "_sipcodecdefs.h"

_LIT8(KTUintFormat, "%u");


// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::CSIPUnsignedIntHeaderBase
// -----------------------------------------------------------------------------
//
CSIPUnsignedIntHeaderBase::CSIPUnsignedIntHeaderBase(TUint aValue)
: iValue (aValue)
	{
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::~CSIPUnsignedIntHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPUnsignedIntHeaderBase::~CSIPUnsignedIntHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::SetValue
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPUnsignedIntHeaderBase::SetValue(TUint aValue)
	{
	iValue = aValue;
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::Value
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPUnsignedIntHeaderBase::Value() const
	{
	return iValue;
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPUnsignedIntHeaderBase::ToTextValueL() const
	{
	const TInt KMaxNumericValueAsTextLength = 80;
	TBuf8<KMaxNumericValueAsTextLength> value;
	value.Format(KTUintFormat, iValue);
	return value.AllocL();
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::ParseL
// -----------------------------------------------------------------------------
//
void CSIPUnsignedIntHeaderBase::ParseL(
    const TDesC8& aValue,
    TInt aErrorIfParsingFails)
	{
	TLex8 lex(aValue);
	lex.SkipSpace();
	if (!SIPSyntaxCheck::UInt(lex.NextToken(),iValue))
		{
		User::Leave (aErrorIfParsingFails);
		}
	lex.SkipSpace();
	if (lex.Remainder().Length() != 0) 
        {
        User::Leave (aErrorIfParsingFails);
        }
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPUnsignedIntHeaderBase::DoInternalizeValueL(RReadStream& aReadStream)
	{
	iValue = aReadStream.ReadUint32L();
	}

// -----------------------------------------------------------------------------
// CSIPUnsignedIntHeaderBase::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPUnsignedIntHeaderBase::ExternalizeValueL(
    RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iValue);
	}
