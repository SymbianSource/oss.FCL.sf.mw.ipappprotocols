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
* Name          : CUserAgent.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef CUSERAGENT_H
#define CUSERAGENT_H

// INCLUDES
#include <stringpool.h>
#include "Lwtimer.h"
#include "TSIPTransportParams.h"

#include "UserAgentBase.h"
#include "SendStatus.h"
#include "TimerValues.h"

// FORWARD DECLARATIONS
class MSipRegistrations;
class MSipRegistrationContact;
class MSigComp;
class CSIPMessage;
class CSIPFromToHeaderBase;
class CURIContainer;
class CDeleteMgr;
class CTransactionBase;
class CTransactionMgr;
class CTransmitter;

class MTransactionOwner;
class CTransactionStore;
class CUserAgentState;
class CUserAgentCreateParams;
class CSIPMessageUtility;
class CUserAgentTimer;


// CLASS DECLARATION
/**
 * CUserAgent is a base class for all different UserAgents.
 */
class CUserAgent :
    public CUserAgentBase,
	public MSendStatus,
	public MExpirationHandler	
	{
public: // Destructor

	virtual ~CUserAgent();

public: // From CUserAgentBase

	void ReceiveL(CSIPRequest* aRequest);
	void ReceiveL(CSIPResponse* aResponse);
	void LeaveOccurred(TInt aReason);
	const TSIPTransportParams& TransportParams() const;
	void IcmpErrorL(const TInetAddr& aAddress,
					CSipConnectionMgr::TICMPError aError);	
	void TransactionEndsL(TInt aReason);
	void Stop(TInt aReason);
	TTransactionId TransactionId() const;    

public:	// From MSendStatus

	void SendCompleteL();
	void SendFailedL(TInt aError);
	void LeaveFromTransmitter(TInt aReason);

public:	// From MExpirationHandler

	void TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam);

public: // New pure virtual functions

	/**
	 * Determines whether the UserAgent is a server or a client side UserAgent.
	 *
	 * @return value ETrue if the UserAgent instance is server side UA,
	 *	EFalse if it is client side UserAgent.
	 */
	virtual TBool IsUAS() const = 0;

protected: // New pure virtual functions

	/**
	 * Creates a suitable transaction state machine. The actual type of the
     * transaction depends on the UserAgent type.
	 */
	virtual void CreateTransactionL() = 0;

	/**
	 * Returns the RegistrationId
	 * @return value TRegistrationId
	 */
	virtual TRegistrationId RegistrationId() const = 0;

