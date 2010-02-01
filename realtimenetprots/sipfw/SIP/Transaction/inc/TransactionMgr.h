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
* Name          : TransactionMgr.h
* Part of       : Transaction
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef TRANSACTIONMGR_H
#define TRANSACTIONMGR_H

// INCLUDES
#include <e32base.h>

#include "TransactionBase.h"

// FORWARD DECLARATIONS
class CUserAgentBase;
class MTimerManager;
class TTimerValues;
class CTransmitter;
class MTransactionStore;

class CInviteClientTa_Calling;
class CInviteClientTa_Proceeding;
class CInviteClientTa_Completed;
class CNormalClientTa_Trying;
class CNormalClientTa_Proceeding;
class CNormalClientTa_Completed;	
class CInviteServerTa_Proceeding;
class CInviteServerTa_Completed;
class CInviteServerTa_Confirmed;
class CInviteServerTa_Sending2xx;
class CNormalServerTa_Trying;
class CNormalServerTa_Proceeding;
class CNormalServerTa_Completed;


// CLASS DECLARATION
/**
 * CTransactionMgr class offers means of creating various transaction state
 * machines.
 */
class CTransactionMgr: public CBase
	{
public: // Constructor and destructor

	/**
	 * Creates a new instance.
	 *
	 * @param aTimers Timer subsystem
	 * @param aTransactionStore TransactionStore	 
	 * @return value New CTransactionMgr object, ownership is transferred.
	 */
	static CTransactionMgr* NewL(MTimerManager& aTimers,							
								 MTransactionStore& aTransactionStore);

	/**
	 * Destructor
	 */
	~CTransactionMgr();

public: // New functions

	/**
	 * Creates a transaction of the specified type.
	 *
	 * @param aType Identifies the type of transaction to be created
	 * @param aUserAgent UserAgent owning the new transaction
	 * @param aTransmitter Transmitter used for sending SIP messages
	 * @param aTimerValues Timer values
	 * @param aRetransmitInvite Only applicable to INVITE client transaction.
	 *		  If ETrue and using UDP, the INVITE is retransmitted even after
	 *		  receiving a provisional response, to keep the NAT binding alive.
	 * @return value New object, ownership is transferred.
	 */
	CTransactionBase*
        CreateTransactionL(CTransactionBase::TTransactionType aType,
						   CUserAgentBase& aUserAgent,
						   CTransmitter& aTransmitter,
						   TTimerValues& aTimerValues,
						   TBool aRetransmitInvite=EFalse);

	/**
	 * Checks if it is possible to send the CANCEL request. Only an invite
	 * client transaction that is in the proceeding state can be canceled.
	 *
	 * @param aTransaction Transaction which is inspected to see if it can be
	 *	canceled.
	 * @return value ETrue If a CANCEL can be sent. EFalse otherwise.
	 */
	TBool IsPossibleToSendCancel(const CTransactionBase& aTransaction) const;

private: // Constructors

	CTransactionMgr(MTimerManager& aTimers,
					MTransactionStore& aTransactionStore);

	void ConstructL();

private: // Data

	//Timer subsystem
	MTimerManager& iTimers;

	//Information of all existing transactions and UserAgents.
	MTransactionStore& iTransactionStore;


	//CTransactionMgr owns all the state objects of different transaction state
	//machines. There is one instance of each CTransactionState derived object.

	//Invite client transaction states
	CInviteClientTa_Calling*	iInviteClientTa_Calling;
	CInviteClientTa_Proceeding* iInviteClientTa_Proceeding;
	CInviteClientTa_Completed*	iInviteClientTa_Completed;	

	//Non-Invite client transaction states
	CNormalClientTa_Trying*		iNormalClientTa_Trying;
	CNormalClientTa_Proceeding* iNormalClientTa_Proceeding;
	CNormalClientTa_Completed*	iNormalClientTa_Completed;

	//Invite server transaction states
	CInviteServerTa_Proceeding* iInviteServerTa_Proceeding;
	CInviteServerTa_Completed*	iInviteServerTa_Completed;
	CInviteServerTa_Confirmed*	iInviteServerTa_Confirmed;
	CInviteServerTa_Sending2xx* iInviteServerTa_Sending2xx;

	//Non-Invite server transaction states
	CNormalServerTa_Trying*		iNormalServerTa_Trying;
	CNormalServerTa_Proceeding* iNormalServerTa_Proceeding;
	CNormalServerTa_Completed*	iNormalServerTa_Completed;

	void __DbgTestInvariant() const;


#ifdef CPPUNIT_TEST
	friend class CTransactionUser_Test;
    friend class CInviteUAS_Accepted_Test;
#endif
	};

#endif // end of TRANSACTIONMGR_H

// End of File
