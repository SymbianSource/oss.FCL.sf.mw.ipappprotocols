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
// Name          : sipprofilesimauthorization.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0 
//



#include "sipprofilesimauthorization.h"
#include "msipprofileimsauthorizationdataobserver.h"
#include "Sipimsprofilesimrecord.h"
#include "SipProfileLog.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::NewL
// ----------------------------------------------------------------------------
//
CSIPProfileSIMAuthorization*
CSIPProfileSIMAuthorization::NewL( 
				MSIPProfileIMSAuthorizationDataObserver& aObserver,
				RMobilePhone& aPhone )
    {
    CSIPProfileSIMAuthorization* self = 
        new (ELeave) CSIPProfileSIMAuthorization( aObserver , aPhone);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::CSIPProfileSIMAuthorization
// ----------------------------------------------------------------------------
//
CSIPProfileSIMAuthorization::CSIPProfileSIMAuthorization( 
        MSIPProfileIMSAuthorizationDataObserver& aObserver,
        RMobilePhone& aPhone ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ), iPhone( aPhone )
    {
    }
        
// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::~CSIPProfileSIMAuthorization
// ----------------------------------------------------------------------------
//
CSIPProfileSIMAuthorization::~CSIPProfileSIMAuthorization()
    {
    Cancel();
    iIMSI.Zero();
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMAuthorization::ConstructL()
    {	
    TUint32 caps(0);    
    User::LeaveIfError(iPhone.GetIdentityCaps(caps));
    if(!(caps & RMobilePhone::KCapsGetSubscriberId))
    	{
	  	User::Leave(KErrNotSupported);
    	}
    CActiveScheduler::Add( this );    
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::RequestL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMAuthorization::RequestL()
    {
    if(!IsActive())
		{
		iPhone.GetSubscriberId(iStatus, iIMSI);
		SetActive();  
		}
    }

// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::IMSI
// ----------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneSubscriberId& CSIPProfileSIMAuthorization::IMSI()
    {
    return iIMSI;
    }   

// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::RunL
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMAuthorization::RunL()
    {
	if ( iStatus.Int() == KErrNone)
        {
        iObserver.AuthorizedSubscriberIdL();
        }
    else
        {        	
        User::LeaveIfError(iStatus.Int());
        }    
    }
        
// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::RunError
// ----------------------------------------------------------------------------
//
TInt CSIPProfileSIMAuthorization::RunError(TInt aError)
    {
    iObserver.AuthorizationSIMFailed();
  	if(aError == KErrNoMemory)
  		{
  		return aError;
  		}
  	return KErrNone;
    }  
       
// ----------------------------------------------------------------------------
// CSIPProfileSIMAuthorization::DoCancel
// ----------------------------------------------------------------------------
//
void CSIPProfileSIMAuthorization::DoCancel()
    {
    iPhone.CancelAsyncRequest(EMobilePhoneGetSubscriberId);
    }

// End of File
