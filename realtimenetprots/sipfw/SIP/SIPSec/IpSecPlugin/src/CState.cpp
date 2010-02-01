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
// Name          : CState.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "sipsecurityheaderbase.h"
#include "sipresponse.h"
#include "CSipSecAgreeRecord.h"
#include "SipSecIpsecParams.h"
#include "MIpSecAgreeContextParams.h"
#include "CIpSecSadbHandler.h"
#include "MIpSecMechanismParams.h"
#include "CState.h"

#ifdef CPPUNIT_TEST
#include "CActiveObjController1.h"
#endif
    
// ----------------------------------------------------------------------------
// CState::~CState
// ----------------------------------------------------------------------------
//  
CState::~CState() 
	{
	}

// ----------------------------------------------------------------------------
// CState::SaAddedL
// ----------------------------------------------------------------------------
//  
void CState::SaAddedL( CSipSecAgreeRecord& /* aRec */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::SaDeleted
// ----------------------------------------------------------------------------
//  
void CState::SaDeleted( CSipSecAgreeRecord& /* aRec */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::PolicyActivatedL
// ----------------------------------------------------------------------------
//  
void CState::PolicyActivatedL( CSipSecAgreeRecord& /* aRec */ )
    {
    }

// ----------------------------------------------------------------------------
// CState::PolicyUnloaded
// ----------------------------------------------------------------------------
//      
void CState::PolicyUnloaded( CSipSecAgreeRecord& /* aRec */ )
    {
    }

// ----------------------------------------------------------------------------
// CState::TimerExpiredL
// ----------------------------------------------------------------------------
// 		
void CState::TimerExpiredL(
	CSipSecAgreeRecord& /* aRec */, 
	TTimerId /* aTimerId */, 
	TAny* /* aTimerParam */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::Resp4xxL
// ----------------------------------------------------------------------------
// 	
void CState::Resp4xxL( 
	CSipSecAgreeRecord& /* aRec */,
    CSIPResponse& /* aResponse */,
    CSIPRequest& /* aRequest */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::AuthKeyL
// ----------------------------------------------------------------------------
// 
TBool CState::AuthKeyL( 
	CSipSecAgreeRecord& /* aRec */,
	const TDesC8& /* aAuthKey */ )
	{
	return EFalse;
	}
	
// ----------------------------------------------------------------------------
// CState::AuthKeyFailedL
// ----------------------------------------------------------------------------
//	
void CState::AuthKeyFailedL( CSipSecAgreeRecord& /* aRec */ )
    {
    }

// ----------------------------------------------------------------------------
// CState::RegisterL
// ----------------------------------------------------------------------------
// 
void CState::RegisterL( 
	CSipSecAgreeRecord& /* aRec */,
	CSIPRequest& /* aRequest */,
	TSIPTransportParams& /* aTransportParams */,
	const TDesC8& /* aOutboundProxy */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::ReguestL
// ----------------------------------------------------------------------------
// 	
void CState::ReguestL( 
	CSipSecAgreeRecord& /* aRec */,
	CSIPRequest& /* aRequest */,
	TSIPTransportParams& /* aTransportParams */,
	const TDesC8& /* aOutboundProxy */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::Resp2xxL
// ----------------------------------------------------------------------------
// 
void CState::Resp2xxL( 
	CSipSecAgreeRecord& /* aRec */,
	CSIPResponse& /* aResponse */ )
	{
	}

// ----------------------------------------------------------------------------
// CState::ProcessSecVerifyL
// ----------------------------------------------------------------------------
// 	
void CState::ProcessSecVerifyL( 
    CSipSecAgreeRecord& /* aRec */,
	RPointerArray<CSIPSecurityVerifyHeader>& /* aSecurityVerify */ )
    {
    }

// ----------------------------------------------------------------------------
// CState::ClearSaL
// ----------------------------------------------------------------------------
// 	
void CState::ClearSaL( CSipSecAgreeRecord& aRec )
	{
	DoClearSaL( aRec );	
	}	

// ----------------------------------------------------------------------------
// CState::ReInitializeL
// ----------------------------------------------------------------------------
// 	
void CState::ReInitializeL( 
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse,
	TBool aFullReInitialize )
	{
	if ( aFullReInitialize )
	    {
	    DeleteSasAndPolicyL( aRec );
	    }
	else
	    {
	    aRec.DeleteSasL();
	    }
			 		 
	aRec.iSecVerifyHeaders.ResetAndDestroy();
	aRec.CopyHeadersFromMessageL( aResponse, EFalse );
	// Wait for Digest plugin generate a key
	// Set timer to 4 min while SA is temporal
	aRec.StartTimerL( KTempTime );
	aRec.NextState( EPendingKey );	
	}

// ----------------------------------------------------------------------------
// CState::DeleteSasL
// ----------------------------------------------------------------------------
// 	
void CState::DeleteSasL( CSipSecAgreeRecord& aRec )
	{
	DeleteSasAndPolicyL( aRec );
    aRec.NextState( EDeleting );	
	}

// ----------------------------------------------------------------------------
// CState::DoClearSaL
// ----------------------------------------------------------------------------
// 	
void CState::DoClearSaL( CSipSecAgreeRecord& aRec )
	{
	DeleteSasAndPolicyL( aRec );				 	 
	aRec.NextState( EClearing );	
	}
	
// ----------------------------------------------------------------------------
// CState::DeleteSasAndPolicyL
// ----------------------------------------------------------------------------
// 	
void CState::DeleteSasAndPolicyL( CSipSecAgreeRecord& aRec )
	{
	aRec.RemoveTransportL();		 	 	
	}

// ----------------------------------------------------------------------------
// CState::UpdateExpirationTimeL
// ----------------------------------------------------------------------------
//		
void CState::UpdateExpirationTimeL(  
	CSipSecAgreeRecord& aRec,
	CSIPResponse& aResponse )
	{
	TUint t = CSipSecAgreeRecord::ExpiresFromContactL( aResponse );
	
	// Set lifetime to expiration time of just completed registration + 
	// 2 x transaction timeout (since timeouted ta is retried without sigcomp)
	const TUint KTATimeoutMultiplier = 128;
	const TUint KSecondAsMillisecs = 1000;
    
    __ASSERT_ALWAYS( 
        t <= ( KMaxTUint / KSecondAsMillisecs - 
               KTATimeoutMultiplier * aRec.iParams.MechParams().T1() ), 
        User::Leave( KErrOverflow ) );

	t = t * KSecondAsMillisecs + 
	    KTATimeoutMultiplier * aRec.iParams.MechParams().T1();

    TUint t2;
    if ( aRec.iParams.HasLongerLifetimeSA( t, t2 ) )
        {
        // t2 is filled with longer lifetime, use it instead
        t = t2;
        }
        
	aRec.StartTimerL( t );
	}
		
// End of File
