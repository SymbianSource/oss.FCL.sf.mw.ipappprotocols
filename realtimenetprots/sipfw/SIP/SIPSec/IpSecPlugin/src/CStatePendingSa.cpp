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
// Name          : CStatePendingSa.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "CSipSecAgreeRecord.h"
#include "MIpSecAgreeContextParams.h"
#include "MIpSecMechanismParams.h"
#include "CStatePendingSa.h"	

// ----------------------------------------------------------------------------
// CPendingSa::CPendingSa
// ----------------------------------------------------------------------------
//
CPendingSa::CPendingSa() 
    {
    }

// ----------------------------------------------------------------------------
// CPendingSa::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CPendingSa::State()
	{
	return EPendingSa;
	}

// ----------------------------------------------------------------------------
// CPendingSa::SaAdded
// ----------------------------------------------------------------------------
//	
void CPendingSa::SaAddedL( CSipSecAgreeRecord& aRec )
	{
	// SAs added successfully.
	// Don't yet inform SA readiness to the FW, wait for successfull 
   	// completion of the policy creation
	aRec.NextState( EPendingPolicy );
	
	// If reserving the ports fails or policy creation fails, notify observer
	// about unsuccess and delete corresponding SAs
	TRAPD( err, aRec.ReserveTransportAndCreatePolicyL() );
	if ( err != KErrNone )
	    {
	    aRec.iParams.SAReady( EFalse );
	    DeleteSasL( aRec );
	    }
	}

// ----------------------------------------------------------------------------
// CPendingSa::TimerExpiredL
// ----------------------------------------------------------------------------
//						  
void CPendingSa::TimerExpiredL(  
	CSipSecAgreeRecord& aRec,
	TTimerId /* aTimerId */, 
	TAny* /* aTimerParam */ )
	{
	aRec.iParams.SAReady( EFalse );
	aRec.iParams.SaDeleted( &aRec );
	}
	
// ----------------------------------------------------------------------------
// CPendingSa::Resp4xxL
// ----------------------------------------------------------------------------
//	
void CPendingSa::Resp4xxL(  
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse,
	CSIPRequest& /* aRequest */ )
	{
	aRec.iParams.SAReady( EFalse );
	ReInitializeL( aRec, aResponse, EFalse );
	}

// ----------------------------------------------------------------------------
// CPendingSa::ClearSaL
// ----------------------------------------------------------------------------
//	
void CPendingSa::ClearSaL( CSipSecAgreeRecord& aRec )
	{
	aRec.iParams.SAReady( EFalse );
	aRec.StopTimer();
	aRec.iParams.SaCleared( &aRec );	
	}

// End of File
