/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/


//  Include Files
#include "MsrpCommon.h"
#include "CMSRPConnectionListener.h"
#include "CMSRPSessionImplementation.h"
#include "MMSRPServerInterface.h"


//  Member Functions

CMSRPConnectionListener* CMSRPConnectionListener::NewL(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface )
    {
    // Perform construction.
    CMSRPConnectionListener* self =
        new ( ELeave ) CMSRPConnectionListener(
            aSessionImpl, aServerInterface );
    return self;
    }


CMSRPConnectionListener::~CMSRPConnectionListener()
    {
    // Cancel any outstanding requests.
    Cancel();
    }


CMSRPConnectionListener::CMSRPConnectionListener(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface )
    : CMSRPListenerBase( aSessionImpl, aServerInterface, EPriorityStandard )
    {
    }

    
void CMSRPConnectionListener::ConnectL( const TDesC8& aRemoteHost,
                                        const TUint aRemotePort,
                                        const TDesC8& aRemoteSessionID )
    {
    if ( !IsActive() )
        {
        // Connection request to server
        iServerInterface.Connect( aRemoteHost, aRemotePort, aRemoteSessionID, iStatus );

        // Set this handler to active so that it can receive requests.
        SetActive();
        }
    else
        {
        User::Leave( KErrInUse );
        }    
    }
    
    
void CMSRPConnectionListener::RunL()
    {
    iSessionImpl.ConnectionEstablishedL( iStatus.Int() );
    }
    
    
void CMSRPConnectionListener::DoCancel()
    {
    // not possible to cancel the connect/disconnect sequence
    }
