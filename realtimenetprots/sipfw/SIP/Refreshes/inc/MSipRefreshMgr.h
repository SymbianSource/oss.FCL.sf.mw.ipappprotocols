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
* Name          : MSipRefreshMgr.h
* Part of       : SIPRefreshes
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPREFRESHMGR_H
#define MSIPREFRESHMGR_H

#include "SipStackServerDefs.h"

class TSIPTransportParams;
class MRefreshOwner;
class MSIPSecUser;
class CSIPRequest;
class CURIContainer;
class CSIPHeaderBase;
class CSIPRouteHeader;
class MTransactionHeaders;


/**
 * @brief An interface for adding, updating and
 * removing refresh requests.
 *
 * @see MRefreshOwner
 */

class MSipRefreshMgr
    {
public:

    virtual ~MSipRefreshMgr () { }

	/**
	 * Add a refresh for a given CSIPRequest.
	 *
	 * @pre aRequest!=0
     * @pre aOwner!=0
     *
	 * @param aTransactionId; an empty transaction id that will be filled by TU
	 *
	 * @param aRefreshId; on return contains the id of the created refresh
	 *
	 * @param aRegistrationId a registration id with which a possible outbound
	 *        proxy is used in TU.
     *
	 * @param aRequest SIP request to be refreshed. The ownership is 
	 *        transferred if parameter aDeleteRequest is ETrue, otherwise not.
     *
     * @param aOwner a call back implemented by the caller
     *
     * @param aSIPSecUser a user for SIPSec
     *
     * @param aRemoteTarget. The remote target to be used.
	 *
	 * @param aDeleteRequest. If it is ETrue, refresh will keep 
	 *        the ownership of the request, otherwise not.	 
	 *
	 * @param aPassAllResponsesToOwner. If ETrue all the responses
	 *        are passed to the owner. 
	 *        If EFalse only the first final response is passed.
	 */
	virtual void RefreshL (TTransactionId& aTransactionId,
	                       TRefreshId& aRefreshId,
	                       TRegistrationId aRegistrationId,
				           CSIPRequest* aRequest,
				           MRefreshOwner* aOwner,
				           MSIPSecUser& aSIPSecUser,
						   CURIContainer& aRemoteTarget,
						   TBool aDeleteRequest,
						   TBool aPassAllResponsesToOwner) = 0;


	/**
	 * Add a refresh for a given CSIPRequest and return the transaction 
	 * related SIP headers.
	 *
	 * @pre aRequest!=0
     * @pre aOwner!=0
     *
	 * @param aTransactionId; an empty transaction id 
	 * that will be filled by TU
	 *
	 * @param aRefreshId; on return contains the id of the created refresh
     *
	 * @param aRegistrationId a registration id with which 
	 *        a possible outbound proxy is used in TU.
     *
	 * @param aRequest SIP request to be refreshed. The ownership is 
     *        transferred to MSipRefreshMgr.
     *
     * @param aOwner a callback interface implemented by the caller
     *
     * @param aSIPSecUser a user for SIPSec
	 *
     * @param aRemoteTarget. The remote target to be used.
     *
	 * @returns the transaction related headers @see MTransactionHeaders
	 *          The ownership is transferred.
	 */
	virtual MTransactionHeaders* RefreshAndGetHeadersL (
	                TTransactionId& aTransactionId,
	                TRefreshId& aRefreshId,
					TRegistrationId aRegistrationId,
					CSIPRequest* aRequest,
					MRefreshOwner* aOwner,
					MSIPSecUser& aSIPSecUser,
					CURIContainer& aRemoteTarget) = 0;

	/**
	 * Update a refresh for a given id. Leaves with
	 * CSIPClient::ENonExistingRefreshId if no refresh for the given id was
     * found.
	 *
	 * @pre aRequest!=0
	 * @pre aRefreshId!=KEmptyRefreshId
     *
	 * @param aTransactionId; an empty transaction id that will 
	 *                        be filled by TU
	 *
	 * @param aRefreshId a refresh id of an existing refresh 
     *
	 * @param aRequest an updated SIP request to be refreshed.
	 *        The ownership is transferred if parameter aDeleteRequest 
	 *        is ETrue, otherwise not.
	 *
	 * @param aOwner a callback interface implemented by the caller
	 *
	 * @param aDeleteRequest. If it is ETrue, refresh will keep the 
	 *        ownership of the request, otherwise not.
	 */
	virtual void UpdateRefreshL (TTransactionId& aTransactionId,
		                         const TRefreshId& aRefreshId,
				                 CSIPRequest* aRequest,
				                 const MRefreshOwner* aOwner,
								 TBool aDeleteRequest) = 0;

	/**
	 * Ends a refresh with a given id. Leaves with
	 * CSIPClient::ENonExistingRefreshId if no refresh for the given id was
     * found.
	 * Sends the given request and receives a response for it
	 * before removing the refresh.
	 *
	 * @pre aRequest!=0
	 * @pre aRefreshId!=KEmptyRefreshId
     *
	 * @param aTransactionId; an empty transaction id 
	 *        that will be filled by TU
	 *
	 * @param aRefreshId a refresh id of an existing refresh 
     *
	 * @param aRequest a SIP request to be sent before ending the refresh.
	 *        The ownership is transferred if parammeter aDeleteRequest 
	 *        is ETrue otherwise not.
	 *
	 * @param aOwner a callback interface implemented by the caller
	 * 
	 * @param aDeleteRequest. If it is ETrue, refresh will keep the 
	 *        ownership of the request, otherwise not.
	 */
	virtual void TerminateRefreshL (TTransactionId& aTransactionId,
		                            const TRefreshId& aRefreshId,
				                    CSIPRequest* aRequest,
				                    const MRefreshOwner* aOwner,
									TBool aDeleteRequest) = 0;

	/**
	 * Terminates a refresh with a given id abnormally. Deletes the
	 * refresh immediately no matter what is the state of the refresh.
     *
	 * @param aRefreshId a refresh id of an existing refresh 
     *
     * @param aOwner a callback interface implemented by the caller
     *
	 * @returns CSIPClient::ENonExistingRefreshId if no refresh with 
	 *          the given id was found, otherwise KErrNone.
	 */
	virtual TInt TerminateRefresh (const TRefreshId& aRefreshId,
	                               const MRefreshOwner* aOwner) = 0;

	/**
	 * Stops all the refreshs for the given refresh owner.
     * Using this function the user detaches itself from 
	 * the refresh subsystem and the object implementing 
	 * the call back can be deleted.
	 *
	 * @pre aOwner!=0
     *
     * @param aOwner a callback interface implemented by the caller
     *
	 * @returns CSIPClient::ENonExistingRefreshId if no refresh for 
	 *          the given owner were found, otherwise KErrNone.
	 */
    virtual TInt RemoveRefreshesFor (const MRefreshOwner* aOwner) = 0;

	/**
	 * Returns a refreshing request.
	 * The request ownership is not transferred.
	 *
	 * @param aRefreshId the id for the refresh whose request is returned
	 *
	 * @returns Request of the refresh. 0 if not found.
     */
	virtual CSIPRequest* Request (const TRefreshId& aRefreshId) = 0;

    /**
	 * Update the remote target.
	 * 
	 * @pre aOwner!=0
     *
     * @param aOwner. The ownership is not transferred.
	 * @param aRemoteTarget the new remote target.
	 *
     */
	virtual void UpdateRemoteTargetL (const MRefreshOwner* aOwner, 
		                              CURIContainer& aRemoteTarget) = 0;

	/**
	 * Update header to a request of given refreshid. Leaves with
	 * CSIPClient::ENonExistingRefreshId if no refresh for 
	 * the given id was found.
	 *
	 * @pre aRefreshId!=KEmptyRefreshId
	 * 
	 * @param aRefreshId a refresh id of an existing refresh
	 *
	 * @param aOwner a callback interface implemented by the caller 
	 *
	 * @param aSIPHeader a SIP header to updated, 
	 *        the ownership is transferred.
	 */
	virtual void UpdateHeaderL (const TRefreshId& aRefreshId,
	                            const MRefreshOwner* aOwner,
								CSIPHeaderBase* aSIPHeader) = 0;

	/**
	 * Update the Route-sets for all the refreshes of a given owner. 
	 * @param aOwner. The ownership is not transferred.
     * @param aRouteSet. An array of CSIPRouteHeader-objects.
	 *        The ownership is not transfered.
	 */
	virtual void UpdateRouteSetsL (const MRefreshOwner* aOwner, 
				                   const RPointerArray<CSIPRouteHeader>& aRouteSet) = 0;

	/**
	 * Removes the Route-sets for all the refreshes of a given owner.
	 * @param aOwner. The ownership is not transferred.
	 */
	virtual void RemoveRouteSets (const MRefreshOwner* aOwner) = 0;

	/**
	 * Set all the registration ids empty for the given refresh owner.
	 *
	 * @param aOwner. The ownership is not transferred.
	 */
	virtual void SetRegistrationIdsEmpty (const MRefreshOwner* aOwner) = 0;
	
	/**
	 * Set the refresh interval for the request. 
	 * If no request is pending adjusts the timer immediately.
	 * If a request is pending, the new interval will be taken into use
	 * after the final response for the pending request has been received. 
	 *
	 * @param aRefreshId identifies the refresh
	 *
	 * @param aInterval the interval with which the request is refreshed
	 */	
	virtual void SetIntervalL  (const TRefreshId& aRefreshId,
	                            TInt aInterval) = 0;
	
	/**
	 * Gets the refresh interval for the request.
	 *
	 * @param aRefreshId identifies the refresh
	 *
	 * @return refresh interval
	 */		
	virtual TInt IntervalL (const TRefreshId& aRefreshId) = 0;
	
	/**
	 * Gets the current transaction id for a refresh.
	 *
	 * @param aRefreshId identifies the refresh
	 *
	 * @return current transaction id, if found, otherwise KEmptyTransactionId
	 */	
	virtual TTransactionId CurrentTransactionId(
	    const TRefreshId& aRefreshId) const = 0;
        
	/**
	 * Gets a MSIPSecUser related to a refresh.
     *
     * @param aRefreshId the refresh id
     *
     * @return MSIPSecUser related to the given refresh id, if found,
     *         otherwise NULL.
	 */    
    virtual const MSIPSecUser* SIPSecUser(
        const TRefreshId& aRefreshId) const = 0;    
    };

#endif // MSIPREFRESHMGR_H

// End of File
