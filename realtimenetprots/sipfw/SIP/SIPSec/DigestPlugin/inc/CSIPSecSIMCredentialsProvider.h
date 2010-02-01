/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPSecSIMCredentialsProvider.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_SIMCREDENTIALSPROVIDER_H__
#define __SIPSEC_SIMCREDENTIALSPROVIDER_H__

// INCLUDES
#include <etelmm.h>
#include "CSIPSecUserCredentialsProvider.h"

// FORWARD DECLARATIONS
class CAsyncRetrieveAuthorizationInfo;
class CSIPSecSIMRecord;


// To run some test cases with the real code, instead of stub, comment the line
// "#define USE_TEST_STUB" below.
// Note that with that done, some AKA test cases are not executed, as they
// crash.
#if defined( CPPUNIT_TEST )
#define USE_TEST_STUB
#endif

#if defined( USE_TEST_STUB )

#include "timermanagerstub.h"
#define SIPSECSTUB inline

#else

#define SIPSECSTUB

#endif


/**
 * @brief Class is observer of async call-backs from SIM
 * 
 * @class MSIPSecIMSAuthenticationDataObserver CSIPSecSIMCredentialsProvider.h 
 * "CSIPSecSIMCredentialsProvider.h"
 */
class MSIPSecIMSAuthenticationDataObserver
	{	
public:
	
    /**
    * Called when user name is been resolved
    */		
    virtual void Authorized() = 0;

    /**
    * Called when authorization failed
    */		
	virtual void AuthorizationFailedL() = 0;

    /**
    * Called when authentication finnishes
    */		
    virtual void AuthenticatedL() = 0;

    /**
    * Called when authentication fails
    * @param aReason Error code telling why authentication failed.
    */
    virtual void AuthenticationFailedL( TInt aReason ) = 0;

    /**
    * Inform SIPSec FW that a non-IMS Authentication specific error occurred.
    * @param aError Error code
    * @param TBool ETrue  if IMS authentication failed
    *			   EFalse if IMS Authorization failed
    * @return TInt Value to return from RunL to active scheduler.
    */
    virtual TInt PassErrorToFramework( TInt aError,
    								   TBool aAuthenticate) const = 0;
	};



/**
 * @brief Class represent SIM authentication
 * 
 * @class CSIPSecIMSAuthentication CSIPSecSIMCredentialsProvider.h 
 * "CSIPSecSIMCredentialsProvider.h"
 */
class CSIPSecIMSAuthentication : public CActive
    {    
public: // Constructors and destructor    
    /**
    * Constructs an object
    * @param aObserver credetnials provider
    * @return new instance, ownership is tranferred
    */
    static CSIPSecIMSAuthentication* NewL( 
            MSIPSecIMSAuthenticationDataObserver& aObserver );

    /**
    * Destructor
    */
    ~CSIPSecIMSAuthentication();

public: // New functions

    /**
    * Starts authentication
    * @param aPhone the mobile phone
    */
    SIPSECSTUB void Request( RMobilePhone& aPhone );

    /**
    * Returns the authenticate data
    * @return RMobilePhone::TImsAuthenticateDataV5
    */
    RMobilePhone::TImsAuthenticateDataV5& Response();
    
public: // From CActive
    
    void RunL();

	TInt RunError( TInt aError );

    void DoCancel();

private: // New functions, for internal use

    /**
    * Constructor
    * @param aObserver credentials provider    
    */
    CSIPSecIMSAuthentication(
            MSIPSecIMSAuthenticationDataObserver& aObserver );

private: // Data

    // Authentication data
    RMobilePhone::TImsAuthenticateDataV5 iData;

    // Observer
    MSIPSecIMSAuthenticationDataObserver& iObserver;

	// Points to CSIPSecSIMCredentialsProvider::iPhone. Must be remembered
	// until the asynchronous request completes, in case the request has to be
	// canceled. Not owned.
    RMobilePhone* iPhone;

    // Message which is sent to ETelServer
    // Having this as member variable helps to extend 
    // its lifetime and help is InterProcess Communication.
    RMobilePhone::TImsAuthenticateDataV5Pckg iMessage;

private: // For testing purposes

#ifdef CPPUNIT_TEST
    friend class CSIPSecSIMCredentialsProviderTest;
#endif    
    };



/**
 * @brief Class represent SIM authorization
 * 
 * @class CSIPSecIMSAuthorization CSIPSecSIMCredentialsProvider.h 
 * "CSIPSecSIMCredentialsProvider.h"
 */
