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
* Name          : CSIPSecDigestCacheEntry.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_DIGESTCACHEENTRY_H__
#define __SIPSEC_DIGESTCACHEENTRY_H__

// INCLUDES
#include <e32base.h>
#include "CSIPSecDigest.h"

// FORWARD DECLARATIONS
class CUri8;
class MSIPSecUser;
class TSIPSecPluginCtxInfo;
class TSIPSecPluginCtxRequest;
class TSIPSecPluginCtxVerify;
class TSIPSecPluginContext;
class CSIPSecUserRecord;


// CLASS DECLARATION
/**
 * @brief Class represents cache entry
 * 
 * @class CSIPSecDigestCacheEntry CSIPSecDigestCacheEntry.h
 * "CSIPSecDigestCacheEntry.h"
 *
 * This class is an abstract base class for credentials
 * It provides functions to assing user credentials and SIP user.
 * It also contains logic to determine, if it should be included in outgoing
 * SIP request 
 */
class CSIPSecDigestCacheEntry : public CBase
	{
public:	// Destructor

    /**
    * Destructor
    */	
    virtual ~CSIPSecDigestCacheEntry();

public:	// New pure virtual functions

    /**
    * Updates entry when a SIP request is passed to SIPSec.
    * @param aContext Request context    
    */
    virtual void UpdateL( TSIPSecPluginCtxRequest& aContext ) = 0;

    /**
    * Updates entry when client sets credentials.
    * @param aUserCredentials User record
    */
    virtual void UpdateL( CSIPSecUserRecord& aUserCredentials )  = 0;

    /**
    * Handles authentication info
    * @param aAuthenticationInfo authentication info context
    */
    virtual void HandleL( TSIPSecPluginCtxInfo& aAuthenticationInfo ) = 0;

    /**
    * Handles security verify
    * @param aSecurityVerifyCtx Security verify context
    */
    virtual void HandleL( TSIPSecPluginCtxVerify& aSecurityVerifyCtx ) = 0;

    /**
    * Returns type of challenge
    * @return Type of challenge
    */
    virtual CSIPSecDigest::TChallengeType Type() const = 0;
    
    /**
    * Returns type of challenge, taking in account the context.
    * @param aContext context
    * @return Type of challenge
    */
    virtual CSIPSecDigest::TChallengeType
    	Type( const TSIPSecPluginContext& aContext ) const = 0;

public:	// New functions

    /**
    * Returns user data
    * @return User data
    */
    CSIPSecUserRecord& UserData();

    /**
    * Sets user data
    * @param aUserData User data
    */
    void SetUserData( CSIPSecUserRecord& aUserData );

    /**
    * Checks, if this entry holds asked user data
    * @param aUserData User data
    */
    TBool HoldsUserData( const CSIPSecUserRecord& aUserData ) const;
    
    /**
    * Checks if this should be included to outgoing request contained in
    * context.
    * @param aContext Context
    * @return ETrue if entry should be included in the SIP request,
    *		  EFalse otherwise
    */
    TBool Match( TSIPSecPluginContext& aContext ) const;    

    /**
    * Checks if the specified SIPSec user is same as in this instance.
    * @param aUser SIPSec user
    * @param aAlsoTrustedUser If ETrue and aUser does not match the SIPSec user
    *	of this instance, then aUser's trusted user is compared to the SIPSec
    *	user of this CSIPSecDigestCacheEntry.
    * @param RegistrationId Registration id
    * @return ETrue  SIPSec user is same
    *		  EFalse Otherwise
    */
    TBool IsUsedByUser( const MSIPSecUser& aUser,
    					TBool aAlsoTrustedUser,
    					TRegistrationId aRegistrationId ) const;

	/**
    * Compares the SIPSec user to a cached SIPSec user.
    * @param aUser SIPSec user
    * @param aCachedUser Cached SIPSec user
    * @param aAlsoTrustedUser If ETrue and aUser does not match the SIPSec user
    *	of this instance, then aUser's trusted user is compared to the SIPSec
    *	user of this CSIPSecDigestCacheEntry.
    * @param RegistrationId Registration id
    * @return ETrue  If the SIPSec user match
    *		  EFalse Otherwise, or if either aUser or aCachedUser is NULL
    */
	static TBool CompareUsers( const MSIPSecUser& aUser,
							   const MSIPSecUser& aCachedUser,
							   TBool aAlsoTrustedUser,
							   TRegistrationId aRegistrationId );

	/**
    * Get the SIPSec user
    * @return MSIPSecUser& SIPSec user
    */
	const MSIPSecUser& SIPSecUser() const;
	
public:	// Data

    static const TInt iSIPSecOffset;

    TSglQueLink iLink;

protected: // Constructors

    /**
    * Constructor
    * @param aUser SIPSec user
    */
	CSIPSecDigestCacheEntry( const MSIPSecUser& aUser );

    /**
    * Second phase constructor
    * @param aType Challenge type
    * @param aRemoteTarget Remote target    
    */
	void ConstructL( CSIPSecDigest::TChallengeType aType,
					 const CUri8& aRemoteTarget );

protected: // Data

    // User credentials, not owned.
    CSIPSecUserRecord* iUserCredentials;

	// SIPSec user
	const MSIPSecUser& iUser;

private: // Data

    // Remote target, owned.
    CUri8* iRemoteTarget;
	};

#endif //__SIPSEC_DIGESTCACHEENTRY_H__

// End of File
