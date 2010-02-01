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
// Name          : CSIPQueryBase.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include <stringpool.h>
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPQueryBase.h"

// ----------------------------------------------------------------------------
// CSIPQueryBase::CSIPQueryBase
// ----------------------------------------------------------------------------
//
CSIPQueryBase::CSIPQueryBase()
	{
	}

// ----------------------------------------------------------------------------
// CSIPQueryBase::~CSIPQueryBase
// ----------------------------------------------------------------------------
//
CSIPQueryBase::~CSIPQueryBase()
	{
	}

// ----------------------------------------------------------------------------
// CSIPQueryBase::ResultTarget
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPQueryBase::ResultTargetL()
	{
	__ASSERT_ALWAYS (EFalse, User::Leave(KErrNotSupported));
	return KNullDesC8();
	}

// ----------------------------------------------------------------------------
// CSIPQueryBase::ResultProtocol
// ----------------------------------------------------------------------------
//
RStringF CSIPQueryBase::ResultProtocolL() 
	{
	__ASSERT_ALWAYS (EFalse, User::Leave(KErrNotSupported));
	return SIPStrings::StringF( SipStrConsts::EEmpty );
	}

// ----------------------------------------------------------------------------
// CSIPQueryBase::ResultPort
// ----------------------------------------------------------------------------
//
TUint CSIPQueryBase::ResultPortL() 
	{
	__ASSERT_ALWAYS (EFalse, User::Leave(KErrNotSupported));
	return 0;
	}
	
// ----------------------------------------------------------------------------
// CSIPQueryBase::RemoveElement
// ----------------------------------------------------------------------------
//
void CSIPQueryBase::RemoveElementL(TInt /*aIndex*/)
	{
	__ASSERT_ALWAYS (EFalse, User::Leave(KErrNotSupported));
	}

// ----------------------------------------------------------------------------
// CSIPQueryBase::ArrayCount
// ----------------------------------------------------------------------------
//
TInt CSIPQueryBase::ArrayCount()
	{
	return KErrNotSupported;
	}
	
// ----------------------------------------------------------------------------
// CSIPQueryBase::SetTargetL
// ----------------------------------------------------------------------------
//
void CSIPQueryBase::SetTargetL(const TDesC8& /*aTarget*/)
	{
	__ASSERT_ALWAYS (EFalse, User::Leave(KErrNotSupported));
	}