class CSIPSecIMSAuthorization : public CActive
    {
public: // Constructors and destructor
    
    /**
    * Constructs an object
    * @param aObserver credentials provider
    * @return new instance, ownership is tranferred
    */
    static CSIPSecIMSAuthorization* NewL( 
        MSIPSecIMSAuthenticationDataObserver& aObserver );
        
    /**
    * Destructor
    */
    ~CSIPSecIMSAuthorization();

public: // New functions

    /**
    * Starts authorization
    * @param aPhone the mobile phone
    */
    SIPSECSTUB void RequestL( RMobilePhone& aPhone );

    /**
    * Returns the authorization data
    * @return RMobilePhone::CImsAuthorizationInfoV5
    */
    RMobilePhone::CImsAuthorizationInfoV5& Response();

public: // From CActive
    
    SIPSECSTUB void RunL();

	TInt RunError( TInt aError );

    void DoCancel();

private: // New functions, for internal use

    /**
    * Constructor
    * @param aObserver credentials provider    
    */
    CSIPSecIMSAuthorization( MSIPSecIMSAuthenticationDataObserver& aObserver );

    /**
    * 2nd phase constructor
    */
    void ConstructL();

private: // Data
    
    // Auth data, owned.    
    RMobilePhone::CImsAuthorizationInfoV5* iData;

    // Retriever, owned.
    CAsyncRetrieveAuthorizationInfo* iRetriever;

    MSIPSecIMSAuthenticationDataObserver& iObserver;

	// For testing purposes        
#ifdef CPPUNIT_TEST
	friend class CSIPSecSIMCredentialsProviderTest;
#endif
    };

    



/**
 * @brief Class represents SIM credentials provider
 * 
 * @class CSIPSecIMSAuthorization CSIPSecSIMCredentialsProvider.h 
 * "CSIPSecSIMCredentialsProvider.h"
 */
class CSIPSecSIMCredentialsProvider :
	public CSIPSecCredentialsProvider,
	public MSIPSecIMSAuthenticationDataObserver
	{
public: // Constructors and destructor

    /**
    * Constructor
    * @param aRecord SIM record
    * @return New instance, ownership is tranferred
    */
    static CSIPSecSIMCredentialsProvider* 
        NewL( CSIPSecSIMRecord& aRecord );
    
    /**
    * Destructor
    */
    ~CSIPSecSIMCredentialsProvider();

public: // From CSIPSecCredentialsProvider

    void RequestL( TSIPSecDigestCtxSetup& aContext );

    void Cancel();

public: // From MSIPSecIMSAuthenticationDataObserver

    void Authorized();

	void AuthorizationFailedL();

    void AuthenticatedL();

    void AuthenticationFailedL( TInt aReason );

	TInt PassErrorToFramework( TInt aError, TBool aAuthenticate) const;
    
private: // Constructors, for internal use

    /**
    * Constructor
    * @param aRecord SIM record
    */
    CSIPSecSIMCredentialsProvider( CSIPSecSIMRecord& aRecord );

    /**
    * 2nd phase constructor
    */
    SIPSECSTUB void ConstructL();

private: // New functions, for internal use

	/**
    * Obtain the name of the currently selected TSY phone module name.
    * @param aModuleName Place holder for name
    */
    void GetTsyModuleNameL( TDes& aTsyModuleName ) const;

	/**
    * Load, open and initialize the phone.
    * @param aModuleName IN: TSY phone module name
    */
    void PreparePhoneL( const TDes& aTsyModuleName );

    /**
    * Requests user name
    */
    void RequestUserNameL();
    
    /**
    * Requests password
    */
    void RequestPassword();    

    /**
    * Common part for the authorization and authentication failures.
    * @param aUserRecord User record
    */
	void Failure( const CSIPSecUserRecord& aUserRecord );
	
private: // Data

    /**  
     * Etel server
     */
    RTelServer iEtelServer;
    
    /**  
     * Phone instance 
     */
    RMobilePhone iPhone;

    /**  
     * Authorization, owned
     */
    CSIPSecIMSAuthorization* iAuthorization;

    /**  
     * Authentication, owned
     */
    CSIPSecIMSAuthentication* iAuthentication;

    /**  
     * Outbound proxy, owned
     */
    HBufC8* iOutboundProxy;
    
private: // For testing purposes

	/**  
     * Write a part of memory to log.
     * @param aMessage Message to write into the log
     * @param aData Memory area which is written to log as hex dump.
     */
	void HexDumpToLog( const TDesC8& aMessage,
					   const TDesC8& aData ) const;

#ifdef CPPUNIT_TEST
    friend class CSIPSecProviderTest;
    friend class CSIPSecSIMCredentialsProviderTest;
#endif
	};


#if defined( USE_TEST_STUB )
#include "CSIPSecSIMCredentialsProviderStub.inl"
#endif

#endif // end of __SIPSEC_SIMCREDENTIALSPROVIDER_H__

// End of File
