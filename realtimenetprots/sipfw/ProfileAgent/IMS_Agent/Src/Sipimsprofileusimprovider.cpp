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
// Name          : Sipimsprofileusimprovider.cpp
// Part of       : ProfileAgent
// Version       : SIP/4.0 
//



#include "Sipimsprofileusimprovider.h"
#include "sipprofileimsauthorization.h"
#include "sipprofilesimauthorization.h"
#include "sipimsprofilemccandmnc.h"
#include "msipprofileimsauthenticationdataobserver.h"
#include "Sipimsprofilesimrecord.h"
#include "SipProfileLog.h"
#include "authinfomonitor.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::NewL
// ----------------------------------------------------------------------------
//
CSIPProfileUsimProvider* CSIPProfileUsimProvider::NewL( 
    CSIPProfileSIMRecord& aUserRecord,
    RTelServer& aTelServer,
    const RTelServer::TPhoneInfo& aPhoneInfo,
    MSIPProfileIMSAuthenticationDataObserver& aObserver )
    {    
    CSIPProfileUsimProvider* self = 
        new (ELeave) CSIPProfileUsimProvider( aUserRecord, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aTelServer, aPhoneInfo );
    CleanupStack::Pop( self );
    return self;    
    }

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::CSIPProfileUsimProvider
// ----------------------------------------------------------------------------
//
CSIPProfileUsimProvider::CSIPProfileUsimProvider( 
    CSIPProfileSIMRecord& aUserRecord,
    MSIPProfileIMSAuthenticationDataObserver& aObserver ) 
 : iProfileSIMRecord( aUserRecord ),
   iObserver( aObserver ),
   iSIMSupported( EFalse ),
   iUSimSupported( EFalse )
	{	
	}

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::ConstructL( 
    RTelServer& aTelServer,
    const RTelServer::TPhoneInfo& aPhoneInfo )
    {
    PROFILE_DEBUG1("CSIPProfileUsimProvider::ConstructL")
    
    User::LeaveIfError( iPhone.Open( aTelServer, aPhoneInfo.iName ) );

	TUint32 caps(0);
	User::LeaveIfError(iPhone.GetIccAccessCaps(caps));
	if(caps & RMobilePhone::KCapsSimAccessSupported)
		{
		PROFILE_DEBUG1("CSIPProfileUsimProvider::ConstructL, sim supported")
		iSIMSupported = ETrue;
		}
	
	//usim supported
	if(caps & RMobilePhone::KCapsUSimAccessSupported)
		{	
		PROFILE_DEBUG1("CSIPProfileUsimProvider::ConstructL, usim supported")		
		iUSimSupported = ETrue;
		}
	
	if(!iUSimSupported && iSIMSupported)
		{
		delete iAuthorization;
		iAuthorization = NULL;
		iAuthorizationSIM = 
					CSIPProfileSIMAuthorization::NewL( *this , iPhone); 
		iAuthorizationHomeNetwork = 
					CSIPIMSProfileMCCandMNC::NewL( *this , iPhone);
		}
	else
		{
		if(iUSimSupported)
			{
			iAuthorization = CSIPProfileIMSAuthorization::NewL( *this,
															    iPhone );
			iAuthorizationSIM = 
					CSIPProfileSIMAuthorization::NewL( *this , iPhone); 
			iAuthorizationHomeNetwork = 
					CSIPIMSProfileMCCandMNC::NewL( *this , iPhone);
			iAuthInfoMonitor = CAuthInfoMonitor::NewL( *this, iPhone );
			}
		}
	if(!iUSimSupported && !iSIMSupported)
		{
		User::Leave(KErrNotSupported);	
		}
		
    PROFILE_DEBUG1("CSIPProfileUsimProvider::ConstructL, exit")
    }

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::IsSIMSupported
// ----------------------------------------------------------------------------
//
TBool CSIPProfileUsimProvider::IsSIMSupported()
    {
    return iSIMSupported;
    }

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::IsUsimSupported
// ----------------------------------------------------------------------------
//
TBool CSIPProfileUsimProvider::IsUsimSupported()
    {
    return iUSimSupported;
    }

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::~CSIPProfileUsimProvider
// ----------------------------------------------------------------------------
//
CSIPProfileUsimProvider::~CSIPProfileUsimProvider()
    {
    delete iAuthorization;
    delete iAuthorizationSIM;
    delete iAuthorizationHomeNetwork;
    delete iAuthInfoMonitor;
    iPhone.Close();
    }

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::RequestL
// ----------------------------------------------------------------------------
//
TBool CSIPProfileUsimProvider::RequestL(TBool aUseUSIM)
    {
    
    TBool requestFailed = EFalse;
    TBool credentialsExists = EFalse;
   	
  	if ( aUseUSIM )
		{
  		if (!iProfileSIMRecord.PrivateIdentity().Length())    
  		    {
            if(iAuthorization->IsImsAuthorizationAllowed())
                {
                iAuthorization->RequestL();
                }
            else
                {
                requestFailed = ETrue;
                }
  		    }
  		else
  		    {
            credentialsExists = ETrue;
  		    }
		}
  	
  	else 
  	    {
        if (!iProfileSIMRecord.SIMPrivateIdentity().Length())
  	        { 	
            if(iAuthorizationSIM->IsSIMAuthorizationAllowed())
                {
                iAuthorizationSIM->RequestL();
                }
            else
                {
                requestFailed = ETrue;
                } 
  	        }
        else
            {
            credentialsExists = ETrue;
            }
  	    }
  	
    if (credentialsExists)
    	{
    	iObserver.AsyncDeltaTimer();
    	}
    
    return requestFailed;
    }   
            
// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::Cancel
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::Cancel()
    {
    if (iAuthorization)
    	{
    	iAuthorization->Cancel();
    	}
    if(iAuthorizationSIM)
    	{
    	iAuthorizationSIM->Cancel();
    	}
    if(iAuthorizationHomeNetwork)
    	{
    	iAuthorizationHomeNetwork->Cancel();
    	}
    }
        
// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::AuthorizedL
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::AuthorizedL()
    {
    PROFILE_DEBUG1("CSIPProfileUsimProvider::AuthorizedL")
    
    if (iAuthorization)
    	{
    	RMobilePhone::CImsAuthorizationInfoV5& authorization = 
                                            iAuthorization->Response();

       	PROFILE_DEBUG3( "CSIPProfileUsimProvider::AuthorizedL, datasource:",
    	               authorization.iAuthenticationDataSource )

    	iProfileSIMRecord.SetPrivateIdL(authorization.iIMPI);
    	iProfileSIMRecord.SetFirstPublicUserNameL(authorization.iIMPUArray[0]);
    	iProfileSIMRecord.SetHomeNetworkDomainNameL(
    									authorization.iHomeNetworkDomainName);
    	iProfileSIMRecord.SetISIMPresent(
    	    authorization.iAuthenticationDataSource == RMobilePhone::EFromISIM);
    	
    	iObserver.AuthorizedL();
    	}
    	
    PROFILE_DEBUG1("CSIPProfileUsimProvider::AuthorizedL, exit")
    }    

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::AuthorizedSubscriberIdL
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::AuthorizedSubscriberIdL()
    {
    iAuthorizationHomeNetwork->RequestL();
    }    

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::AuthorizedHomeNetworkIdL
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::AuthorizedHomeNetworkIdL()
    {
    DeriveValuesL();
    }    

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::DeriveValuesL
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::DeriveValuesL()
    {    
    PROFILE_DEBUG1("CSIPProfileUsimProvider::DeriveValuesL")
    
    _LIT8(Kimsmnc, "ims.mnc");
    _LIT8(Kmcc, ".mcc");
    _LIT8(K3gppnetwork, ".3gppnetwork.org");
    _LIT8(Kato, "@");
    _LIT8(Ksip, "sip:");
    _LIT8(KZero, "0");
    
    //homenetwork domain name
    TUint mncmcclength = 
    		iAuthorizationHomeNetwork->NetworkIdentityCode().Length() 
    		+ iAuthorizationHomeNetwork->NetworkCountryCode().Length();
    if (iAuthorizationHomeNetwork->NetworkIdentityCode().Length() == 2)
    	{
    	mncmcclength = mncmcclength + 1;
    	}
    
    TUint appendlength = Kimsmnc().Length() + Kmcc().Length() 
    					+ K3gppnetwork().Length() + mncmcclength;
    									
    HBufC8* appendingResult = HBufC8::NewL(appendlength);
	TPtr8 appendingResultPtr = appendingResult->Des();

	appendingResultPtr.Append(Kimsmnc);
	if (iAuthorizationHomeNetwork->NetworkIdentityCode().Length() == 2)
    	{
    	appendingResultPtr.Append(KZero);
    	}
	appendingResultPtr.Append(iAuthorizationHomeNetwork->NetworkIdentityCode());
	appendingResultPtr.Append(Kmcc);
	appendingResultPtr.Append(iAuthorizationHomeNetwork->NetworkCountryCode());
	appendingResultPtr.Append(K3gppnetwork);
    CleanupStack::PushL(appendingResult);
    iProfileSIMRecord.SetSIMHomeNetworkDomainNameL(*appendingResult);
    CleanupStack::Pop(appendingResult);
    delete appendingResult;
    appendingResult = NULL;
    
    //private user identity
    appendlength = 0;
    appendlength = iAuthorizationSIM->IMSI().Length() + Kato().Length() 
    			+ Kimsmnc().Length() + mncmcclength + Kmcc().Length() 
    			+ K3gppnetwork().Length();
   
    HBufC8* result = HBufC8::NewL(appendlength);
	TPtr8 resultPtr = result->Des();
	resultPtr.Append(iAuthorizationSIM->IMSI());
	resultPtr.Append(Kato);
	resultPtr.Append(Kimsmnc);
	if (iAuthorizationHomeNetwork->NetworkIdentityCode().Length() == 2)
    	{
    	resultPtr.Append(KZero);
    	}	
	resultPtr.Append(iAuthorizationHomeNetwork->NetworkIdentityCode());	
	resultPtr.Append(Kmcc);	
	resultPtr.Append(iAuthorizationHomeNetwork->NetworkCountryCode());
	resultPtr.Append(K3gppnetwork);
    CleanupStack::PushL(result);
    iProfileSIMRecord.SetSIMPrivateIdL(*result);    
    CleanupStack::Pop(result);
    delete result;
    result = NULL;
    
    //public user identity
    
    appendlength = Ksip().Length() + 
    			   iProfileSIMRecord.SIMPrivateIdentity().Length();
    HBufC8* temp = HBufC8::NewL(appendlength);
    TPtr8 tempPtr = temp->Des();
    tempPtr = temp->Des();
    tempPtr.Append(Ksip);
	tempPtr.Append(iProfileSIMRecord.SIMPrivateIdentity());
    
    CleanupStack::PushL(temp);
    iProfileSIMRecord.SetSIMFirstPublicUserNameL(*temp);
    CleanupStack::Pop(temp);
    delete temp;
    temp = NULL;
    iObserver.AuthorizedL();
    
    PROFILE_DEBUG1("CSIPProfileUsimProvider::DeriveValuesL, exit")
    }    

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::AuthorizationFailed
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::AuthorizationFailed()
    {
    iProfileSIMRecord.UserRecordFailed(KErrArgument);

    iObserver.AuthorizationFailed();
  }
  
// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::AuthorizationSIMFailed
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::AuthorizationSIMFailed()
    {
    iProfileSIMRecord.UserRecordFailed(KErrArgument);

    iObserver.AuthorizationSIMFailed();
  }

// ----------------------------------------------------------------------------
// CSIPProfileUsimProvider::AuthInfoUpdatedL
// ----------------------------------------------------------------------------
//
void CSIPProfileUsimProvider::AuthInfoUpdatedL()
	{
	iProfileSIMRecord.ResetISIMValuesL();
	if(iAuthorizationSIM)
	    {
        iAuthorizationSIM->ResetSIMAuthorizationAllowed(EFalse);
	    }
	if(iAuthorization)
	    {
        iAuthorization->ResetImsAuthorizationAllowed(EFalse);
	    }
	iObserver.UpdateRegistrationsL();
  	if (!iProfileSIMRecord.PrivateIdentity().Length())
  		{   
  		if(iAuthorization)
  			{	
    	iAuthorization->RequestL();
   			}
  		}
	}

// End of File
