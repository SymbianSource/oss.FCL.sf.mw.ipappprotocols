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
* Name          : TimerBase.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/


#ifndef TIMERBASE_H
#define TIMERBASE_H

// INCLUDES
#include "Lwtimer.h"
#include <e32base.h>


// CLASS DECLARATION

/**
 * CTimerBase is a base class for all transaction related timers.
 */
class CTimerBase : public CBase
	{
public: // Destructor

	virtual ~CTimerBase();

public: // New functions

	/**
	 * Stops the timer.
	 */
	void Stop();

protected: // Constructors

	/**
	 * Creates a timer and starts it.
	 *
	 * Note: since there isn't a StartL() function in the CTimerBase, so if
	 * the timer is stopped, it can't be restarted.
	 *
	 * @param aTimerMgr Timer subsystem
	 * @param aObserver Callback for handling timer expiration. Ownership is
     *  not transferred.
	 * @param aDuration Timer duration, in milliseconds	 
	 */
	CTimerBase(MTimerManager& aTimerMgr,
			   MExpirationHandler* aObserver,
			   TUint32 aDuration);

	void ConstructL();

protected: // Data

    // Provides timer services
	MTimerManager& iTimerMgr;

	// Timer expiration handler. CTimerBase doesn't implement MExpirationHandler
	// itself as it does not know which transaction or UA owns it. Not owned.
	MExpirationHandler* iObserver;

	// Timer identifier
	TTimerId iId;

	// Timer duration in milliseconds
	TUint32 iDuration;

#ifdef CPPUNIT_TEST
	friend class CNormalCTa_Trying_Test;
	friend class CNormalCTa_Proceeding_Test;
	friend class CNormalCTa_Completed_Test;
	friend class CNormalSTa_Trying_Test;
	friend class CNormalSTa_Proceeding_Test;
	friend class CNormalSTa_Completed_Test;
	friend class CInviteUAC_Start_Test;
	friend class CInviteUAC_ResolveAddress_Test;
	friend class CInviteUAC_WaitResponse_Test;
	friend class CInviteUAC_WaitAckFromApp_Test;
	friend class CInviteUAC_ResolveAckAddress_Test;
	friend class CInviteUAC_SendingAck_Test;
	friend class CInviteCTa_Calling_Test;
	friend class CInviteCTa_Completed_Test;
	friend class CTransactionUser_Test;
#endif
	};

#endif // end of TIMERBASE_H

// End of File
