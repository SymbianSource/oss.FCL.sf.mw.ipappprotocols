/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : Sipimsprofileusimprovider.h
* Part of       : ProfileAgent
* Version       : SIP/4.1 
*
*/




/**
 @internalComponent
*/

#ifndef __SIPPROFILE_USIM_PROVIDER_H__
#define __SIPPROFILE_USIM_PROVIDER_H__

// INCLUDES
#include <etelmm.h>
#include <mmretrieve.h>
#include "msipprofileimsauthorizationdataobserver.h"
#include "authinfoobserver.h"

// FORWARD DECLARATIONS
class CSIPProfileSIMRecord;
class MSIPProfileIMSAuthenticationDataObserver;
class MSIPProfileIMSAuthorizationDataObserver;
class CSIPProfileIMSAuthorization;
class CSIPProfileSIMAuthorization;
class CSIPIMSProfileMCCandMNC;
class CAuthInfoMonitor;

/**
 * @brief Class 
 * 
 * @class CSIPProfileUsimProvider CSIPProfileUsimProvider.h 
 * "CSIPProfileUsimProvider.h"
 */
class CSIPProfileUsimProvider :
	public CBase,
	public MSIPProfileIMSAuthorizationDataObserver,
	public MAuthInfoObserver
	{
public: // Constructors and destructor

    /**
    * Constructor
    * @param aUserRecord the user record
    * @param aDeltaTimer an object used for ordering timer events
    * @return new instance, ownership is tranferred
    */
    static CSIPProfileUsimProvider* 
        NewL( CSIPProfileSIMRecord& aUserRecord,
              RTelServer& aTelServer,
		      const RTelServer::TPhoneInfo& aPhoneInfo,
              MSIPProfileIMSAuthenticationDataObserver& aObserver );
    
    /**
    * Destructor
    */
    ~CSIPProfileUsimProvider();

public: 

    /**
    * Starts the retrieving of authorization information
    * @return ETrue if request was made
    */		
    TBool RequestL(TBool aUseUSIM=EFalse);
    
    /**
    * Cancels pending operations
    */		
    void Cancel();

public: //From MSIPProfileIMSAuthorizationDataObserver
	
 	
    void AuthorizedL();
    	
	void AuthorizationFailed();
	
	void AuthorizationSIMFailed();
	
	void AuthorizedSubscriberIdL();
	
	void AuthorizedHomeNetworkIdL();
    
    TBool IsSIMSupported();
    
    TBool IsUsimSupported();
    
public: //From MAuthInfoObserver

	void AuthInfoUpdatedL();

private: //methods

    /**
    * Constructor
    * @param aUserRecord the user record
    * where the authorization information is stored
    * homenetwork domain name, private user identity,
    * public user identity
    */
    CSIPProfileUsimProvider( CSIPProfileSIMRecord& aUserRecord, 
        				     MSIPProfileIMSAuthenticationDataObserver& aObserver );

    /**
    * 2nd phase constructor
    */
    void ConstructL( 
        RTelServer& aTelServer,
        const RTelServer::TPhoneInfo& aPhoneInfo );

    /**
    * From the retrieved IMSI/MCC/MNC values 
    * the home network domain name and private user identity
    * and the public user identity are derived
    * and stored to members of CSIPProfileSIMRecord object
    */
    void DeriveValuesL();

private: //data

    RMobilePhone iPhone;    
    CSIPProfileIMSAuthorization* iAuthorization;
	CSIPProfileSIMAuthorization* iAuthorizationSIM;
	CSIPIMSProfileMCCandMNC* iAuthorizationHomeNetwork;
    CSIPProfileSIMRecord& iProfileSIMRecord;
    MSIPProfileIMSAuthenticationDataObserver& iObserver;
  
    TBool 	iSIMSupported;
    TBool   iUSimSupported;    

	// Owned
	CAuthInfoMonitor* iAuthInfoMonitor;

#ifdef CPPUNIT_TEST
    friend class CSIPProfileUSimProviderTest;
    friend class CSIPIMSProfileAgentTest;
#endif
	};


#endif // end of __SIPPROFILE_USIM_PROVIDER_H__

// End of File
