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
* Name          : MRegistrationOwner.h
* Part of       : Registration
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef MREGISTRATIONOWNER_H
#define MREGISTRATIONOWNER_H

#include "SipStackServerDefs.h"
class CSIPResponse;
class TSIPTransportParams;


/**
 * An interface defining callbacks from Registration subsystem to the
 * calling subsystem.
 */
class MRegistrationOwner
	{
public:
	
	virtual ~MRegistrationOwner() {}

	/**
     * Called when registration gets a 2XX response to a SIP request that
     * created a binding or to a SIP request that updated or terminated 
     * an existing binding. In case of update or terminate request, 
     * any class responses go through this function.
	 *
	 * @param aTransactionId identifies the request to which this
	 *        response is related.
	 *
	 * @param aRefreshId identifies the refresh if the binding is refreshed
	 *	 
	 * @param aRegistrationId identifies the registration to which this
	 *        response is related. If a new registration binding was created
	 *        this is the id of the created binding.
	 *        Otherwise this is the id of an existing binding.
	 *
	 * @param aSIPResponse a response. The ownership is transferred
	 *        if the function does not leave.
	 */
	virtual void RegistrationReceivedL (TTransactionId aTransactionId,
	                                    TRefreshId aRefreshId,
		                                TRegistrationId aRegistrationId,
										CSIPResponse* aSIPResponse) = 0;

	/**
	 * Called when a binding has been created, but an update or 
     * a terminate request or a refresh failed.
     * The binding has now been removed.
	 *
	 * @param aTransactionId identifies the request to which this
	 *        response is related.
	 *
	 * @param aRegistrationId identifies the registration to which this
	 *        transaction is related.
	 * 
	 * @param aReason tells why the transaction ended.
	 *
	 * @returns value KErrNone if successful, 
	 *          otherwise a system wide error code
	 */
	virtual TInt RegistrationError (TTransactionId aTransactionId,
		                            TRegistrationId aRegistrationId,
	                                TInt aReason) = 0;

	/**
	 * Gets the transport parameters to be passed to TU or Refreshes
	 * when sending a REGISTER request.
     *
     * @returns the transport parameters
	 */    
    virtual const TSIPTransportParams& TransportParams() const = 0;
    };

#endif // MREGISTRATIONOWNER_H

// End of File
