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
* Name          : CSIPSecChallengeMD5.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPSEC_CHALLENGEMD5_H__
#define __CSIPSEC_CHALLENGEMD5_H__

// INCLUDES
#include "CSIPSecChallenge.h"
#include "MSIPSecAlgorithm.h"

// CONSTANTS

// CLASS DECLARATION

/**
 * @brief Class representing MD5 challenge received in 401/407 response
 * 
 * @class CSIPSecChallengeMD5 CSIPSecChallengeMD5.h "CSIPSecChallengeMD5.h"
 *
 * This class provides to means to process MD5 challenge, which includes
 * calculating digest response based  on challenge values and data in outgoing
 *  SIP request.
 * Class also provides function to calculate secret data, which is hashed value
 * of concatenation of username, realm and password. 
 */
class CSIPSecChallengeMD5 : public CSIPSecChallenge,
                            public MSIPSecAlgorithm
    {
public: // Constructors and destructor

    /**
    * Constructs the object, and pushes it to cleanupstack.
    * @param aType the type of challenge
    * @param aAuthenticateHeader SIP authenticate header
    * @param aQop qop of the challenge
    * @return Return the CSIPSecChallengeMD5 object    
    */
    static CSIPSecChallengeMD5*
    	NewLC( CSIPSecDigest::TChallengeType aType,
    	       CSIPAuthenticateHeaderBase& aAuthenticateHeader,
    	       CSIPSecRequestData::TQop aQop );

    /**
    * Destructor
    */
    ~CSIPSecChallengeMD5();

public: // From CSIPSecChallenge

    inline MSIPSecAlgorithm& Algorithm() 
        {
        return *this;
        }

	void VerifyTypeL( CSIPSecUserRecord::TType aType ) const;

	CSIPSecUserRecord* CreateUserRecordL( const TDesC8& aRealm,
										  const MSIPSecUser& aUser,
										  TRegistrationId aRegistrationId,
									      TTransactionId aTransactionId,
									      TUint aResponseCode ) const;

public: // From MSIPSecAlgorithm

    RStringF AlgorithmName() const;
    
    TBool ProcessResponseL( TSIPSecDigestCtxSetup& aContext,
    			 			TBool& aAskCredentials ) const;

    void ProcessRequestL( TSIPSecDigestCtxProcess& aContext ) const;

    void GenerateCNonceL( TSIPSecDigestCtxSetup& aContext ) const;

public: // New functions

    /**
    * The name of the supported algorithm
    * @return The name of the supported algorithm
    */
    static RStringF SupportedAlgorithm();

protected: // New functions, for internal use

	/*
     * Constructor
     * @param aType the type of challenge
     * @param aQop qop of the challenge
     */
    CSIPSecChallengeMD5( CSIPSecDigest::TChallengeType aType,
    					 CSIPSecRequestData::TQop aQop );

    /**
    * Calculates and returns "request-digest".
    * @param aContext Digest context
    * @return "request-digest"
    */
    HBufC8* RequestDigestL( TSIPSecDigestCtxProcess& aContext ) const;
    
    /**
    * Calculates secret data (A1).
    * @param aContext Digest context
    */
    void CalculateSecretDataA1L( TSIPSecDigestCtxSetup& aContext ) const;

    /**
    * Calculates request data (A2).
    * @param aContext Digest context
    */
    HBufC8* CalculateA2L( TSIPSecDigestCtxProcess& aContext ) const;

	/*
	* Converts the MD5 hash descriptor into an ASCII representation of the
	* hexadecimal values.	
	* @param aDigest Message digest
	* @param aData Data to hash
	* @return Buffer of characters representing the hexadecimal values
	*  ('0'..'9','a'..'f'). Ownership is transferred.
	*/
    HBufC8* HashL( CMD5& aDigest, TPtrC8 aData ) const;    
    };
	
#endif //end of __CSIPSEC_CHALLENGEMD5_H__

// End of File
