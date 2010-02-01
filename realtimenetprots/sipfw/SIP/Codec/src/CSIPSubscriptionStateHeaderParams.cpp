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
// Name          : CSIPSubscriptionStateHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "CSIPSubscriptionStateHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPSubscriptionStateHeaderParams* CSIPSubscriptionStateHeaderParams::NewL(
    const CSIPSubscriptionStateHeaderParams& aParams)
	{
	CSIPSubscriptionStateHeaderParams* self = 
        CSIPSubscriptionStateHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPSubscriptionStateHeaderParams* CSIPSubscriptionStateHeaderParams::NewLC(
    const CSIPSubscriptionStateHeaderParams& aParams)
	{
	CSIPSubscriptionStateHeaderParams* self = 
		new(ELeave)CSIPSubscriptionStateHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderParams::CSIPSubscriptionStateHeaderParams
// ----------------------------------------------------------------------------
//
CSIPSubscriptionStateHeaderParams::CSIPSubscriptionStateHeaderParams()
 : CSIPParamContainerBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderParams::~CSIPSubscriptionStateHeaderParams
// ----------------------------------------------------------------------------
//
CSIPSubscriptionStateHeaderParams::~CSIPSubscriptionStateHeaderParams()
	{
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPSubscriptionStateHeaderParams* 
CSIPSubscriptionStateHeaderParams::InternalizeL(RReadStream& aReadStream)
	{
	CSIPSubscriptionStateHeaderParams* self = 
		new(ELeave)CSIPSubscriptionStateHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSubscriptionStateHeaderParams::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPSubscriptionStateHeaderParams::CheckParamL(CSIPParam& aParam) const
	{
    RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC();
	// ReasonParam    
	if (name == SIPStrings::StringF(SipStrConsts::EReason))
		{
		if (!SIPSyntaxCheck::Token(value))
			{
			User::Leave(KErrSipCodecSubscriptionStateHeader);
			}
		return;
		}
	// expires
	if (name == SIPStrings::StringF(SipStrConsts::EExpires))
		{
		if (!SIPSyntaxCheck::UInt(value))
			{
			User::Leave(KErrSipCodecSubscriptionStateHeader);
			}
		return;
		}
	// retry-after
	if (name == SIPStrings::StringF(SipStrConsts::ERetryAfter))
		{
		if (!SIPSyntaxCheck::UInt(value))
			{
			User::Leave(KErrSipCodecSubscriptionStateHeader);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecSubscriptionStateHeader);
	}
