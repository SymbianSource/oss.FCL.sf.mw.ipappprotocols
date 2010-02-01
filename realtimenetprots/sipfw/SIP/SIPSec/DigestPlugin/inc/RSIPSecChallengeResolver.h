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
* Name          : RSIPSecChallengeResolver.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __RSIPSEC_CHALLENGERESOLVER_H__
#define __RSIPSEC_CHALLENGERESOLVER_H__

// INCLUDES
#include <e32base.h>
#include "CSIPSecDigest.h"
#include "sipsecrequestdata.h"

// FORWARD DECLARATIONS
class MSIPSecUser;
class CSIPAuthHeaderBase;
class CSIPAuthenticateHeaderBase;
class CSIPSecChallenge;
class CSIPSecCredentials;
class TSIPSecPluginCtxResponse;
class RStringF;
class CSIPHeaderBase;
class CSIPResponse;

// RPointerArray doesn't have a destructor, and RSIPSecChallengeResolver
// doesn't have either.
//lint -e1510
			
/**
 * @brief Class represents stack of received challenges
 * 
 * @class RSIPSecChallengeResolver RSIPSecChallengeResolver.h
 *		  "RSIPSecChallengeResolver.h"
 *
 * This class extracts challenges from 401/407 response and stores them into a
 * form of stack. Challenges can then be handled one-by-one. After extraction
 * cache has been updated because of possible re-challenges. 
 */
class RSIPSecChallengeResolver : public RPointerArray< CSIPSecChallenge >
    {
public: // Constructors and destructor

    /*
     * Constructor
     * @param aContext SIP response context
     * @param aMechanism the mechanism
     */
    RSIPSecChallengeResolver( TSIPSecPluginCtxResponse& aContext,
    						  CSIPSecDigest& aMechanism );

public: // New functions

    /*
     * Opens resolver and creates challenges
     * @param aUser SIPSec user
     */
    void OpenL( const MSIPSecUser& aUser );

    /*
     * Closes resolver
     */
    void Close();
    
    /*
     * Pops a challenge.     
     * @return Challenge or NULL if no more challenges exist.
     *	Ownership is transferred.
     */
    CSIPSecChallenge* Pop();
    
    /*
     * If aResponse has a Security-Server header with mechanism Digest, the
     * values of algorithm and qop parameters from that header are copied into
     * aAlgorithm and aQop.
     * @param aResponse IN: SIP response
     * @param aAlgorithm OUT: algoritm value
     * @param aQop OUT: qop value
     * @return ETrue if aResponse has a Security-Server header with digest
     *		   mechanism. EFalse otherwise.
     */
	static TBool GetDigestParamsFromSecurityServer( CSIPResponse& aResponse,
												    RStringF& aAlgorithm,
												    RStringF& aQop );

	/*
     * Check if the header has a Digest challenge with realm and nonce.
     * @param aHeader Authenticate- or Authorization-header
     * @return ETrue Header has a valid challenge, EFalse otherwise
     */	
	static TBool IsValidDigestChallenge( const CSIPAuthHeaderBase& aHeader );

	/*
     * Selects the algorithm to use for the challenge in aHeader.
     * @param aHeader Header containing the challenge
     * @param aDefaultAlgorithm The used default algorith
     * @param aAlgorithm IN: algorithm in SIP response's Security-Server header
     *					 OUT: selected algorithm     
     */
	static void SelectAlgorithm( const CSIPAuthHeaderBase& aHeader,
								 RStringF aDefaultAlgorithm,
								 RStringF& aAlgorithm );

private: // New functions, for internal use

    /*
     * Finds and creates challenge based on SIP header
     * @param aAuthHeaders Authenticate headers
     * @param aType The challenge type
     * @param aCount Cumulative count of found challenges
     * @param aAlgorithm Algorithm value from Security-Server header, or empty
     *		  string if algorithm was not present in Security-Server
     * @param aQop qop value from Security-Server header, or empty string if
     *		  qop was not present in Security-Server
     * @param aDigestInAuthHeaders on return contains ETrue if some of the
     *        auth headers contained digest
     */
    void FindAndCreateChallengesL( TSglQueIter< CSIPHeaderBase >& aAuthHeaders,
                                   CSIPSecDigest::TChallengeType aType,
                                   TInt& aCount,
                                   RStringF aAlgorithm,
                                   RStringF aQop,
                                   TBool& aDigestInAuthHeaders );

    /*
     * Creates a challenge based on the SIP header.
     * @param aHeader authenticate header
     * @param aType The challenge type
     * @param aCount Cumulative count of found challenges
     * @param aAlgorithm Algorithm value from Security-Server header, or empty
     *		  string if algorithm was not present in Security-Server
     * @param aQop qop value from Security-Server header, or empty string if
     *		  qop was not present in Security-Server     
     */
    void CreateChallengeL( CSIPAuthenticateHeaderBase& aHeader,
                           CSIPSecDigest::TChallengeType aType,
                           TInt& aCount,
                           RStringF aAlgorithm,
						   RStringF aQop );

    /*
     * Prepares cache
     * @param aUser SIPSec user
     */
    void PrepareCache( const MSIPSecUser& aUser );	

	/*
     * Selects the qop to use with the challenge in aHeader.     
     * @param aHeader Authenticate header containing the challenge     
     * @param aQopInSecurityServer qop value from Security-Server header     
     * @return The selected qop to use
     */
	CSIPSecRequestData::TQop SelectQopL( CSIPAuthenticateHeaderBase& aHeader,
					  					 RStringF aQopInSecurityServer ) const;

	
	/*
     * Check if the header has a Digest challenge
     * @param aHeader Authenticate- or Authorization-header
     * @return ETrue Header has a challenge, EFalse otherwise
     */
	static TBool IsDigestChallenge( const CSIPAuthHeaderBase& aHeader );
	
	/*
     * Check if the header has realm and nonce.
     * @param aHeader Authenticate- or Authorization-header
     * @return ETrue Header has realm and nonce, EFalse otherwise
     */
	static TBool IsValidChallenge( const CSIPAuthHeaderBase& aHeader );
	
	/*
     * Check if either credentials or challenge has MD5 algorithm and the other
     * has AKAv1-MD5.
     * @param aCredentials IN: credentials in the cache
     * @param aChallenge IN: received challenge
     * @return ETrue If either credentials or challenge has MD5 algorithm and
     *	the other has AKAv1-MD5.
     */
	TBool MixedAlgorithms( CSIPSecCredentials& aCredentials,
						   CSIPSecChallenge& aChallenge ) const;

	/*
     * If there is an AKA challenge, remove such AKA entries from the cache, that
     * have different realm than in the challenge.
     */
	void RemoveObsoleteAKAEntries() const;

private: // Data

    // Not owned
    TSIPSecPluginCtxResponse* iContext;

    CSIPSecDigest& iMechanism;

#ifdef CPPUNIT_TEST
	friend class CSIPSecResolverTest;
#endif
    };

#endif //end of __RSIPSEC_CHALLENGERESOLVER_H__

// End of File
