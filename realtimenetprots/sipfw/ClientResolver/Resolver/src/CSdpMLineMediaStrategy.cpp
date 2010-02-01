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
// Name          : CSdpMLineMediaStrategy.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



#include "CSdpMLineMediaStrategy.h"
#include "sdpmediafield.h"

// ----------------------------------------------------------------------------
// CSdpMLineMediaStrategy::NewLC
// ----------------------------------------------------------------------------
//
CSdpMLineMediaStrategy* CSdpMLineMediaStrategy::NewLC(
    const MSipClients& aSipClients)
	{
	CSdpMLineMediaStrategy* self = 
	    new(ELeave)CSdpMLineMediaStrategy(aSipClients);
	CleanupStack::PushL(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSdpMLineMediaStrategy::CSdpMLineMediaStrategy
// ----------------------------------------------------------------------------
//
CSdpMLineMediaStrategy::CSdpMLineMediaStrategy(const MSipClients& aSipClients) 
 : CSdpMLineStrategyBase(aSipClients)
	{
	}

// ----------------------------------------------------------------------------
// CSdpMLineMediaStrategy::~CSdpMLineMediaStrategy
// ----------------------------------------------------------------------------
//
CSdpMLineMediaStrategy::~CSdpMLineMediaStrategy ()
	{
	}

// ----------------------------------------------------------------------------
// CSdpMLineMediaStrategy::FieldsMatchL
// ----------------------------------------------------------------------------
//
TBool CSdpMLineMediaStrategy::FieldsMatchL(
    const CSdpMediaField& aReceivedMedia,
    const CSdpMediaField& aClientMedia)
    {
	return (aReceivedMedia.Media() == aClientMedia.Media());
	}
