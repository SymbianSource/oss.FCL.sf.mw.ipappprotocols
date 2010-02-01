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
// Name          : CSIPContentTypeHeaderParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPContentTypeHeaderParams.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"

// -----------------------------------------------------------------------------
// CSIPContentTypeHeaderParams::NewL
// -----------------------------------------------------------------------------
//
CSIPContentTypeHeaderParams* CSIPContentTypeHeaderParams::NewL(
    const CSIPContentTypeHeaderParams& aParams)
	{
	CSIPContentTypeHeaderParams* self = 
	    CSIPContentTypeHeaderParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeaderParams::NewLC
// -----------------------------------------------------------------------------
//
CSIPContentTypeHeaderParams* CSIPContentTypeHeaderParams::NewLC(
    const CSIPContentTypeHeaderParams& aParams)
	{
	CSIPContentTypeHeaderParams* self = new(ELeave)CSIPContentTypeHeaderParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPContentTypeHeaderParams::CSIPContentTypeHeaderParams
// -----------------------------------------------------------------------------
//
CSIPContentTypeHeaderParams::CSIPContentTypeHeaderParams()
 : CSIPParamContainerBase(';')
	{
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeaderParams::~CSIPContentTypeHeaderParams
// -----------------------------------------------------------------------------
//
CSIPContentTypeHeaderParams::~CSIPContentTypeHeaderParams()
	{
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeaderParams::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPContentTypeHeaderParams* CSIPContentTypeHeaderParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPContentTypeHeaderParams* self = new(ELeave)CSIPContentTypeHeaderParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPContentTypeHeaderParams::CheckParamL
// -----------------------------------------------------------------------------
//
void CSIPContentTypeHeaderParams::CheckParamL(CSIPParam& aParam) const
	{
	// m-parameter must always have a value 
	if (!aParam.HasValue())
		{
		User::Leave(KErrSipCodecContentTypeHeader);
		}
	// m-attribute (token)
	if (!SIPSyntaxCheck::Token(aParam.Name().DesC()))
		{
		User::Leave(KErrSipCodecContentTypeHeader);
		}
	// m-value (token/quoted-string)
	TPtrC8 value(aParam.Value().DesC());
	if (!SIPSyntaxCheck::Token(value) &&
	    !SIPSyntaxCheck::QuotedString(value))
		{
		User::Leave(KErrSipCodecContentTypeHeader);
		}
	}
