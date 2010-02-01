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
// Name          : CStateTentative.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "siprequest.h"
#include "sipresponse.h"
#include "CSipSecAgreeRecord.h"
#include "SipSecIpsecParams.h"
#include "MIpSecAgreeContextParams.h"
#include "CStateTentative.h"	
		  
// ----------------------------------------------------------------------------
// CTentative::CTentative
// ----------------------------------------------------------------------------
//
CTentative::CTentative() 
    {
    }

// ----------------------------------------------------------------------------
// CTentative::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CTentative::State()
	{
	return ETentative;
	}

// ----------------------------------------------------------------------------
// CTentative::Resp4xxL
// ----------------------------------------------------------------------------
//	  	
void CTentative::Resp4xxL( 
	CSipSecAgreeRecord& aRec,
    CSIPResponse& aResponse,
    CSIPRequest& aRequest )
	{
	// First copy Security-Client Headers from request
	aRec.CopyHeadersFromMessageL( aRequest, ETrue );
	
	// Then convert Security-Server Headers of response 
	// into Security-Verify headers
	aRec.CopyHeadersFromMessageL( aResponse, EFalse );
	
	// Wait for Digest plugin generate a key
	// Set timer to 4 min while SA is temporal
	aRec.StartTimerL( KTempTime );
	aRec.NextState( EPendingKey );
	}
	
// End of File
