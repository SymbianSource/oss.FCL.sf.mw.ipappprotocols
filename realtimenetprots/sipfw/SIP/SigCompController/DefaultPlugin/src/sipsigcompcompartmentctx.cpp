// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipSigcompcompartmentctx.cpp
// Part of       : SIPSigComp
// Version       : SIP/3.0 
//



#include "sipsigcompcompartmentctx.h"
#include "sigcompcompartment.h"
#include "MCompartmentCtxOwner.h"
#include "MCompartmentCtxUser.h"

const TInt KUserArrayGranularity = 4;

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::NewL
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentCtx* CSipSigCompCompartmentCtx::NewL(
    MCompartmentCtxOwner& aMyOwner,
	const CSigComp& aSigComp, 
	const TInetAddr& aAddress,
	TUint32 aIapId,
	TBool aDynamicCompression )
	{
	CSipSigCompCompartmentCtx* self = 
	    CSipSigCompCompartmentCtx::NewLC( aMyOwner, 
	                                      aSigComp, 
	                                      aAddress, 
	                                      aIapId,
								          aDynamicCompression );
	CleanupStack::Pop();
	return self;
	}


// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::NewLC
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentCtx* CSipSigCompCompartmentCtx::NewLC(
    MCompartmentCtxOwner& aMyOwner,
	const CSigComp& aSigComp, 
	const TInetAddr& aAddress,
	TUint32 aIapId,
	TBool aDynamicCompression )
	{
	CSipSigCompCompartmentCtx* self = 
	    new ( ELeave ) CSipSigCompCompartmentCtx( aMyOwner, 
	                                              aSigComp, 
	                                              aAddress, 
	                                              aIapId,
	                                              aDynamicCompression );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::ConstructL()
	{
	iSigCompCompartment = 
	    CSigCompCompartment::NewL( iSigComp, iDynamicCompression );
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::~CSipSigCompCompartmentCtx
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentCtx::~CSipSigCompCompartmentCtx()
	{
	iUserArray.Reset();
	iUserArray.Close();
	delete iSigCompCompartment;
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::Compartment
// -----------------------------------------------------------------------------
//
CSigCompCompartment* CSipSigCompCompartmentCtx::Compartment()
	{
	return iSigCompCompartment;
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::AttachCompartmentUserL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::AttachCompartmentUserL(
    MCompartmentCtxUser* aUser )
	{
	// If entry of aUser existed already in array, fail silently
    TInt err  = iUserArray.InsertInAddressOrder( aUser );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::DetachCompartmentUser
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::DetachCompartmentUser(
    MCompartmentCtxUser* aUser )
	{
	TInt index = iUserArray.Find( aUser );
	if ( index != KErrNotFound )
	    {
	    iUserArray.Remove( index );
	    iUserArray.Compress();
	    }
	    
	// If no more users, compartment can be deleted
	if ( iUserArray.Count() == 0 )
	    {
	    iMyOwner.KillMe( this );
	    }
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::Match
// -----------------------------------------------------------------------------
//
TBool CSipSigCompCompartmentCtx::Match( 
    const TInetAddr& aAddress, 
    TUint32 aIapId ) const
	{
	// Don't care about address port
	return ( !aAddress.IsWildAddr() && 
	         iAddress.Match( aAddress ) &&
	         iIapId == aIapId );
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::SetCurrentState
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::SetCurrentState(
	CSipSigCompCompartmentCtx::TState aCurrentState )
	{
	iCurrentState = aCurrentState;
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::SendFailedL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::SendFailedL()
	{
	if ( iCurrentState == CSipSigCompCompartmentCtx::EAlive )
		{
		ResetCompartmentL();
		}
	else
		{
		NotifyUsersAboutSelfDestruction();
		iMyOwner.KillMe( this );
		}
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::NotifyUsersAboutSelfDestruction
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::NotifyUsersAboutSelfDestruction()
	{
	TInt lastIndex( iUserArray.Count() - 1 );
	for ( TInt i = lastIndex; i >= 0; i-- )
		{
		iUserArray[ i ]->CompartmentDeleted();
		}	
	}

// -----------------------------------------------------------------------------
// CSipSigCompCompartmentCtx::ResetCompartmentL
// -----------------------------------------------------------------------------
//
void CSipSigCompCompartmentCtx::ResetCompartmentL()
	{
	delete iSigCompCompartment; 
	iSigCompCompartment = 0;
	iSigCompCompartment = 
	    CSigCompCompartment::NewL( iSigComp, iDynamicCompression );
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentCtx::CSipSigCompCompartmentCtx(
    MCompartmentCtxOwner& aMyOwner,
	const CSigComp& aSigComp, 
	const TInetAddr& aAddress,
	TUint32 aIapId,
	TBool aDynamicCompression ) : 
    iMyOwner( aMyOwner ), 
    iSigComp( aSigComp ),
    iAddress( aAddress ),
    iIapId( aIapId ),
    iDynamicCompression( aDynamicCompression ),
    iUserArray( KUserArrayGranularity ),
    iCurrentState( CSipSigCompCompartmentCtx::EIdle )
	{
	}

