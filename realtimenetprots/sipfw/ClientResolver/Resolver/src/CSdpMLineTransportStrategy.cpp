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
// Name          : CSdpMLineTransportStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSdpMLineTransportStrategy.h"
#include "sdpmediafield.h"

// ----------------------------------------------------------------------------
// CSdpMLineTransportStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSdpMLineTransportStrategy* CSdpMLineTransportStrategy::NewLC(
    const MSipClients& aSipClients)
	{
	CSdpMLineTransportStrategy* self = 
		new(ELeave)CSdpMLineTransportStrategy(aSipClients);
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpMLineTransportStrategy::CSdpMLineTransportStrategy
// ----------------------------------------------------------------------------
//
CSdpMLineTransportStrategy::CSdpMLineTransportStrategy(
    const MSipClients& aSipClients) 
 : CSdpMLineStrategyBase(aSipClients)
	{
	}

// ----------------------------------------------------------------------------
// CSdpMLineTransportStrategy::~CSdpMLineTransportStrategy
// ----------------------------------------------------------------------------
//
CSdpMLineTransportStrategy::~CSdpMLineTransportStrategy ()
	{
	}

// ----------------------------------------------------------------------------
// CSdpMLineTransportStrategy::FieldsMatchL
// ----------------------------------------------------------------------------
//
TBool CSdpMLineTransportStrategy::FieldsMatchL(
    const CSdpMediaField& aReceivedMedia,
    const CSdpMediaField& aClientMedia)
    {
	if (aReceivedMedia.Media() == aClientMedia.Media() &&
	    aReceivedMedia.Protocol() == aClientMedia.Protocol())
	    {
	    return ETrue;
	    }
	return EFalse;
	}
