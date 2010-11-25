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

// System Includes
#include <e32base.h>

//  Include Files
#include "CMSRPSession.h"
#include "CMSRPSessionImplementation.h"

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
CMSRPSession* CMSRPSession::NewL( 
    RMSRP& aRMSRP,
    MMSRPSessionObserver& aObserver,
    const TUint aIapId,
    const TDesC8& aSessionId )
	{
	MSRPLOG("CMSRP Session.. NewL");
	CMSRPSession* self = CMSRPSession::NewLC( aRMSRP, aObserver, aIapId, aSessionId );
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
CMSRPSession* CMSRPSession::NewLC( 
    RMSRP& aRMSRP,
    MMSRPSessionObserver& aObserver,
    const TUint aIapId,
    const TDesC8& aSessionId )
    {
    MSRPLOG("CMSRP Session.. NewLC");
    CMSRPSession* self = new (ELeave) CMSRPSession;
    CleanupStack::PushL(self);
    self->ConstructL( aRMSRP, aObserver, aIapId, aSessionId );
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
void CMSRPSession::ConstructL( 
    RMSRP& aRMSRP,
    MMSRPSessionObserver& aObserver,
    const TUint aIapId,
    const TDesC8& aSessionId )
	{
	MSRPLOG("CMSRP Session.. ConstructL");
	iMSRPSessionImpl = CMSRPSessionImplementation::NewL( aRMSRP, aObserver, aIapId, aSessionId );	 
	}

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
CMSRPSession::~CMSRPSession()
	{
	MSRPLOG("-> CMSRPSession::~CMSRPSession()");
	delete iMSRPSessionImpl;
    MSRPLOG("<- CMSRPSession::~CMSRPSession()");
	}

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CMSRPSession::LocalMSRPPath()
    {
    MSRPLOG("CMSRP Session.. LocalMSRPPath");
    return iMSRPSessionImpl->LocalMSRPPath();
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPSession::SetSessionParams( CMSRPSessionParams& aSessionParams )
    {
    MSRPLOG("CMSRP Session.. SetSessionParams");
    iMSRPSessionImpl->SetSessionParams( aSessionParams );
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPSession::Connect( const TDesC8& aRemoteMsrpPath )
	{
	MSRPLOG("CMSRP Session.. Connect");
	TRAPD( err, iMSRPSessionImpl->ConnectL( aRemoteMsrpPath ));
	return err;
	}

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPSession::Listen( const TDesC8& aRemoteMsrpPath )
	{
	MSRPLOG("CMSRP Session.. Listen");
	TRAPD( err, iMSRPSessionImpl->ListenL( aRemoteMsrpPath ));
	return err;
	}

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CMSRPSession::SendBuffer(
    const TDesC8& aMessage,
    const TDesC8& aMimeType )
    {
    MSRPLOG("CMSRP Session.. SendBuffer");
    // send the Message
    HBufC8* messageId = NULL;
    TRAPD( err, messageId = iMSRPSessionImpl->SendBufferL( aMessage, aMimeType ) );
    if ( err )
        {
        messageId = NULL;
        }
    return messageId;
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CMSRPSession::SendFile(
    const TFileName& aFileName,
    const TDesC8& aMimeType )
    {
   
    MSRPLOG("CMSRP Session.. SendFile");
    HBufC8* messageId = NULL;
    TRAPD( err, messageId = iMSRPSessionImpl->SendFileL(aFileName, aMimeType ) );
    if ( err )
        {
        messageId = NULL;
        }
    return messageId;
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPSession::CancelReceiving( TDesC8& aMessageId )
    {
    MSRPLOG("-> CMSRPSession::CancelReceiveFile");
    TRAPD( err, iMSRPSessionImpl->CancelReceivingL( aMessageId ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPSession::NotifyProgress(TBool aFlag)
    {
    iMSRPSessionImpl->NotifyProgress( aFlag );
    }

// -----------------------------------------------------------------------------
// CMSRPSession::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPSession::CancelSending( TDesC8& aMessageId )
    {
    MSRPLOG("CMSRP Session.. CancelSending");
    TRAPD( err, iMSRPSessionImpl->CancelSendingL( aMessageId ) );
    return err;
    }

// End of File
