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
* Name          : InviteUAS.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef INVITEUAS_H
#define INVITEUAS_H

// INCLUDES
#include "UserAgentServer.h"

// FORWARD DECLARATIONS
class CTimerRetransmit2xx;
class CTimerStopUA;


// CLASS DECLARATION

class CInviteUAS : public CUserAgentServer
	{
public: // Constructor and destructor

	static CInviteUAS* NewL(CUserAgentCreateParams& aParams,							
							MSipConnectionMgr& aConnectionMgr,		
							MSIPRequestRouter& aRouter,
                            MSipDialogs& aDialogs);

	~CInviteUAS();

public: // From CUserAgent

	void DeleteTimer(const CUserAgentTimer& aTimer);	
	
public: // From CUserAgentServer

	void CreateTransactionL();

public: // New functions

    /**
	 * Offers the ACK to Dialogs, and if Dialogs wants to receive the ACK,
	 * obtains the MTransactionOwner.
	 *
	 * @param aAck ACK request received from network MTransactionOwner
     *  callback.
     * @return ETrue If there is a dialog matching the ACK,
     *		   EFalse otherwise
	 */
    TBool DoesDialogExistForAckL(CSIPRequest& aAck);

	/**
	 * Sending the first 2xx response. The first 2xx is sent using CTransaction
	 * and the retransmissions of 2xx are handled by UAS.
	 *
	 * @param aResp 2xx response. Ownership is transferred.	 
	 */
	void SendFirst2xxL(CSIPResponse* aResp);

	void Retransmit2xxL();

	/**
	 * Cancels the INVITE server transaction, unless it has already sent a
	 * final response.
	 */
	void CancelInviteL();

    /**
	 * Determine if the UAS has sent a 2xx response.
	 *
	 * @return value ETrue If UAS has sent a 2xx, EFalse otherwise.
	 */
    TBool IsSending2xx() const;

	void StartTimerRetransmit2xxL();
	void StopTimerRetransmit2xx();	

	void HandleCancelInviteL(const CUserAgentState& aRejected);

	TBool InformObserverAboutCancel();

	static CInviteUAS& Ptr(CUserAgent& aUserAgent);

private: // Constructors, for internal use

	CInviteUAS(CUserAgentCreateParams& aParams,			   
               MSipDialogs& aDialogs,
               MSIPRequestRouter& aRouter);

	void ConstructL(MSipConnectionMgr& aConnectionMgr);

private: // New functions, for internal use

	void StartTimerWaitAckL();

	void StopTimerWaitAck();

	void CancelAllTimers();

private: // Data

	//Duration (ms) for iTimerRetransmit2xx
	TUint iTimerRetransmit2xxDuration;

	//Timer used retransmitting 2xx response until ACK is received. Owned.
	CTimerRetransmit2xx* iTimerRetransmit2xx;

	//This timer is started when entering Accepted state to wait an ACK. Owned.
	CTimerStopUA* iTimerWaitAck;

	//If ETrue, the INVITE server transaction has been canceled.
	TBool iCanceled;

private: // For testing purposes

#ifdef CPPUNIT_TEST	
	friend class CTransactionUser_Test;
    friend class CInviteUAS_Accepted_Test;
#endif

	void __DbgTestInvariant() const;


	};

#endif // end of INVITEUAS_H

// End of File
