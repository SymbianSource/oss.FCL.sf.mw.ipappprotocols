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
* Name          : CSIPSecCredentials.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPSEC_CREDENTIALS_H__
#define __CSIPSEC_CREDENTIALS_H__

// INCLUDES
#include "CSIPSecDigestCacheEntry.h"

// FORWARD DECLARATIONS
class CSIPAuthorizationHeaderBase;
class CSIPSecChallenge;
class CSIPSecDigest;
class TSIPSecDigestCtxSetup;
class TSIPSecPluginCtxResponse;
class TSIPSecDigestCtxProcess;

// CLASS DECLARATION
/**
 * @brief Class representing credentials created based on challenge received in
 * a 401/407 response
 * 
 * @class CSIPSecCredentials CSIPSecCredentials.h "CSIPSecCredentials.h"
 *
 * This class provides functions to
 * - update, because of SIP request sending with these credentials.
 *   In this case response is calculated.
 * - update, because of re-challenge, which causes update of user credentials 
 * - handle authentication info header
 * - handle security verify header
 */
class CSIPSecCredentials : public CSIPSecDigestCacheEntry
    {
public: // Constructors and destructor

    /**
    * Constructs the object. Leaves on Failure
    * @param aChallenge Challenge, ownership is transferred.
    * @param aMechanism the mechanism
    * @param aContext Response context
    * @return Return A new CSIPSecCredentials object, ownership is transferred.
    */
    static CSIPSecCredentials* NewL( CSIPSecChallenge* aChallenge,
                                     CSIPSecDigest& aMechanism,
                                     TSIPSecPluginCtxResponse& aContext );

    /**
    * Destructor
    */
    ~CSIPSecCredentials();

public: // From CSIPSecDigestCacheEntry

    void UpdateL( CSIPSecUserRecord& aUserCredentials );

    void UpdateL( TSIPSecPluginCtxRequest& aContext );

    void HandleL( TSIPSecPluginCtxInfo& aAuthenticationInfo );

    void HandleL( TSIPSecPluginCtxVerify& aSecurityVerifyCtx );

    CSIPSecDigest::TChallengeType Type() const;
    
    CSIPSecDigest::TChallengeType
    	Type( const TSIPSecPluginContext& aContext ) const;

public: // New functions

    /**
    * Returns type of challenge
    * @param aChallenge The challenge who's type is requested
    * @param aContext Context in which the challenge type is resolved
    * @return Type of aChallenge
    */
    CSIPSecDigest::TChallengeType
		ChallengeType( const CSIPSecChallenge& aChallenge,
		               const TSIPSecPluginContext* aContext = 0 ) const;

    /**
    * Returns content (SIP proxy/WWW authorization header)
    * @return Content
    */
    CSIPAuthorizationHeaderBase& Content();

    /**
    * Updates entry based on a SIP response 
    * @param aContext Response context
    * @param aAskCredentials OUT: ETrue If the credentials for the challenge(s)
    *		 present in the current SIP response, must be asked now.
    *		 EFalse If there is no need to ask the credentials, and maybe they
    *	     are already being asked for due to some earlier SIP response.
    * @return ETrue If credentials must be obtained, EFalse otherwise
    */
    TBool DigestUpdateL( TSIPSecDigestCtxSetup& aContext,
    					 TBool& aAskCredentials );

    /**
    * Handles new challenge reception and determines action
    * @param aNewChallenge New challenge that has been received
    * @return ETrue If this credentials instance must be removed,
    *		  EFalse otherwise
    */
    TBool ChallengeReceived( CSIPSecChallenge& aNewChallenge );

    /**
    * Returns challenge
    * @return Challenge
    */
    CSIPSecChallenge& Challenge();

    /**
    * Returns mechanism
    * @return Mechanism
    */
    CSIPSecDigest& Mechanism() const;

	/**
    * Get the qop value
    * @return Qop value, or empty descriptor if qop is undefined
    */
	const TPtrC8 Qop() const;

	/**
    * Compares aChallenge to the challenge in this CSIPSecCredentials object.
    * @param aChallenge Challenge
    * @param aUser SIPSec user processing SIP response where the aChallenge is
    *	received.
    * @param aRegistrationId Registration id
    * @return ETrue If the challenges match
    *		  EFalse otherwise
    */
	TBool DoesMatch( const CSIPSecChallenge& aChallenge,
					 const MSIPSecUser& aUser,
					 TRegistrationId aRegistrationId ) const;

private: // Constructors, for internal use

	/*
     * Constructor
     * @param aMechanism the mechanism     
     * @param aUser SIPSec user
     */
    CSIPSecCredentials( CSIPSecDigest& aMechanism, const MSIPSecUser& aUser );

    /*
     * Second phase constructor
     * @param aChallenge Challenge, ownership is transferred.
     * @param aContext Response context     
     */
    void ConstructL( CSIPSecChallenge* aChallenge,
    				 TSIPSecPluginCtxResponse& aContext );    

private: // New functions, for internal use

    /*
     * Creates the content
     */
    void CreateContentL( CSIPSecChallenge& aChallenge );

    /*
     * Updates content
     * @param aContext Digest context
     */
    void UpdateContentL( TSIPSecDigestCtxSetup& aContext );

    /*
     * Updates content
     * @param aContext Digest context
     */
    void UpdateContentL( TSIPSecDigestCtxProcess& aContext );

    /*
     * Sets nonce count
     * @param aNonceCount Nonce count
     */
    void SetNonceCountL( TUint aNonceCount );

private: // Data

    // Owned
    CSIPSecChallenge* iChallenge;

    // Owned
    CSIPAuthorizationHeaderBase* iAuthorizationHeader;

    TUint iNonceCount;

    CSIPSecDigest& iMechanism;

	// For testing purposes
#ifdef CPPUNIT_TEST
    friend class CSIPSecCredentialsTest;
#endif
    };

#endif // end of __CSIPSEC_CREDENTIALS_H__

// End of File
