/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// CLASS HEADER
#include "CMSRPStrings.h"

// EXTERNAL INCLUDES
#include <MSRPStrConsts.h>

// ----------------------------------------------------------------------------
// CMSRPStrings::NewLC
// ----------------------------------------------------------------------------
//
CMSRPStrings* CMSRPStrings::NewL()
	{
	CMSRPStrings* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CMSRPStrings::NewLC
// ----------------------------------------------------------------------------
//
CMSRPStrings* CMSRPStrings::NewLC()
	{
	CMSRPStrings* self = new(ELeave)CMSRPStrings;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CMSRPStrings::CMSRPStrings
// ----------------------------------------------------------------------------
//
CMSRPStrings::CMSRPStrings()
 : iUsageCount(1)
	{
	}

// ----------------------------------------------------------------------------
// CMSRPStrings::ConstructL
// ----------------------------------------------------------------------------
//
void CMSRPStrings::ConstructL()
	{
	iPool.OpenL( MSRPStrConsts::Table );
	}

// ----------------------------------------------------------------------------
// CMSRPStrings::~CMSRPStrings
// ----------------------------------------------------------------------------
//
CMSRPStrings::~CMSRPStrings()
	{
	iPool.Close();
	}

// -----------------------------------------------------------------------------
// CMSRPStrings::IncrementUsageCount
// -----------------------------------------------------------------------------
//
void CMSRPStrings::IncrementUsageCount()
	{
	iUsageCount++;
	}

// -----------------------------------------------------------------------------
// CMSRPStrings::DecrementUsageCount
// -----------------------------------------------------------------------------
//
void CMSRPStrings::DecrementUsageCount()
	{
	iUsageCount--;
	}

// -----------------------------------------------------------------------------
// CMSRPStrings::UsageCount
// -----------------------------------------------------------------------------
//
TUint CMSRPStrings::UsageCount() const
	{
	return iUsageCount;
	}

// ----------------------------------------------------------------------------
// CMSRPStrings::StringPool
// ----------------------------------------------------------------------------
//
const RStringPool& CMSRPStrings::Pool() const
	{
	return iPool;
	}

// ----------------------------------------------------------------------------
// CMSRPStrings::StringTable
// ----------------------------------------------------------------------------
//
const TStringTable& CMSRPStrings::Table()
	{
	return MSRPStrConsts::Table;
	}

// End of File
