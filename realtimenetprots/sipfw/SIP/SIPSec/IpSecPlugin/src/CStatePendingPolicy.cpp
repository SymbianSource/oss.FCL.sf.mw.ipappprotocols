// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CStatePendingPolicy.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "sipresponse.h"
#include "CSipSecAgreeRecord.h"
#include "MIpSecAgreeContextParams.h"
#include "CStatePendingPolicy.h"	

// ----------------------------------------------------------------------------
// CPendingPolicy::CPendingPolicy
// ----------------------------------------------------------------------------
//
CPendingPolicy::CPendingPolicy() 
    {
    }

// ----------------------------------------------------------------------------
// CPendingPolicy::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CPendingPolicy::State()
	{
	return EPendingPolicy;
	}

// ----------------------------------------------------------------------------
// CPendingPolicy::PolicyActivatedL
// ----------------------------------------------------------------------------
//	
void CPendingPolicy::PolicyActivatedL( CSipSecAgreeRecord& aRec )
	{
	// Policy loaded and activated succesfully
	aRec.NextState( ETemporal );
	aRec.iParams.SAReady( ETrue );
	}

// ----------------------------------------------------------------------------
// CPendingPolicy::TimerExpiredL
// ----------------------------------------------------------------------------
//		
void CPendingPolicy::TimerExpiredL(  
	CSipSecAgreeRecord& aRec,
	TTimerId /* aTimerId */, 
	TAny* /* aTimerParam */ )
	{
	// SAs were created successfully but policy not, inform the observer
	// about unsuccess
	aRec.iParams.SAReady( EFalse );
	DeleteSasL( aRec );
	}
		
// ----------------------------------------------------------------------------
// CPendingPolicy::Resp4xxL
// ----------------------------------------------------------------------------
//	
void CPendingPolicy::Resp4xxL(  
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse,
	CSIPRequest& /* aRequest */ )
	{
	aRec.iParams.SAReady( EFalse );
	ReInitializeL( aRec, aResponse, ETrue );
	}

// ----------------------------------------------------------------------------
// CPendingPolicy::ClearSaL
// ----------------------------------------------------------------------------
//	
void CPendingPolicy::ClearSaL( CSipSecAgreeRecord& aRec )
	{
	aRec.iParams.SAReady( EFalse );
	aRec.StopTimer();
	DoClearSaL( aRec );	
	}

// End of File
