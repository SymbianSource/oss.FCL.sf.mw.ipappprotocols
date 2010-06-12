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
#include "CMSRP.h"
#include "CMSRPImplementation.h"


//  Member Functions

EXPORT_C CMSRP* CMSRP::NewL( const TUid& aUid )
    {
    MSRPLOG("CMSRP.. NewL");
    CMSRP* self = CMSRP::NewLC(aUid);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CMSRP* CMSRP::NewLC( const TUid& aUid )
    {
    MSRPLOG("CMSRP.. NewLC");
    CMSRP* self = new (ELeave) CMSRP;
    CleanupStack::PushL(self);
    self->ConstructL(aUid);
    return self;
    }

EXPORT_C void CMSRP::ConstructL( const TUid& aUid )
    {
    MSRPLOG("CMSRP.. ConstructL");
    iMSRPImpl = CMSRPImplementation::NewL(aUid);	
    }

EXPORT_C CMSRP::~CMSRP()
    {
    MSRPLOG("CMSRP.. Dtor");
    if(NULL!=iMSRPImpl)
		delete iMSRPImpl;
    }

EXPORT_C CMSRPSession* CMSRP::CreateMSRPSessionL( MMSRPSessionObserver& aObserver, 
                                                 const TUint aIapId )
	{
		// Creates and Returns a MSRP Session. 
		// No paramers are taken at this stage. Primarily because 
		// this is the first step the MSRP user is expected to do before 
		// SDP negotiation. 
		// At this stage the user expects to get the session-id, 
		// avaible trasport etc.
	
	MSRPLOG("CMSRP.. Create MSRP session");

	return iMSRPImpl->CreateMSRPSessionL( aObserver, aIapId );
	}

EXPORT_C void CMSRP::CloseMSRPSession( CMSRPSession* aMsrpSession )
    {
    
    MSRPLOG("CMSRP.. Close MSRP session");

    return iMSRPImpl->CloseMSRPSession( aMsrpSession );
    }



