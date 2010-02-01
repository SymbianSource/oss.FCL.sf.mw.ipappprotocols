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
* Name          : InviteUAC.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef INVITEUAC_H
#define INVITEUAC_H

// INCLUDES
#include "UserAgentClient.h"

// FORWARD DECLARATIONS
class CTimerStopUA;
class CTimerAsyncSIPSec;


// CLASS DECLARATION

class CInviteUAC : public CUserAgentClient
	{
public: // Constructor and destructor

	/**
	 * Creates a new CInviteUAC instance.
	 *
	 * @param aParams UA parameters
	 * @param aConnectionMgr ConnectionMgr used for sending
	 * @param aResolver For resolving URI
	 * @param aSIPSec SIP security subsystem
	 * @param aCSeqNumber CSeq value to put into INVITE if CSeq was not filled
	 *		  by upper layers.
	 * @param aIsPrivateAddress ETrue if the local address is in IPv4 private
	 *		  address space. EFalse otherwise.
	 * @param aAckSenderState The starting state of the FSM of InviteUACs that
	 *		  send the additional ACK requests to network.
	 * @return New object, ownership is transferred.
	 */
	static CInviteUAC* NewL(CUserAgentCreateParams& aParams,
                            MSipConnectionMgr& aConnectionMgr,
                            MSipUriResolver& aResolver,
							CSIPSec& aSIPSec,
							TUint32 aCSeqNumber,
							TBool aIsPrivateAddress,							
							CUserAgentState* aAckSenderState);

	~CInviteUAC();

public: // From CUserAgentClient

	void ReceiveResponseL(CSIPResponse* aResp,
						  const CUserAgentState& aResolve,
						  const CUserAgentState& aWaitResponse,
						  const CUserAgentState& aWaitAckFromApp,
						  const CUserAgentState& aWaitTransactionToEnd);						  

	/**
     * A new CTransmitter instance is created for InviteUAC and it's new
     * transaction. Ownership of the current CTransmitter is passed to the old
     * transaction. Two separate CTransmitter instances are needed to allow
     * both the old and new CInviteClientTransaction to send messages
     * simultaneously.
	 *
     * @param aNewTransactionId TransactionId that is given to the old
     *  transaction.
	 */
    void PrepareTxForNewRequestL(TTransactionId& aNewTransactionId);

private: // From CUserAgentClient

	void CreateTransactionL();
	
public: // From CUserAgent

    void ClearTransactionOwner();

	void DeleteTimer(const CUserAgentTimer& aTimer);

	TBool IsInviteUAC() const;

	TBool IsSubInviteUAC() const;	

public: // New functions

	/**
	 * Checks if the InviteUAC has been canceled.
	 *
	 * @return ETrue: InviteUAC has been canceled, EFalse: otherwise
	 */	
	TBool IsCanceled() const;

	/**
	 * Start a timer to cause the processing of a response to go asynchronous,
	 * also now that SIPSec processed the response synchronously.
	 */
	void StartSIPSecTimerL();

	/**
	 * Send ACK request.
	 *
	 * @param aAck ACK request. Parameter aDeleteRequest determines whether the
	 *	ownership of aAck is transferred.
	 * @param aObserver IN: Callback to upper layer.
	 * @param aParams Transport parameters to be used for sending the ACK
	 * @param aRegisterId Identifies the registration whose outbound proxy may
	 *	be used.
	 * @param aRemoteTarget Remote target of the ACK
	 * @param aDeleteRequest If ETrue, ownership of aAck is transferred.
	 * @param aBranch The branch value to put into the ACK. If this is an empty
	 *		  string, a new branch is generated.
	 */
	void SendAckL(CSIPRequest* aAck,
				  MTransactionOwner& aObserver,
				  const TSIPTransportParams& aParams,
				  TRegistrationId aRegisterId,
				  const CURIContainer& aRemoteTarget,
				  TBool aDeleteRequest,
				  RStringF aBranch);

	/**
	 * The first ACK to a 2xx is sent.
	 *
	 * @param aAck ACK request. Parameter aDeleteRequest determines whether the
	 *	ownership of aAck is transferred.
	 * @param aObserver	IN: Callback to upper layer.
	 * @param aParams Transport parameters to be used for sending the ACK
	 * @param aRegisterId Identifies the registration whose outbound proxy may
	 *	be used.
	 * @param aRemoteTarget Remote target of the ACK
	 * @param aDeleteRequest If ETrue, ownership of aAck is transferred.
	 * @param aBranch The branch value to put into the ACK. If this is an empty
	 *		  string, a new branch is generated.
	 */
	void SendFirstAckL(CSIPRequest* aAck,
					   MTransactionOwner& aObserver,
					   const TSIPTransportParams& aParams,
					   TRegistrationId aRegisterId,
					   const CURIContainer& aRemoteTarget,
					   TBool aDeleteRequest,
					   RStringF aBranch);

	/**
	 * The Invite client transaction is canceled. If InviteUAC doesn't receive
	 * a final response, it ends with a reason KErrCancel.	 
     */	
    void CancelInvite();

    /**
	 * Route headers are copied from INVITE to the CANCEL.
	 *	 
	 * @pre iOutgoingMsg != NULL
	 *
	 * @param aCancel IN/OUT: CANCEL request
     */	
    void CopyRouteHeadersToCancelL(CSIPRequest& aCancel);

	void HandleIcmpErrorL(const TInetAddr& aAddress,
						  const CUserAgentState& aResolveAckAddress,
						  const CUserAgentState& aWaitResponse);	

	/**
	 * Checks that the ACK contains all the necessary information for TU to
     * send it. If it doesn't function leaves.
	 *
	 * @param aAck ACK message	 
	 */	
	void CheckAckL(CSIPRequest& aAck) const;

	/**
	 * Fills the ACK.
	 *
	 * @param aAck ACK request
	 * @param aBranch The branch value to put into the ACK. If this is an empty
	 *		  string, a new branch is generated.
	 */
	void FillAckL(CSIPRequest& aAck, RStringF aBranch);

	/**
	 * The remote address for ACK has been resolved. UserAgent will send the
	 * ACK to the first address received from ServerResolver.	 
	 */
	void AckAddressResolvedL();

	/**
	 * Send another ACK, after at least one ACK has already been sent.
	 * UAC has to resolve the remote address for every ACK, so it creates a
	 * CAckHandler object for resolving and sending all the subsequent ACKs
	 * from the upper layer, after the first ACK. UAC handles the first ACK
	 * from upper layer by itself.
	 *
	 * @param aAck ACK message
	 * @param aObserver IN: Callback to upper layer.
	 * @param aParams Transport parameters to be used for sending the ACK
	 * @param aRegisterId Registration id to get outbound proxy
	 * @param aRemoteTarget Remote target
	 * @param aDeleteRequest If ETrue, CAckHandler will delete aAck when it
	 *	ends. If EFalse, aAck is freed by the upper layer.
	 */		
	void SendAdditionalAckL(CSIPRequest* aAck,
						    MTransactionOwner& aObserver,
							const TSIPTransportParams& aParams,
							TRegistrationId aRegisterId,
							const CURIContainer& aRemoteTarget,
							TBool aDeleteRequest);

	/**
	 * UAC receives another 2xx, which is either a retransmission, or 2xx from
	 * another endpoint with a different To-tag. In either case, the 2xx is
	 * passed to upper layer which will (re)send the ACK.		
	 *
	 * @pre aResp != NULL
	 *
	 * @param aResp 2xx response, ownership is transferred.
	 */
	void HandleAdditional2xxL(CSIPResponse* aResp);

	static CInviteUAC& Ptr(CUserAgent& aUserAgent);
	
	void SetSipSecError(TInt aErr);

private:

	CInviteUAC(CUserAgentCreateParams& aParams,
               MSipConnectionMgr& aConnectionMgr,
               MSipUriResolver& aResolver,
			   CSIPSec& aSIPSec,
			   TUint32 aCSeqNumber,
			   TBool aIsPrivateAddress,
			   CUserAgentState* aAckSenderState);	

	/**
	 * Stop and delete the CTransaction object.
	 *
	 * @pre iTransaction != NULL
	 * @pre iDeleteThisTransaction == NULL
	 * @post iTransaction == NULL
	 */
	void StopTransaction();

	void StopTimerUaTimeout();
	
	void StopTimerSIPSec();

	void CancelAllTimers();

	/**
	 * Selects the branch value to be used for ACK.
	 *
	 * @param aObserver IN: Callback to upper layer.
	 * @return Branch value to use, or an empty descriptor if a new branch will
	 *	       be generated.
	 */
	RStringF SelectBranchToUse(MTransactionOwner& aObserver) const;
	
	RStringF UsedBranch(MTransactionOwner& aObserver) const;

private: // Data

	//Used to create another CTransmitter or clone CInviteUAC.
	MSipConnectionMgr& iConnectionMgr;

	//This timer is started when the first 2xx is received. When the timer
	//expires, UA stops. Owned.
	CTimerStopUA* iTimerWait2xxRetransmissions;

	//Used to handle responses asynchronously even if SIPSec processed it
	//synchronously. Owned.
	CTimerAsyncSIPSec* iSIPSecTimer;

	//Tells if the local address is in IPv4 private address space
	TBool iIsPrivateAddress;

	//Initial state of InviteUACs that send the additional ACKs ("sub UACs").
	//For "sub UAC", the iAckSenderState is NULL. Not owned.
	CUserAgentState* iAckSenderState;

	//For handling ACKs after the first ACK, CInviteUAC creates a "sub UAC" for
	//handling every subsequent ACK. A "sub UAC" resolves the remote address
	//for ACK and sends it to network. "sub UACs" are owned by the main
	//CInviteUAC who created them. "sub UACs" don't receive messages from
	//ConnectionMgr, as they are not stored in CTransactionStore.
	RPointerArray<CInviteUAC> iAckSenders;

	//Remembers the callback that was used when the first ACK to a 2xx was
	//sent. Used to check if a subsequent ACK is a retransmission. Not owned.
	//UAC still updates iObserver each time an ACK is sent, and uses it for
	//calling TransactionEnded.
	MTransactionOwner* iObserverForFirstAck;

	//Indicates is the INVITE client transaction has been canceled.
	//If this flag is ETrue and InviteUAC ends because of timeout, the reason
	//is converted to KErrCancel before passing it to upper layer.
	TBool iCanceled;

	//Used only to store a detached INVITE client transaction that couldn't be
	//given to DeleteMgr due to lack of memory. Owned.
	CTransactionBase* iDeleteThisTransaction;

private: // For testing purposes

#ifdef CPPUNIT_TEST
	friend class CInviteUAC_Start_Test;
	friend class CInviteUAC_ResolveAddress_Test;
	friend class CInviteUAC_WaitResponse_Test;
	friend class CInviteUAC_WaitAckFromApp_Test;
	friend class CInviteUAC_ResolveAckAddress_Test;
	friend class CInviteUAC_SendingAck_Test;
	friend class CUserAgentClient_Test;
	friend class CTransactionUser_Test;
#endif

	void __DbgTestInvariant() const;

	};

#endif // end of INVITEUAC_H

// End of File
