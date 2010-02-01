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
// Name          : CStatePendingKey.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "sipsecurityheaderbase.h"
#include "sipresponse.h"
#include "CSipSecAgreeRecord.h"
#include "SipSecIpsecParams.h"
#include "MIpSecAgreeContextParams.h"
#include "CIpSecSadbHandler.h"
#include "SipSecUtils.h"
#include "CStatePendingKey.h"	

// ----------------------------------------------------------------------------
// CPendingKey::CPendingKey
// ----------------------------------------------------------------------------
//
CPendingKey::CPendingKey() 
    {
    }

// ----------------------------------------------------------------------------
// CPendingKey::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CPendingKey::State()
	{
	return EPendingKey;
	}

// ----------------------------------------------------------------------------
// CPendingKey::TimerExpiredL
// ----------------------------------------------------------------------------
//
void CPendingKey::TimerExpiredL(  
	CSipSecAgreeRecord& aRec,
	TTimerId /* aTimerId */, 
	TAny* /* aTimerParam */ )
	{
	aRec.iParams.SaDeleted( &aRec );
	}

// ----------------------------------------------------------------------------
// CPendingKey::Resp4xxL
// ----------------------------------------------------------------------------
//					  	  	
void CPendingKey::Resp4xxL(  
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse,
	CSIPRequest& /* aRequest */ )
	{
	aRec.iSecVerifyHeaders.ResetAndDestroy();
	aRec.CopyHeadersFromMessageL( aResponse, EFalse );
	// Wait 4 minutes for Digest plugin to generate a new key
	aRec.StartTimerL( KTempTime );
	}

// ----------------------------------------------------------------------------
// CPendingKey::AuthKeyL
// ----------------------------------------------------------------------------
//		
TBool CPendingKey::AuthKeyL(  
	CSipSecAgreeRecord& aRec,
	const TDesC8& aAuthKey )
	{	
	// 1. Find all security-verify headers with KIpSec3gpp mechanism
	// 2. Find which one has highest q
	// 3. Find security-client header that has the same algorithm
	
	CSIPSecurityHeaderBase* 
		vh = SipSecUtils::HeaderWithHighestVaL( aRec.iSecVerifyHeaders,
											     KIpSec3gpp,
											     KPreference );
											     
	__ASSERT_ALWAYS( vh, User::Leave( KErrNotFound ) );
											     
	const RStringF alg = SipSecUtils::ValueFL( *vh, KAlgorithm );
	
	CSIPSecurityHeaderBase* 
		ch = SipSecUtils::HeaderWithParamVaL( 
								 aRec.iSecClientHeaders,
								 KIpSec3gpp,
								 KAlgorithm,
								 alg.DesC() );
	aRec.SetSaParamsL( *ch, *vh, aAuthKey );
	aRec.AddSasL();
			 	  
	aRec.NextState( EPendingSa );
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CPendingKey::AuthKeyFailedL
// ----------------------------------------------------------------------------
//	
void CPendingKey::AuthKeyFailedL( CSipSecAgreeRecord& aRec )
    {
    // aRec will be deleted because of incorrect MAC or SQN in received 401
    ClearSaL( aRec );
    }

// ----------------------------------------------------------------------------
// CPendingKey::ClearSaL
// ----------------------------------------------------------------------------
//	
void CPendingKey::ClearSaL(	CSipSecAgreeRecord& aRec )
	{
	aRec.StopTimer();
	aRec.iParams.SaCleared( &aRec );	
	}
	
// End of File
