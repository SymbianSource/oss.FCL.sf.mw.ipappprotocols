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
* Name          : sipsecplugincontext.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_PLUGINCONTEXT_H__
#define __SIPSEC_PLUGINCONTEXT_H__

// INCLUDES
#include "Lwtimer.h"
#include "SipStackServerDefs.h"
#include <e32std.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class CUri8;
class MSIPSecSecurityMechanismObserver;
class MSIPSecUser;
class CSIPSecDigestCache;
class CSIPRequest;
class CSIPResponse;
class CSIPSecurityVerifyHeader;
class CSIPSecurityServerHeader;
class CSIPAuthenticationInfoHeader;
class CSIPSecDigestPlugin;
class CSIPHeaderBase;

// CLASS DECLARATION
/**
 * @brief Class is base class for plugin context
 * 
 * @class TSIPSecPluginContext sipsecplugincontext.h "sipsecplugincontext.h"
 *
 * Class provides common context data of response or request
 */
class TSIPSecPluginContext
	{	
public: // Constructors and destructor

    /**
    * Constructor
    * @param aPlugin Digest plugin    
    * @param aRemoteTarget Remote target
    * @param aOutboundProxy outbound proxy
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id
    */
    TSIPSecPluginContext( CSIPSecDigestPlugin& aPlugin,                          
                          const CUri8& aRemoteTarget,
                          const TDesC8& aOutboundProxy,
                          MSIPSecUser& aUser,
                          TRegistrationId aRegistrationId,
                          TTransactionId aTransactionId );

	/**
    * Destructor
    */
    inline virtual ~TSIPSecPluginContext()
        {
        }

public: // New functions

	virtual TBool AlsoTrustedUser();

public: // SIP related functions

    /**
    * Returns the remote target
    * @return remote target
    */
    const CUri8& RemoteTarget() const;

    /**
    * Returns the outbound proxy
    * @return outbound proxy
    */
    const TDesC8& OutboundProxy() const;

    /**
    * Returns the SIPSec user
    * @return SIPSec user
    */
    MSIPSecUser& SIPSecUser();

    /**
    * Returns plugin
    * @return plugin
    */
    CSIPSecDigestPlugin& Plugin() const;

	/**
    * Returns registration id.
    * @return Registration id
    */
    TRegistrationId RegistrationId() const;

    /**
    * Returns transaction id.
    * @return Transaction id
    */
    TTransactionId TransactionId() const;

private: // Data

    CSIPSecDigestPlugin& iPlugin;

	//Remote target
    const CUri8& iRemoteTarget;

    const TDesC8& iOutboundProxy;
    MSIPSecUser& iUser;
	TRegistrationId iRegistrationId;
    TTransactionId iTransactionId;
    };



/**
 * @brief Class is plugin context with response
 * 
 * @class TSIPSecPluginCtxResponse sipsecplugincontext.h "sipsecplugincontext.h"
 *
 * Class provides, in addition to common context, SIP response
 */
class TSIPSecPluginCtxResponse : public TSIPSecPluginContext
	{	
public: // Constructor

    /**
    * Constructor
    * @param aPlugin Digest plugin    
    * @param aRemoteTarget Remote target
    * @param aOutboundProxy Outbound proxy
    * @param aSIPResponse SIP response
    * @param aCache Cache
    * @param aObserver Mechanism observer
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id
    */
    TSIPSecPluginCtxResponse( CSIPSecDigestPlugin& aPlugin,
                              const CUri8& aRemoteTarget,
                              const TDesC8& aOutboundProxy,
                              CSIPResponse& aSIPResponse,
                              CSIPSecDigestCache& aCache,
                              MSIPSecSecurityMechanismObserver& aObserver,
                              MSIPSecUser& aUser,
                              TRegistrationId aRegistrationId,
                              TTransactionId aTransactionId );

public: // SIP related functions

    /**
    * Returns cache
    * @return cache
    */
    CSIPSecDigestCache& Cache();

    /**
    * Returns observer
    * @return observer
    */
    MSIPSecSecurityMechanismObserver& Observer();

    /**
    * Returns SIP response
    * @return SIP response
    */
    CSIPResponse& SIPResponse();

private: // Data

    CSIPSecDigestCache& iCache;

    MSIPSecSecurityMechanismObserver& iObserver;

    CSIPResponse& iSIPResponse;
    };
	



