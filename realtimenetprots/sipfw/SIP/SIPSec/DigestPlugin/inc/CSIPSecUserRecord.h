/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPSecUserRecord.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_USERRECORD_H__
#define __SIPSEC_USERRECORD_H__

// INCLUDES
#include "SipStackServerDefs.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPResponse;
class MSIPSecUser;
class MSIPSecSecurityMechanismObserver;
class CSIPSecDigestObserver;
class CSIPSecCredentialsProvider;
class TSIPSecDigestCtxSetup;

// CLASS DECLARATION

/**
 * @brief Class represents user credentials
 *
 * @class CSIPSecUserRecord CSIPSecUserRecord.h "CSIPSecUserRecord.h"
 *
 * This class provides functions to request and set username and password, and
 * for creating provider and observer, which are needed when obtaining the user
 * credentials.
 */
class CSIPSecUserRecord : public CBase
	{
public: // Enumerations

    enum TUpdateState
        {
        ENoState,
        EReceivingCredentials,
        ECredentialsReceived,
        ECredentialsFailed,
        };

    enum TType
        {
        EIETF,
        EIMS
        };
	
public: // Constructors and destructor

    /**
    * Constructs an object.
    * @param aRealm Realm for the credentials
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    * @return New instance, ownership is transferred.
    */
    static CSIPSecUserRecord* NewL( const TDesC8& aRealm,
    								const MSIPSecUser& aUser,
    								TRegistrationId aRegistrationId,
    								TTransactionId aTransactionId );

    /**
    * Constructs an object and adds the pointer to the cleanup stack.    
    * @param aRealm Realm for the credentials
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    * @return New instance, ownership is transferred.
    */
    static CSIPSecUserRecord* NewLC( const TDesC8& aRealm,
    								 const MSIPSecUser& aUser,
    								 TRegistrationId aRegistrationId,
    								 TTransactionId aTransactionId );

    /**
    * Destructor
    */
    ~CSIPSecUserRecord();

public: // New functions

    /**
    * Returns type
    * @return type
    */		
    TType Type() const;    

    /**
    * Returns realm
    * @return realm
    */
    const TDesC8& Realm() const;

    /**
    * Compares aRealm to the stored realm.
    * Realm value is a quoted string and thus case-sensitive.
    * @return ETrue if realms are equal, EFalse otherwise.
    */
    TBool IsSameRealm( const TDesC8& aRealm ) const;

	/**
    * Compares aUser to the SIPSec user in this instance.
    * @param aUser SIPSec user to compare
    * @param aRegistrationId Registration id
    * @return ETrue if SIPSec users are same
    */
	TBool CompareUser( const MSIPSecUser& aUser,
					   TRegistrationId aRegistrationId ) const;

	/**
    * Returns the transaction id.
    * @return Transaction id
    */
	TTransactionId TransactionId() const;

	/**
    * Returns the transaction id that was passed to
    * MSIPSecUser::RequestCredentialsL(CSIPResponse&,TTransactionId,TRefreshId)
    * or KEmptyTransactionId if MSIPSecUser::RequestCredentialsL(const TDesC8&)
    * was used.
    * @return Transaction id
    */
	TTransactionId TransactionIdPassedToClient() const;

    /**
    * Sets the user credentials
    * @param aUsername Username
    * @param aPassword Password
    * @return ETrue If the set credentials are valid
    */		
    virtual TBool SetUserCredentialsL( const TDesC8& aUsername,
    								   const TDesC8& aPassword );

	void ClearUsernameAndPassword();

	/**
    * Returns the SIPSec user that has been asked to provide credentials, or
    * which has already provided the credentials.
    * @return MSIPSecUser& SIPSec user
    */
	const MSIPSecUser& SIPSecUser() const;
							   			  
    /**
    * Creates a credentials observer
    * @param aObserver Observer, which is needed to notify
    * @return Credentials observer, ownership is transferred.
    */
    virtual CSIPSecDigestObserver*
    	CreateObserverL( MSIPSecSecurityMechanismObserver& aObserver );

    /**
    * Creates a credential provider
    * @return Provider, ownership is transferred.
    */		
    virtual CSIPSecCredentialsProvider* CreateProviderL();
    
    /**
    * Invalidates user credentials
    * @param aState Optionally state can be forced to this
    */
    virtual void Invalidate( CSIPSecUserRecord::TUpdateState aState );

    /**
    * Checks if the credentials are valid.
    * @return ETrue if valid
    *		  EFalse not valid and thus needed to request
    */
    TBool IsValid() const;

    /**
    * Returns username
    * @return Username
    */
    const TDesC8& UserName() const;

    /**
    * Returns password
    * @return Password
    */
    const TDesC8& Password() const;

    /**
    * Sets secret data.
    * @param aSecretData Secret data. Ownership is transferred.
    */
    void SetSecretData( HBufC8* aSecretData );

    /**
    * Returns secret data
    * @return Secret data
    */
    const TDesC8& SecretData() const;

    /**
    * Sets outbound proxy
    * @param aOutboundProxy Outbound proxy
    */
    void SetOutboundProxyL( const TDesC8& aOutboundProxy );

    /**
    * Returns outbound proxy
    * @return Outbound proxy
    */		
    const TDesC8& OutboundProxy() const;

    /**
    * Called when credentials are attached.
    */
    void CredentialsAttach();

    /**
    * Called when credentials are detached.
    */
    void CredentialsDetach();

    /**
    * Check if any credentials are attached.
    * @return ETrue if at least one credential is attached
    *		  EFalse otherwise
    */
    TBool CredentialsAttached() const;

	/**
    * Checks if the authentication failed.    
    * @return ETrue Authentication failed
    *		  EFalse Otherwise
    */
	virtual TBool DidAuthenticationFail() const;

public: // New functions, requesting user related

    /**
    * Starts requesting credentials from user.
    * @param aContext The context
    * @param aObserver Observer, which is needed to notify
    * @return ETrue If the credentials for the challenge(s) present in the
    *		  current SIP response, must be asked now.
    *		  EFalse If there is no need to ask the credentials. Maybe they are
    * 	      already being asked for due to some earlier SIP response.
    */
    TBool RequestL( TSIPSecDigestCtxSetup& aContext,
    			    CSIPSecDigestObserver& aObserver );

    /**
    * Checks if the user credentials are currently being updated.
    * @return ETrue If updating credentials, EFalse otherwise
    */
    TBool IsUpdating() const;

    /**
    * Sets user record updated
    */
    void Updated();

    /**
    * Cancels pending operations
    * @param aObserver The observer, who cancelled the operation
    */
    void CancelPendingOperations( MSIPSecSecurityMechanismObserver* aObserver );

	/**
    * Gets the SIPSec user.
    * @param aUser SIPSec user
    */
    const MSIPSecUser& User() const;

	/**
    * Sets the SIPSec user.
    * @param aUser SIPSec user
    */
    void SetUser( const MSIPSecUser& aUser );

public: // Data

    static const TInt iSIPSecOffset;

    TSglQueLink iLink;

protected: // New functions

	/**
    * Constructor
    * @param aType Record type
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    */
    CSIPSecUserRecord( TType aType,
    				   const MSIPSecUser& aUser,
    				   TRegistrationId aRegistrationId,
    				   TTransactionId aTransactionId );

    /**
    * Second phase constructor
    * @param aRealm Realm
    */
    void ConstructL( const TDesC8& aRealm );

    const TDesC8& ValueOrEmptyDescriptor( HBufC8* aDescriptor ) const;

protected: // Data

    const TType iType;

    TUpdateState iState;

    // Are credentials valid
    TBool iValid;

    // How many credentials (CSIPSecCredentials objects) are attached to this
    // CSIPSecUserRecord instance.
    TInt iCredentialsAttachedCount;

private: // New functions, for internal use

    /**
    * Removes observer
    * @param aRemoveData Data holding information for removing
    */
    static void RemoveObserver( TAny* aRemoveData );

    void SetUsernameAndPasswordL( const TDesC8& aUsername,
								  const TDesC8& aPassword );	

	TBool RequestCredentialsFromOtherUser();

	const MSIPSecUser* TrustedUser() const;

private: // Data

	// Owned
    HBufC8* iRealm;

	// SIPSec user associated with this CSIPSecUserRecord. Not owned.
	const MSIPSecUser* iUser;

    // Owned
    HBufC8* iUserName;

    // Owned
    HBufC8* iPassword;

    // Owned
    HBufC8* iSecretData;

    // Owned
    HBufC8* iOutboundProxy;

    // Credentials provider, owned.
    CSIPSecCredentialsProvider* iProvider;

    // Observers waiting to be notified, not owned.
    // Array has more than one entry if two transactions with the same SIPSec
    // user are challenged with the same realm. Two CSIPSecCredentials are
    // created, both pointing to same CSIPSecUserRecord. Credentials are asked
    // from the client just once.
	RPointerArray<CSIPSecDigestObserver> iObservers;

	// Registration id related to the SIP transaction that lead to creation of
	// this CSIPSecUserRecord.
	// SIPSec user & it's truster user always have the same registration id.
	TRegistrationId iRegistrationId;

	// Transaction id of the SIP response. Contains the real transaction id also
	// when client was only given the realm of the challenge.
	TTransactionId iTransactionId;

	// If credentials are asked from a trusted user and main SIPSec user wants
	// whole SIP response (PassOnlyRealmsToUser() == EFalse), iSIPResponse
	// points to the SIP response.
	// Not owned. TransactionUser's UA owns the response.
	CSIPResponse* iSIPResponse;

	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSIPSecUserRecordTest;
    friend class CSIPSecDigestTest;
	friend class CSIPSecDigestCacheTest;
	friend class CSIPSecUserCredentialsTest;
#endif
	};

#endif // __SIPSEC_USERRECORD_H__

// End of File
