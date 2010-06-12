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


//  Member Functions

CMSRPSession* CMSRPSession::NewL( RMSRP& aRMSRP,
                                           MMSRPSessionObserver& aObserver,
                                           const TUint aIapId )
	{
	MSRPLOG("CMSRP Session.. NewL");
	CMSRPSession* self = CMSRPSession::NewLC( aRMSRP, aObserver, aIapId );
	CleanupStack::Pop(self);
	return self;
	}


CMSRPSession* CMSRPSession::NewLC( RMSRP& aRMSRP,
                                            MMSRPSessionObserver& aObserver,
                                            const TUint aIapId )
    {
    MSRPLOG("CMSRP Session.. NewLC");
    CMSRPSession* self = new (ELeave) CMSRPSession;
    CleanupStack::PushL(self);
    self->ConstructL( aRMSRP, aObserver, aIapId );
    return self;
    }


void CMSRPSession::ConstructL( RMSRP& aRMSRP,
                               MMSRPSessionObserver& aObserver,
                               const TUint aIapId )
	{
	MSRPLOG("CMSRP Session.. ConstructL");
	iMSRPSessionImpl = CMSRPSessionImplementation::NewL( aRMSRP, aObserver, aIapId );	 
	}


CMSRPSession::~CMSRPSession()
	{
	MSRPLOG("CMSRP Session.. Dtor");
	if(NULL != iMSRPSessionImpl)
		delete iMSRPSessionImpl;
	}


EXPORT_C TDesC8& CMSRPSession::LocalMSRPPath()
    {
    MSRPLOG("CMSRP Session.. LocalMSRPPath");
    return iMSRPSessionImpl->LocalMSRPPath();
    }


EXPORT_C void CMSRPSession::SetSessionParams( CMSRPSessionParams& aSessionParams )
    {
    MSRPLOG("CMSRP Session.. SetSessionParams");
    iMSRPSessionImpl->SetSessionParams( aSessionParams );
    }

    
EXPORT_C TInt CMSRPSession::Connect( const TDesC8& aRemoteMsrpPath )
	{
	MSRPLOG("CMSRP Session.. Connect");
	TRAPD( err, iMSRPSessionImpl->ConnectL( aRemoteMsrpPath ));
	return err;
	}


EXPORT_C TInt CMSRPSession::Listen( const TDesC8& aRemoteMsrpPath )
	{
	MSRPLOG("CMSRP Session.. Listen");
	TRAPD( err, iMSRPSessionImpl->ListenL( aRemoteMsrpPath ));
	return err;
	}


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
        return NULL;
        }
    return messageId;
    }

EXPORT_C TInt CMSRPSession::SendFile(
    const TFileName& aFileName,
    const TDesC8& aMimeType )
    {
   
    MSRPLOG("CMSRP Session.. SendFile");
    TRAPD( err,iMSRPSessionImpl->SendFileL(aFileName, aMimeType ) );
    return err;
    
    }

EXPORT_C TInt CMSRPSession::CancelFileSending( )
    {
    MSRPLOG("CMSRP Session.. CancelFileSending");
/*    TInt err;
    return err;*/
    return 0;
    }

EXPORT_C TInt CMSRPSession::ReceiveFile(
        const TFileName& aFileName,
        const TInt aFileSize,
        const TDesC8& aMimeType)
    {
    MSRPLOG("CMSRP Session.. ReceiveFile");
    TRAPD( err,iMSRPSessionImpl->ReceiveFileL(aFileName,aFileSize,aMimeType ));
    return err;
    }

EXPORT_C TInt CMSRPSession::CancelReceiveFile()
    {
    return 0;
    }

EXPORT_C void CMSRPSession::NotifyProgress(TBool aFlag)
    {
    iMSRPSessionImpl->NotifyProgress(aFlag);
    }

EXPORT_C TInt CMSRPSession::CancelSending( TDesC8& aMessageId )
    {
    MSRPLOG("CMSRP Session.. CancelSending");
    TRAPD( err, iMSRPSessionImpl->CancelSendingL( aMessageId ) );
    return err;
    }

