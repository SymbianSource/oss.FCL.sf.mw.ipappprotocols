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
// Name          : CSIPViaHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "CSIPViaHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::DecodeL
// -----------------------------------------------------------------------------
//
CSIPViaHeaderParams* CSIPViaHeaderParams::DecodeL(const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecViaHeader));

	CSIPViaHeaderParams* params = new(ELeave)CSIPViaHeaderParams;
	CleanupStack::PushL (params);
	params->ParseL(aValue);
	CleanupStack::Pop(params);
	return params;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::NewL
// -----------------------------------------------------------------------------
//
CSIPViaHeaderParams* CSIPViaHeaderParams::NewL(
    const CSIPViaHeaderParams& aParams)
	{
	CSIPViaHeaderParams* self = CSIPViaHeaderParams::NewLC (aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPViaHeaderParams* CSIPViaHeaderParams::NewLC(
    const CSIPViaHeaderParams& aParams)
	{
	CSIPViaHeaderParams* self = new(ELeave)CSIPViaHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::CSIPViaHeaderParams
// -----------------------------------------------------------------------------
//
CSIPViaHeaderParams::CSIPViaHeaderParams ()
 : CSIPParamContainerBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::~CSIPViaHeaderParams
// -----------------------------------------------------------------------------
//
CSIPViaHeaderParams::~CSIPViaHeaderParams ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPViaHeaderParams::operator==(const CSIPViaHeaderParams& aParams) const
	{
	// All parameters in a Via-header must be present in the other one
	// and their values must match.
	for (TInt i=0; i < iParams.Count(); i++)
		{
		TInt index = aParams.FindParamIndex(iParams[i]->Name());
		if (index < 0)
			{
			return EFalse;
			}
		if (!(*(aParams.iParams[index]) == *(iParams[i])))
			{
			return EFalse;
			}
		}
	for (TInt j=0; j < aParams.iParams.Count(); j++)
		{
		TInt index = FindParamIndex(aParams.iParams[j]->Name());
		if (index < 0)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPViaHeaderParams::CheckParamL
// -----------------------------------------------------------------------------
//
void CSIPViaHeaderParams::CheckParamL (CSIPParam& aParam) const
	{
	RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC(); 	
	// ttl
	if (name == SIPStrings::StringF(SipStrConsts::ETtl))
		{
		if (!SIPSyntaxCheck::TtlValue(value))
			{
			User::Leave(KErrSipCodecViaHeader);
			}
		return;
		}
	// maddr
	if (name == SIPStrings::StringF(SipStrConsts::EMaddr))
		{
		if (!SIPSyntaxCheck::Host(value))
			{
			User::Leave(KErrSipCodecViaHeader);
			}
		return;
		}
	// received
	if (name == SIPStrings::StringF(SipStrConsts::EReceived))
		{
		CSIPHostPort::TType hostType;
		if (SIPSyntaxCheck::HostType(value,hostType) != KErrNone)
			{
			User::Leave(KErrSipCodecViaHeader);
			}
		if (!(hostType == CSIPHostPort::ESIPIpv4 || 
		      hostType == CSIPHostPort::ESIPIpv6))
			{
			User::Leave(KErrSipCodecViaHeader);
			}
		return;
		}
	// branch
	if (name == SIPStrings::StringF(SipStrConsts::EBranch))
		{
		if (!SIPSyntaxCheck::Token(value))
			{
			User::Leave(KErrSipCodecViaHeader);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecViaHeader);
	}
