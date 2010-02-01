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
// Name          : CSIPContactHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPContactHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPContactHeaderParams* CSIPContactHeaderParams::NewL(
    const CSIPContactHeaderParams& aParams)
	{
	CSIPContactHeaderParams* self = CSIPContactHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPContactHeaderParams* CSIPContactHeaderParams::NewLC(
    const CSIPContactHeaderParams& aParams)
	{
	CSIPContactHeaderParams* self = new(ELeave)CSIPContactHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::CSIPContactHeaderParams
// ----------------------------------------------------------------------------
//
CSIPContactHeaderParams::CSIPContactHeaderParams ()
 : CSIPParamContainerBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::~CSIPContactHeaderParams
// ----------------------------------------------------------------------------
//
CSIPContactHeaderParams::~CSIPContactHeaderParams ()
	{
	}

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::operator==
// ----------------------------------------------------------------------------
//
TBool 
CSIPContactHeaderParams::operator==(
                               const CSIPContactHeaderParams& aParams) const
	{
	// If a Contact-header has a "q" or "expires" parameter the other one
	// has to have it too and their values must match. 
	// The extension parameters must match only if they are present in both.
	for (TInt i=0; i < iParams.Count(); i++)
		{
		TInt index = aParams.FindParamIndex(iParams[i]->Name());
		if (index >= 0)
			{
			if (!(*(aParams.iParams[index]) == *(iParams[i]))) 
                {
                return EFalse;
                }
			}
		else
			{
			RStringF name = iParams[i]->Name();
			if (name == SIPStrings::StringF(SipStrConsts::EQ) ||
				name == SIPStrings::StringF(SipStrConsts::EExpires))
				{
				return EFalse;
				}
			}
		}
	for (TInt j=0; j < aParams.iParams.Count(); j++)
		{
		TInt index = FindParamIndex(aParams.iParams[j]->Name());
		if (index >= 0)
			{
			if (!(*(iParams[index]) == *(aParams.iParams[j]))) 
                {
                return EFalse;
                }
			}
		else
			{
			RStringF name = aParams.iParams[j]->Name();
			if (name == SIPStrings::StringF(SipStrConsts::EQ) ||
				name == SIPStrings::StringF(SipStrConsts::EExpires))
				{
				return EFalse;
				}
			}
		}
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPContactHeaderParams* CSIPContactHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPContactHeaderParams* self = new(ELeave)CSIPContactHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPContactHeaderParams::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPContactHeaderParams::CheckParamL (CSIPParam& aParam) const
	{
	RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC();
	// expires
	if (name == SIPStrings::StringF(SipStrConsts::EExpires))
		{
		if (!SIPSyntaxCheck::UInt(value))
			{
			User::Leave(KErrSipCodecContactParams);
			}
		return;
		}
	// q
	if (name == SIPStrings::StringF(SipStrConsts::EQ))
		{
		if (!SIPSyntaxCheck::QValue(value))
			{
			User::Leave(KErrSipCodecContactParams);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecContactParams);
	}
