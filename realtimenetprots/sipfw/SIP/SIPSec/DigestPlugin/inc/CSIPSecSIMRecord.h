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
* Name          : CSIPSecSIMRecord.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_SIMRECORD_H__
#define  __SIPSEC_SIMRECORD_H__

// INCLUDES
#include "CSIPSecUserRecord.h"


// CLASS DECLARATION
/**
 * @brief Class represents credentials stored in SIM 
 * 
 * @class CSIPSecSIMRecord CSIPSecSIMRecord.h "CSIPSecSIMRecord.h"
 *
 * This class provides additions to user credentials functions to indicate, if
 * authentication or authorization with SIP has failed and why.
 *
 */
class CSIPSecSIMRecord : public CSIPSecUserRecord
	{
public: // Enumerations

	enum TFailureType
	    {
	    ENoFailure,
	    // AUTN derived SQN value is not in the expected range
	    ESQN,
	    // AUTN derived MAC does not match locally calculated MAC
	    EMAC,
	    EPrivateIdentity
	    };

public: // Constructors and destructor

    /**
    * Constructs an object; leaves on failure.
    * @param aRealm Realm for the credentials
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    * @return new instance
    */
    static CSIPSecSIMRecord* NewL( const TDesC8& aRealm,
    							   const MSIPSecUser& aUser,
    							   TRegistrationId aRegistrationId,
    							   TTransactionId aTransactionId );

    /**
    * Constructs an object and adds the pointer to the cleanup stack;
    * leaves on failure.
    * @param aRealm Realm for the credentials
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    * @return new instance
    */		
    static CSIPSecSIMRecord* NewLC( const TDesC8& aRealm,
    								const MSIPSecUser& aUser,
    								TRegistrationId aRegistrationId,
    								TTransactionId aTransactionId );

    /**
    * Destructor
    */
    ~CSIPSecSIMRecord();

public: // From CSIPSecUserRecord

    TBool SetUserCredentialsL( const TDesC8& aUsername,
    						   const TDesC8& aPassword );

    CSIPSecDigestObserver*
    	CreateObserverL( MSIPSecSecurityMechanismObserver& aObserver );

    CSIPSecCredentialsProvider* CreateProviderL();

    void Invalidate( CSIPSecUserRecord::TUpdateState aState );

    TBool DidAuthenticationFail() const;

public: // New functions

    /**
    * Called when authorization or authentication fails
    * @pre aFailure != ENoFailure
    * @param aFailure indicates type of failure
    */
    void UserCredentialsFailed( TFailureType aFailure );

    /**
    * Get the failure type.
    * @return Failure type
    */		
    TFailureType Failure() const;

    /**
    * Sets authentication data
    * @param aNonce Nonce
    */		
    void SetAuthenticationDataL( const TDesC8& aNonce );

    /**
    * Gets the RAND value
    * @return RAND value
    */		
    const TDesC8& RAND() const;

    /**
    * Gets the AUTN value
    * @return AUTN value
    */		
    const TDesC8& AUTN() const;

    /**
    * Sets re-synchronization data
    * @pre aAUTS.Length() > 0
    * @param aAUTS AUTS parameter
    */
    void SetResynchronizationDataL( const TDesC8& aAUTS );

    /**
    * Returns re-synchronization data
    * @return Re-synchronization data
    */
    const TDesC8& AUTS() const;

	/**
    * Keep track on how many successive 401 responses are received.
    * @param aResponseCode SIP response code
    * @return ETrue if too many 401s have been received, EFalse otherwise.
    */
	TBool UpdateMessageCounter( TUint aResponseCode );

private: // Emumerations

	enum TFailureCount
		{
		// Maximum amount of authentication failures
		ESIPSecMaxAuthenticationFailuresAllowed = 5,

		EMaxSuccessive401ResponsesAllowed = 2
		};

private: // For internal use

    /**
    * Constructor
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    */		
    CSIPSecSIMRecord( const MSIPSecUser& aUser,
    				  TRegistrationId aRegistrationId,
    				  TTransactionId aTransactionId );
    
    /**
    * 2nd phase constructor
    * @param aRealm realm
    */
    void ConstructL( const TDesC8& aRealm );

    /**
    * Decodes Base64 encoded data
    */
    TInt DecodeBase64Encoded( const TDesC8& aEncoded, TDes8& aDecoded );

private: // Data

    TFailureType iFailureType;
    TInt 		 iFailureCount;

	// Owned
    HBufC8* iRAND;
    // Owned
    HBufC8* iAUTN;
    // Owned
    HBufC8* iAUTS;

	// Amount of 401 SIP responses that have been received in a row.
	TInt iSuccessive401Responses;

#ifdef CPPUNIT_TEST
	friend class CSIPSecSIMRecordTest;
	friend class CSIPSecUserCredentialsTest;
	friend class CSIPSecDigestTest;
#endif
	};

#endif // __SIPSEC_SIMRECORD_H__

// End of File
