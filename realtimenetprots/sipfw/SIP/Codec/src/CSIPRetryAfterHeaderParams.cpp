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
// Name          : CSIPRetryAfterHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPRetryAfterHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeaderParams* CSIPRetryAfterHeaderParams::NewL(
    const CSIPRetryAfterHeaderParams& aParams)
	{
	CSIPRetryAfterHeaderParams* self = 
	    CSIPRetryAfterHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeaderParams* CSIPRetryAfterHeaderParams::NewLC(
    const CSIPRetryAfterHeaderParams& aParams)
	{
	CSIPRetryAfterHeaderParams* self = new(ELeave)CSIPRetryAfterHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeaderParams::CSIPRetryAfterHeaderParams
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeaderParams::CSIPRetryAfterHeaderParams()
 : CSIPParamContainerBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeaderParams::~CSIPRetryAfterHeaderParams
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeaderParams::~CSIPRetryAfterHeaderParams()
	{
	}

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeaderParams* CSIPRetryAfterHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPRetryAfterHeaderParams* self = new(ELeave)CSIPRetryAfterHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeaderParams::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPRetryAfterHeaderParams::CheckParamL(CSIPParam& aParam) const
	{
	// duration
	if (aParam.Name() == SIPStrings::StringF(SipStrConsts::EDuration))
		{
		if (!SIPSyntaxCheck::UInt(aParam.Value().DesC()))
			{
			User::Leave(KErrSipCodecRetryAfterParams);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecRetryAfterParams);
	}