public: // New functions

	/**
	 * Sends a SIP request.
	 *
	 * @pre aReq != NULL
	 *
	 * @param aReq SIP request, ownership is transferred.
     * @param aRegisterId Registration identifier
     * @param aRemoteTarget Remote target of the request	 
	 */
	void SendRequestL(CSIPRequest* aReq,
					  TRegistrationId aRegisterId,
					  const CURIContainer& aRemoteTarget);

    /**
	 * Sends a SIP response.
	 *
	 * @pre aResp != NULL
	 *
	 * @param aResp SIP response, ownership is transferred.
	 * @param aParams Transport parameters to use when sending the response
	 */
	void SendResponseL(CSIPResponse* aResp, const TSIPTransportParams& aParams);

	/**
	 * Initiates the CancelUAC state machine. A CANCEL request will be created,
	 * based on the INVITE identified by aInviteTaId, and sent to network.	 
	 *
	 * @param aInviteTaId TransactionId of the INVITE client transaction to be
	 *	canceled.
	 */	
	void SendCancelL(TTransactionId aInviteTaId);

	/**
	 * Returns the observer callback of the CUserAgent.
	 *	 
	 * @return value Callback to the upper layer, can be NULL. Ownership isn't
     *  transferred.
	 */	
	const MTransactionOwner* TransactionOwner() const;
	
	/**
	 * Clears the MTransactionOwner callback. UserAgent won't use the callback
	 * after this.
	 *
	 * @post iObserver = NULL	 
	 */	
	virtual void ClearTransactionOwner();

    /**
	 * If UA has aTimer running, it is stopped and deleted.
	 *
	 * @param aTimer Timer to be deleted
	 */
	virtual void DeleteTimer(const CUserAgentTimer& aTimer);

	/**
	 * CUserAgent state machine moves to another state.
	 *
	 * @param aNewState State which CUserAgent will enter	 
	 */
	void ChangeState(const CUserAgentState& aNewState);

	/**
	 * Determines if the UserAgent is a client side INVITE UserAgent.
	 *
	 * @return value ETrue: the UserAgent instance is a client side INVITE
	 *	UserAgent. EFalse: otherwise
	 */
	virtual TBool IsInviteUAC() const;

	/**
	 * Checks if the UserAgent is an InviteUAC created for handling an
	 * additional ACK passed from the upper layer.
	 *
	 * @return value ETrue: this is a "sub InviteUAC", EFalse: otherwise
	 */	
	virtual TBool IsSubInviteUAC() const;

	/**
	 * Stores the message (aMsg) into UserAgent's send buffer (iOutgoingMsg).
	 * The possibly existing previous message is deleted from the send buffer.
	 *
	 * @pre aMsg != NULL
	 * @post iOutgoingMsg == aMsg
	 *
	 * @param aMsg SIP message, ownership is transferred	 
	 */
	void StoreOutgoingMsg(CSIPMessage* aMsg);
		
	/**
	 * Sends the SIP message to MTransactionOwner.	 
	 *
	 * @pre aMsg != NULL	 
	 *
	 * @param aMsg SIP message. ownership is transferred.	 
	 */
	virtual void PassMsgToTransactionOwnerL(CSIPMessage* aMsg) const;

    /**
     * Checks if there are Contact headers in the SIP message (aMsg). If a
     * Contact header without host is found, Registration subsystem is asked to
     * complete the Contact header.
	 *
     * @param aMsg SIP message
     * @param aFromToHeader Either a From- or To-header of the aMsg (depends on
     *  whether a UAC or UAS is using this method). Will be given to
     *  Registration subsystem in case it needs to complete a Contact header.
	 * @return value -
	 */
    void CheckContactHeadersL(CSIPMessage& aMsg,
                              const CSIPFromToHeaderBase* aFromToHeader) const;

	/**
	 * Removes transaction's information from CTransactionStore but doesn't
	 * delete the CUserAgent and CTransaction objects.
	 * As the transaction is no longer in CTransactionStore, it can't receive
	 * any longer events from ServerCore nor any incoming SIP messages.	 
	 */
	void RemoveFromStore() const;
	
	/**
	 * Sets a new value for MTransactionOwner callback, replacing the previous
	 * value.
	 *
	 * @pre aNewObserver != NULL	 
	 *
	 * @param aNewObserver Callback object	 
	 */
	void UpdateTransactionOwner(MTransactionOwner* aNewObserver);

	/**
	 * Uses DeleteMgr to free CTransaction. If not successful, leaves with
     * a system wide error code.
	 *
	 * @post iTransaction == NULL	 
	 */
	void RequestDeletionOfTransactionL();

    /**
	 * Returns pointer to the associated transaction instance.
	 *
	 * @return value Pointer to transaction or NULL if no transaction is
     *  associated with the UserAgent. Ownership isn't transferred.
	 */
	CTransactionBase* Transaction();
	
    /**
	 * When CleanupStack frees the TCleanupItem, this function is called to
	 * release all resources associated for the UserAgent. Not just delete the
	 * UserAgent, but also remove its information from CTransactionStore.
     * MTransactionOwner is not notified.
	 *
	 * @pre aUserAgent != NULL	 
	 *
	 * @param aUserAgent Pointer to the UserAgent
	 */
    static void CleanupSilently(TAny* aUserAgent);

	/**
	 * Tells if the UserAgent is currently waiting an address to be resolved.
	 *	 
	 * @return value ETrue if the UserAgent is resolving, EFalse otherwise
	 */	
	TBool IsResolving() const;

