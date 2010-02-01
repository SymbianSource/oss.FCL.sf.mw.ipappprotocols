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
* Name          : sipclient.h
* Part of       : SIPClient
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENT_H
#define CSIPCLIENT_H

#include "sipclientserver.h"
#include <e32base.h>
#include <badesca.h>
#include "_sipcodecdefs.h"

class MSIPClientObserver;
class MSIPHttpDigestChallengeObserver;
class MSIPHttpDigestChallengeObserver2;
class CSIPClientConnection;
class RSIP;
class CSIPClientReceiver;


/**
 * @brief Class for connecting to SIP server and for managing SIP transactions.
 * 
 * @class CSIPClient sipclient.h "sipclient.h"
 *
 * This class is used to communicate with the SIP server
 * Class provides functions for sending and receiving 
 * SIP requests and SIP responses.
 * This class also provides functions for creating and destroying SIP dialogs,
 * sending and receiving SIP requests and responses within SIP dialogs,
 * canceling the SIP requests.
 *
 * This class can only be created once by an application (a singleton class)
 *
 * If the preconditions or postconditions are not met the 
 * function call will result in panics. 
 * 
 * For SIP specific error codes @see TError
 */

class CSIPClient : public CBase
	{
public:

	/** SIP specific errors */
	enum TError
		{		
		/** Non-existing request Id provided */
		ENonExistingRequestId = 10,
		/** Non-existing dialog Id provided */
		ENonExistingDialogId,
		/** Non-existing refresh Id provided */
		ENonExistingRefreshId,
		/** Non-existing registration Id provided */
		ENonExistingRegistrationId
		};

	/**
	 * Constructs an object; leaves on failure.
	 *
	 * @param aUid an unique id that identifies an application
	 * @param aSIPObserver a sip observer. 
     *        This callback is used for passing 
     *        SIP requests and internal errors to application.
	 * @returns new instance, the ownership is transferred
	 */
	static CSIPClient* NewL (const TUid& aUid,
		                     MSIPClientObserver& aSIPObserver);

	/**
	 * Constructs an object and adds the pointer to the cleanup stack;
	 *
	 * @param aUid an unique id that identifies an application
	 * @param aSIPObserver a sip observer. 
     *        This callback is used for passing 
     *        SIP requests and internal errors to application.
	 * @returns new instance, the ownership is transferred
	 */
	static CSIPClient* NewLC (const TUid& aUid,
		                      MSIPClientObserver& aSIPObserver);

	/**
	 * Destructor
	 */

	~CSIPClient ();

    /**
     * Gets the names of all the supported security mechanisms
	 * @returns Supported SIP security mechanisms names (e.g. digest)
     *          The ownership is transferred.
	 */
    CDesC8Array* SupportedSecurityMechanismsL() const;
    
	/**
	 * Gets negotiated security mechanism with the next hop
	 * on the signaling path.
	 * @param aHop the next hop's FQDN or numeric IP address
	 * @return negotiated security mechanism or zero-pointer in case no
	 *		  security mechanism was negotiated with the next hop;
	 *		  the ownership is transferred
	 */
	HBufC8* NegotiatedSecurityMechanismL(const TDesC8& aHop);    

    /**
     * Checks if the signalling compression (SigComp) is supported.
     * Leaves on failure with a system wide error code.
     * @returns ETrue if supported, EFalse otherwise
     */
    TBool IsSigCompSupportedL() const;

	/**
     * Checks if the user has a connection created for given IAP-id
     * @param aIapId IAP-id
     * @returns a connection or 0-pointer if connection does not exist
     */
	CSIPClientConnection* Connection (TUint32 aIapId) const;

    /*
     * By default the SIP built-in security mechanisms are enabled.
     * @param aEnabled if set to EFalse the build-in SIP security handling 
     *        mechanism is disabled.
     */
    void SetSecurityHandlingL(TBool aEnabled);

	/**
	 * Sets the observer to listen for the possible received challenges.
	 * @param aObserver an observer for the received challenges.
	 * @returns KErrNone if the observer was set successfully.
	 *          KErrNoMemory if out of memory.
	 */
    TInt SetHttpDigestObserver(MSIPHttpDigestChallengeObserver& aObserver);
	
	/**
	 * Sets the observer to listen for the possible received challenges.
	 * @param aObserver an observer for the received challenges.
	 * @returns KErrNone if the observer was set successfully.
	 *          KErrNoMemory if out of memory.
	 */
    TInt SetHttpDigestObserver(MSIPHttpDigestChallengeObserver2& aObserver);	

	/**
	 * Removes the observer that listens for the possible received challenges.
	 */	
	void RemoveHttpDigestObserver();
	
    /**
	 * Sets credentials for a realm.
	 * @param aRequestId request id if needed, otherwise zero
	 * @param aRefreshId refresh id if needed, otherwise zero
	 * @param aRealm servers's realm
	 * @param aUsername user's name
	 * @param aPasswd user's password for the given server's realm
	 * @param aOutboundProxy an outbound proxy (FQDN or IP address)
	 *        Must be set to KNullDesC8 if not used.	 
	 * @leave KErrNoMemory if out of memory
	 */
    void SetCredentialsL(TUint32 aRequestId,
                         TUint32 aRefreshId,
                         const TDesC8& aRealm,
			             const TDesC8& aUsername,
			             const TDesC8& aPasswd,
			             const TDesC8& aOutboundProxy);
			            
    /**
	 * Removes all set credentials for the realm 
	 * @param aRealm servers's realm. 
	 *        If set to KNullDesC8 clears all the credentials for
	 *        this CSIPClient.
	 * @return KErrNone if no error; KErrNoMemory if out of memory; 
	 *         KErrNotFound if the given realm was not found
     */
	TInt RemoveCredentials(const TDesC8& aRealm);                          
		
    /**
     * Ignores the challenge for the realm. 
     * As a result the error will be generated to the original request.
	 * @param aRequestId request id if needed, otherwise zero
	 * @param aRefreshId refresh id if needed, otherwise zero      
	 * @param aRealm a realm for which the challenge was ignored
	 * @returns KErrNone if no error; KErrNoMemory if out of memory;
	 *          KErrNotFound if the given realm was not found
	 */
    TInt IgnoreChallenge(TUint32 aRequestId,
                         TUint32 aRefreshId,
                         const TDesC8& aRealm);	


public: // For SIP Client's internal use

    void AddL (CSIPClientConnection* aConnection);
    TInt Remove (const CSIPClientConnection* aConnection);
    RSIP& SIP ();

private:

	CSIPClient ();
	void ConstructL (const TUid& aUid, MSIPClientObserver& aObserver);
    TInt FindIndex (TUint32 aIapId) const;
    TInt SendHttpDigestObserverTypeToServer(
        const TSipHttpDigestObserverType& aObserverType); 

private: // Data
    
    RPointerArray<CSIPClientConnection> iConnections;
    RSIP* iSip;
	CSIPClientReceiver* iReceiver;

private: // For testing purposes

    UNIT_TEST(CSIPClientTest)
	};

#endif // CSIPCLIENT_H

// End of File
