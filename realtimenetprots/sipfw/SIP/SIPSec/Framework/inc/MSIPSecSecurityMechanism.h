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
* Name          : MSIPSecSecurityMechanism.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef MSIPSECSECURITYMECHANISM_H
#define MSIPSECSECURITYMECHANISM_H

#include <e32base.h>
#include <in_sock.h>
#include "SipStackServerDefs.h"

class CSIPRequest;
class CSIPResponse;
class CUri8;
class MSIPSecUser;
class MSIPSecSecurityMechanismObserver;
class CSIPSecurityClientHeader;
class CSIPSecurityServerHeader;
class CSIPSecurityVerifyHeader;
class TSIPTransportParams;

/**
 * @brief Interface class for maintaining 
 *        security information in security cache
 * 
 * Class provides functions for maintaing 
 * security information in security cache.
 */
class MSIPSecSecurityMechanism 
    {
public:
    
    virtual ~MSIPSecSecurityMechanism() {}

    /**
    * Gets the name of the mechanism
    *
    * @return name of the mechanism
    */
    virtual const TDesC8& Name() const = 0;
    
    /**
    * Mechanism should verify that Security-Client header contains all
    * parameters which are needed for the mechanism.
    * This function is called only when the first request 
    * is being sent to the next hop.
    *
    * @param aSecurityClient Security-Client header
    */
    virtual void InitializeSecurityClientL(
        CSIPSecurityClientHeader& aSecurityClient ) = 0;

    /**
    * Mechanism should verify that Security-Verify header contains all
    * parameters which are needed for the mechanism.
    * The function is called for every request after sec-agreement
    * was established with the next hop.
    *
    * @param aTransportParams Params to be used when creating IPSec SAs
    * @param aRequest SIP request
    * @param aNextHop Next-hop address
    * @param aRemoteTarget Remote target of the request
    * @param aOutboundProxy Outbound proxy or empty descriptor
    * @param aUser SIPSec user. The ownership is NOT transferred.
    * @param aRegistrationId Registration id
    * @param aSecurityServer Security-Server headers
    * @param aSecurityVerify Security-Verify headers
    */
    virtual void ProcessSecurityVerifyL(
        TSIPTransportParams& aTransportParams,
        CSIPRequest& aRequest,
        TInetAddr& aNextHop,
        const CUri8& aRemoteTarget,
        const TDesC8& aOutboundProxy,
        MSIPSecUser* aUser,
        TRegistrationId aRegistrationId,
        RPointerArray<CSIPSecurityServerHeader>& aSecurityServer,
        RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify ) = 0;

    /**
    * Adds the cached security parameters to the SIP request.
    * If no cache entries are found returns without any error.
    *
    * @param aTransportParams Params to be used when creating IPSec SAs
    * @param aRequest SIP request where the security parameters will be added
    * @param aRegistrationId registration ID
    * @param aTransactionId transaction ID
    * @param aNextHop the next hop of the request
    *        IP address or a domain name
    * @param aRemoteTarget Remote target of the request    
    * @param aOutboundProxy the outbound proxy of the request
    *        IP address or a domain name. Zero-length if not present.
    * @param aUser the user for the possible cache entries
    *        The ownership is NOT transferred.
    */
    virtual void AddSecurityParamsL(
        TSIPTransportParams& aTransportParams,
        CSIPRequest& aRequest,
        TRegistrationId aRegistrationId,
        TTransactionId aTransactionId,
        TInetAddr& aNextHop,
        const CUri8& aRemoteTarget,
        const TDesC8& aOutboundProxy,
        MSIPSecUser* aUser ) = 0;

    /**
    * Updates the SIPSec cache if necessary.
    * If nothing was updated returns without any error.
    * May lead to asynchronous processing. If so, returns ETrue
    * and the user must wait for the cache to be updated before
    * sending the SIP request again.
    *
    * @param aTransportParams Params to be used when creating IPSec SAs
    * @param aResponse SIP response to be processed to update the cache
    * @param aRequest IN: SIP request related to the aResponse
    * @param aRegistrationId registration ID
    * @param aTransactionId transaction ID
    * @param aNextHop the next hop of the original request
    *        IP address or a domain name
    * @param aRemoteTarget Remote target of the original request    
    * @param aOutboundProxy the outbound proxy of the original request
    *        IP address or a domain name. Zero-length if not present.
    * @param aUser the user for the possible cache entries,
    *        Can be used to getting the credentials for the cache.
    *        The ownership is NOT transferred.
    * @param aObserver observer that will be notified when
    *        after an asynchronous cache update has been completed.
    *        The ownership is NOT transferred.
    * @return ETrue if the user needs to wait asynchronously for the
    *         cache to be updated. Otherwise EFalse.
    */
    virtual TBool ResponseReceivedL(
        TSIPTransportParams& aTransportParams,
        CSIPResponse& aResponse,
        CSIPRequest& aRequest,
        TRegistrationId aRegistrationId,
        TTransactionId aTransactionId,
        TInetAddr& aNextHop,
        const CUri8& aRemoteTarget,
        const TDesC8& aOutboundProxy,
        MSIPSecUser* aUser,
        MSIPSecSecurityMechanismObserver& aObserver ) = 0;

    /**
    * Mechanism returns information whether it is allowed to be selected when
    * security agreement is initiated by the server.
    *
    * @return ETrue This mechanism can be used also in server initiated case,
    *          EFalse otherwise
    */
    virtual TBool IsServerInitiatedSecAgreeAllowed() const = 0;

    /**
    * Inform the plugin that the SIPSec parameters have been updated to
    * MSIPSecEngineContext.    
    *
    * @param aUser SIPSec user. The ownership is NOT transferred.
    * @return ETrue If the user needs to wait asynchronously for the plugin to
    *         complete parameter handling. Otherwise EFalse.
    */
    virtual TBool ParametersUpdatedL( MSIPSecUser* aUser ) = 0;

    /**
    * Cancels all the pending asynchronous operations for the given observer.
    *
    * @param aObserver observer waiting for an asynchronous cache update
    *        The ownership is NOT transferred.
    */                            
    virtual void CancelPendingOperations(
        MSIPSecSecurityMechanismObserver* aObserver) = 0;

    /**
    * Clears all the cached information for the given user.
    *
    * @param aUser the user for the cache entries
    *        The ownership is NOT transferred.
    */
    virtual void ClearCache( MSIPSecUser* aUser ) = 0;

    /**
    * Will be called as a result to asynchronous call to
    * MSIPSecUser::RequestCredentialsL.    
    *
    * @param aTransactionId transaction ID if available, 
    *        otherwise KEmptyTransactionId
    * @param aRealm the realm for the credentials
    * @param aOutboundProxy Outbound proxy
    * @param aUserName the username
    * @param aPassword the password
    */
    virtual void SetCredentialsL(
        TTransactionId aTransactionId,
        const TDesC8& aRealm,
        const TDesC8& aOutboundProxy, 
        const TDesC8& aUserName,
        const TDesC8& aPassword ) = 0;

  /**
    * Will be called as a result to asynchronous call to
    * MSIPSecUser::RequestCredentialsL.    
    *
    * @param aUser SIPSec user who provides the credentials
    * @param aRealm the realm for the credentials
    * @param aOutboundProxy Outbound proxy
    * @param aUserName the username
    * @param aPassword the password
    */
    virtual void SetCredentialsL(
        const MSIPSecUser& aUser,
        const TDesC8& aRealm,
        const TDesC8& aOutboundProxy, 
        const TDesC8& aUserName,
        const TDesC8& aPassword ) = 0;
    
    /**
    * Ignores the challenge for the realm. As a result the error will be
    * generated to the original request in case. 
    *
    * @param aTransactionId transaction ID if available, 
    *        otherwise KEmptyTransactionId    
    * @param aRealm a realm for which the challenge was ignored
    * @param aTrustedUser the trusted user for the ignored challenge
    * @return KErrNone if no error
    *          KErrNotFound if the given realm was not found
    */
    virtual TInt IgnoreChallenge(
        TTransactionId aTransactionId,
        const TDesC8& aRealm,
        const MSIPSecUser* aTrustedUser ) = 0;
    
    /**
    * Removes all set credentials for the realm
    *
    * @param aRealm servers's realm
    * @return KErrNone if no error
    *          KErrNoMemory if out of memory; KErrNotFound if the given realm
    *          was not found
    */
    virtual TInt RemoveCredentials( const TDesC8& aRealm ) = 0;
    };
    
#endif // MSIPSECSECURITYMECHANISM_H

// End of File
