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
* Name          : sipclientconnection.h
* Part of       : SIPClient
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPCLIENTCONNECTION_H
#define CSIPCLIENTCONNECTION_H

#include <e32base.h>
#include <stringpool.h>
#include <uri8.h>
#include <in_sock.h>
#include "sipconnection.h"
#include "_sipcodecdefs.h"

class CSIPClient;
class RSIPConnection;
class CSIPITCSender;
class MSIPClientConnectionObserver;
class CSIPClientConnectionReceiver;
class CSIPToHeader;
class CSIPFromHeader;
class CSIPRouteHeader;
class CSIPCallIDHeader;
class CSIPMessageElements;
class CSIPRequestElements;
class CSIPResponseElements;
class CSIPContactHeader;

/**
 * @brief Class for openening connection using particular IAP and 
 *        for managing SIP transactions/dialogs using given connection.
 * 
 *
 * Class provides functions for sending and receiving 
 * SIP requests and SIP responses.
 * This class also provides functions for creating and destroying SIP dialogs,
 * sending and receiving SIP requests and responses within SIP dialogs,
 * canceling the SIP requests.
 *
 * All SIP messages will be sent/received using connection (defined using IAP Id)
 * given by the user.
 *
 * If the preconditions or postconditions are not met the 
 * function will leave with KErrArgument error code.
 *
 */
