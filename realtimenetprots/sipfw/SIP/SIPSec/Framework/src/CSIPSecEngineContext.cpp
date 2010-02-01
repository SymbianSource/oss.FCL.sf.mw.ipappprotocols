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
// Name          : CSIPSecEngineContext.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "CSIPSecEngineContext.h"
#include "TSIPSecParamIter.h"

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::NewL
// ----------------------------------------------------------------------------
//
CSIPSecEngineContext* CSIPSecEngineContext::NewL()
	{
	CSIPSecEngineContext* self = CSIPSecEngineContext::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::NewLC
// ----------------------------------------------------------------------------
//
CSIPSecEngineContext* CSIPSecEngineContext::NewLC()
	{
	CSIPSecEngineContext* self = new (ELeave) CSIPSecEngineContext();
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::CSIPSecEngineContext
// ----------------------------------------------------------------------------
//
CSIPSecEngineContext::CSIPSecEngineContext()
#ifdef CPPUNIT_TEST
	//In unit tests, use array granularity 1 to make every append operation to
	//allocate memory.
  	: iSIPSecParams(1)
#endif
	{	
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::~CSIPSecEngineContext
// ----------------------------------------------------------------------------
//
CSIPSecEngineContext::~CSIPSecEngineContext()
	{	
	iSIPSecParams.ResetAndDestroy();
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::SetValueL
// ----------------------------------------------------------------------------
//	
void CSIPSecEngineContext::SetValueL(const TDesC8& aName, 
									 const TDesC8& aValue)
	{
	CSIPSecParam* sipsecparam = CSIPSecParam::NewLC(aName, aValue);
	iSIPSecParams.AppendL(sipsecparam);
	CleanupStack::Pop(sipsecparam);
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::RemoveValue
// ----------------------------------------------------------------------------
//
void CSIPSecEngineContext::RemoveValue(const TDesC8& aName)
	{
	TSIPSecParamIter paramIter(iSIPSecParams);
	if (FindParam(aName, paramIter))
		{
		paramIter.DeleteCurrent();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::ValueL
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecEngineContext::ValueL(const TDesC8& aName)
	{
	TSIPSecParamIter paramIter(iSIPSecParams);
	
	if (!FindParam(aName, paramIter))
		{
		User::Leave(KErrNotFound);
		}

	CSIPSecParam* secparam = paramIter.Current();
	if (!secparam)
		{
		User::Leave(KErrNotFound);
		}
	return secparam->Value();
	}

// ----------------------------------------------------------------------------
// CSIPSecEngineContext::FindParam
// ----------------------------------------------------------------------------
//
TBool 
CSIPSecEngineContext::FindParam(const TDesC8& aName, TSIPSecParamIter& aParams)
	{
	CSIPSecParam* secparam = aParams.First();
	while (secparam != NULL)
		{
		if (secparam->ParamName().CompareF(aName) == 0)
			{
			return ETrue;
			}

		secparam = aParams.Next();		
		}
	return EFalse;
	}
