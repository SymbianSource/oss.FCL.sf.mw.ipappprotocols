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
* Name        : csipsectlsplugin.h
* Part of     : SIPSec TLS Plugin
* Interface   : 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef C_SIPSECTLSPLUGIN_H
#define C_SIPSECTLSPLUGIN_H

// INCLUDES
#include "sipsecsecuritymechanism.h"

// FORWARD DECLARATIONS
class MSIPTransportMgr;
class CTLSCache;

// CLASS DECLARATION

/**
 * @brief This class implements TLS mechanism, and is an ECOM plugin
 */
class CSIPSecTlsPlugin : public CSIPSecSecurityMechanism 
    {
public: // Constructors and destructor

    /**
    * Constructs the object.
	* @param aInitParams Initialization parameters
    * @return Return New object, ownership is transferred.    
    */
    static CSIPSecTlsPlugin* NewL( TAny* aInitParams );
    
    /**
    * Destructor
    */
    ~CSIPSecTlsPlugin();

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

private: // Constructors, for internal use

	/*
     * Constructor
     * @param aTransportMgr Transport manager
     */
    CSIPSecTlsPlugin( MSIPTransportMgr& aTransportMgr );

    /*
     * Second phase constructor	 
     */
    void ConstructL();    

private: // New functions, for internal use

	void ProcessRequestL( TSIPTransportParams& aTransportParams,
						  const TInetAddr& aNextHop,	
						  const MSIPSecUser& aUser );

private: // Data

	//For handling TLS transports
    MSIPTransportMgr& iTransportMgr;

    //Owned
    CTLSCache* iCache;

#ifdef CPPUNIT_TEST
	friend class CSIPSecTlsPluginTest;
#endif
    };

#endif //end of C_SIPSECTLSPLUGIN_H

// End of File