class CSIPClientConnection : public CBase
	{
public:

	/**
	 * Constructs an instance of CSIPClientConnection
     * The connection using IAP-Id is initialized during construction.
     * @param aClient a handle to SIP server session
     * @param aObserver an observer for receiving notifications
     * @param aIapId IAP Id that will be used for the connection.
	 * @returns new instance, the ownership is transferred
	 */

	static CSIPClientConnection* NewL (CSIPClient& aClient,
		                               MSIPClientConnectionObserver& aObserver,
									   TUint32 aIapId);

	/**
	 * Constructs an instance of CSIPClientConnection and adds a pointer to it 
     * to the CleanupStack
     * The connection using IAP-Id is initialized during construction.
     * @param aClient a handle to SIP server session
     * @param aObserver an observer for receiving notifications
     * @param aIapId IAP Id that will be used for the connection.
	 * @returns new instance, the ownership is transferred
	 */

	static CSIPClientConnection* NewLC (CSIPClient& aClient,
		                                MSIPClientConnectionObserver& aObserver,
										TUint32 aIapId);

	/**
	 * Destructor
	 */

	~CSIPClientConnection ();

	/**
	 * Gets the current state of the connection
     * @returns the connection state
	 */

    CSIPConnection::TState State () const;

	/**
	 * Gets the IAP id of this connection
     * @returns the IAP id
	 */

	TUint32 IapId () const;
	
    /**
	 * Gets the local IP address used for this CSIPClientConnection
	 * when sending SIP messages to the network.
	 * @pre State() == EActive
	 * @param aAddr local IP for this CSIPClientConnection.
     * @leave A system-wide error codes
     */ 	
     void GetLocalAddrL(TInetAddr& aAddr) const;	
	
    /**
	 * Sets a RSocket option used for all the sockets used for sending 
	 * SIP requests and responses in this CSIPConnection.
	 * @pre State() == EActive
	 * @param aOptionName An integer constant which identifies an option.
     * @param aOptionLevel An integer constant which identifies 
     *        level of an option.
     * @param aOption Option value packaged in a descriptor.
     * @leave A system-wide error code
     */        
	 void SetOptL(TUint aOptionName,
	              TUint aOptionLevel,
	              const TDesC8& aOption=KNullDesC8);
	         
    /**
	 * Sets a RSocket option used for all the sockets used for sending 
	 * SIP requests and responses in this CSIPConnection.
	 * @pre State() == EActive
	 * @param aOptionName An integer constant which identifies an option.
     * @param aOptionLevel An integer constant which identifies 
     *        level of an option.
     * @param aOption Option value as an integer.
     * @leave A system-wide error codes
     */ 	                         
	 void SetOptL(TUint aOptionName,
	              TUint aOptionLevel,
	              TInt aOption);  	
	

	/**
	 * Sets the Outbound Proxy for a registration binding. 
	 *
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response to an earlier REGISTER request.
     *        This value is used to identify the binding for which 
     *        the outbound proxy setting is to be set.
	 * @param aOutboundProxy an outbound proxy.
	 */
	 
	void SetOutboundProxyL (TUint32 aRegistrationId,
	                        const CSIPRouteHeader& aOutboundProxy);

	/**
	 * Removes the outbound proxy which was used for the particular AOR.
	 *
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response to an earlier REGISTER request. 
     *        This value is used to identify the binding for which 
     *        the outbound proxy setting is to be removed.
	 * @returns KErrNone if a proxy was found and removed.
     *          KErrNotFound if a proxy was not found. 
     *          Otherwise a system wide error code.
	 */

	TInt RemoveOutboundProxy (TUint32 aRegistrationId);

	/**
	 * Gets the Outbound Proxy as set by any application for an AOR.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * Applications should retrieve this value first and examine if outbound
	 * proxy address, for the particular AOR, is suitable before sending SIP
	 * requests to the sip stack. If not suitable, 
     * the application may add a Route-header to the SIP Request 
     * in order to override this setting for that particular request.
	 *
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response to an earlier REGISTER request.
     *        This value is used to identify the binding for which 
     *        the outbound proxy setting is to be fetched.
	 * @returns An outbound proxy object or 0 if not found,
	 *          The ownership is transferred.
	 */

	CSIPRouteHeader* OutboundProxyL (TUint32 aRegistrationId);

	/**
	 * Gets the AOR for a given registration id.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 *
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response to an earlier REGISTER request.
     *        This value is used to identify the binding for which 
     *        the AOR is to be fetched.
	 * @returns An AOR as a From-header object or 0 if not found,
	 *          The ownership is transferred.
	 */

	CSIPFromHeader* AorL (TUint32 aRegistrationId);

	/**
	 * Creates SIP REGISTER request and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code
	 * This function call creates a registration binding.
	 * The Registration-Id for this REGISTER is returned with a 
     * future 2xx response received for this REGISTER.
	 * A 2xx response to this REGISTER creates a binding.
     * There MUST NOT be more than one Contact-header and 
     * it MUST NOT be set to STAR.
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre aTo.SIPAddress().URI().HasUser()
     * @pre !aTo.HasTag()
     * @pre !aFrom.HasTag()
     * @pre aRemoteUri->IsSIPURI()
     *
	 * @param aRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aTo a To-header
	 * @param aTo a From-header 
	 * @param aElements contains optional SIP message headers and body.
     * @param aRemoteUri a remote-URI.
     *        The ownership is NOT transferred.
     * @param aOutboundProxy the outbound proxy address.
     *        The ownership is NOT transferred.
	 * @param aRefresh refresh flag. Refreshing of this REGISTER is requested 
     *        from the SIP Stack by setting this flag to ETrue.
 	 * @param aSendWithExpires when set to EFalse REGISTER message is sent
	 *        without expires value
	 * @param aCacheOutboundProxyIP when set to ETrue outbound proxy IP
	 *        of REGISTER message is cached in registration context
	 */		

	void SendRegisterL (TUint32& aRequestId,
		                const CSIPToHeader& aTo,
						const CSIPFromHeader& aFrom,
						const CSIPMessageElements& aElements,
						const CUri8* aRemoteUri=0,
						const CSIPRouteHeader* aOutboundProxy=0,
						TBool aRefresh=EFalse,
						TBool aSendWithExpires=ETrue,
						TBool aCacheOutboundProxyIP=EFalse);

	/**
	 * Creates SIP (update) REGISTER request and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * User of this function does not need to, but can, 
     * set a new expires values in the expires-param of the SIP REGISTER request.
	 * This is done to update the refresh interval.
	 * A 2xx response to this request updates a binding 
     * as well as the refresh interval.
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response, to an earlier REGISTER, 
     *        that created the binding. This value is used to 
	 *		  identify the binding to be updated.
	 * @param aNewRequestId the returned request Id, 
     *        to be used to match a response to this request.
	 * @param aElements contains optional SIP message headers and body.
	 */		

	void UpdateRegistrationL (TUint32 aRegistrationId,
		                      TUint32& aNewRequestId,
							  const CSIPMessageElements* aElements=0);

	/**
	 * Creates SIP (un)REGISTER request and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * User must NOT set any expires values in the
	 * SIP REGISTER request (aSIPMessageOptionalElements).
	 * Any SIP response (2xx - 6xx) to this request removes a binding.
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre "Contact-headers".Count() == 0
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response, to an earlier REGISTER,
     *        that created the binding. This value is used to 
	 *        identify the binding to be removed.
	 * @param aNewRequestId the returned request Id, 
     *        to be used to match a response to this request.
	 * @param aElements contains optional SIP message headers and body.
	 */

	void UnregisterL (TUint32 aRegistrationId,
		              TUint32& aNewRequestId,
					  const CSIPMessageElements* aElements=0);

	/**
	 * Creates SIP REGISTER request and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * User must NOT set any expires values 
     * to the request (aMessageOptionalElements).
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre !aElements.RemoteURI()->HasUser()
     * @pre aElements.ToHeader().SIPAddress().URI().HasUser()
	 * @pre "Contact-headers".Count() == 0
	 * @param aElements contains remote address and 
     *        optionally SIP message headers and body.
	 * @param aRequestId the returned request Id, 
     *        to be used to match a response to this request.
	 */		

	void FetchRegistrationsL (const CSIPRequestElements& aElements,
		                      TUint32& aRequestId);

	/**
	 * Creates SIP request and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * Function returns an error if a SIP request with a known method
	 * as defined in preconditions is sent. 
     * This provokes the Non-invite state machine.
	 * The registration Id is used to select the outbound proxy.
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
	 * @pre aRequestElements.Method() != "REGISTER" | "OPTIONS" | "INVITE" 
     *      | "BYE" | "CANCEL" | "ACK" | "SUBSCRIBE" | "NOTIFY" | "REFER" 
     *      | "MESSAGE".
	 * @param aElements contains local and remote addresses,
	 *        optional SIP message headers and body.
	 * @param aRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aRegistrationId registration id for the sender's AOR
	 * @param aRefresh refresh flag. Refreshing of the SIP request is requested
     *        from the SIP Stack by setting this flag to ETrue.
	 */		

	void SendRequestL (const CSIPRequestElements& aElements,
		               TUint32& aRequestId,
					   TUint32 aRegistrationId=0,
					   TBool aRefresh=EFalse);

	/**
	 * Sends a SIP request and updates an existing refresh. 
     * The method and To- and From-headers
     * from the original request will be used.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @param aRefreshId the value of the refresh-Id that was returned 
     *        in a response to an earlier request that created the refresh.
	 * @param aNewRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aElements contains optional SIP message headers and body.
	 */		

	void SendRequestAndUpdateRefreshL (TUint32 aRefreshId,
		                               TUint32& aNewRequestId,
									   const CSIPMessageElements* aElements=0);


	/**
	 * Sends a SIP request and terminates the refresh after sending the request.
     * The method and To- and From-headers
     * from the original request will be used.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * Any SIP response (2xx - 6xx) to this request terminates a refresh.
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @param aRefreshId the value of the refresh-Id that was returned 
     *        in a response to an earlier request that created the refresh.
	 * @param aNewRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aElements contains optional SIP message headers and body.
	 */		

	void SendRequestAndTerminateRefreshL (TUint32 aRefreshId,
		                                  TUint32& aNewRequestId,
										  const CSIPMessageElements* aElements=0);

	/**
	 * Creates SIP response and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * This response does not create a dialog. This function MUST
	 * be used only if the request received had no Dialog-Id.
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
	 * @param aElements contains Status Code, optional Reason Phrase 
     *        and optional SIP message headers and body.
	 * @param aRequestId the value of the request id as returned 
     *        by an earlier request. The value is used to identify the request
     *        to which this response is for.
	 */		
	
	void SendResponseL (const CSIPResponseElements& aElements,
		                TUint32 aRequestId);
		                
	/**
	 * Creates SIP ACK request within a dialog and sends it 
     * to the given destination. 
     * Leaves on failure with a system wide or a SIP stack specific error code.
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre aElements.OptionalElements()->ContactHeaders().Count() == 0
	 * @param aDialogId the value of the Dialog-Id that was returned 
     *        in a response to an earlier INVITE, that created the dialog.
	 * @param aRequestId the value of the INVITE Request-Id.
	 * @param aElements contains optional SIP message headers and body.
	 */		

	void SendAckL (TUint32 aDialogId,
		           TUint32 aRequestId,
				   const CSIPMessageElements* aElements=0);

	/**
	 * Creates SIP CANCEL request for an existing INVITE request and 
	 * sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
	 * @param aInviteRequestId the id of the INVITE request to be cancelled.
	 * @param aCancelRequestId the id to be created for the CANCEL request.
	 */		

	void SendCancelL (TUint32 aInviteRequestId,
		              TUint32& aCancelRequestId);

	/**
	 * Creates SIP response and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * This response creates a dialog. 
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
	 * @param aElements contains Status Code, 
     *        optional Reason Phrase and optional SIP message headers and body.
	 * @param aRequestId the value of the request id as returned by an 
     *        earlier received request. This value is used to identify 
     *        the request to which this response is for.
	 * @param aDialogId a dialog id of the dialog that this response creates.
	 * @param aRegistrationId registration id
	 */		

	void SendResponseAndCreateDialogL (const CSIPResponseElements& aElements,
		                               TUint32 aRequestId,
									   TUint32& aDialogId,
									   TUint32 aRegistrationId);
	 
	/**
	 * Creates SIP response to a request that is within a dialog
	 * and sends it to the given destination.
     * Leaves on failure with a system wide or a SIP stack specific error code.
     * Contact-header MUST NOT be set if aTargetRefresh flag is EFalse.
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
	 * @param aElements contains Status Code, optional Reason Phrase
     *        and optional SIP message headers and body.
	 * @param aRequestId the value of the request id as returned 
     *        by an earlier received request.
	 *        This value identifies the request to which this response is for.
	 * @param aDialogId the value of the dialog id as returned 
     *        by an earlier received response.
	 *        This value identifies the dialog to which this response is for.
	 * @param aTargetRefresh indicates if a received request 
     *        sent within a dialog was a target refresh request.
	 */		

	void SendResponseWithinDialogL (const CSIPResponseElements& aElements,
		                            TUint32 aRequestId,
									TUint32 aDialogId,
									TBool aTargetRefresh=EFalse);

	/**
	 * Creates a SIP request and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * The Dialog-Id for this request is returned 
     * with a future 2xx response received for this request. 
     * This provokes the Non-invite state machine
	 * The registration Id is used to select the outbound proxy.
     * There MUST NOT be more than one Contact-header and 
     * it MUST NOT be set to STAR.
	 * 
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre aMethod.Length() > 0
	 * @pre aMethod != "REGISTER" | "OPTIONS" | "BYE" | "CANCEL" |
     *                 "ACK" | "NOTIFY" | "MESSAGE" | "PRACK" | "UPDATE"
     * @pre aRemoteUri->IsSIPURI()
     * @pre !aTo->HasTag()
     * @pre !aFrom.HasTag()
     *
	 * @param aRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aRefreshId if the request is refresh,
	 *        contains on return the id of the created refresh.  
	 * @param aDialogId the returned dialog Id of the establishing dialog     
     * @param aMethod the method
     * @param aRemoteUri a remote-URI.
	 * @param aFrom a From-header. If not given, aRegistrationId must be given,
     *        so that the stack can fill in the From-header based on it.
     *        The ownership is NOT transferred.
	 * @param aElements contains optional SIP message headers and body.
	 * @param aTo a To-header. The ownership is NOT transferred.	 
	 * @param aRegistrationId registration id for the sender's AOR
	 * @param aRefresh refresh flag. Refreshing of this SIP request is requested 
     *        from the SIP Stack by setting this flag to ETrue.
	 */		

	void SendRequestAndCreateDialogL (TUint32& aRequestId,
	                                  TUint32& aRefreshId,
	                                  TUint32& aDialogId,
		                              RStringF aMethod,
		                              const CUri8& aRemoteUri,
									  const CSIPFromHeader* aFrom,
									  const CSIPMessageElements* aElements=0,
									  const CSIPToHeader* aTo=0,
									  TUint32 aRegistrationId=0,
									  TBool aRefresh=EFalse);
	/**
	 * Creates a SIP request within a dialog and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 * Contact header SHOULD NOT be created unless user intends to redirect 
     * the future requests within this dialog to a different remote target. 
     * Contact-header MUST NOT be set if aTargetRefresh flag is EFalse.
	 * This provokes the Non-invite state machine
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre aMethod.Length() > 0
	 * @pre aMethod != "REGISTER" | "CANCEL" | "ACK"
	 * @param aDialogId the value of the Dialog-Id that was returned in 
     *        a response to an earlier request that created the dialog.
	 * @param aNewRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aRefreshId if the request is refresh,
	 *        contains on return the id of the created refresh.
     * @param aMethod the method of the request
	 * @param aElements contains optional SIP message headers and body.
	 * @param aTargetRefresh indicates if an unknown request 
     *        is a target refresh request.
	 * @param aRefresh refresh flag. Refreshing of this SIP request 
     *        is requested from the SIP Stack by setting this flag to ETrue.
	 */		

	void SendRequestWithinDialogL (TUint32 aDialogId,
		                           TUint32& aNewRequestId,
		                           TUint32& aRefreshId,
								   RStringF aMethod,
								   const CSIPMessageElements* aElements=0,
								   TBool aTargetRefresh=EFalse,
								   TBool aRefresh=EFalse);

	/**
	 * Creates a SIP request within a dialog and sends it to the given destination.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
     * Contact-header MUST NOT be set.
     * Üpdates an existing refresh within a dialog.
     * This function can also be used to terminate an existing refresh
     * by setting the Expires-header's value to zero.
     * Expires header is mandatory.
	 *
	 * @pre CSIPConnection::State() == CSIPConnection::EActive.
     * @pre aMethod.Length() > 0
	 * @param aDialogId the value of the Dialog-Id that was returned in 
     *        a response to an earlier request that created the dialog.
	 * @param aRefreshId the value of the Refresh-Id that was returned
     *        in a response to an earlier request that created the refresh.
	 * @param aNewRequestId the returned request Id,
     *        to be used to match a response to this request.
	 * @param aElements contains optional SIP message headers and body.
	 */		

	void SendRequestWithinDialogL (TUint32 aDialogId,
		                           TUint32 aRefreshId,
								   TUint32& aNewRequestId,
								   const CSIPMessageElements* aElements);

	/**
	 * Gets the Call-ID for a given dialog id.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 *
	 * @param aDialogId a dialog id
	 * @returns A Call-ID header if the dialog was found. 
	 *          The ownership is transferred.
	 */

	CSIPCallIDHeader* CallIDL (TUint32 aDialogId);
	
	/**
	 * Gets the local tag for a given dialog id.
	 * Leaves on failure with a system wide or a SIP stack specific error code.
	 *
	 * @param aDialogId a dialog id
	 * @returns Local tag if the dialog was found. 
	 *          The ownership is transferred (the caller must call Close).
	 */

	RStringF LocalTagL (TUint32 aDialogId);	

	/**
	 * Gets the Contact-header that has been registered
	 * with the given registration id.
	 *
	 * @param aRegistrationId the value of the registration-Id 
     *        that was returned in a response to an earlier REGISTER request.
     *        This value is used to identify the binding for which 
     *        the Contact-header is to be fetched.
	 * @returns the registered Contact-header
	 */
    CSIPContactHeader* ContactL (TUint32 aRegistrationId);

	/**
	 * Terminates a dialog. This function can be used to terminate a
	 * dialog abnormally or when the dialog does not have any more
	 * associations (E.g. After receiving a final response to a BYE request).
	 *
	 * @param aDialogId dialog id for the dialog to be terminated.
	 * @returns system wide or SIP stack specific error code.
	 */
	TInt TerminateDialog (TUint32 aDialogId);

	/**
	 * Terminates a refresh abnormally.
	 *
	 * @param aRefreshId refresh id of the refresh to be terminated.
	 * @returns system wide or SIP stack specific error code.
	 */
	TInt TerminateRefresh (TUint32 aRefreshId);


    void SetRefreshIntervalL (TUint32 aRefreshId, TUint aInterval);


    TUint RefreshIntervalL (TUint32 aRefreshId);


	TInt TerminateDialogRefresh (TUint32 aDialogId,
                                 TUint32 aRefreshId);

    TInt TerminateRegistration (TUint32 aId);

    TInt ResetDialogState (TUint32 aDialogId);

	/**
	 * Clear the link from CSIPClientConnection to CSIPClient.
	 */
	void DetachClient();

	/**
	 * Refreshes the connection
	 */
	void RefreshConnection() const;
	
	/**
	 * Returns the connection error code
	 */ 
	TInt ConnectionErrorL() const;
	
private:

	CSIPClientConnection (CSIPClient& aClient,
                          MSIPClientConnectionObserver& aObserver,
                          TUint32 aIapId);

	void ConstructL ();

    void CheckDialogCreationRequestL (const CSIPToHeader* aTo,
                                      const CSIPFromHeader* aFrom,
                                      const CSIPMessageElements* aElements,
                                      TUint32 aRegistrationId,
                                      TBool aRefresh=EFalse);

    void CheckContactExpiresL (const CSIPMessageElements& aElements) const;
	void CheckContactL (const CSIPMessageElements* aElements) const;
    void CheckContactL (const CSIPMessageElements& aElements) const;
    void CheckExpiresHeaderL (const CSIPMessageElements* aElements,
                              TBool aZeroAllowed=EFalse) const;
    void CheckExpiresHeaderL (const CSIPMessageElements& aElements,
                              TBool aZeroAllowed=EFalse) const;

                              
private: // Data                              

    CSIPClient& iClient;
    MSIPClientConnectionObserver& iObserver;
    TUint32 iIapId;
	RSIPConnection* iConnection;
    CSIPITCSender* iSender;
	CSIPClientConnectionReceiver* iReceiver;
    TBool iAdded;
    RStringF iTag;
    RStringF iContactHeaderName;
    RStringF iExpiresHeaderName;

private: // For testing purposes

    UNIT_TEST(CSIPClientConnectionTest)
    UNIT_TEST(CSIPClientTest)
	};

#endif // CSIPCLIENTCONNECTION_H

// End of File
