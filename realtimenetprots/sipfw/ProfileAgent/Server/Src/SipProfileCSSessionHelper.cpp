// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofilecssessionhelper
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include <s32mem.h>
#include "sipprofilecs.h"
#include "SipProfileCSServer.h"
#include "SipProfileCSSessionHelper.h"
#include "sipconcreteprofile.h"
#include "sipconcreteprofileholder.h"
#include "sipprofileslots.h"
#include "sipprofileplugins.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::ServiceFunction
// -----------------------------------------------------------------------------
//
int CSIPProfileCSSessionHelper::ServiceFunction(const RMessage2& aMessage) const
	{
	return aMessage.Function();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::CompleteService
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSessionHelper::CompleteService(
	const RMessage2& aMessage, TInt aCode) const
	{
	aMessage.Complete(aCode);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::IPCArgTUintL
// -----------------------------------------------------------------------------
//
TUint CSIPProfileCSSessionHelper::IPCArgTUintL(
	TSipProfileItcArguments aIpcArgIndex,
    const RMessage2&  aMessage) const
	{
    TUint value(0);
    switch (static_cast<TInt>(aIpcArgIndex))
		{
        case 0: value = aMessage.Int0(); break;
        case 1: value = aMessage.Int1(); break;
        case 2: value = aMessage.Int2(); break;
        case 3: value = aMessage.Int3(); break;
        default: User::Leave(KErrArgument); break;
		}
    return value; 
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::ReadLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPProfileCSSessionHelper::ReadLC(
	TSipProfileItcArguments aItcArgIndex,
	const RMessage2&  aMessage) const
	{
    TInt length = aMessage.GetDesLength(aItcArgIndex);
    __ASSERT_ALWAYS(length >= 0, User::Leave(KErrArgument));

    HBufC8* buf = HBufC8::NewLC(length);
	TPtr8 bufPtr(buf->Des());
    aMessage.ReadL(aItcArgIndex, bufPtr);
    
    return buf;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::ReadSipIdsL
// -----------------------------------------------------------------------------
//
TSIPProfileSlots CSIPProfileCSSessionHelper::ReadSipIdsL(
	const RMessage2& aMessage) const
	{
    TPckgBuf<TSIPProfileSlots> sipIdsPckg;
    aMessage.ReadL(ESipProfileItcArgSlots, sipIdsPckg);
    return sipIdsPckg();
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::WriteL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSessionHelper::WriteL(
	const RMessage2& aMessage,
    const TSIPProfileSlots& aIds)
	{
    TPckgBuf<TSIPProfileSlots> sipIdsPckg(aIds);
    WriteL(ESipProfileItcArgSlots, aMessage, sipIdsPckg);
	}

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::WriteL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSessionHelper::WriteL(TSipProfileItcArguments aItcArgIndex,
	const RMessage2&  aMessage,
	const TDesC8&    aDes)
	{
    TInt length = aMessage.GetDesMaxLength(aItcArgIndex);
    __ASSERT_ALWAYS(length >= aDes.Length(), User::Leave (KErrOverflow));
    aMessage.WriteL (aItcArgIndex, aDes);
    }

// -----------------------------------------------------------------------------
// CSIPProfileCSSessionHelper::ReadSIPProfileTypeInfoL
// -----------------------------------------------------------------------------
//
void CSIPProfileCSSessionHelper::ReadSIPProfileTypeInfoL(
    const RMessage2& aMessage,
    TSIPProfileTypeInfo& aTypeInfo) const
    {
	HBufC8* type = ReadLC(ESipProfileItcArgNarrator,aMessage);

	__ASSERT_ALWAYS(type->Length() <= aTypeInfo.iSIPProfileName.MaxLength(),
	                User::Leave(KErrArgument));

	aTypeInfo.iSIPProfileName = *type;
	aTypeInfo.iSIPProfileClass = 
	    static_cast<TSIPProfileTypeInfo::TSIPProfileClass>(
	        IPCArgTUintL(ESipProfileItcArgType, aMessage));
		
	CleanupStack::PopAndDestroy(type);	    
    }
