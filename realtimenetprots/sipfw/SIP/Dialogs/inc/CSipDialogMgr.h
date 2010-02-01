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
* Name          : CSipDialogMgr.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPDIALOGMGR_H
#define CSIPDIALOGMGR_H

#include <e32base.h>
#include <stringpool.h>
#include "SipStackServerDefs.h"
#include "sipinternalstates.h"
#include "MSipDialogs.h"
#include "MConnectionStateObserver.h"

class CSipDialogItem;
class CDialogsStore;
class MSipRegistrationContact;
class MTransactionUser;
class MTransactionHeaders;
class MSipRefreshMgr;
class MDialogOwner;
class CDeleteMgr;
class CSIPFromToHeaderBase;
class CSIPFromHeader;
class CSIPToHeader;
class CSIPRouteHeaderBase;
class CURIContainer;
class CSIPContactHeader;
class CSIPCallIDHeader;
class CSIPMessage;
class CEstablishing;
class CEarly;
class CConfirmed;
class CTerminated;
class CSIPSec;
class MSIPSecUser;
class MSigComp;
class TSIPTransportParams;


/**
 * Dialog manager creates, controls and deletes dialogs.
 * Dialog manager provides requests and responses from server core to transaction user.
 * Transaction user responses for requests provided by dialog manager and dialog manager
 * creates dialogs for successfull requests. 
 * Dialog manager includes dialogs, which contain states. When provisional response is got
 * for a request dialog is created. Dialogs states are also provided.
 */
