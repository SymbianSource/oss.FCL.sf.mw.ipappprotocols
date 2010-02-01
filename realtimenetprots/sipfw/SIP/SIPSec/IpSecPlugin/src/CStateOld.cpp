// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CStateOld.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/5.0
//



#include "CSipSecAgreeRecord.h"
#include "MIpSecAgreeContextParams.h"
#include "MIpSecMechanismParams.h"
#include "CStateOld.h"	

// ----------------------------------------------------------------------------
// COld::COld
// ----------------------------------------------------------------------------
//
COld::COld() 
    {
    }

// ----------------------------------------------------------------------------
// COld::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState COld::State()
	{
	return EOld;
	}
	
// ----------------------------------------------------------------------------
// COld::TimerExpiredL
// ----------------------------------------------------------------------------
//	
void COld::TimerExpiredL( CSipSecAgreeRecord& aRec,
						  TTimerId /* aTimerId */, 
						  TAny* /* aTimerParam */ )
	{
	DeleteSasL( aRec );
	}

// ----------------------------------------------------------------------------
// COld::ReguestL
// A non-REGISTER request is being sent to network.
// ----------------------------------------------------------------------------
//
void COld::ReguestL( CSipSecAgreeRecord& aRec,
					 CSIPRequest& /* aRequest */,
					 TSIPTransportParams& /* aTransportParams */,
					 const TDesC8& /* aOutboundProxy */ )
	{
	// 2 x transaction timeout (since timeouted ta is retried without sigcomp)
	const TUint KTATimeoutMultiplier = 128;
	aRec.StartTimerL( aRec.iParams.MechParams().T1() * KTATimeoutMultiplier );
	aRec.NextState( EDying );
	}

// End of File
