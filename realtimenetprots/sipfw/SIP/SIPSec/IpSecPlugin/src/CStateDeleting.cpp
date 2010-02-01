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
// Name          : CStateDeleting.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/4.0 
//



#include "CSipSecAgreeRecord.h"
#include "MIpSecAgreeContextParams.h"
#include "CStateDeleting.h"	
#include "MIpSecMechanismParams.h"

// ----------------------------------------------------------------------------
// CDeleting::CDeleting
// ----------------------------------------------------------------------------
//
CDeleting::CDeleting() 
    {
    }

// ----------------------------------------------------------------------------
// CDeleting::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CDeleting::State()
	{
	return EDeleting;
	}

// ----------------------------------------------------------------------------
// CDeleting::RegisterL
// ----------------------------------------------------------------------------
//			
void CDeleting::RegisterL(  
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
// CDeleting::SaDeleted
// ----------------------------------------------------------------------------
//	
void CDeleting::SaDeleted( CSipSecAgreeRecord& aRec )
	{
	CompleteIfReady( aRec );
	}
	
// ----------------------------------------------------------------------------
// CDeleting::PolicyUnloaded
// ----------------------------------------------------------------------------
//      
void CDeleting::PolicyUnloaded( CSipSecAgreeRecord& aRec )
    {
    CompleteIfReady( aRec );
    }

// ----------------------------------------------------------------------------
// CDeleting::ClearSaL
// ----------------------------------------------------------------------------
//	
void CDeleting::ClearSaL( CSipSecAgreeRecord& aRec )
	{
	aRec.NextState( EClearing );
	}
	
// ----------------------------------------------------------------------------
// CDeleting::CompleteIfReady
// ----------------------------------------------------------------------------
//    
void CDeleting::CompleteIfReady( CSipSecAgreeRecord& aRec )
    {
    if ( aRec.UnloadingAndDeletionDone() )
	    {
	    aRec.StopTimer();
	    // Leads to deletion of aRec
	    aRec.iParams.SaDeleted( &aRec );
	    }
    }
// End of File
