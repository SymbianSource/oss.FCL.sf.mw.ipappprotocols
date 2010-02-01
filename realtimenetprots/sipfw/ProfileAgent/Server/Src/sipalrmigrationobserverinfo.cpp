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
// Name          : sipalrmigrationobserverinfo.cpp
// Part of       : SIP / SIP Profile Server
// Version       : SIP/6.0 
//



#include "sipalrmigrationobserverinfo.h"

// -----------------------------------------------------------------------------
// TSipAlrMigrationObserverInfo::TSipAlrMigrationObserverInfo
// -----------------------------------------------------------------------------
//
TSipAlrMigrationObserverInfo::TSipAlrMigrationObserverInfo(
	MSipAlrMigrationObserver& aObserver) :
	iObserver(aObserver),
	iIsWaitingForPermission(EFalse),
	iIsWaitingForAcceptance(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// TSipAlrMigrationObserverInfo::CompareObservers
// -----------------------------------------------------------------------------
//
TBool TSipAlrMigrationObserverInfo::CompareObservers(
	const TSipAlrMigrationObserverInfo& aItem,
	const TSipAlrMigrationObserverInfo& aItem2)
	{
	return &aItem.iObserver == &aItem2.iObserver;
	}
