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
* Name          : UserAgentClient.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef USERAGENTCLIENT_H
#define USERAGENTCLIENT_H

// INCLUDES
#include "sipsecobserver.h"
#include "MSIPServerResolverObserver.h"
#include "CUserAgent.h"

// FORWARD DECLARATIONS
class RStringF;
class CSIPViaHeader;
class CSIPSec;
class CRouteSet;
class CResolvingResults;

// CLASS DECLARATION

/**
 * This is a base class for all UACs. 
 */
class CUserAgentClient :
	public CUserAgent,
	public MSIPServerResolverObserver,
	public MSIPSecObserver
	{
public: // Destructor

	virtual ~CUserAgentClient();

public: // From CUserAgent

	TBool IsUAS() const;

protected: // From CUserAgent

	virtual void CreateTransactionL() = 0;

private:  // From CUserAgent

	TRegistrationId RegistrationId() const;

	/**
	 * UAC detaches all references to the outgoing SIP message.
	 * Depending on the current state of UAC; it may need to stop as in
	 * certain states it cannot proceed without the outgoing SIP request.
	 *
	 * @post iOutgoingMsg = NULL
	 */
	virtual void DetachOutgoingMsg();

public: // From MSIPServerResolverObserver

	void CompletedL();

	void ErrorOccured(TInt aError);	

public:	// From MSIPSecObserver

	void SIPSecCacheUpdated(TBool aSuccess);

public: //New functions

	/**
	 * Handle a SIP request sent by upper layer. Request is checked, filled and
	 * the remote address is determined and resolved. Transaction related parts
	 * of the request are stored in CTransactionStore.
	 *	 
	 * @pre aReq != NULL
	 *
	 * @param aReq SIP request message. Ownership is transferred.
	 * @param aRegistrationId Registration id by which TU tries to find an
	 *	outbound proxy for the request.
	 * @param aRemoteTarget Remote target of the request
     * @param aResolve State the UAC will enter when it starts resolving URI
	 */
	void HandleSendRequestL(CSIPRequest* aReq,
							TRegistrationId aRegisterId,
							const CURIContainer& aRemoteTarget,
                            const CUserAgentState& aResolve);

	/**
	 * Handle a SIP response received from network.
	 *	 
	 * @pre aResp != NULL
	 *
	 * @param aResp SIP response, ownership is transferred
	 * @param aResolve State the UAC enters if it starts resolving URI
	 * @param aWaitResponse State the UAC enters if it sends the request again
	 * @param aWaitAckFromApp State InviteUAC enters if a 2xx response is
	 *	passed to upper layer
	 * @param aWaitTransactionToEnd State InviteUAC enters if an error response
	 *	is passed to upper layer	 
	 */	
	virtual void ReceiveResponseL(CSIPResponse* aResp,						  
						  const CUserAgentState& aResolve,
						  const CUserAgentState& aWaitResponse,
						  const CUserAgentState& aWaitAckFromApp,
						  const CUserAgentState& aWaitTransactionToEnd) = 0;
	
	/**
	 * Sets the value for CUserAgent::iOwnsOutgoingMsg
	 *
	 * @param aOwnsRequest ETrue: UAC owns the request in the send buffer
	 *	(CUserAgent::iOutgoingMsg), EFalse: UAC doesn't own the request.
	 */	
	void SetRequestOwnership(TBool aOwnsRequest);

	/**
	 * Store the SIP response for asynchronous processing.
	 *	 
	 * @pre iIncomingMsg == NULL
	 *
	 * @param aResponse SIP response, ownership is transferred.
	 */	
	void StoreResponse(CSIPResponse* aResponse);

	/**
	 * Pick the next remote address and transport protocol from the resolved
	 * addresses.
	 *	 
	 * @pre iOutgoingMsg != NULL && iResolvingResults != NULL
	 *
	 * @return ETrue if an address and protocol were found. EFalse if no more
	 *	addresses are to try.
	 */	
	TBool SelectNextRemoteAddressL();

	/**
	 * The remote address has now been resolved and UAC can send the SIP
	 * request to CTransaction.
	 *
	 * @param aNextState State where UAC moves after sending the request	 
	 */	
	void RemoteAddressResolvedL(const CUserAgentState& aNextState);	

	/**
	 * Returns information whether the UserAgent has passed a final response to
	 * the upper layer.
	 *
	 * @return ETrue: UAC has already passed a final response, no more
	 *	responses can be passed. EFalse: final response not passed.
	 */
	TBool FinalRespPassed() const;

	/**
	 * Dummy timer used to make a synchronous SIPSec call to work as
	 * asynchronous, has completed.
	 */
	void SIPSecTimerExpiredL();

	/**
	 * SIPSec processing has been completed.
	 */
	TInt SIPSecReady(TBool aSuccess);

	/**
	 * SIPSec has completed updating the cache.
	 *
	 * @pre iIncomingMsg != NULL	 
	 * @post iIncomingMsg == NULL
	 *
	 * @param aSuccess status of the authorized SIP request
	 *		  ETrue: SIPSec has executed successfully
	 *		  EFalse: otherwise
	 * @param aResolve State the UAC enters if it starts resolving URI
	 * @param aWaitResponse State the UAC enters if it sends the request again
	 * @param aWaitAckFromApp State InviteUAC enters if a 2xx response is
	 *	passed to upper layer
	 * @param aWaitTransactionToEnd State InviteUAC enters if an error response
	 *	is passed to upper layer	 
	 *	upper layer	 
	 */
	void SIPSecCacheUpdatedL(TBool aSuccess,
						     const CUserAgentState& aResolve,
						     const CUserAgentState& aWaitResponse,
							 const CUserAgentState& aWaitAckFromApp,
							 const CUserAgentState& aWaitTransactionToEnd);

	/**
	 * Offers the aResp to SIPSec for processing.
	 * Provisional responses are not offered.
	 *
	 * @pre iOutgoingMsg != NULL && iRemoteTarget != NULL
	 *
	 * @param aResp SIP response received from the network.
     * @return ETrue  SIPSec processes the response asynchronously.     
     *		   EFalse SIPSec has synchronously processed the response.     
	 */
	TBool PassResponseToSIPSecL(CSIPResponse& aResp);

    /**
	 * If UAC has more remote addresses to try, updates the SIP request and
	 * sends it to the next address using a new CTransaction object.
	 *
	 * @param aError Reason why the next address will be tried. If aError is
	 *	KErrNone, aResp must have a SIP response. If aError is not KErrNone,
	 *	aResp must be NULL.
	 * @param aResolve State which UAC will enter if it decides to resolve the
	 *	outbound proxy.
	 * @param aWaitResponse State which UAC enters if it decides to send
	 *	request to the next ip-address.
	 * @param aResp If transaction ended because of SIP response, this is the
	 *	response. Ownership is transferred if TryNextAddressL returns ETrue.
     * @return value ETrue if the request will be sent to next address.
     *	Ownership of aResp has been taken.
	 *	EFalse If request won't be sent again. Ownership of aResp has not been
	 *	taken.
	 */
	TBool TryNextAddressL(TInt aError,
						  const CUserAgentState& aResolve,
						  const CUserAgentState& aWaitResponse,
						  CSIPResponse* aResp = NULL);

	/**
	 * UAC receives a response from network. Determines whether UAC should
	 * send the original request again, possibly to another address, or should
	 * UAC pass the response to upper layer.
	 *
	 * @pre aResp != NULL	 
	 *
	 * @param aResp SIP response. If UAC sends the original request again, the
	 *	ownership of aResp is transferred.
	 * @param aResolve State which UAC enters if it decides to resolve the
	 *	outbound proxy.
	 * @param aWaitResponse State which UAC enters if it decides to send
	 *	request to the next ip-address.
	 * @return ETrue: UAC sends the request again and aResp isn't passed to
	 *	upper layer. Ownership of aResp has been taken.
	 *	EFalse: aResp should be passed to upper layer and it's ownership has
	 *  not been taken.
	 */
	TBool ShouldUaTryAgainL(CSIPResponse* aResp,
						    const CUserAgentState& aResolve,
						    const CUserAgentState& aWaitResponse);

	/**
	 * CTransaction has ended before a final response was received
	 *
	 * @param aReason Reason why CTransaction ended
	 * @param aResolve State which UAC will enter if it decides to resolve URI
	 * @param aWaitResponse State which UAC enters if it decides to send
	 *	request to the next ip-address.	 
	 */    
	void TransactionEndsWithoutFinalResponseL(TInt aReason,
									 const CUserAgentState& aResolve,
									 const CUserAgentState& aWaitResponse);

    /**
	 * Remove message headers from CTransactionStore, so that in case a
     * retransmission of a response is received, it will be dropped.
     * This is required when ClientTransaction terminates before SIPSec has
     * completed adding authentication parameters to the request.	 
	 */
    void IgnoreResponseRetransmissionsL();	

    /**
	 * Cancel a pending asynchronous request issued to SIPSec.	 
	 */
    void CancelSIPSecRequest();

	/**
	 * Stores the remote target, replacing the possibly existing value.
	 *
	 * @param aRemoteTarget Remote target
	 */
	void StoreRemoteTargetL(const CURIContainer& aRemoteTarget);	

	/**
	 * Handle the situation when resolving address has failed.
	 *	 
	 * @param aResolve State which UAC will enter if it decides to resolve
	 *	another address
	 * @param aWaitResponse State which UAC enters if it decides to send
	 *	request to the next ip-address.	 
	 */							  
	void HandleResolvingFailure(const CUserAgentState& aResolve,
						  		const CUserAgentState& aWaitResponse);

    /**
	 * Returns the next hop address.	 
	 */
	const TInetAddr& NextHopIP() const;
	
protected:

	CUserAgentClient(CUserAgentCreateParams& aParams,
					 MSipUriResolver& aResolver,
					 CSIPSec& aSIPSec,
					 TUint32 aCSeqNumber);

	void ConstructL();	

	/**
	 * Fills new Via header with branch, to the aReq.
	 * Host part will be filled later by ConnectionMgr.
	 *
	 * @post aReq has one Via header
	 * @param aReq SIP request
	 * @param aBranch The branch value to put into the Via-header.
	 *		  If this is an empty string, a new branch is generated.
	 */
	void FillNewViaL(CSIPRequest& aReq, RStringF aBranch) const;

    /**
	 * Stores the Request-URI of aReq into iRequestUri member. If iRequestUri
	 * already had a value, it is replaced with the new value.
	 *
	 * @pre aReq.RequestURI() != NULL
	 * @post iRequestUri != NULL
	 *
	 * @param aReq SIP request
	 */
	void StoreRequestUriL(CSIPRequest &aReq);

	/**
	 * Updates the SIP request, so it can be re-sent after modifying it, or
	 * after obtaining another address.
	 * Current Via is removed and an updated Via with a new branch and
	 * transport to match the new remote address. CSeq is always updated.	 
	 *
	 * @pre iOutgoingMsg != NULL
	 * @post The request in iOutgoingMsg has new Via branch & CSeq number
	 */
	void UpdateViaAndCSeqL() const;

	/**
	 * Updates transaction object and some SIP message headers to the
	 * CTransactionStore.
	 *
	 * @param aReq SIP request from where a certain headers are updated to
	 *	the store. If NULL, the headers are not updated.
	 */
	void UpdateInfoToStoreL(CSIPRequest* aReq) const;

	/**
	 * Sends the SIP request in the send buffer (CUserAgent::iOutgoingMsg) to
	 * network. If the request is ACK, it is sent directly with CTransmitter.
	 * Other requests are sent using CTransaction.
	 *
	 * @pre iOutgoingMsg != NULL
	 */
	void SendRequestToNetworkL();

	/**
	 * Passes the response to upper layer, except 100 response.	 
	 *
	 * @pre aResp != NULL, iFinalRespPassed == EFalse	 
	 *
	 * @param aResp SIP response, ownership is transferred
	 * @param aUserAgent If non-NULL, points to UserAgent which has aTimer
	 * @param aTimer If non-NULL, points to timer that will be stopped
	 */
	void PassRespToTransactionOwnerL(CSIPResponse* aResp,
									 CUserAgent* aUserAgent=NULL,
									 CUserAgentTimer* aTimer=NULL);
	
	/**
	 * Copies the currently used remote address and transport protocol from the
	 * given UAC (aSrc).
	 *
	 * @param aSrc UAC from which the address is copied
	 */	
	void CopyRemoteAddress(const CUserAgentClient& aSrc);

	/**
	 * Copies the route set and remote target from the given UAC (aSrc).
	 *
	 * @pre iRouteSet = NULL
	 * @pre iRemoteTarget = NULL
	 * @pre aSrc.iRouteSet != NULL
	 * @pre aSrc.iRemoteTarget != NULL
	 * @param aSrc UAC from which the route set and remote target are copied
	 */
	void CopyRouteSetAndRemoteTargetL(const CUserAgentClient& aSrc);    

	/**
	 * If UAC used a pre-configured route, but that failed, UAC tries to send
	 * the request to outbound proxy (if one has been configured).
	 *
	 * @param aResolve State which UAC will enter if it decides to try and
	 *	resolve the outbound proxy.
	 * @return value ETrue if the request will be sent to next address,
	 *	EFalse otherwise
	 */
	TBool TryOutboundProxyL(const CUserAgentState& aResolve);	

	/**
	 * Gets the next hop URI.
	 *
	 * @pre iRouteSet != NULL
	 * @pre iRemoteTarget != NULL
	 * @param aUseResolvedProxy, ETrue if resolved address of registration
	 * @return CURIContainer& Next hop URI
	 */
	CURIContainer& NextHopL(TBool aUseResolvedProxy = EFalse) const;

	/**
	 * Resolves the next hop URI.
	 *  
	 * @param aCheckSigComp ETrue if sigcomp support is checked, EFalse
	 * @param aUseResolvedProxy, ETrue if resolved address of registration
	 *	otherwise.
	 */
	void ResolveNextHopL(TBool aCheckSigComp,TBool aUseResolvedProxy = EFalse);
    
	/**
	 * Makes sure the CTransmitter is ready to be used for an updated request.
	 *
     * @param aNewTransactionId TransactionId that is given to the old
     *  transaction.
	 */
    virtual void PrepareTxForNewRequestL(TTransactionId& aNewTransactionId) = 0;

    /**
	 * Let SIPSec fill the security parameters into outgoing request.
	 *
	 * @pre iOutgoingMsg != NULL, iRemoteTarget != NULL
	 *
	 * @return value ETrue if successful, EFalse in case of error
	 */
	TBool FillSecurityParamsL();

protected: // Data

	//SIP Security subsystem
	CSIPSec& iSIPSec;

	//CSeq sequence number to use in the SIP request, unless filled by upper
	//layer.
	TUint32 iCSeqNumber;

	//Address where the request is sent
	TInetAddr iRemoteAddr;

	//ETrue if the transport of iRemoteAddr must not be changed from UDP
	TBool iForceUDP;

	//RegistrationId provided by the upper layer when it sent the request.	
	TRegistrationId iRegisterId;

    //Route set associated with the SIP request, owned.
    CRouteSet* iRouteSet;

	//Remote target of the SIP request, owned.
    CURIContainer* iRemoteTarget;

	//Resolver subsystem
	MSipUriResolver& iResolver;

	//Results obtained from resolving an URI. Owned.
	CResolvingResults* iResolvingResults;
	
	//SIPSec Error
	TInt iSipSecError;

private:

	/**
	 * Fill the transaction related headers to the SIP request, unless they
	 * have already been filled.
	 *
	 * @pre aReq must contain From header
	 * @pre iRemoteTarget != NULL	 
	 *
	 * @param aReq SIP request which is filled	 
	 */
	void FillRequestL(CSIPRequest& aReq) const;

	/**
	 * Fills Call-ID header into aReq
	 *
	 * @param aReq SIP request	 
	 */
	void FillNewCallIdL(CSIPRequest& aReq) const;	

	/**
	 * Sets branch parameter to Via header.
	 *
	 * @post aVia Via header
	 * @param aBranch The branch value to put into the Via-header.
	 *		  If this is an empty string, a new branch is generated.
	 */
	void SetBranchL(CSIPViaHeader& aVia, RStringF aBranch) const;

	/**
	 * Processes the status of a SIPSec operation that can leave. If SIPSec
	 * reported an error, UAC is stopped and HandleSIPSecErrorL leaves.
	 *
	 * @param aError If leave occurred from the SIPSec operation, this is the
	 *				 leave code. Otherwise KErrNone.
	 * @leave If aError != KErrNone, HandleSIPSecErrorL will leave.
	 */
	void HandleSIPSecErrorL(TInt aError);

    /**
	 * Client transaction terminated because of a timeout.
     * If the request was sent compressed, send it uncompressed to the same
     * address. Otherwise if there are other addresses to try, they are
     * used and resolved if necessary.
	 *
	 * @param aResolve UAC will enter this state if it decides to resolve URI.
	 *	Resolving isn't done if UAC already has more resolving results to try.
	 * @param aWaitResponse UAC enters this state if it sends request again.
	 * @return value ETrue if UAC sends the request again or resolves another
     *  address. EFalse if UAC can't continue and will end.
	 */
    TBool HandleTimeoutL(const CUserAgentState& aResolve,
    					 const CUserAgentState& aWaitResponse);	

    /**
	 * Updates Request-URI and Route-headers of the request when reverting to
     * try the outbound proxy.
	 *
	 * @param aProxy Outbound proxy URI	 	 
	 */
    void UpdateRequestUriAndRouteL(const CURIContainer& aProxy);
	
	/**
	 * Detach the current CTransaction object owned by the UserAgent. If the
	 * CTransaction has reached Terminated state, it is freed using DeleteMgr,
	 * otherwise it'll run independently of the UserAgent and when it enters
	 * Terminated state, it'll delete itself using DeleteMgr.	 
	 */	
	void DetachCurrentTransactionL();

	/**
	 * Updates the SIP request and sends it using a new CTransaction object.
	 *
	 * The information of the current CTransaction object is removed from
	 * CTransactionStore, and the old CTransaction is detached from UserAgent.
	 * A new CTransaction instance is created and its information is added into
	 * the CTransactionStore and the SIP request is sent to the new
	 * CTransaction.
	 *	 
	 * @pre iOutgoingMsg != NULL
	 *	 
	 * @param aResolve UAC enters this state
	 */
	void UpdateAndSendRequestL(const CUserAgentState& aWaitResponse);

	/**
	 * Updates the SIP request, resolves the next hop URI and enters state
	 * aResolve.
	 *
	 * @pre iOutgoingMsg != NULL
	 *	 
	 * @param aCheckSigComp ETrue if sigcomp is checked from aUri
	 * @param aResolve UAC enters this state
	 */
	void UpdateAndResolveL(TBool aCheckSigComp,
						   const CUserAgentState& aResolve);

    /**
	 * Stop with a previously stored error code. If a stored response exists,
	 * pass it to upper layer before stopping.
	 */
	void StopWithLastError();	

	/**
	 * If the next hop is an outbound proxy, this function returns the outbound
	 * proxy as descriptor. Otherwise returns an empty descriptor.	 
	 *
	 * @return const TDesC8& Outbound proxy, or empty descritor.
	 */
	const TDesC8& ProxyL() const;

	/**
	 * Determine if SIP messages should be handed to SIPSec or not.
	 * @return ETrue if SIP messages are given to SIPSec, EFalse otherwise
	 */
	TBool UseSIPSec() const;

private: // Data

	// Indicates if UAC has passed a final response to MTransactionOwner
	TBool iFinalRespPassed;

	// Tells if any SIP response has been received.
	TBool iResponseReceived;

    // ETrue if UAC has attempted to resolve the outbound proxy, or sent a
    // request to the outbound proxy. EFalse otherwise.
    TBool iOutboundProxyHasBeenTried;

	// Request-URI of the request, owned
	CURIContainer* iRequestUri;

    // Incremented each time a request and response are passed to SIPSec.
    // Prevents infinite loop if the server always re-challenges.
    TInt iSIPSecCounter;

	// If transaction fails and UAC resolves the current URI again, the reason
	// why transaction failed is stored here. If resolving the same URI fails,
	// the stored error is passed to upper layer.
    TInt iLastError;

	// Normally EFalse. Temporarily set to ETrue to ignore SIPSec's callback
	// when UAC cancels its pending SIPSec requests.
	TBool iIgnoreSIPSecCallback;

private: // For testing purposes

#ifdef CPPUNIT_TEST	
	friend class CUserAgentClient_Test;
	friend class CTransactionUser_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;
	friend class CInviteUAC_ResolveAddress_Test;
	friend class CInviteUAC_WaitResponse_Test;
	friend class CInviteUAC_WaitAckFromApp_Test;
	friend class CInviteUAC_ResolveAckAddress_Test;
	friend class CInviteUAC_SendingAck_Test;
	friend class CInviteUAC_Start_Test;
	friend class CUserAgent_Test;
	friend class CTransactionStore_Test;	
#endif

	void __DbgTestInvariant() const;

	};

#endif // end of USERAGENTCLIENT_H

// End of File
