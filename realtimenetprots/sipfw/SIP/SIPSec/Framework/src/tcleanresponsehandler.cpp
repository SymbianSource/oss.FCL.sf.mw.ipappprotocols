// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TCleanResponseHandler.cpp
// Part of       : SIPSec
// Version       : SIP/4.0
//



#include "sipsec.h"
#include "tcleanresponsehandler.h"
#include "sipsecresponsehandler.h"

// -----------------------------------------------------------------------------
// TCleanResponseHandler::TCleanResponseHandler
// -----------------------------------------------------------------------------
//
TCleanResponseHandler::TCleanResponseHandler( CSIPSec* aSIPSec,
									CSIPSecResponseHandler* aResponseHandler ):
    iSIPSec( aSIPSec ),
    iResponseHandler( aResponseHandler )
    {
    }

// -----------------------------------------------------------------------------
// TCleanResponseHandler::CleanupItem
// -----------------------------------------------------------------------------
//
TCleanupItem TCleanResponseHandler::CleanupItem()
    {
    return TCleanupItem( RemoveHandler, this );
    }

// -----------------------------------------------------------------------------
// TCleanResponseHandler::RemoveHandler
// -----------------------------------------------------------------------------
//
void TCleanResponseHandler::RemoveHandler( TAny* aCleanResponseHandler )
    {
    TCleanResponseHandler* responsehandler =
    	reinterpret_cast<TCleanResponseHandler*>( aCleanResponseHandler );

	responsehandler->iSIPSec->Remove( *responsehandler->iResponseHandler );
	delete responsehandler->iResponseHandler;
	}
