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
#include "RMSRP.h"
#include "CMSRPSession.h"
#include "CMSRPImplementation.h"
#include "MMSRPSessionObserver.h"


//  Member Functions 

CMSRPImplementation* CMSRPImplementation::NewL( const TUid& aUid )
	{
	MSRPLOG("CMSRP Impl.. NewL");
	CMSRPImplementation* self = CMSRPImplementation::NewLC(aUid);
	CleanupStack::Pop(self);
	return self;
	}
 

CMSRPImplementation* CMSRPImplementation::NewLC( const TUid& aUid )
    {
    MSRPLOG("CMSRP Impl.. NewLC");
    CMSRPImplementation* self = new (ELeave) CMSRPImplementation(aUid);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
  

void CMSRPImplementation::ConstructL()
	{
	MSRPLOG("CMSRP Impl.. ConstructL");
	iMSRP = new ( ELeave ) RMSRP();
	User::LeaveIfError(iMSRP->CreateServerSession());	
	}


CMSRPImplementation::CMSRPImplementation( const TUid& aUid )
    :iUid(aUid)
    {
    MSRPLOG("CMSRP Impl.. Ctor");
    }


CMSRPImplementation::~CMSRPImplementation()
	{
	MSRPLOG("CMSRP Impl.. Dtor");
	
	if( iMSRPSessionsArray.Count() )
	    {
        for(TUint i=0; i<iMSRPSessionsArray.Count(); i++)
            {
            delete iMSRPSessionsArray[i];
            }
	    }
	
	iMSRPSessionsArray.Close();
	iMSRP->Close();
	delete iMSRP;	
	
	}
 
 
CMSRPSession* CMSRPImplementation::CreateMSRPSessionL( MMSRPSessionObserver& aObserver,
                                                       const TUint aIapId )
	{
	// Creates and Returns a MSRP Session. 
	// No paramers are taken at this stage. Primarily because 
	// this is the first step the MSRP user is expected to do before 
	// SDP negotiation. 
	// At this stage the user expects to get the session-id, 
	// avaible trasport etc. 
	
	MSRPLOG("CMSRP Impl.. Create MSRP session");

	CMSRPSession* msrpSession 
	 			= CMSRPSession::NewL( *iMSRP, aObserver, aIapId );

	// CMSRPImplementation would like to keep a tab on number of MSRP sessions 
	// that it is serving.Append the MSRPSession to a session array.
	
	iMSRPSessionsArray.AppendL(msrpSession);

	return msrpSession;
		 
	}
 
 
void CMSRPImplementation::CloseMSRPSession( CMSRPSession* aMsrpSession )
    {
   
    MSRPLOG("CMSRP Impl.. Close MSRP session");
    
    TInt index = iMSRPSessionsArray.Find(aMsrpSession);
    
    delete iMSRPSessionsArray[index];
    iMSRPSessionsArray.Remove(index);       
        
    }

