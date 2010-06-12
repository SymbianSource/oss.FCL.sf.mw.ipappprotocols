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
#include "CMSRPSendResultListener.h"
#include "CMSRPSessionImplementation.h"
#include "MMSRPServerInterface.h"


//  Member Functions

CMSRPSendResultListener* CMSRPSendResultListener::NewL(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface,
    const TDesC8& aSessionID )
    {
    
    CMSRPSendResultListener* self =
        new ( ELeave ) CMSRPSendResultListener(
            aSessionImpl, aServerInterface, aSessionID );
    return self;
    }


CMSRPSendResultListener::CMSRPSendResultListener(
    CMSRPSessionImplementation& aSessionImpl,
    MMSRPServerInterface& aServerInterface,
    const TDesC8& aSessionID )
    : CMSRPListenerBase( aSessionImpl, aServerInterface, EPriorityStandard ),
      iSessionID( aSessionID )
    {
    }
    
    
CMSRPSendResultListener::~CMSRPSendResultListener()
    {
    // Cancel any outstanding requests.
    MSRPLOG("~CMSRPSendResultListener");
    Cancel();
    MSRPLOG("~CMSRPSendResultListener Exit");
    }


void CMSRPSendResultListener::ListenSendResultL( )
    {
    if ( !IsActive() )
        {        
        // sending message to server
        iServerInterface.ListenSendResult( iSessionID, iStatus );

        // Set this handler to active so that it can receive requests.
        SetActive();
        }
    else
        {
        User::Leave( KErrInUse );
        }
    }


void CMSRPSendResultListener::RunL()
    {
    MSRPLOG2( "CMSRPSendResultListener::RunL enter, status = %d", iStatus.Int() )    
    /*RProcess proc;
    RThread thread;
    MSRPLOG2( "CMSRPServerSession::ServiceL... Client Process Priority%d", proc.Priority());
    MSRPLOG2( "CMSRPServerSession::ServiceL... Client Thread Priority%d", thread.Priority());*/

    if ( iStatus.Int() == KErrNone )
        {
        if(iServerInterface.GetSendProgress())
            {
            iSessionImpl.SendProgress(iServerInterface.GetBytesSent(), iServerInterface.GetTotalBytes());
            //handle send progress
            //to session observer                           
            }
        else
            {
            iSessionImpl.SendStatusL( iServerInterface.GetStatusOfSentMessage(), 
                    iServerInterface.GetMessageIdOfSentMessage() );
            //iSessionImpl.SendStatusL( iStatus.Int(), iServerInterface.GetMessageIdOfSentMessage() );
            }        
        }
    else
        {
        MSRPLOG( "CMSRPSendResultListener::RunL enter, status error")
        iSessionImpl.SendStatusL( iStatus.Int(), iServerInterface.GetMessageIdOfSentMessage() );
        }
    
    iServerInterface.ListenSendResult( iSessionID, iStatus );
    SetActive();
    }


void CMSRPSendResultListener::DoCancel()
    {    
    if ( IsActive() )
        {
        MSRPLOG( "CMSRPSendResultListener::DoCancel ... canceling");
        iServerInterface.CancelSendResultListening( );
        }   
        
    }
