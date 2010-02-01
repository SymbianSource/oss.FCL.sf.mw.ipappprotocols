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
* Name          : CSIPSecDigest.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPSEC_DIGEST_H__
#define __CSIPSEC_DIGEST_H__

// INCLUDES
#include "SipStackServerDefs.h"
#include <e32base.h>
#include <stringpool.h>

// CONSTANTS
_LIT8( KSIPSecDigestName, "digest");
_LIT8( KSIPSecDigestScheme, "Digest");
_LIT8( KSIPSecAuthInt, "auth-int");
_LIT8( KSIPSecAuth, "auth");

const TInt KSIPSecDigestHashHexSize( 32 );

// FORWARD DECLARATIONS
class RStringF;
class TSIPSecPluginCtxResponse;
class CMD5;
class CSIPSecurityClientHeader;
class CSIPSecurityServerHeader;

// CLASS DECLARATION
/**
 * @brief Class represents digest mechanism.
 * 
 * @class CSIPSecDigest CSIPSecDigest.h "CSIPSecDigest.h"
 *
 * This class provides functions to handle security negotiation
 * and cache update when response has been received
 */
class CSIPSecDigest : public CBase
    {
public: // Enumerations

    enum TChallengeType
        {
        EOutboundProxy = 0,
        EProxy         = 1,
        EEndPoint      = 2
        };

public: // Constructors and destructor

    /**
    * Constructs the object.
    * @return New CSIPSecDigest object, ownership is tranferred.
    */
    static CSIPSecDigest* NewL();

    /**
    * Destructor
    */
    ~CSIPSecDigest();

public: // New functions

  	/**
  	 * Offers digest parameter in sec-agree negotiation
	 * @param aSecurityClient Security client header
	 */
    void OfferL( CSIPSecurityClientHeader& aSecurityClient );

  	/**
  	 * Initialiazes mechanism when it has been selected	 
	 * @param aSecurityServer Security server header
	 */
    void InitializeL( CSIPSecurityServerHeader& aSecurityServer );
    
  	/**
  	 * Gets the name of the mechanism
	 *
	 * @return Name of the mechanism
	 */
	const TDesC8& Name() const;

	/**
	 * Updates cache based on an incoming SIP response.	 
	 * @param aContext Response context
	 * @return ETrue Asynchronous operation has been started, and must wait for
	 *         it to complete the action.
	 */
    TBool UpdateCacheL( TSIPSecPluginCtxResponse& aContext );

	/**
	 * Returns message digest functionality
	 * @return Message digest
	 */
    CMD5& MessageDigest();

  	/**
  	 * Default algorithm	 
	 * @return Name of the algorithm
	 */	
	RStringF Algorithm() const;

  	/**
  	 * Sets default algorithm
	 * @param aAlgorithm name of algorithm
	 */
	void SetDefaultAlgorithm( RStringF aAlgorithm );

private: // New functions, for internal use

    /*
     * Constructor
     */
    CSIPSecDigest();

    /*
     * Second phase constructor
     */
    void ConstructL();

private: // Data

	// Owned
    CMD5* iMessageDigest;

    RStringF iAlgorithm;

	TBool iStringPoolOpened;
    };
	
#endif //end of __CSIPSEC_DIGEST_H__

// End of File
