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
// Name          : CStateClearing.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "CSipSecAgreeRecord.h"
#include "MIpSecAgreeContextParams.h"
#include "CStateClearing.h"	
#include "MIpSecMechanismParams.h"

// ----------------------------------------------------------------------------
// CClearing::CClearing
// ----------------------------------------------------------------------------
//
CClearing::CClearing() 
    {
    }

// ----------------------------------------------------------------------------
// CClearing::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CClearing::State()
	{
	return EClearing;
	}

// ----------------------------------------------------------------------------
// CClearing::RegisterL
// ----------------------------------------------------------------------------
//			
void CClearing::RegisterL(  
	CSipSecAgreeRecord& aRec,
	CSIPRequest& aRequest,
	TSIPTransportParams& aTransportParams,
	const TDesC8& /*aOutboundProxy*/ )
	{
    // If new registration attempt occurs very fast, SAs deletion might be
    // still ongoing, treat the register as a fresh registration
    if ( aRec.iParams.HasOnlyOneRecord() )
        {
        aRec.iParams.MechParams().ProcessInitialReqisterL( aTransportParams, 
                                                           aRequest );
        }
	}
	
// ----------------------------------------------------------------------------
// CClearing::SaDeleted
// ----------------------------------------------------------------------------
//	
void CClearing::SaDeleted( CSipSecAgreeRecord& aRec )
	{
    CompleteIfReady( aRec );
	}
	
// ----------------------------------------------------------------------------
// CClearing::PolicyUnloaded
// ----------------------------------------------------------------------------
//      
void CClearing::PolicyUnloaded( CSipSecAgreeRecord& aRec )
    {
    CompleteIfReady( aRec );
    }

// ----------------------------------------------------------------------------
// CClearing::CompleteIfReady
// ----------------------------------------------------------------------------
//    
void CClearing::CompleteIfReady( CSipSecAgreeRecord& aRec )
    {
    if ( aRec.UnloadingAndDeletionDone() )
	    {
	    aRec.StopTimer();
	    // Leads to deletion of aRec and possibly to whole agree context
	    aRec.iParams.SaCleared( &aRec );
	    }
    }
	
// End of File
