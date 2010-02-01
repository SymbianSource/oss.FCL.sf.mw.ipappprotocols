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
* Name          : CSIPSecChallenge.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPSEC_CHALLENGE_H__
#define __CSIPSEC_CHALLENGE_H__

// INCLUDES
#include "CSIPSecDigest.h"
#include "sipsecrequestdata.h"
#include "CSIPSecUserRecord.h"
#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class CSIPAuthenticateHeaderBase;
class MSIPSecAlgorithm;
class TSIPSecDigestCtxSetup;
class MSIPSecUser;


// CLASS DECLARATION

/**
 * @brief Class representing challenge received in 401/407 response
 * 
 * @class CSIPSecChallenge CSIPSecChallenge.h "CSIPSecChallenge.h"
 *
 * This class is a base class for challenge, whose inherited classes implement
 * functionality of algotithm defined in challenge.
 * Class provides functions for populating common values to credentials
 * (created from this challenge)
 * This class also provides functions for resolving type of challenge, realm
 * and content (SIP authenticate header)
 *
 * Inherited class must implement methods:
 * - Algorithm, which provides digest calculation interface
 * - VerifyType, that checks the challenge type is correct
 */
 class CSIPSecChallenge : public CBase 
    {
public: // Constructors and destructor

    /**
    * Destructor
    */
    virtual ~CSIPSecChallenge();

public: // New pure virtual functions

    /**
    * Returns algorithm
    * @return Algorithm
    */
    virtual MSIPSecAlgorithm& Algorithm() = 0;

	/**
    * Checks that the type is correct.
    * @param aType Record type
    * @leave KErrPermissionDenied if aType is not correct.
    */
	virtual void VerifyTypeL( CSIPSecUserRecord::TType aType ) const = 0;

	/**
    * Creates a new CSIPSecUserRecord.
    * @param aRealm Realm
    * @param aUser SIPSec user who is processing the SIP response that leads to
    *		 	   creating a new user record.
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id of the SIP response
    * @param aResponseCode Response code of SIP response
    */
	virtual CSIPSecUserRecord*
		CreateUserRecordL( const TDesC8& aRealm,
						   const MSIPSecUser& aUser,
						   TRegistrationId aRegistrationId,
			      		   TTransactionId aTransactionId,
			      		   TUint aResponseCode ) const = 0;

public: // New functions

	/**
    * Populates credentials
    * @param aContext Digest context    
    */
    void PopulateCredentialsL( TSIPSecDigestCtxSetup& aContext ) const;

    /**
    * Returns the type of challenge
    * @return Type of challenge
    */
    CSIPSecDigest::TChallengeType Type() const;

    /**
    * Returns the realm of challenge
    * @return Realm
    */
    const TDesC8& Realm() const;

    /**
    * Returns a modifiable content of the challenge (SIP authenticate header).
    * @return Content of challenge
    */
    CSIPAuthenticateHeaderBase& Content();

    /**
    * Returns a non-modifiable content of the challenge (SIP authenticate
    * header).
    * @return Content of challenge
    */
    const CSIPAuthenticateHeaderBase& Content() const;		

	/**
    * Check if the qop exists
    * @return ETrue if qop exists, EFalse otherwise
    */
	TBool HasQop() const;

	/**
    * Gets the qop as descriptor.
    * @return qop as descriptor, or empty descriptor if qop doesn't exist
    */
	const TDesC8& QopDescriptor() const;

	/**
    * Gets the owner of this challenge
    * @return Owner of the challenge. Ownership is not transferred.
    */
    const MSIPSecUser* Owner() const;

	/**
    * Sets the owner of this challenge
    * @return Owner. Ownership is not transferred.
    */
    void SetOwner( const MSIPSecUser* aOwner );

protected: //methods

    /**
    * Constructor
    * @param aType the type of challenge
    * @param aQop qop of the challenge
    */
    CSIPSecChallenge( CSIPSecDigest::TChallengeType aType,
    				  CSIPSecRequestData::TQop aQop );
    
    /**
	* Second phase constructor
	* @param aAuthenticateHeader authenticate header 
	*/
	void ConstructL( CSIPAuthenticateHeaderBase& aAuthenticateHeader );
     
    /**
    * Gets the qop value.
    * @return qop value
    */
	CSIPSecRequestData::TQop Qop() const;	

private:

	/**
    * Copies the parameter from challenge to authorization header.
    * @param aContext Digest context
    * @param aParam String pool constant of the parameter name    
    */
	void FillCredentialParamL( TSIPSecDigestCtxSetup& aContext,
							   TInt aParam ) const;

private: // Data

	// Owned
    CSIPAuthenticateHeaderBase* iAuthenticateHeader;

    CSIPSecDigest::TChallengeType iType;

	// Qop to be used for the challenge.	
	CSIPSecRequestData::TQop iQop;

    // Owner of this challenge. Not owned.
	const MSIPSecUser* iOwner;

	// For testing purposes
#ifdef CPPUNIT_TEST
    friend class CSIPSecChallengeTest;
#endif
    };

#endif //end of __CSIPSEC_CHALLENGE_H__

// End of File