class CSipDialogMgr : public CBase,
					  public MSipDialogs,
					  public MConnectionStateObserver
    {
public: // Constructors and destructor
	
    static CSipDialogMgr* NewL(MTransactionUser& aTU,
                               MSipRefreshMgr& aRefreshMgr,
                               MSipRegistrationContact& aRegistrationContact,
                               CSIPSec& aSIPSec,
                               MSigComp& aSigComp);
    ~CSipDialogMgr ();


public: // From MSipDialogs

	MTransactionOwner* TransactionOwnerL(CSIPRequest& aRequest,
	                                     CSIPResponse** aResponse);


public: // From MConnectionStateObserver

	void ConnectionStateChangedL(TUint32 aIapId,
                                 CSIPConnection::TState aState);


public: // New functions

	/**
	* Dialog manager creates dialog and 
    * sends request provided by server core to TU or Refreshes.
	*
    * @pre aSipRequest != 0
    * @pre aOwner != 0
    *
	* @param aTransactionId on return contains the id 
	*        of the created transaction
	* @param aRefreshId if the request is refreshed 
	*        on return contains the id of the created refresh	
	* @param aRegistrationId a registration id with 
	*        which a possible outbound proxy is used in TU.
	* @param aDialogId on return contains the id of the created dialog
	* @param aSipRequest a SIP Request to be sent. 
	*        The ownership is transferred.
    * @param aRemoteTarget; a remote target to be copied 
    *        to the dialog to be established.
	* @param aOwner Dialog owner
	* @param aSIPSecUser a user of SIPSec
	* @param aRefresh If EFalse, the request will be sent to TU. 
    *        Otherwise a refresh is created.
	*/
	void SendAndCreateDialogL(TTransactionId& aTransactionId,
	                          TRefreshId& aRefreshId,
	                          TRegistrationId aRegistrationId,
	                          TDialogId& aDialogId,
	                          CSIPRequest* aSipRequest,
	                          CURIContainer& aRemoteTarget,
	                          MDialogOwner* aOwner,
	                          MSIPSecUser& aSIPSecUser,
	                          TBool aRefresh=EFalse);

	/**
	* Dialog already created. Request is sent within a dialog.
	*	 
	* @param aTransactionId on return contains the id 
	*        of the created transaction
	* @param aRefreshId if the request is refreshed 
	*        on return contains the id of the created refresh
	* @param aDialogId an existing dialog id
	* @param aSipRequest SIP Request. The ownership is transferred.
	* @param aOwner Dialog owner
	* @param aRefresh If EFalse, request will be sent TU.
    *        Otherwise a new refresh is created. 
	* @param aAllowOverlap If ETrue, any requests can be sent.
    *        Otherwise requests with the same method are not allowed
	*		  while this request is pending.
	* @param aTargetRefresh If ETrue, localand remote target will be updated.
	*/	                           
	void SendWithinDialogL(TTransactionId& aTransactionId,
	                       TRefreshId& aRefreshId,
	                       const TDialogId& aDialogId,
	                       CSIPRequest* aSipRequest,
	                       const MDialogOwner* aOwner,
	                       TBool aRefresh=EFalse,
	                       TBool aAllowOverlap=ETrue,
	                       TBool aTargetRefresh=EFalse);

	/**
	* Dialog already created. Request is sent within a dialog and
    * an exiting refresh is updated.
	*	
	* @param aTransactionId transaction id
    * @param aRefreshId an existing refresh id
	* @param aDialogId an existing dialog id
	* @param aSipRequest SIP Request. The ownership is transferred.
	* @param aOwner Dialog owner
	*/
	void SendAndUpdateRefreshL(TTransactionId& aTransactionId,
	                           const TRefreshId& aRefreshId,
	                           const TDialogId& aDialogId,
	                           CSIPRequest* aSipRequest,
	                           const MDialogOwner* aOwner);

	/**
	* Dialog manager creates dialog and sends the response to TU.
	*
	* @param aTransactionId Transaction id
	* @param aRegistrationId a registration id with 
	*        which a possible outbound proxy is used in TU.
	* @param aDialogId Dialog id 
	* @param aSipResponse SIP Response. The ownership is transferred.
	* @param aOwner Dialog owner
	* @param aSIPSecUser a user of SIPSec
	*/
	void SendAndCreateDialogL(const TTransactionId& aTransactionId,
							  TRegistrationId aRegistrationId,
	                          TDialogId& aDialogId,
	                          CSIPResponse* aSipResponse,
	                          MDialogOwner* aOwner,
	                          MSIPSecUser& aSIPSecUser);

	/**
	* Dialog already created. Response is sent within a dialog. 
	*
	* @param aTransactionId transaction id
	* @param aDialogId an existing dialog id
	* @param aSipResponse SIP Response. The ownership is transferred.
	* @param aOwner Dialog owner
	* @param aTargetRefresh If ETrue, remote and local targets are updated.
	*/
	void SendWithinDialogL(const TTransactionId& aTransactionId,
	                       const TDialogId& aDialogId,
	                       CSIPResponse* aSipResponse,
	                       const MDialogOwner* aOwner,
	                       TBool aTargetRefresh=EFalse);

    /**
	* The dialog is removed. 
    * Dialogs will not anymore call the dialog owner.
	*
	* @param aDialogId an existing dialog id
	* @param aOwner Dialog owner
    * @returns KErrNone if the dialog was found, otherwise KErrNotFound
	*/
	TInt TerminateDialog(const TDialogId& aDialogId,
	                     const MDialogOwner* aOwner);

	/**
	* All the dialogs for a dialog owner are removed.
    * Dialogs will not anymore call the dialog owner.
	*
	* @param aDialogId	an existing dialog id
    * @returns KErrNone if dialogs for the owner were found,
    *          otherwise KErrNotFound.
	*/
    TInt TerminateDialogsFor(const MDialogOwner* aOwner);
	
    /**
    * Terminates a refresh within a dialog abnormally. 
    *
    * @param aDialogId an existing dialog id
    * @param aRefreshId an existing refresh id
    * @param aOwner Dialog owner
    * @returns KErrNone if the dialog and the refrsh were found,
    *          otherwise KErrNotFound.
    */	
    TInt TerminateDialogRefresh(const TDialogId& aDialogId,
                                const TRefreshId& aRefreshId,
                                const MDialogOwner* aOwner);

    /**
    * Gets existing dialogs state if the dialog is not found
    * leaves with KErrNotFound
    *
    * @param aDialogId an existing dialog id
    * @param aState contains dialog state 
    */
	void GetStateL(const TDialogId& aDialogId,
	               CSIPInternalStates::TState& aState);

    /**
    * Gets a Call-ID-header related the given dialog id.
    * Leaves with KErrNotFound if the dialog was not found.
    *
    * @param aDialogId an existing dialog id
    * @param aOwner Dialog owner
    * @returns a Call-ID-header related to the dialog
    */
	const CSIPCallIDHeader& CallIDL(const TDialogId& aDialogId,
	                                const MDialogOwner* aOwner);
	 
    /**
    * Gets the local tag related the given dialog id.
    * Leaves with KErrNotFound if the dialog was not found.
    *
    * @param aDialogId an existing dialog id
    * @param aOwner Dialog owner
    * @returns the local tag related to the dialog
    */	                                
	const TDesC8& LocalTagL(const TDialogId& aDialogId,
	                        const MDialogOwner* aOwner);

    /**
	* Resets the state of the dialog but keeps the data such as 
	* Call-ID, CSeq, local and remote target, local tag and route set.
	*
	* @param aDialogId an existing dialog id
	* @param aOwner Dialog owner
	*/    
	void ResetStateL(const TDialogId& aDialogId,
	                 const MDialogOwner* aOwner);
	                 
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

    CSipDialogMgr (MTransactionUser& aTU,
                   MSipRefreshMgr& aRefreshMgr,
                   MSipRegistrationContact& aRegistratinonContact,
                   CSIPSec& aSIPSec,
                   MSigComp& aSigComp);
    
	void ConstructL ();
	
    CSIPContactHeader* ExtractContactL (
        TTransactionId aTransactionId,
        MTransactionHeaders& aHeaders,
        const TSIPTransportParams& aParams,
        TBool aContactMandatory);
                                        
    void CheckRecordRouteL (TTransactionId aTransactionId,
                            RPointerArray<CSIPRouteHeaderBase>& aRecordRoute,
                            const TSIPTransportParams& aParams);                                   

    void AddContactHeaderL (TUint32 aIapId,
                            TRegistrationId aRegistrationId,
                            CSIPRequest& aRequest,
                            const CSIPFromHeader* aFrom);

    void AddContactHeaderL (TUint32 aIapId,
                            CSIPResponse& aResponse,
                            const CSIPToHeader* aTo,
							const CURIContainer& aRequestUri);

	void AddContactHeaderL (CSipDialogItem& aDialog,
                            CSIPMessage& aMessage,
                            const CSIPFromToHeaderBase* aFromTo,
                            TBool aTargetRefresh);

	void CreateAndAddContactL (TUint32 aIapId,
	                           TRegistrationId aRegistrationId,
                               CSIPMessage& aMessage,
                               const CSIPFromToHeaderBase* aFromTo);

	void FillHostInContactL (TUint32 aIapId,
	                         TRegistrationId aRegistrationId,
		                     CSIPMessage& aMessage,
							 const CSIPFromToHeaderBase* aFromTo);

	void CreateAndAddContactL (const CURIContainer& aUri,
	                           CSIPMessage& aMessage);
	                                                    
	void SendErrorResponseAndLeaveL (TTransactionId aTransactionId,
	                                 TInt aLeaveCode,
	                                 const TSIPTransportParams& aParams);
	                                 
	static void DeleteTransactionHeaders (TAny* aTransactionHeaders);
	
	static void DeleteRouteSet (TAny* aRouteSet);
	
	static void DeleteContacts (TAny* aContacts);	


private: // Data

    MTransactionUser& iTU;
	MSipRefreshMgr& iRefreshMgr;
	MSipRegistrationContact& iRegistrationContact;
	CSIPSec& iSIPSec;
	MSigComp& iSigComp;
	CDeleteMgr*	iDeleteMgr;
	CDialogsStore* iDialogsStore;
	RStringF iContactHeaderName;
	RStringF iRouteHeaderName;
	RStringF iTagParamName;
	RStringF iPhraseCallTransactionDoesNotExist;
	RStringF iPhraseBadRequest;
    
private: // For testing purposes

	UNIT_TEST(CSipDialogMgrTest)
    };

#endif // CSIPDIALOGMGR_H

// End of File