/**
 * @brief Class is plugin context with request
 * 
 * @class TSIPSecPluginCtxRequest sipsecplugincontext.h "sipsecplugincontext.h"
 *
 * Class provides, in addition to common context, SIP request
 */
class TSIPSecPluginCtxRequest : public TSIPSecPluginContext
	{
	
public: // Constructor

    /**
    * Constructor
    * @param aPlugin Digest plugin    
    * @param aRemoteTarget remote target
    * @param aOutboundProxy outbound proxy
    * @param aSIPRequest SIP request
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @param aTransactionId Transaction id
    */
    TSIPSecPluginCtxRequest( CSIPSecDigestPlugin& aPlugin,                             
                             const CUri8& aRemoteTarget,
                             const TDesC8& aOutboundProxy,
                             CSIPRequest& aSIPRequest,
                             MSIPSecUser& aUser,
                             TRegistrationId aRegistrationId,
                             TTransactionId aTransactionId );                             

public: // SIP related functions

    /**
    * Returns the SIP request
    * @return SIP request
    */
    CSIPRequest& SIPRequest();

private: // Data
    
    CSIPRequest& iSIPRequest;
    };




/**
 * @brief Class is plugin context with sec-agree related data
 * 
 * @class TSIPSecPluginCtxVerify sipsecplugincontext.h "sipsecplugincontext.h"
 *
 * Class provides, in addition to request context, security verify and server
 * header
 */
class TSIPSecPluginCtxVerify : public TSIPSecPluginCtxRequest
	{	
public: // Constructor

    /**
    * Constructor
    * @param aPlugin Digest plugin
    * @param aRemoteTarget remote target
    * @param aOutboundProxy outbound proxy
    * @param aSIPRequest SIP request
    * @param aVerifyHeader security verify header
    * @param aServerHeader security server header
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    */
    TSIPSecPluginCtxVerify( CSIPSecDigestPlugin& aPlugin,
                   const CUri8& aRemoteTarget,
                   const TDesC8& aOutboundProxy,
                   CSIPRequest& aSIPRequest, 
                   CSIPSecurityVerifyHeader& aVerifyHeader,
                   RPointerArray<CSIPSecurityServerHeader>& aServerHeaders,
                   MSIPSecUser& aUser,
                   TRegistrationId aRegistrationId );

public: // SIP related functions

    /**
    * Returns SIP Security-Verify header
    * @return Security-Verify header
    */
    CSIPSecurityVerifyHeader& SecurityVerifyHeader();

    /**
    * Returns SIP Security-Server headers
    * @return Queue of Security-Server headers
    */
    TSglQue<CSIPHeaderBase>& SecurityServerHeaders();

private: // Data

   	CSIPSecurityVerifyHeader& iVerifyHeader;
   	TSglQue<CSIPHeaderBase> iServerHeaders;
	};


/**
 * @brief Class is plugin context with authentication info related data
 * 
 * @class TSIPSecPluginCtxInfo sipsecplugincontext.h "sipsecplugincontext.h"
 *
 * Class provides, in addition to request context, authentication info header
 */
class TSIPSecPluginCtxInfo : public TSIPSecPluginCtxResponse
	{	
public: // Constructor

    /**
    * Constructor
    * @param aPlugin Digest plugin    
    * @param aRemoteTarget Remote target
    * @param aOutboundProxy Outbound proxy
    * @param aSIPResponse SIP response
    * @param aAuthInfo Authorization-Info header
    * @param aCache Cache
    * @param aObserver Mechanism observer
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    */
    TSIPSecPluginCtxInfo( CSIPSecDigestPlugin& aPlugin,    
                          const CUri8& aRemoteTarget,
                          const TDesC8& aOutboundProxy,
                          CSIPResponse& aSIPResponse,
                          CSIPAuthenticationInfoHeader& aAuthInfo,
                          CSIPSecDigestCache& aCache,
                          MSIPSecSecurityMechanismObserver& aObserver,
                          MSIPSecUser& aUser,
                          TRegistrationId aRegistrationId );

public: // From TSIPSecPluginContext

	TBool AlsoTrustedUser();

public: // SIP related functions

    /**
    * Returns SIP Authentication-Info header
    * @return Authentication-Info header
    */
    CSIPAuthenticationInfoHeader& AuthenticationInfoHeader();	

private: // Data

	CSIPAuthenticationInfoHeader& iAuthInfo;
	};

#endif //end of __SIPSEC_PLUGINCONTEXT_H__

// End of File
