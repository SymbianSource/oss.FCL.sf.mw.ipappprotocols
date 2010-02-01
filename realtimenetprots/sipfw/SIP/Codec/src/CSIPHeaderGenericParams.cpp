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
// Name          : CSIPHeaderGenericParams.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "CSIPHeaderGenericParams.h"
#include "sipcodecerr.h"

// ----------------------------------------------------------------------------
// CSIPHeaderGenericParams::NewL
// ----------------------------------------------------------------------------
//
CSIPHeaderGenericParams* CSIPHeaderGenericParams::NewL(
    const CSIPHeaderGenericParams& aParams)
	{
	CSIPHeaderGenericParams* self = CSIPHeaderGenericParams::NewLC(aParams);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPHeaderGenericParams::NewLC
// ----------------------------------------------------------------------------
//
CSIPHeaderGenericParams* CSIPHeaderGenericParams::NewLC(
    const CSIPHeaderGenericParams& aParams)
	{
	CSIPHeaderGenericParams* self = new(ELeave)CSIPHeaderGenericParams;
	CleanupStack::PushL(self);
	self->ConstructL(aParams);
	return self;
	}	

// ----------------------------------------------------------------------------
// CSIPHeaderGenericParams::CSIPHeaderGenericParams
// ----------------------------------------------------------------------------
//
CSIPHeaderGenericParams::CSIPHeaderGenericParams()
 : CSIPParamContainerBase(';')
	{
	}

// ----------------------------------------------------------------------------
// CSIPHeaderGenericParams::~CSIPHeaderGenericParams
// ----------------------------------------------------------------------------
//
CSIPHeaderGenericParams::~CSIPHeaderGenericParams()
	{
	}

// ----------------------------------------------------------------------------
// CSIPHeaderGenericParams::InternalizeL
// ----------------------------------------------------------------------------
//
CSIPHeaderGenericParams* CSIPHeaderGenericParams::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPHeaderGenericParams* self = new(ELeave)CSIPHeaderGenericParams;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPHeaderGenericParams::CheckParamL
// ----------------------------------------------------------------------------
//
void CSIPHeaderGenericParams::CheckParamL (CSIPParam& aParam) const
	{
    CheckGenericParamL(aParam, KErrSipCodecAnyParam);
	}
