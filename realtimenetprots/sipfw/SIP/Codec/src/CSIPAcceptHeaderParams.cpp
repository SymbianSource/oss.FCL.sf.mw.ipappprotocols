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
// Name          : CSIPAcceptHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPAcceptHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// ----------------------------------------------------------------------------
// CSIPAcceptHeaderParams::NewL
// ----------------------------------------------------------------------------
//
CSIPAcceptHeaderParams* CSIPAcceptHeaderParams::NewL (
    const CSIPAcceptHeaderParams& aParams)
	{
	CSIPAcceptHeaderParams* self = CSIPAcceptHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeaderParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPAcceptHeaderParams* CSIPAcceptHeaderParams::NewLC (
    const CSIPAcceptHeaderParams& aParams)
	{
	CSIPAcceptHeaderParams* self = new(ELeave)CSIPAcceptHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPAcceptHeaderParams::CSIPAcceptHeaderParams
// ----------------------------------------------------------------------------
//
CSIPAcceptHeaderParams::CSIPAcceptHeaderParams ()
 : CSIPParamContainerBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeaderParams::~CSIPAcceptHeaderParams
// ----------------------------------------------------------------------------
//
CSIPAcceptHeaderParams::~CSIPAcceptHeaderParams ()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPAcceptHeaderParams* CSIPAcceptHeaderParams::InternalizeL (
    RReadStream& aReadStream)
	{
	CSIPAcceptHeaderParams* self = new(ELeave)CSIPAcceptHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAcceptHeaderParams::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPAcceptHeaderParams::CheckParamL (CSIPParam& aParam) const
	{    
    RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC();
	// q
	if (name == SIPStrings::StringF(SipStrConsts::EQ))
		{
		if (!SIPSyntaxCheck::QValue(value))
			{
			User::Leave(KErrSipCodecAcceptParams);
			}
		return;
		}

	// m-attribute (token)
	if (!SIPSyntaxCheck::Token(name.DesC()))
		{
		User::Leave(KErrSipCodecAcceptParams);
		}

	if (!aParam.HasValue()) 
        {
        return;
        }

	// m-value or accept-param (token/quoted-string)
	if (!SIPSyntaxCheck::Token(value))
		{
		if (!SIPSyntaxCheck::QuotedString(value))
			{
			if (!SIPSyntaxCheck::GenericParamValue(value))
				{
				User::Leave(KErrSipCodecAcceptParams);
				}
			}
		}	
	}
