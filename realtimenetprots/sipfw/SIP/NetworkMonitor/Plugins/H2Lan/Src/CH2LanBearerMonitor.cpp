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
// Name        : CH2LanBearerMonitor.cpp
// Part of     : NetworkMonitor
// Implementation.
// Version     : SIP/4.0
//



#include "CH2LanBearerMonitor.h"
#include <sipbearerowner.h>
#include <sipnetworkinfoobserver.h>
#include <sipaccessnetworkinfo.h>

// -----------------------------------------------------------------------------
// CH2LanBearerMonitor::NewL
// -----------------------------------------------------------------------------
//
CH2LanBearerMonitor* CH2LanBearerMonitor::NewL( TAny* aParams )
	{
	CH2LanBearerMonitor* self = new( ELeave ) CH2LanBearerMonitor( aParams );

	CleanupClosePushL( *self ); 
	self->ConstructL();
	CleanupStack::Pop(); // self

	return self;
	}

// -----------------------------------------------------------------------------
// CH2LanBearerMonitor::~CH2LanBearerMonitor
// -----------------------------------------------------------------------------
//
CH2LanBearerMonitor::~CH2LanBearerMonitor()
	{
	}

// -----------------------------------------------------------------------------
// CH2LanBearerMonitor::MonitorStateChanged
// -----------------------------------------------------------------------------
//
void CH2LanBearerMonitor::MonitorStateChanged( TInt aError )
	{
	iParent.BearerStateChanged( this, aError );
	}

// -----------------------------------------------------------------------------
// CH2LanBearerMonitor::CH2LanBearerMonitor
// -----------------------------------------------------------------------------
//
CH2LanBearerMonitor::CH2LanBearerMonitor( TAny* aParams )
	: CSIPBearerMonitor( aParams )
	{
	iState = MSIPNetworkObserver::ENetworkActive;
	}

// -----------------------------------------------------------------------------
// CH2LanBearerMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CH2LanBearerMonitor::ConstructL()
	{
	// Generate dummy access network info and give it to observer
	_LIT( KDummyCountryCode, "12" );
    _LIT( KDummyNetworkCode, "345" );
    const TUint KDummyLocationAreaCode = 0x6789;
    const TUint KDummyCellId = 0xABCD;
    
    TSIPAccessNetworkInfo accessNetworkInfo;
	
    accessNetworkInfo.iNetworkMode = RMobilePhone::ENetworkModeGsm; 
    accessNetworkInfo.iMobileCountryCode.Copy( KDummyCountryCode );
    accessNetworkInfo.iMobileNetworkCode.Copy( KDummyNetworkCode );
    accessNetworkInfo.iAreaKnown = ETrue;
    accessNetworkInfo.iLocationAreaCode = KDummyLocationAreaCode;
    accessNetworkInfo.iCellId = KDummyCellId;
	
	iNetworkInfoObserver.InfoChanged( accessNetworkInfo );
	}

