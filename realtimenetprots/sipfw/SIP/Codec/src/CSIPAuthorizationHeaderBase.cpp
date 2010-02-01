// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPAuthorizationHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/6.0 
//



#include "sipauthorizationheaderbase.h"
#include "CSIPAuthorizationHeaderParams.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::~CSIPAuthorizationHeaderBase
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAuthorizationHeaderBase::~CSIPAuthorizationHeaderBase()
	{
	delete iParams;
	}
	
// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::CSIPAuthorizationHeaderBase
// ----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderBase::CSIPAuthorizationHeaderBase()
	{
	}	

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAuthorizationHeaderBase::ConstructL()
	{
	iParams = new(ELeave)CSIPAuthorizationHeaderParams;
	}	
	
// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAuthorizationHeaderBase::ConstructL(
    const CSIPAuthorizationHeaderBase& aHeader)
	{
	CSIPAuthHeaderBase::ConstructL(aHeader.AuthScheme());
	iParams = CSIPAuthorizationHeaderParams::NewL(*(aHeader.iParams));
	}	
    
// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPAuthorizationHeaderBase::Params () const
    {
    return *iParams; 
    }

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::Params
// From CSIPParameterHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPAuthorizationHeaderBase::Params ()
    {
    return *iParams; 
    }  

// ----------------------------------------------------------------------------
// CSIPAuthorizationHeaderBase::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPAuthorizationHeaderBase::DoInternalizeValueL(RReadStream& aReadStream)
	{
    HBufC8* authScheme = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    CleanupStack::PushL(authScheme);
    SetAuthSchemeL(*authScheme);
    CleanupStack::PopAndDestroy(authScheme);
	iParams = CSIPAuthorizationHeaderParams::InternalizeL (aReadStream);	
	}
