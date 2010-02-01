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
// Name          : sipsigcompcompartmentuser.cpp
// Part of       : SIPSigComp
// Version       : SIP/4.0 
//



#include "sipsigcompcompartmentuser.h"
#include "sipsigcompcompartmentctx.h"
#include "MCompartmentUserOwner.h"

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::NewL
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentUser* CSipSigCompCompartmentUser::NewL(
    MCompartmentUserOwner& aMyOwner,
    TUint32 aCompartmentId,
	TUint32 aIapId,
	TBool aDynamicCompression )
	{
	CSipSigCompCompartmentUser* self = 
	    CSipSigCompCompartmentUser::NewLC( aMyOwner, 
	                                       aCompartmentId,
	                                       aIapId, 
	                                       aDynamicCompression );
	CleanupStack::Pop();
	return self;
	}


// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::NewLC
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentUser* CSipSigCompCompartmentUser::NewLC(
    MCompartmentUserOwner& aMyOwner,
    TUint32 aCompartmentId,
	TUint32 aIapId,
	TBool aDynamicCompression )
	{
	CSipSigCompCompartmentUser* self = 
	    new ( ELeave ) CSipSigCompCompartmentUser( aMyOwner, 
	                                               aCompartmentId, 
	                                               aIapId,
	                                               aDynamicCompression );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentUser::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::~CSipSigCompCompartmentUser
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentUser::~CSipSigCompCompartmentUser()
	{
	if ( iCompartmentCtx )
	    {
	    iCompartmentCtx->DetachCompartmentUser( this );
	    }
	}
    
// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::CompartmentDeleted
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentUser::CompartmentDeleted()
    {
    // Cannot use ctx anymore
    iCompartmentCtx = 0;
    }
    
// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::Compartment
// -----------------------------------------------------------------------------
//
CSigCompCompartment* CSipSigCompCompartmentUser::Compartment()
	{
	if ( iCompartmentCtx )
	    {
	    return iCompartmentCtx->Compartment();
	    }
	return 0;
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::SetAddressL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentUser::SetAddressL( const TInetAddr& aAddress )
	{
	CSipSigCompCompartmentCtx* compCtx = 
	        iMyOwner.SearchCompartmentByInetAddr( aAddress, iIapId );
	
    if ( compCtx )
        {
        if ( iCompartmentCtx && iCompartmentCtx != compCtx )
            {
            // Change compartment, first detach from old compartment
            iCompartmentCtx->DetachCompartmentUser( this );
            }
        }
    else
        {
        // Create new compartment because there wasn't any for this
        // remote addr
        compCtx = 
            iMyOwner.CreateCompartmentL( aAddress, iIapId, iDynamicCompression );
        }
    
    iCompartmentCtx = compCtx;
    
    // Attach to compartment, no effect if already attached
    iCompartmentCtx->AttachCompartmentUserL( this ); 
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::Match
// -----------------------------------------------------------------------------
//
TBool CSipSigCompCompartmentUser::Match( const TInetAddr& aAddress ) const
	{
	return ( iCompartmentCtx && iCompartmentCtx->Match( aAddress, iIapId ) );
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::Match
// -----------------------------------------------------------------------------
//
TBool CSipSigCompCompartmentUser::Match( TUint32 aCompartmentId ) const
    {
    return ( iCompartmentId == aCompartmentId );
    }
	
// -----------------------------------------------------------------------------
// CSipSigCompCompartmentUser::SendFailedL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentUser::SendFailedL()
    {
    if ( iCompartmentCtx )
        {
        iCompartmentCtx->SendFailedL();
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentUser::CSipSigCompCompartmentUser(
    MCompartmentUserOwner& aMyOwner,
    TUint32 aCompartmentId,
	TUint32 aIapId,
	TBool aDynamicCompression ) : 
    iMyOwner( aMyOwner ), 
    iCompartmentId( aCompartmentId ),
    iIapId( aIapId ),
    iDynamicCompression( aDynamicCompression )
	{
	}

