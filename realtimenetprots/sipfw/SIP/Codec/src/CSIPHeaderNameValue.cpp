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
// Name          : CSIPHeaderNameValue.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPHeaderNameValue.h"
#include "sipcodecerr.h"
#include "sipstrings.h"

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::NewL
// -----------------------------------------------------------------------------
//
CSIPHeaderNameValue* CSIPHeaderNameValue::NewL(
    const TDesC8& aName,
    const TPtrC8 aValue)
	{
	CSIPHeaderNameValue* self = CSIPHeaderNameValue::NewLC(aName,aValue);
	CleanupStack::Pop(self);
	return self;
	} 

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::NewLC
// -----------------------------------------------------------------------------
//
CSIPHeaderNameValue* CSIPHeaderNameValue::NewLC(
    const TDesC8& aName,const TPtrC8 aValue)
	{
    __ASSERT_ALWAYS(aName.Length() > 0, User::Leave(KErrSipCodecHeaderName));
	CSIPHeaderNameValue* self = new(ELeave)CSIPHeaderNameValue(aValue);
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPHeaderNameValue::ConstructL(const TDesC8& aName)
	{
	TPtrC8 name(ParseNameL(aName));
	iName = SIPStrings::Pool().OpenFStringL(name);
	}

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::CSIPHeaderNameValue
// -----------------------------------------------------------------------------
//
CSIPHeaderNameValue::CSIPHeaderNameValue(const TPtrC8 aValue)
: iValue (aValue)
	{
	}

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::~CSIPHeaderNameValue
// -----------------------------------------------------------------------------
//
CSIPHeaderNameValue::~CSIPHeaderNameValue()
	{
	iName.Close();
	}

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::Name
// -----------------------------------------------------------------------------
//
RStringF CSIPHeaderNameValue::Name() 
	{
	return iName;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::Value
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPHeaderNameValue::Value() 
	{
	return iValue; 
	}

// -----------------------------------------------------------------------------
// CSIPHeaderNameValue::ParseNameL
// -----------------------------------------------------------------------------
//
TPtrC8 CSIPHeaderNameValue::ParseNameL(const TDesC8& aName)
	{
	TLex8 lex(aName);
	TPtrC8 name(lex.NextToken());
	if (name.Length() == 0)
		{
		User::Leave (KErrSipCodecHeaderName);
		}
	lex.SkipSpace();
	if (lex.Remainder().Length() > 0) 
		{
		User::Leave (KErrSipCodecHeaderName);
		}
	return name;
	}
