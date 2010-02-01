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
// Name          : CSdpMLineNameStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSdpMLineNameStrategy.h"
#include "sdpdocument.h"
#include "sdpmediafield.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecstringpool.h"

// ----------------------------------------------------------------------------
// CSdpMLineNameStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSdpMLineNameStrategy* CSdpMLineNameStrategy::NewLC(
    const MSipClients& aSipClients)
	{
	CSdpMLineNameStrategy* self = 
		new(ELeave)CSdpMLineNameStrategy(aSipClients);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpMLineNameStrategy::CSdpMLineNameStrategy
// ----------------------------------------------------------------------------
//
CSdpMLineNameStrategy::CSdpMLineNameStrategy(const MSipClients& aSipClients) 
 : CSdpMLineStrategyBase(aSipClients)
	{
	}

// ----------------------------------------------------------------------------
// CSdpMLineNameStrategy::ConstructL
// ----------------------------------------------------------------------------
//
void CSdpMLineNameStrategy::ConstructL()
	{
	iApplicationStringF = 
	    SdpCodecStringPool::StringPoolL().StringF(
	        SdpCodecStringConstants::EMediaApplication,
	        SdpCodecStringPool::StringTableL());
	}

// ----------------------------------------------------------------------------
// CSdpMLineNameStrategy::~CSdpMLineNameStrategy
// ----------------------------------------------------------------------------
//
CSdpMLineNameStrategy::~CSdpMLineNameStrategy ()
	{
	iApplicationStringF.Close();
	}

// ----------------------------------------------------------------------------
// CSdpMLineNameStrategy::IsApplicable
// ----------------------------------------------------------------------------
//	
TBool CSdpMLineNameStrategy::IsApplicable(CSdpMediaField& aMediaField)
    {
 	return (aMediaField.Media() == iApplicationStringF);
    }

// ----------------------------------------------------------------------------
// CSdpMLineNameStrategy::FieldsMatchL
// ----------------------------------------------------------------------------
//
TBool CSdpMLineNameStrategy::FieldsMatchL(
    const CSdpMediaField& aReceivedMedia,
    const CSdpMediaField& aClientMedia)
    {    
	if (aReceivedMedia.Media() == iApplicationStringF &&
	    aClientMedia.Media() == iApplicationStringF &&
	    aReceivedMedia.FormatList() == aClientMedia.FormatList())
	    {
	    return ETrue;
	    }
	return EFalse;
	}
