// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : TimerValues.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "TimerValues.h"

const TInt KBitsToShiftToMultiplyBy64 = 6;

// -----------------------------------------------------------------------------
// TTimerValues::TTimerValues
// -----------------------------------------------------------------------------
//
TTimerValues::TTimerValues( TUint aT1, TUint aT2, TUint aT4 ) :
	iT1( aT1 ),
	iT2( aT2 ),
	iT4( aT4 )
	{
    }

// -----------------------------------------------------------------------------
// TTimerValues::TTimerValues
// -----------------------------------------------------------------------------
//
TTimerValues::TTimerValues() : iT1( 0 ), iT2( 0 ), iT4( 0 )
	{
	}

// -----------------------------------------------------------------------------
// TTimerValues::CheckValuesL
// If T1 is larger than T2, T1 wouldn't affect retransmissions at all, it would
// only affect the transaction timeout duration.
// It doesn't make sense to set T1, T2 or T4 with a zero value.
// If T1 is very large, calculating 64*T1 with Duration64xT1 would be too large
// to fit into TUint.
// -----------------------------------------------------------------------------
//
TBool TTimerValues::CheckValues() const
	{
	return ( iT1 <= iT2 ) &&
		   ( iT1 > 0 ) &&
		   ( iT2 > 0 ) &&
		   ( iT4 > 0 ) &&
		   ( iT1 <= ( KMaxTUint >> KBitsToShiftToMultiplyBy64 ) );
	}

// -----------------------------------------------------------------------------
// TTimerValues::DoubleUptoT2
// -----------------------------------------------------------------------------
//
void TTimerValues::DoubleUptoT2( TUint& aDuration ) const
	{
	if ( aDuration == 0 )
		{
		aDuration = iT1;
		}
	else
		{
		aDuration = aDuration << 1;
		}

	if ( aDuration > iT2 )
		{
		aDuration = iT2;
		}
	}

// -----------------------------------------------------------------------------
// TTimerValues::Duration64xT1
// -----------------------------------------------------------------------------
//
TUint TTimerValues::Duration64xT1() const
	{
	return iT1 << KBitsToShiftToMultiplyBy64;
	}

// -----------------------------------------------------------------------------
// TTimerValues::T1
// -----------------------------------------------------------------------------
//
TUint TTimerValues::T1() const
	{
	return iT1;
	}

// -----------------------------------------------------------------------------
// TTimerValues::T2
// -----------------------------------------------------------------------------
//
TUint TTimerValues::T2() const
	{
	return iT2;
	}

// -----------------------------------------------------------------------------
// TTimerValues::T4
// -----------------------------------------------------------------------------
//
TUint TTimerValues::T4() const
	{
	return iT4;
	}
