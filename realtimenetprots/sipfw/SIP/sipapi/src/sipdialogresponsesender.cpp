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
// Name        : sipdialogresponsesender.cpp
// Part of     : SIP Client
// Interface   : SDK API, SIP API
// Version     : 1.0
//



#include "csipdialogresponsesender.h"
#include "sip.h"
#include "sipclientconnection.h"
#include "SipDialogImplementation.h"


// -----------------------------------------------------------------------------
// CSIPDialogResponseSender::NewL
// -----------------------------------------------------------------------------
//
CSIPDialogResponseSender*
CSIPDialogResponseSender::NewL(CSIPDialogImplementation& aDialog)
    {
    return new (ELeave) CSIPDialogResponseSender(aDialog);
    }

// -----------------------------------------------------------------------------
// CSIPDialogResponseSender::CSIPDialogResponseSender
// -----------------------------------------------------------------------------
//
CSIPDialogResponseSender::CSIPDialogResponseSender(
								CSIPDialogImplementation& aDialog) :
    iDialog(aDialog)
    {
    }

// -----------------------------------------------------------------------------
// CSIPDialogResponseSender::~CSIPDialogResponseSender
// -----------------------------------------------------------------------------
//
CSIPDialogResponseSender::~CSIPDialogResponseSender()
    {
    }

// -----------------------------------------------------------------------------
// CSIPDialogResponseSender::SendResponseL
// -----------------------------------------------------------------------------
//
void CSIPDialogResponseSender::SendResponseL(
                                       CSIPClientConnection& /*aConnection*/,
                                       const CSIPResponseElements& aElements,
						               TUint32 aRequestId,
                                       TBool aAffectsDialogState,
                                       TBool aTargetRefresh)
    {
    iDialog.SendResponseL(aElements,
                          aRequestId,
                          aAffectsDialogState,
                          aTargetRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPDialogResponseSender::Dialog
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation* CSIPDialogResponseSender::Dialog() const
	{
	return &iDialog;
	}
