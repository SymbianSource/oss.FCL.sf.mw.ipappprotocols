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
// Name        : sipresponsesender.cpp
// Part of     : SIP Client
// Interface   : SDK API, SIP API
// Version     : 1.0
//



#include "csipresponsesender.h"
#include "sipclientconnection.h"


// -----------------------------------------------------------------------------
// CSIPResponseSender::NewL
// -----------------------------------------------------------------------------
//
CSIPResponseSender* CSIPResponseSender::NewL()
    {
    return new (ELeave) CSIPResponseSender();
    }

// -----------------------------------------------------------------------------
// CSIPResponseSender::CSIPResponseSender
// -----------------------------------------------------------------------------
//
CSIPResponseSender::CSIPResponseSender()
    {
    }

// -----------------------------------------------------------------------------
// CSIPResponseSender::~CSIPResponseSender
// -----------------------------------------------------------------------------
//
CSIPResponseSender::~CSIPResponseSender()
    {
    }

// -----------------------------------------------------------------------------
// CSIPResponseSender::SendResponseL
// -----------------------------------------------------------------------------
//
void CSIPResponseSender::SendResponseL(CSIPClientConnection& aConnection,
                                       const CSIPResponseElements& aElements,
						               TUint32 aRequestId,
                                       TBool /*aAffectsDialogState*/,
                                       TBool /*aTargetRefresh*/)
    {
    aConnection.SendResponseL(aElements, aRequestId);
    }

// -----------------------------------------------------------------------------
// CSIPResponseSender::Dialog
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation* CSIPResponseSender::Dialog() const
	{
	return NULL;
	}
