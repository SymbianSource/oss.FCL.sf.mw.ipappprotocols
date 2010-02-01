// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPSecSIMCredentialsObserver.cpp
// Part of       : SIPSec
// Version       : SIP/5.0
//



#include "CSIPSecSIMCredentialsObserver.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsObserver::CSIPSecSIMCredentialsObserver
// ----------------------------------------------------------------------------
//	
CSIPSecSIMCredentialsObserver::CSIPSecSIMCredentialsObserver(
	MSIPSecSecurityMechanismObserver& aObserver ) :
    CSIPSecDigestObserver( aObserver ),
    iAuthenticated( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsObserver::~CSIPSecSIMCredentialsObserver
// ----------------------------------------------------------------------------
//	
CSIPSecSIMCredentialsObserver::~CSIPSecSIMCredentialsObserver()
    {    
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsObserver::OperationCompleted
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsObserver::OperationCompleted( TStatus aReason,
														TBool aAuthenticated )
	{
	if ( aAuthenticated )
		{
		iAuthenticated = ETrue;
		}

	CSIPSecDigestObserver::OperationCompleted( aReason, aAuthenticated );
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsObserver::DoCompleted
// ----------------------------------------------------------------------------
//	
void CSIPSecSIMCredentialsObserver::DoCompleted()
	{	
	if ( iStatus != ECancelled )
	    {
    	iObserver->CacheUpdated( iStatus != EFailure, iAuthenticated );
	    }	
	}
