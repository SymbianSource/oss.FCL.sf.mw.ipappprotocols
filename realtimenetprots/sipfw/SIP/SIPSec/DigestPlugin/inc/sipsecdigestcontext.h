/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsecdigestcontext.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_DIGESTCONTEXT_H__
#define __SIPSEC_DIGESTCONTEXT_H__

// INCLUDES
#include <e32std.h>
#include "sipsecplugincontext.h"
#include "sipsecrequestdata.h"

// FORWARD DECLARATIONS
class CSIPSecCredentials;
class CSIPSecDigest;
class CSIPAuthHeaderBase;
class CSIPSecRequestData;
class TSIPSecPluginContext;
class TSIPSecPluginCtxResponse;
class TSIPSecPluginCtxVerify;
class CSIPSecUserRecord;


/**
 * @brief Class is base class for digest context
 * 
 * @class TSIPSecDigestContext sipsecdigestcontext.h "sipsecdigestcontext.h"
 *
 * Class provides common digest context and plugin context
 */
class TSIPSecDigestContext
	{
public: // Constructor and destructor

	/**
    * constructor
	* @param aCredentials credentials
	* @param aParent parent context, if any
    */
	TSIPSecDigestContext( CSIPSecCredentials& aCredentials, 
	                      TSIPSecPluginContext* aParent = NULL );
	
	/**
    * Destructor
    */
    inline virtual ~TSIPSecDigestContext() 
        {
        }
    
public: // Digest related methods

    /**
    * returns mechanism
    * @return mechanism
    */
    CSIPSecDigest& Mechanism() const;

    /**
    * returns parent context
	* @return parent context
    */
    TSIPSecPluginContext& Parent();

    /**
    * returns user data
	* @return user data
    */
	CSIPSecUserRecord& UserData();

    /**
    * Sets user data
	* @return user data
    */
	void SetUserData( CSIPSecUserRecord& aUserData );

public: // General parameter methods

    /**
    * Gets any parameter
    * @param aHeader SIP header
    * @param aParam Parameter
    * @return parameter value
    */
    static const TDesC8& GetParam( const CSIPAuthHeaderBase& aHeader,
    							   TInt aParam );

	/**
    * Gets a parameter that is accessed using DesParamValue
    * @param aHeader SIP header
    * @param aParam Parameter
    * @return parameter value
    */
    static const TDesC8& GetDesParam( const CSIPAuthHeaderBase& aHeader,
    								  TInt aParam );

    /**
    * Sets any parameter
    * @param aHeader SIP header
    * @param aParam Parameter
    * @param aValue Parameter's value
    */
    static void SetParamValueL( CSIPAuthHeaderBase& aHeader,
    							TInt aParam,
    							const TDesC8& aValue );

	/**
    * Sets a parameter that is accessed using SetDesParamL.
    * @param aHeader SIP header
    * @param aParam Parameter
    * @param aValue Parameter's value
    */
	static void SetDesParamValueL( CSIPAuthHeaderBase& aHeader,
							   	   TInt aParam,
							       const TDesC8& aValue );

    /**
    * Gets any parameter
    * @param aParam Parameter
    * @return Parameter's value
    */
    const TDesC8& GetParam( TInt aParam ) const;

	/**
    * Gets a parameter that is accessed using DesParamValue
    * @param aParam Parameter
    * @return Parameter's value
    */
    const TDesC8& GetDesParam( TInt aParam ) const;

    /**
    * Sets any parameter
    * @param aParam Parameter
    * @param aValue Parameter's value
    */
    void SetParamValueL( TInt aParam, const TDesC8& aValue );

	/**
    * Sets a parameter that is accessed using SetDesParamL.
    * @param aParam Parameter
    * @param aValue Parameter's value
    */
    void SetDesParamValueL( TInt aParam, const TDesC8& aValue );

	/**
    * Sets a parameter to have empty value in quotes (e.g. response="").
    * @param aParam Parameter    
    */
	void SetParamWithEmptyValueL( TInt aParam );

public: // Credentials parameters

    /**
    * Returns nonce
	* @return Nonce value
    */
	const TDesC8& Nonce() const;

    /**
    * Returns nonce count
	* @return Nonce count value
    */
	const TDesC8& NonceCount() const;

	/**
    * Returns cnonce
	* @return Cnonce value
    */
	const TDesC8& CNonce() const;

    /**
    * Returns Request-URI
    * @return Request-URI value
    */
    virtual const TDesC8& DigestURI() const;

protected: // Data

    // Credentials
    CSIPSecCredentials& iCredentials;

    // Parent context, not owned
	TSIPSecPluginContext* iParent;
	};



