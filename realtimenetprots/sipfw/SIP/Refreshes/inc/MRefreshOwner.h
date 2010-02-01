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
* Name          : MRefreshOwner.h
* Part of       : SIPRefreshes
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MREFRESHOWNER_H
#define MREFRESHOWNER_H

#include "SipStackServerDefs.h"

class CSIPResponse;
class TSIPTransportParams;
class MSIPSecUser;

/**
 * @brief A call back interface to be implemented by the users of MRefreshMgr 
 *
 * @see MRefreshMgr, MTransactionOwner, CSIPResponse
 */

class MRefreshOwner
	{
public:

	virtual ~MRefreshOwner() {}

	/**
	 * Called when refreshes gets a response to a previously
	 * issued SIP request.
	 *
	 * @param aTransactionId identifies the request to which this
	 *        response is related.
	 *
	 * @param aRefreshId identifies the refresh to which this
	 *        response is related.
	 *
	 * @param aResponse a response. The ownership is transferred
	 *        if the function does not leave.
	 */
	virtual void RefreshReceivedL (TTransactionId aTransactionId,
		                           TRefreshId aRefreshId,
	                               CSIPResponse* aResponse) = 0;

	/**
	 * An asynchrounous error related to a refresh has occured.
	 *
	 * @param aRefreshId identifies the refresh to which this
	 *        transaction is related. 
	 *
	 * @param aTransactionId identifies the transaction that ended
	 * 
	 * @param aReason tells why the transaction ended.
	 *
	 * @returns value KErrNone if successful, 
	 *          otherwise a system wide error code
	 */
	virtual TInt RefreshError (TRefreshId aRefreshId,
		                       TTransactionId aTransactionId,
	                           TInt aReason) = 0;

	/**
	 * Returns the next CSeq of the implementing subsystem.
     *
     * @param aCSeq an in-out parameter. While entering the function contains
     *        the current CSeq of the caller. On return contains 
	 *        the incremented CSeq of the callee. 
	 *        If the subsystem implementing the interface does not 
	 *        keep track of the CSeqs it should simply increment
     *        aCSeq by one.
     *
     * @returns KErrNotFound, if the refresh ID was not found, otherwise
     *          KErrNone.
	 */
    virtual TInt NextRefreshCSeq (TUint& aCSeq) = 0;
    
	/**
	 * Gets the transport parameters to be passed to TU
	 * when sending a refreshed SIP request.
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

	/**
	 * Gets the refresh interval duration.
     *
     * @returns the refresh duration,
	 */    
    virtual TUint RefreshDurationL() = 0;
    };

#endif // MREFRESHOWNER_H

// End of File
