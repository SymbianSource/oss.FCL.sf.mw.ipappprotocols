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
* Name          : CSIPSecDigestPlugin.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPSEC_DIGESTPLUGIN_H__
#define __CSIPSEC_DIGESTPLUGIN_H__

// INCLUDES
#include "sipsecsecuritymechanism.h"

// FORWARD DECLARATIONS
class RStringF;
class CSIPSecDigest;
class CSIPSecDigestCache;
class CSIPSecUserRecord;
class CSIPAuthenticationInfoHeader;
class MTimerManager;
class CSIPSecurityVerifyHeader;
class CSIPAuthHeaderBase;
class CSIPAuthorizationHeader;
class MSIPSecEngineContext;
class CSIPMessage;

// CLASS DECLARATION

/**
 * @brief Class implements SIP-sec mechanism, and is ECOM plugin
 *
 * @class CSIPSecDigestPlugin CSIPSecDigestPlugin.h "CSIPSecDigestPlugin.h"
 */
class CSIPSecDigestPlugin : public CSIPSecSecurityMechanism 
    {
public: // Constructors and destructor

    /**
    * Constructs the object.
	* @param aInitParams Initialization parameters
    * @return Return New CSIPSecDigest object, ownership is transferred.    
    */
    static CSIPSecDigestPlugin* NewL( TAny* aInitParams );
    
    /**
    * Destructor
    */
    ~CSIPSecDigestPlugin();

public: // From MSIPSecSecurityMechanism

	const TDesC8& Name() const;

  	void InitializeSecurityClientL( CSIPSecurityClientHeader& aSecurityClient );

  	void ProcessSecurityVerifyL( TSIPTransportParams& aTransportParams,
  		CSIPRequest& aRequest,
		TInetAddr& aNextHop,
		const CUri8& aRemoteTarget,
		const TDesC8& aOutboundProxy,
		MSIPSecUser* aUser,
		TRegistrationId aRegistrationId,
		RPointerArray<CSIPSecurityServerHeader>& aSecurityServer,
		RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify );

	void AddSecurityParamsL( TSIPTransportParams& aTransportParams,
							 CSIPRequest& aRequest,
							 TRegistrationId aRegistrationId,
							 TTransactionId aTransactionId,
		                     TInetAddr& aNextHop,
		                     const CUri8& aRemoteTarget,
		                     const TDesC8& aOutboundProxy,
						     MSIPSecUser* aUser );

	TBool ResponseReceivedL( TSIPTransportParams& aTransportParams,
							 CSIPResponse& aResponse,
							 CSIPRequest& aRequest,
							 TRegistrationId aRegistrationId,
							 TTransactionId aTransactionId,
                             TInetAddr& aNextHop,
		                     const CUri8& aRemoteTarget,
		                     const TDesC8& aOutboundProxy,
						     MSIPSecUser* aUser,
						     MSIPSecSecurityMechanismObserver& aObserver );

	TBool IsServerInitiatedSecAgreeAllowed() const;

	TBool ParametersUpdatedL(MSIPSecUser* aUser);

	void CancelPendingOperations( MSIPSecSecurityMechanismObserver* aObserver );

	void ClearCache(MSIPSecUser* aUser);

	void SetCredentialsL( TTransactionId aTransactionId,
	                      const TDesC8& aRealm,
			              const TDesC8& aOutboundProxy, 
						  const TDesC8& aUserName,
						  const TDesC8& aPassword);

	void SetCredentialsL( const MSIPSecUser& aUser,
	                      const TDesC8& aRealm,
		                  const TDesC8& aOutboundProxy, 
					      const TDesC8& aUserName,
					      const TDesC8& aPassword );

	TInt IgnoreChallenge( TTransactionId aTransactionId,
	                      const TDesC8& aRealm,
	                      const MSIPSecUser* aTrustedUser );
	
	TInt RemoveCredentials( const TDesC8& aRealm );

public: // New functions

    /*
     * Returns engine context
     * @return SIPSec engine context
     */
	MSIPSecEngineContext& EngineContext();

	TBool EmptyResponseAfterSqnFailure() const;
	
private: // New functions, for internal use

    /*
     * Two phase constructor
	 * @param aTimerMgr a timer manager
     */
    void ConstructL( MTimerManager& aTimerMgr );

    /*
     * Constructor
     * @param aEngineContext SIPSec engine context
     */
    CSIPSecDigestPlugin( MSIPSecEngineContext& aEngineContext );

    /*
     * Returns authentication info from response, if any.
	 * @param aResponse the response
     * @return Authentication info, ownership is not transferred.
     */
    CSIPAuthenticationInfoHeader*
    	AuthenticationInfo( CSIPResponse& aResponse ) const;

    /*
     * Prepares outgoing request by removing all headers, which must be removed
	 * @param aSIPRequest SIP request
	 * @return Authorization header, which was set by digest, or NULL.
	 *	Ownership is not transferred.
     */
    CSIPAuthorizationHeader* PrepareRequestL( CSIPRequest& aSIPRequest ) const;

	/*
     * Set credentials to the user record and update cache.
	 * @param aRecord User record to be updated
	 * @param aOutboundProxy Outbound proxy
	 * @param aUserName Username
	 * @param aPassword Password
     */
	void SetCredentialsToRecordL( CSIPSecUserRecord& aRecord,
								  const TDesC8& aOutboundProxy,
                           		  const TDesC8& aUserName,
                           		  const TDesC8& aPassword );

private: // Data

    // Digest, owned.
    CSIPSecDigest* iDigestMechanism;

    // Cache, owned.
    CSIPSecDigestCache* iCache;

    // SIPSec Engine context
    MSIPSecEngineContext& iEngineContext;

	// If ETrue, an empty Response-parameter is sent if AKA authentication
	// results an SQN failure.
	// If EFalse, the Response-parameter is computed using an empty password,
	// if AKA authentication results an SQN failure.
	TBool iSendEmptyResponseParameterAfterSqnFailure;

	// For testing purposes        
#ifdef CPPUNIT_TEST
	friend class CSIPSecDigestTest;
	friend class CSIPSecCredentialsTest;
	friend class CSIPSecUserCredentialsTest;
	friend class CSIPSecResolverTest;
	friend class CSIPSecRequestDataTest;
	friend class CSIPSecSIMCredentialsProviderTest;
#endif
    };

#endif //end of __CSIPSEC_DIGESTPLUGIN_H__

// End of File
