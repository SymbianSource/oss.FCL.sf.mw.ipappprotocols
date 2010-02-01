/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MDialogOwner.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MDIALOGOWNER_H
#define MDIALOGOWNER_H

#include "SipStackServerDefs.h"
class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;
class MSIPSecUser;


/**
 * This class defines the events which the Dialogs subsystem will send
 * to server core.
 */
class MDialogOwner
	{
public:

	/**
	* Virtual destructor
	*/
	virtual ~MDialogOwner() { }

	/**
	* Dialogs has received a SIP request from transaction user or refreshes
	* and passes the message to upper layer.
	*
	* @param aTransactionId 	Identifies the transaction
	* @param aDialogId			Dialog identifier
	* @param aRequest			SIP request. The ownership is transferred.
	*/
	virtual void DialogsReceivedL (TTransactionId aTransactionId, 
						           TDialogId aDialogId,
						           CSIPRequest* aRequest) = 0;

   /**
	 * Dialogs has received a SIP provisional response or error
	 * response to a previously issued SIP request and dialog id
	 * does not yet exist.
	 *
	 * @param aTransactionId    Identifies the transaction
	 * @param aResponse         SIP response. The ownership is transferred.
	 */
	virtual void DialogsReceivedL (TTransactionId aTransactionId,
								   CSIPResponse* aResponse) = 0;

	/**
	 * Dialog manager has received a SIP response from transaction user or 
	 * refreshes and passes the message to upper layer.
	 *
	 * @pre Server core has been sent a request to the dialog manager. 
	 * @param aTransactionId Transaction ID
     * @param aRefreshId     Refresh ID
	 * @param aDialogId      Dialog ID
	 * @param aResponse      SIP Response. The ownership is transferred.
	 */
	virtual void DialogsReceivedL (TTransactionId aTransactionId,
                                   TRefreshId aRefreshId,
						           TDialogId aDialogId,
						           CSIPResponse* aResponse) = 0;

	/**
	 * The specified transaction has ended. A dialogs has not received
	 * response for this request.
	 *
	 * @param aTransaction identifies the transaction that ended
	 * 
	 * @param aReason Tells why the transaction ended.
	 *
	 * @returns value KErrNone if successful, otherwise a system wide error code
	 */
	virtual TInt DialogsError (TTransactionId aTransactionId,
	                           TInt aReason) = 0;

	/**
	 * The specified transaction/refresh has ended.
	 *
	 * @param aTransactionId Transaction ID
     * @param aRefreshId     Refresh ID
	 * @param aDialogId      Dialog ID
	 * @param aReason        Tells why the transaction ended.
	 * @returns KErrNone if successful, otherwise a system wide error code
	 */
	virtual TInt DialogsError (TTransactionId aTransactionId,
                               TRefreshId aRefreshId,
		                       TDialogId aDialogId,
	                           TInt aReason) = 0;
	/**
	 * The specified INVITE transaction has ended successfully.
	 *
	 * @param aId identifies the transaction that ended
	 *
	 * @returns value KErrNone if successful, otherwise a system wide error code
	 */
	virtual TInt InviteTransactionCompleted (TTransactionId aId) = 0;
	
	/**
	 * Gets the transport parameters to be passed to TU or Refreshes
	 * when sending a SIP request or a response.
     *
     * @param aRegistrationId the registration id
     *
     * @returns the transport parameters
	 */    
    virtual TSIPTransportParams TransportParams(
        const TRegistrationId& aRegistrationId) = 0;
        
	/**
	 * Gets the MSIPSecUser related to the given registration id.
     *
     * @param aRegistrationId the registration id
     *
     * @returns the user related to the given registration id if found,
     *          otherwise NULL.
	 */    
    virtual const MSIPSecUser* SIPSecUser(
        const TRegistrationId& aRegistrationId) = 0;        	
	};

#endif // MDIALOGOWNER_H

// End of File
