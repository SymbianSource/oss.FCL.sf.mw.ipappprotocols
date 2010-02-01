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
// Name          : sipprofileimsauthorization.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0 
//



#include "sipprofileimsauthorization.h"
#include "msipprofileimsauthorizationdataobserver.h"
#include "Sipimsprofilesimrecord.h"
#include "SipProfileLog.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::NewL
// ----------------------------------------------------------------------------
//
CSIPProfileIMSAuthorization*
CSIPProfileIMSAuthorization::NewL( MSIPProfileIMSAuthorizationDataObserver& 
										aObserver, RMobilePhone& aPhone )
    {
    CSIPProfileIMSAuthorization* self = 
        new (ELeave) CSIPProfileIMSAuthorization( aObserver, aPhone );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::CSIPProfileIMSAuthorization
// ----------------------------------------------------------------------------
//
CSIPProfileIMSAuthorization::CSIPProfileIMSAuthorization( 
        MSIPProfileIMSAuthorizationDataObserver& aObserver,
        RMobilePhone& aPhone ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ), iPhone( aPhone )
    {
    }
        
// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::~CSIPProfileIMSAuthorization
// ----------------------------------------------------------------------------
//
CSIPProfileIMSAuthorization::~CSIPProfileIMSAuthorization()
    {
    Cancel();
    delete iData;
    delete iRetriever;
    }

// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPProfileIMSAuthorization::ConstructL()
    {	
    iData = RMobilePhone::CImsAuthorizationInfoV5::NewL();
    CActiveScheduler::Add( this );    
    }

// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::RequestL
// ----------------------------------------------------------------------------
//
void CSIPProfileIMSAuthorization::RequestL()
    {
    if(!IsActive())
		{
		RMobilePhone::CImsAuthorizationInfoV5& data = Response();
  		data.iIMPI.Zero();
    	data.iIMPUArray.Reset();
    	data.iHomeNetworkDomainName.Zero();
		
		delete iRetriever;
    	iRetriever = NULL;
    	iRetriever = CAsyncRetrieveAuthorizationInfo::NewL( iPhone, *iData );
		
		iRetriever->Start( iStatus );
		SetActive();  
		}
    }

// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::Response
// ----------------------------------------------------------------------------
//
RMobilePhone::CImsAuthorizationInfoV5& CSIPProfileIMSAuthorization::Response()
    {
    return *iData;
    }   

// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::RunL
// ----------------------------------------------------------------------------
//
void CSIPProfileIMSAuthorization::RunL()
    {
	if ( iStatus.Int() == KErrNone && iRetriever->AuthInfoValid())
        {
      iObserver.AuthorizedL();
        }
    else
        {      	
        User::LeaveIfError(iStatus.Int());
        }    
    }
        
 // ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::RunError
// ----------------------------------------------------------------------------
//
TInt CSIPProfileIMSAuthorization::RunError(TInt aError)
    {
    iObserver.AuthorizationFailed();
  	if(aError == KErrNoMemory)
  		{
  		return aError;
  		}
  	return KErrNone;
    }  
       
// ----------------------------------------------------------------------------
// CSIPProfileIMSAuthorization::DoCancel
// ----------------------------------------------------------------------------
//
void CSIPProfileIMSAuthorization::DoCancel()
    {
    delete iRetriever;
    iRetriever = NULL;
    }

// End of File
