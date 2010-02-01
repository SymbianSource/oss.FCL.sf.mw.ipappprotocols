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
* Name          : UserAgentServer.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef USERAGENTSERVER_H
#define USERAGENTSERVER_H

// INCLUDES
#include <stringpool.h>

#include "MSIPRequestRouterObserver.h"

#include "CUserAgent.h"

// FORWARD DECLARATIONS
class MSIPRequestRouter;
class MSipDialogs;
class CTimerOwnerResolver;

// CLASS DECLARATION

class CUserAgentServer :
	public CUserAgent,
	public MSIPRequestRouterObserver
	{
public: // Destructor

	virtual ~CUserAgentServer();

public: // From CUserAgent

	TBool IsUAS() const;

protected: // From CUserAgent

	virtual void CreateTransactionL() = 0;

private: // From CUserAgent

	TRegistrationId RegistrationId() const;

public: // From MSIPRequestRouterObserver

	void OwnerFoundL(TUint32 aRequestId, MTransactionOwner* aOwner);

	void OwnerNotFoundL(TUint32 aRequestId, CSIPResponse* aResp);

	void ErrorOccurred(TUint32 aRequestId, TInt aError);

public: // New functions

	/**
	 * The initial request has been received by the UAS.
	 *
	 * @pre aReq != NULL
	 *
	 * @param aReq SIP request received from network. Ownership is transferred.
	 * @param aGetTxOwner State the UAS enters if it tries to obtain
	 * 	MTranactionOwner.
	 * @param aWaitRespFromApp State the UAS enters if the request belongs to
	 *	an existing dialog.
	 * @param aErrorRespSent State the UAS enters when either Dialog subsystem
	 *	or UAS itself decides to send a final response without passing the
	 *	request.	 
	 */
	void InitialRequestReceivedL(CSIPRequest* aReq,
								 const CUserAgentState& aGetTxOwner,
								 const CUserAgentState& aWaitRespFromApp,
								 const CUserAgentState& aErrorRespSent);

	/**
	 * Callback to upper layer has been obtained. UAS passes the request there
	 * and starts waiting for a response.
	 *
	 * @pre aOwner != NULL
	 *
	 * @param aOwner Callback to upper layer, ownership isn't transferred
	 * @param aWaitRespFromApp State which UAS enters next.	 
	 */
	void HandleOwnerFoundL(MTransactionOwner* aOwner,
						   const CUserAgentState& aWaitRespFromApp);

	/**
	 * Application to receive the request was not found. Instead a SIP response
	 * will be sent to network.
	 *
	 * @pre aResp != NULL
	 *
	 * @param aResp Final response. May not be 2xx in case of INVITE.
	 *	Ownership is transferred.
	 * @param aErrorRespSent State which UAS enters next.	 
	 */
	void HandleOwnerNotFoundL(CSIPResponse* aResp,
							  const CUserAgentState& aFinalRespSent);

    /**
	 * Error occurred during the routing of an incoming SIP request.
	 */
    void RequestRouterErrorL();

    /**
	 * Sends error response to remote endpoint.
	 *
     * @param aErrorRespSent UserAgent enters this state	 
	 */
    void HandleRequestRouterErrorL(const CUserAgentState& aErrorRespSent);

	/**
	 * Upper layer sends response. UAS checks that response is correct and
	 * fills the transaction related headers.
	 *
	 * @pre aResp != NULL
	 *
	 * @param aResp SIP response, ownership isn't transferred
	 * @param aParam Transport parameters to use when sending the response	 
	 */
	void HandleSendResponseL(CSIPResponse* aResp,
							 const TSIPTransportParams& aParams);

	/**
	 * Checks that the SIP response message given by upper layer has the
	 * required parts filled. If the response isn't ok, function leaves.
	 *
	 * @param aResp SIP response	 
	 */
	void CheckResponseL(const CSIPResponse& aResp) const;
	
	/**
	 * Fills the transaction related parts into the SIP response.	 
	 *
	 * @param aResp IN/OUT: Partially filled SIP response from upper layer.	 
	 */
	void FillResponseL(CSIPResponse& aResp);

	/**
	 * Contacts Dialogs subsystem to find out if there is a dialog for the
	 * received request
	 *
	 * @pre iObserver = NULL, iOutgoingMsg = NULL
	 *
	 * @param aReq Request received from network
	 * @return value ETrue: Dialog exists, EFalse: otherwise
	 */
	TBool DoesDialogExistForRequestL(CSIPRequest& aReq);

	/**
	 * Pass the SIP response to transaction for sending.
	 *
	 * @pre iTransaction != NULL, aResp != NULL
	 *
	 * @param aResp SIP response to send, ownership is transferred.	 
	 */
	void SendResponseToTransactionL(CSIPResponse* aResp) const;

	/**
	 * Checks the request received from network is correct.
	 *
	 * @pre aReasonPhrase != NULL	 
	 *
	 * @param aReq SIP request received from network
	 * @param aResponseCode If function returns EFalse, this parameter will
	 *  contain the response code of the response that should be sent back to
     *  the sender of the invalid request.
	 * @param aReasonPhrase If function returns EFalse, this parameter will
	 *  have the reason phrase that should be sent back to the sender of the
	 *  invalid request.
	 * @return value ETrue if the request is ok, EFalse otherwise.	 
	 */
	TBool CheckReceivedRequest(CSIPRequest& aReq,
							   TInt& aResponseCode,
							   RStringF& aReasonPhrase) const;

	/**
	 * Obtains the To tag
	 *	 	 
	 * @return value To tag
	 */
	RStringF ToTag() const;

protected:

	CUserAgentServer(CUserAgentCreateParams& aParams,
					 MSipDialogs& aDialogs,
					 MSIPRequestRouter& aRouter);	

	/**
	 * Creates and sends a final response
	 *
	 * @pre aResponseCode >= 300
	 *	 
	 * @param aResponseCode Response code to be put into the response
	 * @param aReasonPhrase Reason phrase to be put in the response
	 * @param aErrorRespSent State which UAS enters after sending the response	 
	 */
	void SendErrorResponseL(TInt aResponseCode,
						    RStringF aReasonPhrase,
						    const CUserAgentState& aErrorRespSent);

    /**
	 * Cancels the asynchronous request to obtain MTransactionOwner callback.
	 */
	void CancelGetOwnerRequest() const;

    void StopTimerOwnerResolver();

    /**
	 * Store the tag of To-header, if it exists.
	 *
	 * @pre iToTag.DesC().Length() == 0
	 *	 
	 * @param aMsg SIP message
	 */
    void StoreToTag(CSIPMessage& aMsg);

protected: // Data

    //Dialog subsystem
	MSipDialogs& iDialogs;

    //Timer specifying how long UAS waits for RequestRouter to provide the
    //MTransactionOwner of upper layer or an error response. Owned.
    CTimerOwnerResolver* iTimerOwnerResolver;

private:

	/**
	 * The received request doesn't belong to any dialog
	 *
	 * @pre aReq != NULL
	 *
	 * @param aReq SIP request. Ownership is transferred.
	 * @param aGetTxOwner State which UAS enters if it contacts AssocMgr for
	 *	obtaining MTranactionOwner.
	 * @param aErrorRespSent State which UAS enters when either Dialog
	 * subsystem or UAS itself decides to send a final response without passing
	 *	the request.
	 */
	void RequestOutsideDialogL(CSIPRequest* aReq,
							   const CUserAgentState& aGetTxOwner,
							   const CUserAgentState& aErrorRespSent);

	/**
	 * Fill and send the SIP response got from Dialogs subsystem. The
	 * response is located in the send buffer (iOutgoingMsg).
	 * Ownership of the response is passed to transaction.
	 *
	 * @pre iOutgoingMsg != NULL
	 * @pre SIP response in iOutgoingMsg must be a 3xx-6xx
	 * @post iOutgoingMsg == NULL
	 *
	 * @param aNextState State where UserAgent moves after sending the response	 
	 */
	void HandleResponseFromDialogsL(const CUserAgentState& aNextState);

    /**
	 * Send the stored SIP request located in the receive buffer
     * (iIncomingMsg) to MTransactionOwner.
	 *
	 * @pre iIncomingMsg != NULL
	 * @post iIncomingMsg == NULL	 
	 */
	void PassStoredRequestToTransactionOwnerL();    

    /**
	 * Start timer determining how long UAS waits for OwnerResolver to
	 * return a callback to upper layer.
     * Timer duration must be shorter than timer F2.
	 *
	 * @pre iTimerOwnerResolver = NULL
	 */
    void StartTimerOwnerResolverL();

private: // Data

	//Router which decides where the incoming SIP request will be forwarded.
	MSIPRequestRouter& iRouter;

	//RequestId received from iRouter
	TUint32 iRouterRequestId;

	//Tag to be inserted into the To-header of SIP responses
	RStringF iToTag;

private: // For testing purposes

#ifdef CPPUNIT_TEST	
	friend class CNormalUAS_GetTxOwner_Test;
	friend class CUserAgentServer_Test;
	friend class CTransactionUser_Test;
#endif

	void __DbgTestInvariant() const;

	};

#endif // end of USERAGENTSERVER_H

// End of File
