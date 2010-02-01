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
// Name          : CSIPFromToHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPFromToHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::NewL
// -----------------------------------------------------------------------------
//
CSIPFromToHeaderParams* CSIPFromToHeaderParams::NewL(
    const CSIPFromToHeaderParams& aParams)
	{
	CSIPFromToHeaderParams* self = CSIPFromToHeaderParams::NewLC (aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPFromToHeaderParams* CSIPFromToHeaderParams::NewLC(
    const CSIPFromToHeaderParams& aParams)
	{
	CSIPFromToHeaderParams* self = new(ELeave)CSIPFromToHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::CSIPFromToHeaderParams
// -----------------------------------------------------------------------------
//
CSIPFromToHeaderParams::CSIPFromToHeaderParams ()
 : CSIPParamContainerBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::~CSIPFromToHeaderParams
// -----------------------------------------------------------------------------
//
CSIPFromToHeaderParams::~CSIPFromToHeaderParams ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPFromToHeaderParams::operator==(
    const CSIPFromToHeaderParams& aParams) const
	{
	// If a From/To-header has a "tag" parameter the other one
	// has to have it too and their values must match. 
	// The extension parameters must match only if they are present in both.
	for (TInt i=0; i < iParams.Count(); i++)
		{
		RStringF name = iParams[i]->Name();
		TInt index = aParams.FindParamIndex(name);
		if (index >= 0)
			{
			if (!(*(aParams.iParams[index]) == *(iParams[i])))
				{
				return EFalse;
				}
			}
		else
			{
			if (name == SIPStrings::StringF(SipStrConsts::ETag))
				{
				return EFalse;
				}
			}
		}
	for (TInt j=0; j < aParams.iParams.Count(); j++)
		{
		RStringF name = aParams.iParams[j]->Name();
		TInt index = FindParamIndex(name);
		if (index >= 0)
			{
			if (!(*(iParams[index]) == *(aParams.iParams[j])))
				{
				return EFalse;
				}
			}
		else
			{
			if (name == SIPStrings::StringF(SipStrConsts::ETag))
				{
				return EFalse;
				}
			}
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPFromToHeaderParams* CSIPFromToHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPFromToHeaderParams* self = new(ELeave)CSIPFromToHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPFromToHeaderParams::CheckParamL
// -----------------------------------------------------------------------------
//
void CSIPFromToHeaderParams::CheckParamL(CSIPParam& aParam) const
	{
	// tag
	if (aParam.Name() == SIPStrings::StringF(SipStrConsts::ETag))
		{
		if (!SIPSyntaxCheck::Token(aParam.Value().DesC()))
			{
			User::Leave(KErrSipCodecFromOrToParams);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecFromOrToParams);
	}
