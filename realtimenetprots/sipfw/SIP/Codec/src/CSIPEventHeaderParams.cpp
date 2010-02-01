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
// Name          : CSIPEventHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPEventHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPEventHeaderParams* CSIPEventHeaderParams::NewL(
    const CSIPEventHeaderParams& aParams)
	{
	CSIPEventHeaderParams* self = CSIPEventHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPEventHeaderParams* CSIPEventHeaderParams::NewLC(
    const CSIPEventHeaderParams& aParams)
	{
	CSIPEventHeaderParams* self = new(ELeave)CSIPEventHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::CSIPEventHeaderParams
// ----------------------------------------------------------------------------
//
CSIPEventHeaderParams::CSIPEventHeaderParams()
 : CSIPParamContainerBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::~CSIPEventHeaderParams
// ----------------------------------------------------------------------------
//
CSIPEventHeaderParams::~CSIPEventHeaderParams()
	{
	}

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::operator==
// ----------------------------------------------------------------------------
//
TBool CSIPEventHeaderParams::operator==(
    const CSIPEventHeaderParams& aParams) const
	{
	// If a Event-header has a "id" parameter the other one
	// has to have it too and their values must match.
	if(this->HasParam(SIPStrings::StringF(SipStrConsts::EId)))
		{
		if(!aParams.HasParam(SIPStrings::StringF(SipStrConsts::EId))) 
			{
			return EFalse;
			}

		if(ParamValue(SIPStrings::StringF(SipStrConsts::EId)) !=
           aParams.ParamValue(SIPStrings::StringF(SipStrConsts::EId))) 
			{
			return EFalse;
			}
		return ETrue;
		}
	else
		{
		return (!aParams.HasParam(SIPStrings::StringF(SipStrConsts::EId)));
		}
	}

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPEventHeaderParams* CSIPEventHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPEventHeaderParams* self = new(ELeave)CSIPEventHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPEventHeaderParams::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPEventHeaderParams::CheckParamL(CSIPParam& aParam) const
	{
	// IdParam
	if (aParam.Name() == SIPStrings::StringF(SipStrConsts::EId))
		{
		if (!SIPSyntaxCheck::Token(aParam.Value().DesC()))
			{
			User::Leave(KErrSipCodecEventHeader);
			}
		return;
		}
    // generic-param
    CheckGenericParamL(aParam, KErrSipCodecEventHeader);
	}
