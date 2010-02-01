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
// Name          : CStateTemporal.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "siprequest.h"
#include "sipresponse.h"
#include "TSIPTransportParams.h"
#include "CSipSecAgreeRecord.h"
#include "CStateTemporal.h"	

// ----------------------------------------------------------------------------
// CTemporal::CTemporal
// ----------------------------------------------------------------------------
//
CTemporal::CTemporal() 
    {
    }

// ----------------------------------------------------------------------------
// CTemporal::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CTemporal::State()
	{	
	return ETemporal;
	}

// ----------------------------------------------------------------------------
// CTemporal::TimerExpiredL
// ----------------------------------------------------------------------------
//		
void CTemporal::TimerExpiredL(  
	CSipSecAgreeRecord& aRec,
	TTimerId /* aTimerId */, 
	TAny* /* aTimerParam */ )
	{
	DeleteSasL( aRec );
	}

// ----------------------------------------------------------------------------
// CTemporal::Resp4xxL
// ----------------------------------------------------------------------------
//		
void CTemporal::Resp4xxL(  
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse,
	CSIPRequest& /* aRequest */ )
	{
	ReInitializeL( aRec, aResponse, ETrue );
	}

// ----------------------------------------------------------------------------
// CTemporal::RegisterL
// ----------------------------------------------------------------------------
//		
void CTemporal::RegisterL(  
	CSipSecAgreeRecord& aRec,
	CSIPRequest& aRequest,
	TSIPTransportParams& aTransportParams,
	const TDesC8& aOutboundProxy )
	{
	// Copy sec client headers to the request
	aRec.CopyHeadersToMessageL( aRequest, ETrue );
	
	// Set transport id for outgoing register so that protected transports
	// are used already at this stage
    aTransportParams.SetTransportId( aRec.iTransportParams.TransportId() );
    
    // Enable SigComp usage
    aTransportParams.SetIgnoreCompartmentId( EFalse );
    		
	// Add port_ps to next hop address
	aRec.UpdateNextHopPort( aRequest, aOutboundProxy );
	}

// ----------------------------------------------------------------------------
// CTemporal::Resp2xxL
// ----------------------------------------------------------------------------
//		
void CTemporal::Resp2xxL(  
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse )
	{
	UpdateExpirationTimeL( aRec, aResponse );

	aRec.NextState( EInUse );
	}
	
// End of File
