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
* Name          : TimerValues.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef TIMERVALUES_H
#define TIMERVALUES_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * TTimerValues wraps together the values of timers which are read from
 * a configuration file.
 */
class TTimerValues
	{
public: // Constructors
	
	/**
	 * Default constructor
	 */
	TTimerValues();
	
	/**
	 * Constructor
	 *
	 * @param aT1 Value for T1
	 * @param aT2 Value for T2
	 * @param aT4 Value for T4
	 */
	TTimerValues(TUint aT1, TUint aT2, TUint aT4);

public: // New functions

	/**
	 * Checks if the timer values are reasonable. If they aren't, function
     * leaves.
	 *
	 * @return value ETrue if the values are ok, EFalse otherwise.
	 */
	TBool CheckValues() const;

	/**
	 * Doubles the value of aDuration, until it reaches T2. If aDuration is
	 * zero, it is set to T1.
	 *
	 * @param aDuration IN: old value of aDuration, OUT: new value	 
	 */
	void DoubleUptoT2(TUint& aDuration) const;

	/**
	 * Returns the value of 64 times T1.
	 *
	 * @return 64xT1
	 */
	TUint Duration64xT1() const;

	/**
	 * Returns the value of T1
	 *
	 * @return T1
	 */
	TUint T1() const;

	/**
	 * Returns the value of T2
	 *
	 * @return T2
	 */
	TUint T2() const;

	/**
	 * Returns the value of T4
	 *	 
	 * @return T4
	 */
	TUint T4() const;    

private: // Data

	TUint iT1;
	TUint iT2;
	TUint iT4;

	// For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CInviteCTa_Calling_Test;
	friend class CNormalCTa_Proceeding_Test;
	friend class CInviteUAC_WaitAckFromApp_Test;
	friend class ClientTransaction_Test;
	friend class CServerTransaction_Test;
    friend class CUserAgentClient_Test;
    friend class CTransactionUser_Test;
#endif	
	};

#endif // end of TIMERVALUES_H

// End of File
