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
* Name          : CTransactionUser.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CTRANSACTIONUSER_H
#define CTRANSACTIONUSER_H

// INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include <stringpool.h>
#include <commsdattypesv1_1.h>

#include "sipinternalstates.h"

#include "MTransactionUser.h"
#include "MServerTaFactory.h"
#include "MConnectionStateObserver.h"

// FORWARD DECLARATIONS
class MSIPRequestRouter;
class MTimerManager;
class CSipConnectionMgr;
class MSipDialogs;
class MSipRegistrations;
class MSipRegistrationContact;
class CDeleteMgr;
class CSIPSec;
class CTransactionMgr;

class MTransactionOwner;
class CTransactionStore;
class CUserAgent;
class CCancelUAC;
class CUserAgentCreateParams;
class CTimerValueRetriever;
class CSIPMessageUtility;
class CInviteUAC_Start;
class CInviteUAC_ResolveAddress;
class CInviteUAC_WaitResponse;
class CInviteUAC_WaitAckFromApp;
class CInviteUAC_ResolveAckAddress;
class CInviteUAC_SendingAck;
class CInviteUAC_WaitTransactionToEnd;
class CInviteUAC_WaitAuthentication;
class CNormalUAC_Start;
class CNormalUAC_ResolveAddress;
class CNormalUAC_WaitResponse;
class CNormalUAC_WaitAuthentication;
class CInviteUAS_Start;
class CInviteUAS_GetTxOwner;
class CInviteUAS_WaitRespFromApp;
class CInviteUAS_Accepted;
class CInviteUAS_Rejected;
class CNormalUAS_Start;
class CNormalUAS_GetTxOwner;
class CNormalUAS_WaitRespFromApp;
class CNormalUAS_FinalRespSent;
class CCancelUAC_Start;
class CCancelUAC_ResolveAddress;
class CCancelUAC_WaitResponse;
class CCancelUAC_WaitAuthentication;
class CCancelUAS_Start;
class CCancelUAS_FinalRespSent;

// CLASS DECLARATION

/**
 * CTransactionUser class is the interface for using TransactionUser subsystem.
 */
