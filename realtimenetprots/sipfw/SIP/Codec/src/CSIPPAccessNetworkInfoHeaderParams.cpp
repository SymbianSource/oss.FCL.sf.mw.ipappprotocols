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
// Name          : CSIPPAccessNetworkInfoHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPPAccessNetworkInfoHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// -----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeaderParams::NewL
// -----------------------------------------------------------------------------
//
CSIPPAccessNetworkInfoHeaderParams* CSIPPAccessNetworkInfoHeaderParams::NewL(
    const CSIPPAccessNetworkInfoHeaderParams& aParams)
	{
	CSIPPAccessNetworkInfoHeaderParams* self = 
        CSIPPAccessNetworkInfoHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeaderParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPPAccessNetworkInfoHeaderParams* CSIPPAccessNetworkInfoHeaderParams::NewLC(
    const CSIPPAccessNetworkInfoHeaderParams& aParams)
	{
	CSIPPAccessNetworkInfoHeaderParams* self = 
        new(ELeave)CSIPPAccessNetworkInfoHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeaderParams::CSIPPAccessNetworkInfoHeaderParams
// -----------------------------------------------------------------------------
//
CSIPPAccessNetworkInfoHeaderParams::CSIPPAccessNetworkInfoHeaderParams()
 : CSIPParamContainerBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeaderParams::~CSIPPAccessNetworkInfoHeaderParams
// -----------------------------------------------------------------------------
//
CSIPPAccessNetworkInfoHeaderParams::~CSIPPAccessNetworkInfoHeaderParams()
	{
	}

// ----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeaderParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPPAccessNetworkInfoHeaderParams* 
CSIPPAccessNetworkInfoHeaderParams::InternalizeL (RReadStream& aReadStream)
	{
	CSIPPAccessNetworkInfoHeaderParams* self = 
		new(ELeave)CSIPPAccessNetworkInfoHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPPAccessNetworkInfoHeaderParams::CheckParamL
// -----------------------------------------------------------------------------
//
void CSIPPAccessNetworkInfoHeaderParams::CheckParamL (CSIPParam& aParam) const
	{
    RStringF name = aParam.Name();
	TPtrC8 value = aParam.Value().DesC();
	// cgi-3gpp / utran-cell-id-3gpp
	if(name == SIPStrings::StringF(SipStrConsts::ECgi3gpp) ||
       name == SIPStrings::StringF(SipStrConsts::EUtranCellId3gpp))
		{
        // token/quoted-string
        if(!SIPSyntaxCheck::Token(value) &&
           !SIPSyntaxCheck::QuotedString(value))
			{
			User::Leave(KErrSipCodecPAccessNetworkInfoHeader);
			}
		return;
		}

    // extension-access-info
    // token / host / quoted-string
	if(!SIPSyntaxCheck::Token(name.DesC()) &&
       !SIPSyntaxCheck::QuotedString(name.DesC()) &&
       !SIPSyntaxCheck::Host(name.DesC()))
		{
		User::Leave(KErrSipCodecPAccessNetworkInfoHeader);
		}
	}
