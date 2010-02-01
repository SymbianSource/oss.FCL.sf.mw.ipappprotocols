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
// Name          : sipimsprofilemccandmnc.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0 
//



#include "sipimsprofilemccandmnc.h"
#include "msipprofileimsauthorizationdataobserver.h"
#include "Sipimsprofilesimrecord.h"
#include "SipProfileLog.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::NewL
// ----------------------------------------------------------------------------
//
CSIPIMSProfileMCCandMNC*
CSIPIMSProfileMCCandMNC::NewL( MSIPProfileIMSAuthorizationDataObserver&
							 aObserver, RMobilePhone& aPhone )
    {
    CSIPIMSProfileMCCandMNC* self = 
        new (ELeave) CSIPIMSProfileMCCandMNC( aObserver, aPhone);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::CSIPIMSProfileMCCandMNC
// ----------------------------------------------------------------------------
//
CSIPIMSProfileMCCandMNC::CSIPIMSProfileMCCandMNC( 
        MSIPProfileIMSAuthorizationDataObserver& aObserver,
        RMobilePhone& aPhone ) :
    CActive( CActive::EPriorityStandard ), iObserver( aObserver ),
     iPhone( aPhone )
    {
    }
        
// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::~CSIPIMSProfileMCCandMNC
// ----------------------------------------------------------------------------
//
CSIPIMSProfileMCCandMNC::~CSIPIMSProfileMCCandMNC()
    {
    Cancel();
    iNetworkInfoPckg.Zero();
    }

// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPIMSProfileMCCandMNC::ConstructL()
    {	
    TUint32 caps(0);    
    User::LeaveIfError(iPhone.GetNetworkCaps(caps));
    if(!(caps & RMobilePhone::KCapsGetHomeNetwork))
    	{
 	  	User::Leave(KErrNotSupported);
    	}
    CActiveScheduler::Add( this );    
    }

// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::RequestL
// ----------------------------------------------------------------------------
//
void CSIPIMSProfileMCCandMNC::RequestL()
    {
    if(!IsActive())
		{
		iPhone.GetHomeNetwork(iStatus, iNetworkInfoPckg);
		SetActive(); 
		}
    }
      
// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::NetworkCountryCode
// ----------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneNetworkCountryCode& 
			CSIPIMSProfileMCCandMNC::NetworkCountryCode()
    {
    return iCountryCode = iNetworkInfoPckg().iCountryCode;
    }   

// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::NetworkIdentityCode
// ----------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneNetworkIdentity& 
	CSIPIMSProfileMCCandMNC::NetworkIdentityCode()
    {
    return iNetworkCode = iNetworkInfoPckg().iNetworkId;
    }   

// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::RunL
// ----------------------------------------------------------------------------
//
void CSIPIMSProfileMCCandMNC::RunL()
    {
	if ( iStatus.Int() == KErrNone)
        {
        iObserver.AuthorizedHomeNetworkIdL();
        }
    else
        { 
        User::LeaveIfError(iStatus.Int());
        }    
    }
        
 // ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::RunError
// ----------------------------------------------------------------------------
//
TInt CSIPIMSProfileMCCandMNC::RunError(TInt aError)
    {
    iObserver.AuthorizationSIMFailed();
  	if(aError == KErrNoMemory)
  		{
  		return aError;
  		}
  	return KErrNone;
    }  
       
// ----------------------------------------------------------------------------
// CSIPIMSProfileMCCandMNC::DoCancel
// ----------------------------------------------------------------------------
//
void CSIPIMSProfileMCCandMNC::DoCancel()
    {
 	iPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork);
    }


// End of File
