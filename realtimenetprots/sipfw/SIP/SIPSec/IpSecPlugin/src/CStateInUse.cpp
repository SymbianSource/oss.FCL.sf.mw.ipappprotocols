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
// Name          : CStateInUse.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/5.0
//



#include "sipsecurityverifyheader.h"
#include "sipsecurityclientheader.h"
#include "siprequest.h"
#include "TSIPTransportParams.h"
#include "CSipSecAgreeRecord.h"
#include "MIpSecAgreeContextParams.h"
#include "CStateInUse.h"	
#include "MIpSecMechanismParams.h"
#include "CleanupResetAndDestroy.h"

// ----------------------------------------------------------------------------
// CInUse::CInUse
// ----------------------------------------------------------------------------
//
CInUse::CInUse()
    {
    }

// ----------------------------------------------------------------------------
// CInUse::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CInUse::State()
	{
	return EInUse;
	}

// ----------------------------------------------------------------------------
// CInUse::TimerExpiredL
// ----------------------------------------------------------------------------
//
void CInUse::TimerExpiredL( CSipSecAgreeRecord& aRec,
							TTimerId /* aTimerId */,
							TAny* /* aTimerParam */ )
	{
	DeleteSasL( aRec );
	}

// ----------------------------------------------------------------------------
// CInUse::RegisterL
// ----------------------------------------------------------------------------
//			
void CInUse::RegisterL( CSipSecAgreeRecord& aRec,
						CSIPRequest& aRequest,
						TSIPTransportParams& aTransportParams,
						const TDesC8& aOutboundProxy )
	{
    // If other record is already in temporal state, this record is not used
    // for registration
	if ( aRec.iParams.HasRecordInState( ETemporal ) )
		{	
		return;
		}
	
	// Create temporary sec client headers
    RPointerArray<CSIPSecurityHeaderBase> clientHdrs;
    
    CleanupResetAndDestroyPushL( clientHdrs );
    
    for ( TInt i = 0; i < aRec.iSecClientHeaders.Count(); i++ )
        {
        CSIPSecurityClientHeader* header = 
            static_cast<CSIPSecurityClientHeader*>( 
                aRec.iSecClientHeaders[ i ]->CloneL() );
        CleanupStack::PushL( header );
        clientHdrs.AppendL( header );
        CleanupStack::Pop( header );
        }
    
	aRec.iParams.MechParams().UpdateSecCliL( clientHdrs );	
	
	// Copy updated sec client headers to the request
	CSipSecAgreeRecord::CopyHeadersToMessageL( clientHdrs, aRequest, ETrue );

	// Temporary client headers are not needed anymore 
	CleanupStack::PopAndDestroy( 1 ); // clientHdrs

    // Use still existing SAs
	aTransportParams.SetTransportId( aRec.iTransportParams.TransportId() );

	// Make sure that SigComp usage is not disabled
    aTransportParams.SetIgnoreCompartmentId( EFalse );

	// Add port_ps to next hop address
	aRec.UpdateNextHopPort( aRequest, aOutboundProxy );
	}

// ----------------------------------------------------------------------------
// CInUse::ReguestL
// ----------------------------------------------------------------------------
//
void CInUse::ReguestL( CSipSecAgreeRecord& aRec,
					   CSIPRequest& aRequest,
					   TSIPTransportParams& aTransportParams,
					   const TDesC8& aOutboundProxy )
	{
	// Set transport id for outgoing request so that protected transports
	// are used
	aTransportParams.SetTransportId( aRec.iTransportParams.TransportId() );

	// Make sure that SigComp usage is not disabled
    aTransportParams.SetIgnoreCompartmentId( EFalse );
    
	// Add port_ps to next hop address
	aRec.UpdateNextHopPort( aRequest, aOutboundProxy );
	}	      

// ----------------------------------------------------------------------------
// CInUse::Resp2xxL
// ----------------------------------------------------------------------------
//		
void CInUse::Resp2xxL( CSipSecAgreeRecord& aRec, CSIPResponse& aResponse )
	{
	// Check whether 200 OK was received directly for re-registration
	if ( aRec.iParams.HasOnlyOneRecord() )
	    {
	    UpdateExpirationTimeL( aRec, aResponse );
	    }
	else
	    {
	    // There are now two SA records, the second record was created when a
	    // 401 was received earlier. The new record enters "in use" state when
	    // it gets the 2xx and this record, which already is "in use" enters
	    // "old" state.
    	aRec.NextState( EOld );
	    }
	}

// ----------------------------------------------------------------------------
// CInUse::ProcessSecVerifyL
// ----------------------------------------------------------------------------
// 	
void CInUse::ProcessSecVerifyL( 
    CSipSecAgreeRecord& aRec,
	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify )
    {
    // If two records exist but more recent is not yet in "temporal" state,
    // old sec verify hdrs should be used
    if ( !aRec.iParams.HasOnlyOneRecord() &&
         !aRec.iParams.HasRecordInState( ETemporal ) )
        {
	    aSecurityVerify.ResetAndDestroy();
	    RPointerArray<CSIPSecurityHeaderBase>& secVerifys = 
			aRec.SecurityHeaders( EFalse );
	    for ( TInt i = 0; i < secVerifys.Count(); i++ )
	        {
	        CSIPSecurityVerifyHeader* secHdr = 
	            static_cast<CSIPSecurityVerifyHeader*>(
					secVerifys[ i ]->CloneL() );
	        CleanupStack::PushL( secHdr );
	        aSecurityVerify.AppendL( secHdr );
	        CleanupStack::Pop( secHdr );
	        }
		}
    }

// End of File