protected:

	CUserAgent(const CUserAgentCreateParams& aParams);

	void ConstructL(MSipConnectionMgr& aConnectionMgr);

    /**
	 * Returns the current state of the UserAgent.
     *
     * @return UserAgent's current state
	 */
    const CUserAgentState& State() const;

	/**
	 * Adds a specified amount of random characters into the descriptor.
	 *
	 * @pre aLength can't be longer than the free space left in the aBuf.	 	 
	 *
	 * @param aBuf Descriptor into which the random characters are added. The
	 *	existing characters are not overwritten.
	 * @param aLength Indicates how many characters should be added to aBuf.
	 *	This is often (but not always) same as the length of aBuf.
	 * @param aCaseSensitive ETrue if both upper and lowercase characters are
	 *	allowed to be added into aBuf. EFalse if only lowercase characters are
	 *	to be used.
	 */
	void AddRandomStringL(TDes8& aBuf,
						  TInt aLength,
						  TBool aCaseSensitive) const;

	/**
	 * Adds a tag into the To/From header.
	 *
	 * @param aToFromHeader To or From header to which a tag is added	 
	 */
	void AddTagL(CSIPFromToHeaderBase& aToFromHeader) const;

    /**
	 * Sets the iTransportProtocol member according to the transport protocol
	 * in the top Via header of aMsg.
	 *
	 * @param aMsg SIP message
	 * @return value ETrue: if successful, EFalse: otherwise
	 */
	TBool UpdateTransportProtocol(CSIPMessage& aMsg);

    /**
	 * Checks if aUri has parameter comp=sigcomp. If it exists, but SigComp is
     * not installed, function leaves with KErrNotSupported.
	 *
	 * @param aUri URI from which the comp parameter is checked	 
	 */
    void CheckSigCompL(const CURIContainer& aUri) const;

	/**
	 * Checks if the UserAgent has stopped.
	 *	 
	 * @return value ETrue if UserAgent has stopped, EFalse if not
	 */
	TBool HasStopped() const;

	/**
	 * Checks if the UserAgent has been canceled. Only certain types of
	 * UserAgent can be canceled. The default implementation returns EFalse.
	 * Those derived UserAgents that may be canceled, need to override this
	 * function.
	 *
	 * @return EFalse
	 */
	virtual TBool IsCanceled() const;

	/**
	 * UserAgent will detach all references to the outgoing SIP message.
	 * The default implementation does nothing. Derived classes may needs to
	 * override the default behaviour.	 
	 */
	virtual void DetachOutgoingMsg();	

protected: // Data

	// Callback to upper layer. Not owned. NULL if the upper layer has
	// indicated it doesn't want events anymore, or if UAS hasn't yet discovered
	// the upper layer.
	MTransactionOwner* iObserver;

	// Timer services
	MTimerManager& iTimers;

	MSipRegistrations& iRegistrations;

    MSipRegistrationContact& iRegistrationContact;

	// Signalling compression services
    MSigComp& iSigComp;

	// Information about all the existing transactions
	CTransactionStore& iTransactionStore;

	// UA and CTransaction use the same CTransmitter as they never send
	// simultaneously. Owned.
	CTransmitter* iTransmitter;

	// Transaction layer for creating transaction instances.
	CTransactionMgr& iTransactionMgr;

	// Transaction state machine, owned.
	CTransactionBase* iTransaction;

	// SIP message to be sent to network.
	CSIPMessage* iOutgoingMsg;

	// SIP message received from CTransaction (or directly from ConnectionMgr).
    // Owned.
	// If a response is passed to SIPSec, UAC stores the response here until
    // SIPSec has added the authentication parameters to the original request.
	// UAS stores the request here until MTransactionOwner has been obtained and
	// then forwards the request to MTransactionOwner.
	CSIPMessage* iIncomingMsg;

	// Transport protocol currently used.
	RStringF iTransportProtocol;

    TSIPTransportParams iTransportParams;
    
	// Copy of the values in CTimerValueRetriever::TTimerValues, at the time the
	// UA was created. Later changes to CTimerValueRetriever::TTimerValues won't
	// reflect here.
	TTimerValues iTimerValues;

    // Tells if UA owns the SIP message in iOutgoingMsg
	TBool iOwnsOutgoingMsg;

	// Used for asynchronous deleting.
	CDeleteMgr&	iDeleteMgr;

	CSIPMessageUtility& iSIPMsgUtility;
	
private:

    /**
	 * Uses DeleteMgr to free UA.
	 *
	 * @return value KErrNone if successful, otherwise a system wide error code
	 */
	TInt RequestDeletionOfUserAgent();

private: // Data

	// Current state of the UA. Not owned.
	const CUserAgentState* iState;

	// Identifies a SIP transaction.
	TTransactionId iTransactionId;

	// ETrue if UA state machine has ended. UA is deleted soon after it has
	// ended.
	TBool iStopped;

private: // For testing purposes

	void __DbgTestInvariant() const;



#ifdef CPPUNIT_TEST	
	friend class CTransactionUser_Test;
	friend class CUserAgent_Test;
	friend class CUserAgentClient_Test;
	friend class CNormalCTa_Trying_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;	
	friend class CInviteUAC_Start_Test;
	friend class CInviteUAC_ResolveAckAddress_Test;
    friend class CInviteUAS_Accepted_Test;
	friend class ClientTransaction_Test;
	friend class CServerTransaction_Test;
	friend class CInviteServerTa_Test;
	friend class CUserAgentServer_Test;
	friend class CNormalUAS_Start_Test;
	friend class CNormalUAS_GetTxOwner_Test;
	friend class CTransactionStore_Test;
	friend class CNormalSTa_Trying_Test;
	friend class CTransactionInfo_Test;
#endif
	};

#endif // end of CUSERAGENT_H

// End of File