/**
 * @brief Class is for 'setuping' digest 
 * 
 * @class TSIPSecDigestCtxSetup sipsecdigestcontext.h "sipsecdigestcontext.h"
 *
 */
class TSIPSecDigestCtxSetup : public TSIPSecDigestContext
	{	
public: // Constructor

	/**
    * Constructor
	* @param aCredentials Credentials
	* @param aTransactionId Transaction id
	* @param aParent Parent context, ownership is not transferred
    */
	TSIPSecDigestCtxSetup( CSIPSecCredentials& aCredentials,
						   TTransactionId aTransactionId,
	                       TSIPSecPluginCtxResponse* aParent = NULL );

public: // New functions

    /**
    * Requests user credentials
    * @return ETrue If the credentials for the challenge(s) present in the
    *		  current SIP response, must be asked now.
    *		  EFalse If there is no need to ask the credentials. Maybe they are
    * 	      already being asked for due to some earlier SIP response.
    */
    TBool RequestUserCredentialsL();

    /**
    * Sets cnonce
	* @param aCnonce Cnonce
    */
	void SetCNonceL( const TDesC8& aCnonce );

	/**
    * Get transaction id.
	* @return Transaction id
    */
	TTransactionId TransactionId() const;

private: // Data

	TTransactionId iTransactionId;
	};




/**
 * @brief Class is for processing digest 
 * 
 * @class TSIPSecDigestCtxProcess sipsecdigestcontext.h "sipsecdigestcontext.h"
 *
 */
class TSIPSecDigestCtxProcess : public TSIPSecDigestContext
	{
public: // Constructor

	/**
    * constructor
	* @param aCredentials credentials
	* @param aParent parent context
    */
    TSIPSecDigestCtxProcess( CSIPSecCredentials& aCredentials, 
	                         TSIPSecPluginContext* aParent = NULL );
	
public: // Digest related methods

    /**
    * Returns request data
    * @param aQop QOP value
    * @return aRequestData Request data, ownership is transferred
    */
    virtual CSIPSecRequestData* RequestDataL( CSIPSecRequestData::TQop aQop );
    
    /**
    * Sets response parameter
    * @param aResponse Response
    */
    virtual void SetResponseL( const TDesC8& aResponse );

    /**
    * Returns method
    * @return Method
    */
    virtual const TDesC8& Method();

    /**
    * Returns SIP message
    * @return SIP message
    */
    virtual const TDesC8& Message();
    };




/**
 * @brief Class is for handling digest verify
 * 
 * @class TSIPSecDigestVerifyContext sipsecdigestcontext.h
 *		  "sipsecdigestcontext.h"
 */
class TSIPSecDigestVerifyContext : public TSIPSecDigestCtxProcess
	{
public: // Constructor

	/**
    * Constructor
	* @param aCredentials Credentials
	* @param aUri Request-URI
	* @param aParent Parent context, ownership is not transferred
    */
	TSIPSecDigestVerifyContext( CSIPSecCredentials& aCredentials,
	                            const TDesC8& aUri, 
	                            TSIPSecPluginCtxVerify* aParent );

public: // From TSIPSecDigestContext
    
    const TDesC8& DigestURI() const;

public: // From TSIPSecDigestCtxProcess

    CSIPSecRequestData* RequestDataL( CSIPSecRequestData::TQop aQop );
	
    void SetResponseL( const TDesC8& aResponse );
    
public: // New functions

    /**
    * Returns SIP combined security server header
    * @return SIP combined security server header
    */
    HBufC8* SecurityServerHeaderL();

public: // Data

    // URI
    const TDesC8& iUri;
    };



/**
 * @brief Class is for handling authentication info
 * 
 * @class TSIPSecDigestAuthInfoContext sipsecdigestcontext.h
 *		  "sipsecdigestcontext.h"
 */
class TSIPSecDigestAuthInfoContext : public TSIPSecDigestCtxProcess
	{
	
public:// Constructor

	/**
    * Constructor
	* @param aCredentials Credentials
	* @param aResponse Response place holder
	* @param aParent Parent context, ownership is not transferred
	*/
	TSIPSecDigestAuthInfoContext( CSIPSecCredentials& aCredentials,
	                              TDes8& aResponse,
	                              TSIPSecPluginCtxResponse* aParent );

public: // From TSIPSecDigestCtxProcess
    
    void SetResponseL( const TDesC8& aResponse );

    const TDesC8& Method();
    
    const TDesC8& Message();

public: // Data

    // Response    
    TDes8& iResponse;
    };

#endif //end of __SIPSEC_DIGESTCONTEXT_H__

// End of File
