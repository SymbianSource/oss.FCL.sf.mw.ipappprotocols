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
// Name          : CSIPSecParam.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "CSIPSecParam.h"

// ----------------------------------------------------------------------------
// CSIPSecParam::NewL
// ----------------------------------------------------------------------------
//
CSIPSecParam* CSIPSecParam::NewL(const TDesC8& aName, const TDesC8& aValue)
	{
	CSIPSecParam* self = CSIPSecParam::NewLC(aName, aValue);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecParam::NewLC
// ----------------------------------------------------------------------------
//
CSIPSecParam* CSIPSecParam::NewLC(const TDesC8& aName, const TDesC8& aValue)
	{
	CSIPSecParam* self = new (ELeave) CSIPSecParam();
	CleanupStack::PushL(self);
	self->ConstructL(aName, aValue);
	return self;
	}

// ----------------------------------------------------------------------------
//	CSIPSecParam::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSecParam::ConstructL(const TDesC8& aName, const TDesC8& aValue)
	{	
	iParamName = aName.AllocL();
	iValue = aValue.AllocL();
	}

// ----------------------------------------------------------------------------
//	CSIPSecParam::~CSIPSecParam
// ----------------------------------------------------------------------------
//	
CSIPSecParam::~CSIPSecParam()
	{
	delete iParamName;
	delete iValue;
	}
	
// ----------------------------------------------------------------------------
// CSIPSecParam::CSIPSecParam
// ----------------------------------------------------------------------------
//
CSIPSecParam::CSIPSecParam()
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPSecParam::UpdateValueL
// ----------------------------------------------------------------------------
//	
void CSIPSecParam::UpdateValueL(const TDesC8& aValue)
	{
	HBufC8* newValue = aValue.AllocL();
	delete iValue;
	iValue = newValue;	
	}

// ----------------------------------------------------------------------------
// CSIPSecParam::ParamName
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecParam::ParamName() const
	{
	return *iParamName;
	}

// ----------------------------------------------------------------------------
// CSIPSecParam::Value
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecParam::Value() const
	{
	return *iValue;
	}
