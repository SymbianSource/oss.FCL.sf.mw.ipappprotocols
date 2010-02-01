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
* Name          : CSIPSecChallengeAKA.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPSEC_CHALLENGEAKA_H__
#define __CSIPSEC_CHALLENGEAKA_H__

#include "CSIPSecChallengeMD5.h"


/**
 * @brief Class representing AKA challenge received in 401/407 response
 * 
 * @class CSIPSecChallengeAKA CSIPSecChallengeAKA.h "CSIPSecChallengeAKA.h"
 *
 * This class extends MD5 challenge functionality
 * 
 * Class provides functions to process challenge and 
 * populate AKA specifig values to credentials 
 *
 */
class CSIPSecChallengeAKA : public CSIPSecChallengeMD5
    {
public: // Constructors and destructor

    /**
    * Constructs the object, and pushes it to cleanupstack.
    * @param aType The type of challenge
    * @param aAuthenticateHeader SIP authenticate header
    * @param aQop qop of the challenge
    * @return Return New CSIPSecChallengeAKA object
    */
    static CSIPSecChallengeAKA*
    	NewLC( CSIPSecDigest::TChallengeType aType,
    	       CSIPAuthenticateHeaderBase& aAuthenticateHeader,
    	       CSIPSecRequestData::TQop aQop );

    /**
    * Destructor
    */
    ~CSIPSecChallengeAKA();

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

public: // New functions

    /**
    * The name of the supported algorithm
    * @return The name of the supported algorithm
    */
    static RStringF SupportedAlgorithm();


protected: // Constructor, for internal use

    /*
     * Constructor
     * @param aType the type of challenge
     * @param aQop qop of the challenge
     */
    CSIPSecChallengeAKA( CSIPSecDigest::TChallengeType aType,
    					 CSIPSecRequestData::TQop aQop );
    };
	
#endif //end of __CSIPSEC_CHALLENGEAKA_H__

// End of File
