/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipclientconnectionobserver.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPCLIENTCONNECTIONOBSERVER_H
#define MSIPCLIENTCONNECTIONOBSERVER_H

#include "sipconnection.h"
class CSIPRequestElements;
class CSIPResponseElements;

/**
 * @brief an interface to be implemented by users of CSIPClientConnection
 *        to be able to receive requests, responses, network state
 *        notifications and error notifications from sip stack
 *
 */
class MSIPClientConnectionObserver
	{
public:

	/**
	 * A SIP request outside a dialog has been received from the network.
	 *
	 * @param aRequest contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the id to be used when sending the SIP response.
	 */

	virtual void IncomingRequestL (CSIPRequestElements* aRequest,                                  
	                               TUint32 aRequestId) = 0;

	/**
	 * A SIP request within a dialog has been received from the network.
	 *
	 * @param aRequest contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the id to be used when sending the SIP response
	 * @param aDialogId identifies the dialog to which this request belongs to.
	 */

	virtual void IncomingDialogRequestL (CSIPRequestElements* aRequest,
		                                 TUint32 aRequestId,
								         TUint32 aDialogId) = 0;

	/**
	 * A SIP response that doesn't create a dialog,
	 * refresh, or registration binding has been received from the network. 
	 *
	 * @param aResponse contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the request id to be used when matching 
     *        the response to the sent request.
	 */

	virtual void IncomingResponseL (CSIPResponseElements* aResponse,
                                    TUint32 aRequestId) = 0;

	/**
	 * A SIP response within a dialog or a one that creates a dialog 
     * has been received from the network. 
	 *
	 * @param aResponse contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the request id to be used when matching 
     *        the response to the sent request.
	 * @param aDialogId the id to be used when sending requests within dialogs.
	 */

	virtual void IncomingDialogResponseL (CSIPResponseElements* aResponse,
		                                  TUint32 aRequestId,
								          TUint32 aDialogId) = 0;

	/**
	 * A SIP response within a dialog or a one that creates a dialog 
     * has been received from the network. 
	 *
	 * @param aResponse contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the request id to be used when matching 
     *        the response to the sent request.
     * @param aRefreshId the refresh id
	 * @param aDialogId the id to be used when sending requests within dialogs.
	 */

	virtual void IncomingDialogResponseL (CSIPResponseElements* aResponse,
		                                  TUint32 aRequestId,
                                          TUint32 aRefreshId,
								          TUint32 aDialogId) = 0;

	/**
	 * A SIP response that creates a registration binding or an error response 
     * that is related to an existing refreshed registration binding  
     * has been received from the network.
	 *
	 * @param aResponse contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the request id to be used when matching 
     *        the response to the sent request.
	 * @param aRefreshId the refresh id, if the registration is refreshed     
	 * @param aRegistrationId the registration id to be used when sending 
     *        requests for updating bindings or for using an outbound proxy.
	 */

	virtual void IncomingRegistrationResponseL (CSIPResponseElements* aResponse,
		                                        TUint32 aRequestId,
		                                        TUint32 aRefreshId,
								                TUint32 aRegistrationId) = 0;
 
	/**
	 * A SIP response that creates a refresh or an error response that 
     * is related to an existing refresh has been received from the network. 
	 *
	 * @param aResponse contains local address,
	 *        remote address of a sip message,
	 *        as well as optional SIP message method, headers and body.
	 *        The ownership is transferred.
	 * @param aRequestId the request id to be used when matching 
     *        the response to the sent request.
	 * @param aRefreshId the refresh id to be used when sending requests 
     *        that update or terminate the ongoing refresh.
	 */

	virtual void IncomingRefreshResponseL (CSIPResponseElements* aResponse,
		                                   TUint32 aRequestId,
								           TUint32 aRefreshId) = 0;

	/**
	 * An asynchronous error has occurred in the stack related to the
     * request indicated by the given request id.
	 *
	 * @param aError error code
	 * @param aRequestId the request id
	 */

	virtual void ErrorOccured (TInt aError,
	                           TUint32 aRequestId) = 0; 


	/**
	 * An asynchronous error related to 
     * an ongoing refresh has occurred in the stack. 
     * This means that the refresh has failed and it has been removed.
	 *
	 * @param aError error code
	 * @param aRefreshId the refresh id
	 */

	virtual void RefreshErrorOccured (TInt aError,
                                      TUint32 aRefreshId) = 0; 

	/**
	 * An asynchronous error related to an existing registration
     * has occurred in the stack. For a refreshed registration this means 
     * that the registration has been removed from the stack.
     * For an update of a non-refreshed registration this means that
     * the update was not successful but the original registration is
     * still valid. For a failing refreshed registration the request id 
     * contains the id of the original refreshed request.
	 *
	 * @param aError error code
	 * @param aRegistartionId the registration id
     * @param aRequestId the request id
	 */

	virtual void RegistrationErrorOccured (TInt aError,
                                           TUint32 aRegistrationId,
                                           TUint32 aRequestId) = 0;

	/**
	 * An asynchronous error has occured related to a request within
     * an existing dialog.
	 *
	 * @param aError error code
	 * @param aDialogId the dialog id
	 * @param aRequestId the request id
	 */

	virtual void DialogErrorOccured (TInt aError,
                                     TUint32 aDialogId,
                                     TUint32 aRequestId) = 0;

	/**
	 * An asynchronous error has occured related to a request within
     * an existing dialog.
	 *
	 * @param aError error code
	 * @param aDialogId the dialog id
	 * @param aRefreshId the refresh id
	 * @param aRequestId the request id
	 */

	virtual void DialogErrorOccured (TInt aError,
                                     TUint32 aDialogId,
                                     TUint32 aRefreshId,
                                     TUint32 aRequestId) = 0;

	/**
	 * The specified INVITE transaction has ended successfully.
	 *
	 * @param aRequestId the request id
	 */
	virtual void InviteCompleted (TUint32 aRequestId) = 0;

	/**
	 * Is called when the connection state changes
	 * 
	 * @param aState indicates the current connection state
	 */

	virtual void ConnectionStateChanged (CSIPConnection::TState aState) = 0;
	};

#endif // MSIPCLIENTCONNECTIONOBSERVER_H

// End of File