class CTransactionUser :	
	public CBase,
    public MTransactionUser,
	public MServerTaFactory,
    public MConnectionStateObserver
	{
public: // Constructors and destructor

	/**
	 * Creates TransactionUser subsystem.
	 *
	 * @pre aRouter != NULL, aTimers != NULL
	 *
	 * @param aRouter Router interface. Ownership isn't transferred.
	 * @param aTimers Lwtimer interface. Ownership isn't transferred.
	 * @param aObserver Callback to pass connection state information
	 * @return value New CTransactionUser object, ownership is transferred.
	 */	
	static CTransactionUser* NewL(MSIPRequestRouter* aRouter,
							      MTimerManager* aTimers,
								  MConnectionStateObserver& aObserver);

	/**
	 * Destructor. All existing transactions are immediately stopped.
	 */
	~CTransactionUser();

public : // From MTransactionUser

	void SendL(TTransactionId& aTransactionId,
	           TRegistrationId aRegistrationId,
               CSIPRequest* aRequest,
               MTransactionOwner* aObserver,
               CURIContainer& aRemoteTarget,
               const TSIPTransportParams& aTransportParams,
               TBool aDeleteRequest);
					    
	MTransactionHeaders* SendAndGetHeadersL(
						TTransactionId& aTransactionId,
						TRegistrationId aRegistrationId,
						CSIPRequest* aRequest,
						MTransactionOwner* aObserver,						
                        CURIContainer& aRemoteTarget,
                        const TSIPTransportParams& aTransportParams,
						TBool aDeleteRequest);
						
	void SendL(TTransactionId aTransactionId,
	           CSIPResponse* aResponse,
	           const TSIPTransportParams& aTransportParams) const;
	           
	void SendL(TTransactionId aTransactionId,
	           CSIPResponse* aResponse,
	           MTransactionOwner* aObserver,
	           const TSIPTransportParams& aTransportParams) const;
					    
	void SendCancelL(TTransactionId aInviteTaId,
					 TTransactionId& aCancelTaId,
					 MTransactionOwner* aObserver);

	TInt ClearTransactionOwner(TTransactionId aTransactionId) const;

	TInt ClearTransactionOwner(MTransactionOwner* aObserver) const;


    void GetLocalAddress(TUint32 aIapId, TInetAddr& aAddr) const;

	void FreeResources(MSIPNATBindingObserver& aSIPNATBindingObserver );

	MTransactionHeaders*
        TransactionHeadersL(TTransactionId aTransactionId) const;

	void MakeTagL(TDes8& aTag) const;
	
	virtual TInt NextHopIP(
		const TTransactionId& aTransactionId, TInetAddr& aAddr);

public: // From MServerTaFactory

	virtual MReceiverObserver*
        NewTransactionL(CSIPRequest& aRequest,
                        const TSIPTransportParams& aParams,
						TTransactionId& aTransactionId);

public: // From MConnectionStateObserver

	void ConnectionStateChangedL(TUint32 aIapId, CSIPConnection::TState aState);

public: // New functions

    /**
 	 * Returns a reference to SIPSec. 
	 *
	 * @returns SIPSec
	 */
    CSIPSec& SIPSec();

    /**
 	 * Returns a reference to SIP ConnnectionMgr.
	 *
	 * @returns ConnectionMgr
	 */    
    CSipConnectionMgr& SipConnectionMgr();

	/**
	 * Gives the address of SIP Dialogs subsystem to TransactionUser subsystem.	 
	 *
	 * @pre aSipDialogs != NULL
	 *
	 * @param aSipDialogs IN: address of SIP Dialogs subsystem. Ownership isn't
	 *	transferred
	 */
	void SetDialogs(MSipDialogs* aSipDialogs);

	/**
	 * Gives the interfaces of SIP Registrations subsystem for TransactionUser
	 * subsystem.
	 *
	 * @pre aSipRegistrations != NULL, aRegistrationContact != NULL
	 *
	 * @param aSipRegistrations IN: interface for checking AORs. Ownership
     *  isn't transferred
	 * @param aRegistrationContact IN: interface for obtaining Contact header.
	 *	Ownership isn't transferred
	 */
	void SetRegistrations(MSipRegistrations* aSipRegistrations,
						  MSipRegistrationContact* aRegistrationContact);        

    /**
	 * Obtains the state of the specified transaction.
	 *
	 * @param aTransactionId TransactionId of the transaction whose state is
     *  requested.
	 * @param aState OUT: Transaction state
	 * @leave KErrNotFound if no such transaction exists.
	 */
    void GetStateL(TTransactionId aTransactionId,
                   CSIPInternalStates::TState& aState) const;

private: // Constructors, for internal use

	CTransactionUser(MSIPRequestRouter* aRouter,
                     MTimerManager* aTimers,
                     MConnectionStateObserver& aStateObserver);

	void ConstructL();

private: // New functions, for internal use

	/**
	 * Builds UA state machines by creating the state objects and linking them
	 * together.
	 */
	void ConstructUaStateMachinesL();

	/**
	 * Generate a new value to be used as a CSeq sequence number.
	 *
	 * @return value Sequence number
	 */
	TUint32 NewCSeqValue();

	/**
	 * Fills the common parameters for creating a new UA.	 
	 *
	 * @param aTransactionId Transaction id for the new UserAgent
	 * @param aObserver Callback to the upper layer. Can be NULL. Ownership
	 *	isn't transferred.
	 * @param aDeleteOutgoingMsg If ETrue, UserAgent deletes iOutgoingMsg when
	 *	UserAgent itself is deleted.
     * @param aParams IN: Transport parameters to be used when handing a SIP
     *					  message to SIP Connection Mgr.
	 * @return Object containing the common parameters for all UserAgent types.
	 *	Ownership is transferred.
	 */
	CUserAgentCreateParams* FillUserAgentParamsLC(TTransactionId aTransactionId,
									  MTransactionOwner* aObserver,
									  TBool aDeleteOutgoingMsg,
			                          const TSIPTransportParams& aParams) const;

	/**
	 * Creates an UAS for handling a SIP request received from network.
	 *
	 * @param aTaId TransactionId for the UAS
	 * @param aMethod Method of the SIP request
     * @param aParams IN: Transport parameters to be used when handing a SIP
     *					  message to SIP Connection Mgr.
	 * @return value New UA object, ownership is transferred
	 */
	CUserAgent* CreateUasL(TTransactionId aTaId,
						   RStringF aMethod,
                           const TSIPTransportParams& aParams) const;

	/**
	 * Creates an UAC for sending a new SIP request to network.	 
	 *
	 * @param aTaId TransactionId for the UAC
	 * @param aMethod Method of the SIP request
	 * @param aObserver Callback where UAC sends the response and other events
	 * @param aDeleteRequest If ETrue, UAC will delete the SIP request when UAC
	 *	ends. If EFalse, UAC doesn't delete the SIP request. In either case,
	 *  UAC can modify the request.
	 * @param aCSeqNumber CSeq sequence number for UAC to use, unless filled by
	 *	upper layer.
     * @param aParams IN: Transport parameters to be used when handing a SIP
     *					  request to SIP Connection Mgr.
	 * @return value New UA object, ownership is transferred
	 */
	CUserAgent* CreateUacL(TTransactionId aTaId,
						   RStringF aMethod,
						   MTransactionOwner& aObserver,
						   TBool aDeleteRequest,
						   TUint32 aCSeqNumber,
                           const TSIPTransportParams& aParams) const;

    /**
	 * Creates UAC for sending CANCEL request to network.
	 *	 
	 * @param aTaId TransactionId for the UAC	 
	 * @param aObserver Callback where UAC sends the response an other events	 
     * @param aCSeqNumber CSeq sequence number for UAC to use, unless filled by
	 *	upper layer.
	 * @param aParams IN: Transport parameters to be used when handing a SIP
     *					  request to SIP Connection Mgr.
	 * @return value New CancelUAC object, ownership is transferred
	 */
    CCancelUAC*
    	CreateCancelUacL(TTransactionId aTaId,
						 MTransactionOwner& aObserver,
                         TUint32 aCSeqNumber,
                         const TSIPTransportParams& aParams) const;
    
    /**
     * Returns the Timer Retriever Mapped bearer type
     * @param aIapID IapID for which bearer type needs to be determined 
     */
    TInt TimerRetrieverMappedBearerTypeL(TUint32 aIapID) const;

private: // Data

	//Determines where incoming requests are routed. Not owned.
	MSIPRequestRouter* iRouter;

	//Timer services. Not owned.
	MTimerManager* iTimers;

    //Connection state changes are reported to this callback
	MConnectionStateObserver& iStateObserver;

	//Dialog subsystem. Not owned.
	MSipDialogs* iDialogs;

	//Registration subsystem. Not owned.
	MSipRegistrations* iRegistrations;

	//Used for filling in information to Contact-headers of outgoing SIP
    //messages. Not owned.
	MSipRegistrationContact* iRegistrationContact;

	//Transport layer. Owned.
	CSipConnectionMgr* iConnectionMgr;

	//Contains information of all existing transactions and UAs. Owned.
	CTransactionStore* iTransactionStore;

	//Used for freeing memory asynchronously. Owned.
	CDeleteMgr*	iDeleteMgr;	


	//There is one instance of each UA state object. They're all owned.

	//Invite user agent client states
	CInviteUAC_Start*				 iInviteUAC_Start;
	CInviteUAC_ResolveAddress*		 iInviteUAC_ResolveAddress;
	CInviteUAC_WaitResponse*		 iInviteUAC_WaitResponse;
	CInviteUAC_WaitAckFromApp*		 iInviteUAC_WaitAckFromApp;
	CInviteUAC_ResolveAckAddress*	 iInviteUAC_ResolveAckAddress;
	CInviteUAC_SendingAck*			 iInviteUAC_SendingAck;
	CInviteUAC_WaitTransactionToEnd* iInviteUAC_WaitTransactionToEnd;
	CInviteUAC_WaitAuthentication*	 iInviteUAC_WaitAuthentication;	

	//Non-Invite user agent client states
	CNormalUAC_Start*				iNormalUAC_Start;
	CNormalUAC_ResolveAddress*		iNormalUAC_ResolveAddress;
	CNormalUAC_WaitResponse*		iNormalUAC_WaitResponse;
	CNormalUAC_WaitAuthentication*	iNormalUAC_WaitAuthentication;	

	//Cancel user agent client states
	CCancelUAC_Start*				iCancelUAC_Start;
	CCancelUAC_ResolveAddress*		iCancelUAC_ResolveAddress;
	CCancelUAC_WaitResponse*		iCancelUAC_WaitResponse;
	CCancelUAC_WaitAuthentication*	iCancelUAC_WaitAuthentication;

	//Invite user agent server states
	CInviteUAS_Start*			iInviteUAS_Start;
	CInviteUAS_GetTxOwner*		iInviteUAS_GetTxOwner;
	CInviteUAS_WaitRespFromApp* iInviteUAS_WaitRespFromApp;
	CInviteUAS_Accepted*		iInviteUAS_Accepted;
	CInviteUAS_Rejected*		iInviteUAS_Rejected;
	
	//Non-Invite user agent server states
	CNormalUAS_Start*			iNormalUAS_Start;
	CNormalUAS_GetTxOwner*		iNormalUAS_GetTxOwner;
	CNormalUAS_WaitRespFromApp* iNormalUAS_WaitRespFromApp;
	CNormalUAS_FinalRespSent*	iNormalUAS_FinalRespSent;	

	//Cancel user agent server states
	CCancelUAS_Start*			iCancelUAS_Start;
	CCancelUAS_FinalRespSent*	iCancelUAS_FinalRespSent;


	//Counter used to produce CSeq numbers for outgoing requests when the upper
	//layers have not filled the CSeq number.	
	TUint32 iCSeqCounter;

	//Transaction layer. Owned.
	CTransactionMgr* iTransactionMgr;

	//Timer values, owned.
	CTimerValueRetriever* iTimerValueRetriever;

    //SIP message manipulating utilities. Owned.
	CSIPMessageUtility* iSIPMsgUtility;

	//SIP Security subsystem. Owned.
	CSIPSec* iSIPSec;

private: // For testing purposes

#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
#endif

	void __DbgTestInvariant() const;

	};

#endif // end of CTRANSACTIONUSER_H

// End of File
