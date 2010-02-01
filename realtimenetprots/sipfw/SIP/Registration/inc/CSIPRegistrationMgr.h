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
* Name          : CSIPRegistrationMgr.h
* Part of       : Registration
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONMGR_H
#define CSIPREGISTRATIONMGR_H

#include <e32base.h>
#include <in_sock.h>
#include "MSipRegistrations.h"
#include "MSipRegistrationContact.h"
#include "MConnectionStateObserver.h"
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class CSIPRegistrationBindingStore;
class CSIPRegistrarStore;
class CSIPRouteHeader;
class CSIPFromHeader;
class CSIPRequest;
class CSIPContactHeader;
class CURIContainer;
class CSIPURI;
class MTransactionUser;
class MSipRefreshMgr;
class MTimerManager;
class CSIPSec;
class MSIPSecUser;
class MSigComp;
class MSIPTransportMgr;
class MRegistrationOwner;
class TSIPTransportParams;
class MTransactionOwner;


/**
 * This class implements interface functions of Sip Registration subsystem.
 */
class CSIPRegistrationMgr : public CBase,
						    public MSipRegistrations,
							public MSipRegistrationContact,
							public MConnectionStateObserver
	{
public: // Constructors and destructor

	static CSIPRegistrationMgr* NewL(MTransactionUser& aTU, 
									 MSipRefreshMgr& aRefreshMgr,
									 MTimerManager& aTimerManager,
									 CSIPSec& aSIPSec,
									 MSigComp& aSigComp,
									 MSIPTransportMgr& aTransportMgr);

	~CSIPRegistrationMgr();


public: // From MSipRegistrations

	TBool CheckRequestURI(CURIContainer& aRequestUri);

	const CSIPRouteHeader* OutboundProxy(TRegistrationId aRegistrationId);

	TBool HasOutboundProxy(TRegistrationId aRegistrationId) const;
   
    TBool IsOutboundProxy(const CURIContainer& aUri);
       
    void URIFailed(CURIContainer& aUri);
    
    TBool IsCacheOutboundProxyIPEanbled(
    	TRegistrationId aRegistrationId) const;
    	
    CSIPRouteHeader& OutboundProxyIPL(
    	TRegistrationId aRegistrationId) const;

public: // From MSipRegistrationContact

	void ContactL(TUint32                     aIapId,
	              TRegistrationId             aRegistrationId,
		          const CSIPFromToHeaderBase* aFromToHeader,
	              CSIPContactHeader&          aContact);


public: // From MConnectionStateObserver		

	void ConnectionStateChangedL (TUint32                aIapId,
                                  CSIPConnection::TState aState);
	

public: // New functions

	/** 
	 * Function sends a Register request to registration subsystem.  
	 *
	 * @pre aRequest!=0
	 * @pre aRegistrationOwner!=0
	 *
	 * @param  aTransactionId; On return contains the id 
	 *         of the created transaction.
	 * @param  aRegistrationId; On return contains the id 
	 *         of the created registration binding object.
	 * @param  aRequest; a sip request. the ownership is transferred.
     * @param  aRemoteTarget; a remote target to be copied to the
     *         registrar if it the registrar does not already exist.
     *         The ownership is NOT transferred.
	 * @param  aOwner; the ownership is NOT transferred.
	 * @param  aSIPSecUser a user for SIPSec	
	 * @param  aOutboundProxy; an outbound proxy to be used with the
	 *         binding to be created. The ownership is transferred.
	 *         If 0 no outbound proxy will be used.
	 * @param aSendWithExpires when set to EFalse REGISTER message is sent
	 *        without expires value
	 * @param aCacheOutboundProxyIP when set to ETrue outbiund proxy IP
	 *        of REGISTER message is cached in registration context
	 * @param  aRefresh; If ETrue, the request will be refreshed.
	 */
    void RegisterL(TTransactionId& aTransactionId,
                   TRegistrationId& aRegistrationId,
                   CSIPRequest* aRequest,
                   CURIContainer& aRemoteTarget,
                   MRegistrationOwner* aOwner,
                   MSIPSecUser& aSIPSecUser,
                   CSIPRouteHeader* aOutboundProxy=0,
                   TBool aRefresh=EFalse,
                   TBool aSendWithExpires=ETrue,
                   TBool aCacheOutboundProxyIP=EFalse);

	/** 
	 * Function sends an update request.
	 * Registration fills To, From, Call-ID and CSeq.
	 * Also Contact will be filled if not present.
	 * If a different CSIPAddress is provided in Contact, leaves
	 * with KErrArgument. Leaves with KErrSIPInvalidRegistrationState
	 * if no registration binding was found.
	 *
	 * @pre aRequest!=0
	 * @param  aRegistrationId; the id of the registration binding to be
	 *         updated.
	 * @param  aTransactionId;  an empty transaction id that will 
	 *         be filled by TU.
	 * @param  aRequest; a sip request. The ownership is transferred.
	 * @param  aOwner the owner that must match to the original owner
	 */
    void UpdateRegistrationL(const TRegistrationId& aRegistrationId,
                             TTransactionId& aTransactionId,
                             CSIPRequest* aRequest,
                             const MRegistrationOwner* aOwner);

	/** 
	 * Function sends an unregister request.
	 * Registration fills To, From, Contact, Call-ID, CSeq.
	 * Contact-header's expire-parameter is updated to 0 by
	 * registration before added to the request.
	 * Leaves with CSIPClient::ENonExistingRegistrationId if 
	 * no registration binding was found.
	 *
	 * @pre aRequest!=0
	 * @param  aRegistrationId; the id of the registration binding to be
	 *         removed by unregistering.
	 * @param  aTransactionId;  an empty transaction id that will be 
	 *         filled by TU.
	 * @param  aRequest; a sip request. The ownership is transferred.
	 *         No Contact is allowed to be present. Panics if there is.
	 * @param  aOwner the owner that must match to the original owner
	 */
    void UnRegisterL(const TRegistrationId& aRegistrationId,
                     TTransactionId& aTransactionId,
                     CSIPRequest* aRequest,
                     const MRegistrationOwner* aOwner);

	/** 
	 * Function sets an outbound proxy for a registration binding.
	 * Leaves if the binding was not found. 
	 *
	 * @pre aOutboundProxy != 0
	 * @param  aRegistrationId; the id of the registration binding
	 *         for which the outbound proxy should be set.
	 * @param  aOutboundProxy; an outbound proxy to be used with the
	 *         binding to be created. The ownership is transferred.
	 * @param  aOwner the owner that must match to the original owner        
	 */	
    void SetOutboundProxyL(const TRegistrationId& aRegistrationId,
                           CSIPRouteHeader* aOutboundProxy,
                           const MRegistrationOwner* aOwner);
	
	/** 
	 * Function removes an outbound proxy for a specified registration
	 * binding.
	 *
	 * @param  aRegistrationId; the id of the registration binding
	 *         from where the outbound proxy should be removed.
	 *
	 * @param  aOwner the owner that must match to the original owner
	 *
	 * @return KErrNotFound if the binding
	 *         with a given id was not found, otherwise KErrNone.          
	 */
    TInt RemoveOutboundProxy(const TRegistrationId& aRegistrationId,
                             const MRegistrationOwner* aOwner);

	/** 
	 * Fetch all the registrations. Leaves on failure.
	 *
     * @pre aRequest!=0
	 * @pre !aRequest.HasHeader(KContactHeaderFullName)
     * @pre aRemoteTarget!=0
     * @pre aTransactionOwner!=0
     *
     * @param  aTransportParams transport parameters to be used by TU
	 * @param  aTransactionId; an empty transaction id that is filled by TU.
	 * @param  aRequest; a sip request. the ownership is transferred.
     * @param  aRemoteTarget; The remote target to be used.
     * @param  aTransactionOwner; To be passed as such to TU.
     *         The ownership is NOT transferred.
	 *
	 */
    void FetchRegistrationsL(const TSIPTransportParams& aTransportParams,
                             TTransactionId& aTransactionId,
                             CSIPRequest* aRequest,
                             CURIContainer& aRemoteTarget,
                             MTransactionOwner* aTransactionOwner);

    
	/** 
	 * Copies the POINTERS of the Route-set objects for 
     * the given registration-id to the givena array.
     * If a Route-set was not found, the function silently returns
     * without filling anything.
	 *
	 * @param  aRegistrationId; the id of the registration
	 *         for which the Route-set should be filled.
     * @param  aRouteSet. IN: empty array, OUT: array filled with pointers
     *         to the stored Route-headers.
     * @param  aAddOutboundProxy
     *         If ETrue, add pointer to the outbound proxy as the FIRST entry. 
     *         If ETrue and there is no Service-Route but an outbound proxy,
     *         do NOT add anything.
     *         If EFalse ignore outbound proxy, but add Service-Route if any. 
     *
	 */
    void GetRouteL(const TRegistrationId& aRegistrationId,
                   RPointerArray<CSIPRouteHeader>& aRouteSet,
                   TBool aAddOutboundProxy=EFalse);

    /**
     * Creates a from header by giving registration Id.
     *
     * @param aRegistrationId; a registrationId of existing registration,
     *
     * @returns a new From-header instance. The ownership is transferred.
     */
    CSIPFromHeader* CreateFromLC(const TRegistrationId& aRegistrationId);

	/** 
	 * Delete all the registration bindings related to the registration 
	 * owner passed as parameter. Does NOT delete the registration owner.
	 *
	 * @param  aOwner; a owner
	 *
	 * @return KErrNone if the owner was found and all related bindings 
	 *         were detached.
	 */
    TInt ClearRegistrationOwner(const MRegistrationOwner* aOwner);		   
	   
    /* Terminates a registration with a given registration id abnormally. 
	 * Deletes the binding immediately.
     *
	 * @param aRegistrationId; a registration Id of an existing registration. 
     *
     * @param  aOwner the owner that must match to the original owner
     *
	 * @returns KErrNotFound if no registration
	 *          with the given id was found, otherwise KErrNone.
     */
    TInt TerminateRegistration (const TRegistrationId& aRegistrationId,
                                const MRegistrationOwner* aOwner);
    
    /**
     * Gets a pointer to the contact header related 
     * to the registration binding having the given ID
     *
     * @param aRegistrationId a ID of a registration binding
     *
     * @returns a pointer to a Contact-header if the binding was found.
     *          NULL if the binding was not found
     *          The ownership is not transferred.
     */    
    const CSIPContactHeader* Contact(const TRegistrationId& aRegistrationId);

    /**
     * Gets the transport parameters used for a registration binding.
     *
     * @param aRegistrationId a ID of a registration binding
     * @param aTransportParams on return contains the 
     *        transport parameters if found
     *
     * @returns ETrue if the registration binding was found and 
     *          the transport parameters were filled, otherwise EFalse.
     */    
    TBool FillTransportParams(
        const TRegistrationId& aRegistrationId,
        TSIPTransportParams& aTransportParams);
        
	/**
	 * Gets the MSIPSecUser related to the given registration id.
     *
     * @param aRegistrationId the registration id
     *
     * @returns the user related to the given registration id if found,
     *          otherwise NULL.
	 */    
    const MSIPSecUser* SIPSecUser(const TRegistrationId& aRegistrationId);         

	/**
	 * Gets a MSIPSecUser related to a transaction.
     *
     * @param aTransactionId the transaction id
     *
     * @return MSIPSecUser related to the given transaction id, if found,
     *         otherwise NULL.
	 */    
    const MSIPSecUser* SIPSecUserForTransaction(
        const TTransactionId& aTransactionId);


private: // New functions

	CSIPRegistrationMgr(MTransactionUser& aTU, 
						MSipRefreshMgr&   aRefreshMgr,
						MTimerManager&    aTimerManager,
						CSIPSec&          aSIPSec,
						MSigComp&         aSigComp,
						MSIPTransportMgr& aTransportMgr);

	void ConstructL();

    void FillHostWithLocalIPL (TUint32 aIapId, CSIPURI& aSipUri);    

	static void DetachBinding(TAny* aBinding);

	
private: // Data

	MTransactionUser&             iTU;
	MSipRefreshMgr&               iRefreshMgr;
	MTimerManager&                iTimerMgr;
	CSIPSec&                      iSIPSec;
	MSigComp&                     iSigComp;
	MSIPTransportMgr&             iTransportMgr;
	CSIPRegistrationBindingStore* iBindingStore;
	CSIPRegistrarStore*           iRegistrarStore;

private: // For testing purposes

	UNIT_TEST(CSIPRegistrationMgrTest)
	};

#endif // CSIPREGISTRATIONMGR_H

// End of File
