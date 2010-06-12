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
#include "CMSRPListenerBase.h"
#include "CMSRPIncomingListener.h"
#include "CMSRPSessionImplementation.h"
#include "MMSRPServerInterface.h"


//  Member Functions

CMSRPIncomingListener* CMSRPIncomingListener::NewL(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface )
    {
    CMSRPIncomingListener* self =
        new ( ELeave ) CMSRPIncomingListener(
                aSessionImpl, aServerInterface );
    
    return self;
    }


CMSRPIncomingListener::CMSRPIncomingListener(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface )
    : CMSRPListenerBase( aSessionImpl, aServerInterface, EPriorityStandard )
    {
    }


CMSRPIncomingListener::~CMSRPIncomingListener()
    {
    Cancel();    
    }


void CMSRPIncomingListener::ListenConnections(
    const TDesC8& aRemoteHost,
    const TUint aRemotePort,
    const TDesC8& aRemoteSessionID )
    {
    if ( !IsActive() )
        {
        iIsMessage = EFalse;                
        iServerInterface.ListenConnections( aRemoteHost, aRemotePort, aRemoteSessionID, iIsMessage, iStatus );

        // Set this handler to active so that it can receive requests.
        SetActive();
        }
    }


void CMSRPIncomingListener::ListenMessages( )
    {
    if ( !IsActive() )
        {
        iIsMessage = ETrue;             
        iServerInterface.ListenMessages( iIsMessage, iStatus );

        // Set this handler to active so that it can receive requests.
        SetActive();
        }
    }


void CMSRPIncomingListener::RunL()
    {    
    if ( !iIsMessage )
        {
        // connection was made
        iSessionImpl.ConnectionEstablishedL( iStatus.Int() );
        }
    else
        {
        if(iStatus == KErrNone)
            {
            if(iServerInterface.GetListenProgress())
                {
                //handle listen progress
                iSessionImpl.ReceiveProgress(iServerInterface.GetBytesReceived(), iServerInterface.GetTotalBytesReceived());                       
                }
            else
                {
                // Handle incoming message by reading it from server interface
                iSessionImpl.HandleIncomingMessageL( iServerInterface.GetReceivedMessage( ),
                    iServerInterface.GetStatusOfReceivedMessage() );
                }

            // issue a new request
            iServerInterface.ListenMessages( iIsMessage, iStatus );
            SetActive();
            }
        else
            {
            // Error scenario.
            iSessionImpl.HandleConnectionErrors(iStatus.Int());            
            }
        }
    }


void CMSRPIncomingListener::DoCancel()
    {
    if ( IsActive() )
        {
        iServerInterface.CancelReceiving();
        }
   
    }
