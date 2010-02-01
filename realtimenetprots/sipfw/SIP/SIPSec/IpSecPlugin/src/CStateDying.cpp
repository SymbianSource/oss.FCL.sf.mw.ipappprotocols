// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CStateDying.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "CSipSecAgreeRecord.h"
#include "CStateDying.h"	

// ----------------------------------------------------------------------------
// CDying::CDying
// ----------------------------------------------------------------------------
//
CDying::CDying() 
    {
    }

// ----------------------------------------------------------------------------
// CDying::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CDying::State()
	{
	return EDying;
	}
	
// ----------------------------------------------------------------------------
// CDying::TimerExpiredL
// ----------------------------------------------------------------------------
//
void CDying::TimerExpiredL(  
	CSipSecAgreeRecord& aRec,
	TTimerId /* aTimerId */, 
	TAny* /* aTimerParam */ )
	{
	DeleteSasL( aRec );	
	}
	
// End of File
