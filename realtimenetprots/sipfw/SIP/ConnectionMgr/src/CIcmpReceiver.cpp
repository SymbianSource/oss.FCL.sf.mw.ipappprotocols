// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CIcmpReceiver.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0
//



#include "CIcmpReceiver.h"
#include "CIcmpV4Receiver.h"
#include "CIcmpV6Receiver.h"


// -----------------------------------------------------------------------------
// CIcmpReceiver::NewL
// -----------------------------------------------------------------------------
//
CIcmpReceiver* CIcmpReceiver::NewL(MIcmpErrorObserver& aObserver,
								   RSocketServ& aServer)
	{
	CIcmpReceiver* self = new (ELeave) CIcmpReceiver();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aServer);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CIcmpReceiver::ConstructL
// -----------------------------------------------------------------------------
//
void CIcmpReceiver::ConstructL(MIcmpErrorObserver& aObserver,
							   RSocketServ& aServer)
	{
	iIcmpV4Receiver = CIcmpV4Receiver::NewL(aObserver, aServer);
	iIcmpV6Receiver = CIcmpV6Receiver::NewL(aObserver, aServer);
	}

// -----------------------------------------------------------------------------
// CIcmpReceiver::~CIcmpReceiver
// -----------------------------------------------------------------------------
//
CIcmpReceiver::~CIcmpReceiver()
	{
	delete iIcmpV4Receiver; 
	delete iIcmpV6Receiver;
	}

// -----------------------------------------------------------------------------
// CIcmpReceiver::CIcmpReceiver
// -----------------------------------------------------------------------------
//
CIcmpReceiver::CIcmpReceiver()
	{
	}
