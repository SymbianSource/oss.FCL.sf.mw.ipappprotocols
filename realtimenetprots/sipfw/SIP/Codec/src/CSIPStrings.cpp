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
// Name          : CSIPStrings.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include <stringpool.h>
#include "CSIPStrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPStrings::NewLC
// ----------------------------------------------------------------------------
//
CSIPStrings* CSIPStrings::NewL()
	{
	CSIPStrings* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPStrings::NewLC
// ----------------------------------------------------------------------------
//
CSIPStrings* CSIPStrings::NewLC()
	{
	CSIPStrings* self = new(ELeave)CSIPStrings;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPStrings::CSIPStrings
// ----------------------------------------------------------------------------
//
CSIPStrings::CSIPStrings()
 : iUsageCount(1)
	{
	}
	
// ----------------------------------------------------------------------------
// CSIPStrings::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPStrings::ConstructL()
	{
	iPool.OpenL(SipStrConsts::Table);
	}

// ----------------------------------------------------------------------------
// CSIPStrings::~CSIPStrings
// ----------------------------------------------------------------------------
//
CSIPStrings::~CSIPStrings()
	{
	iPool.Close();
	}

// -----------------------------------------------------------------------------
// CSIPStrings::IncrementUsageCount
// -----------------------------------------------------------------------------
//
void CSIPStrings::IncrementUsageCount()
	{
	iUsageCount++;
	}

// -----------------------------------------------------------------------------
// CSIPStrings::DecrementUsageCount
// -----------------------------------------------------------------------------
//
void CSIPStrings::DecrementUsageCount()
	{
	iUsageCount--;
	}

// -----------------------------------------------------------------------------
// CSIPStrings::UsageCount
// -----------------------------------------------------------------------------
//
TUint CSIPStrings::UsageCount()
	{
	return iUsageCount;
	}

// ----------------------------------------------------------------------------
// CSIPStrings::StringPool
// ----------------------------------------------------------------------------
//
RStringPool& CSIPStrings::Pool()
	{
	return iPool;
	}

// ----------------------------------------------------------------------------
// CSIPStrings::StringTable
// ----------------------------------------------------------------------------
//
const TStringTable& CSIPStrings::Table()
	{
	return SipStrConsts::Table;
	}

