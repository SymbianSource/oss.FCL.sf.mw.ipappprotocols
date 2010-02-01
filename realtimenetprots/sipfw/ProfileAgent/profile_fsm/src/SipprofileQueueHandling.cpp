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
// Name        : sipprofilequeuehandling.cpp
// Part of     : sip profile fsm
// implementation
// Version     : 1.0
//



// INCLUDE FILES
#include <bamdesca.h>
#include "sipprofilequeuehandling.h"
#include "sipprofilefsmuser.h"
#include "sipconcreteprofile.h"
#include "sipgendefs.h"
#include "sipmanagedprofile.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileQueueHandling* CSIPProfileQueueHandling::NewLC(
	MSIPProfileFSMUser& aUser )
	{
	CSIPProfileQueueHandling* self = 
	    new(ELeave)CSIPProfileQueueHandling( aUser );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileQueueHandling* CSIPProfileQueueHandling::NewL( 
	MSIPProfileFSMUser& aUser )
	{
	CSIPProfileQueueHandling* self = 
	    CSIPProfileQueueHandling::NewLC( aUser );
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::CSIPProfileQueueHandling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileQueueHandling::CSIPProfileQueueHandling( 
	MSIPProfileFSMUser& aUser ) : 
	iUser( aUser )
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::ConstructL()
// -----------------------------------------------------------------------------
//	
void CSIPProfileQueueHandling::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::~CSIPProfileQueueHandling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfileQueueHandling::~CSIPProfileQueueHandling()
	{
	iRegisteringQueue.Close();
	iDeregisteringQueue.Close();
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::AddIntoQueue()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPProfileQueueHandling::AddIntoQueue( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
	return iUser.AddProfileIntoQueue( aSIPConcreteProfile );
	}	
	
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::AddIntoDeregisterQueueL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPProfileQueueHandling::AddIntoDeregisterQueueL( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
	TSIPProfileQueueItem item( aSIPConcreteProfile, EFalse );
	return AddToQueueNoDuplicatesL( iDeregisteringQueue, item );
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::AddIntoRegisterQueueL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPProfileQueueHandling::AddIntoRegisterQueueL( 
    CSIPConcreteProfile& aSIPConcreteProfile,
    TBool aRetryRegistration )
	{
	TSIPProfileQueueItem item( aSIPConcreteProfile, aRetryRegistration );
	return AddToQueueNoDuplicatesL( iRegisteringQueue, item );
	}		
	
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::AddDeregisterToQueueL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileQueueHandling::AddDeregisterToQueueL( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
	TBool result = AddIntoQueue( aSIPConcreteProfile ); 
	if (result)
		{
		result = AddIntoDeregisterQueueL( aSIPConcreteProfile );
		}
	return result;
	}
		
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::AddRegisterToQueueL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileQueueHandling::AddRegisterToQueueL( 
	CSIPConcreteProfile& aSIPConcreteProfile,
	TBool aRetryRegistration )
	{
	TBool result = AddIntoQueue( aSIPConcreteProfile ); 
	if (result)
		{
		result = AddIntoRegisterQueueL( aSIPConcreteProfile, 
		                                aRetryRegistration );
		}
	return result;
	}	
		
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::RegisterFromQueueL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfileQueueHandling::RegisterFromQueueL( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
	// Called from ErrorEvent or SIPProfileStatusEvent
    if ( !CheckRegisterQueueL( aSIPConcreteProfile ) )
        {
        CheckDeregisterQueueL( aSIPConcreteProfile );
		}
	}		
		
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::FoundIPSecProfileCandidate()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileQueueHandling::FoundIPSecProfileCandidate()
	{
	TBool result = EFalse;
	for (TInt i=0; i< iRegisteringQueue.Count() && !result; i++)
		{
		CSIPConcreteProfile& profile = iRegisteringQueue[i].iConcreteProfile;
		result = profile.IsSecurityNegotiationEnabled() &&
		(!(profile.ServerParameter(KSIPRegistrar, KSIPDigestRealm).Length() 
		  &&
	      (profile.PrivateIdentity().Length() || 
	       profile.ServerParameter(KSIPRegistrar, KSIPDigestUserName).Length())
	      &&
	      profile.ServerParameter(KSIPRegistrar,KSIPDigestPassword).Length()));
		}
	return result;
	}	

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::CheckDeregisterQueueL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPProfileQueueHandling::CheckDeregisterQueueL( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
	TInt index = FindIndex( aSIPConcreteProfile, iDeregisteringQueue );
    if ( index >= 0 )
    	{
    	CSIPConcreteProfile& profile = 
    	    iDeregisteringQueue[index].iConcreteProfile;
    	iUser.DeregisterProfileAfterQueueL( profile );
        iDeregisteringQueue.Remove(index);
        iDeregisteringQueue.Compress();     	
    	}				
	return (index >= 0);
	}		
	
// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::CheckRegisterQueueL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPProfileQueueHandling::CheckRegisterQueueL( 
	CSIPConcreteProfile& aSIPConcreteProfile )
	{
    TInt index = FindIndex( aSIPConcreteProfile, iRegisteringQueue );
    if ( index >= 0 )
    	{
    	CSIPConcreteProfile& profile = 
    	    iRegisteringQueue[index].iConcreteProfile;
    	if ( iRegisteringQueue[index].iRetryRegistration )
    	    {
    	    iUser.RetryProfileRegistrationAfterQueueL( profile );
    	    }
    	else
    	    {
    	    iUser.RegisterProfileAfterQueueL( profile );
    	    }
        iRegisteringQueue.Remove(index);
        iRegisteringQueue.Compress();
    	}
	return (index >= 0);
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::FindIndex()
// -----------------------------------------------------------------------------
//
TInt CSIPProfileQueueHandling::FindIndex( 
	CSIPConcreteProfile& aSIPConcreteProfile,
	RArray<TSIPProfileQueueItem>& aProfileArray )
	{
	TInt index = KErrNotFound;
	for (TInt i=0; i< aProfileArray.Count() && index == KErrNotFound; i++)
		{
		if ( aProfileArray[i].iConcreteProfile.Server( KSIPRegistrar ).Compare( 
		     aSIPConcreteProfile.Server( KSIPRegistrar ) ) == 0 )
		    {
		    index = i;
		    }
		}
	return index;
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::IPSecProfileCandidate()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfile* CSIPProfileQueueHandling::IPSecProfileCandidate(
	TBool& aRegistering )
	{
	CSIPConcreteProfile* profile = NULL;
	TBool found = EFalse;
	aRegistering = ETrue;
	
	for (TInt i=0; i<iRegisteringQueue.Count() && !found; i++)
		{
		CSIPConcreteProfile& tmp = iRegisteringQueue[i].iConcreteProfile;
		found = tmp.IsSecurityNegotiationEnabled() &&
		(!(tmp.ServerParameter(KSIPRegistrar, KSIPDigestRealm).Length() 
		 &&
	     (tmp.PrivateIdentity().Length() || 
	      tmp.ServerParameter(KSIPRegistrar, KSIPDigestUserName).Length()) 
	     &&
	     tmp.ServerParameter(KSIPRegistrar,KSIPDigestPassword).Length()));
	    				
	   	if (found)
    		{
    		profile = &tmp;
    		iRegisteringQueue.Remove(i);
    		iRegisteringQueue.Compress();
    		}
		}
	
	for (TInt i=0; i< iDeregisteringQueue.Count() && !found; i++)
		{
		CSIPConcreteProfile& tmp = iDeregisteringQueue[i].iConcreteProfile;
		found = tmp.IsSecurityNegotiationEnabled() &&
		(!(tmp.ServerParameter(KSIPRegistrar, KSIPDigestRealm).Length() 
		 &&
	     (tmp.PrivateIdentity().Length() || 
	      tmp.ServerParameter(KSIPRegistrar, KSIPDigestUserName).Length()) 
	     &&
	    tmp.ServerParameter(KSIPRegistrar,KSIPDigestPassword).Length()));
	    aRegistering = EFalse;
		}
			
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::WaitingForAllowedNetworkProfile()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfile* 
CSIPProfileQueueHandling::WaitingForAllowedNetworkProfile(
    TBool /*aOnHomeNetwork*/ )
	{
	// This function call and execution has been removed, but still in place as it is Exported and for future Use.
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::RemoveProfileFromRegQueueL()
// (other items were commented in a header).
// testi versio
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPConcreteProfile* 
CSIPProfileQueueHandling::RemoveProfileFromRegQueueL(
    CSIPConcreteProfile& aSIPConcreteProfile)
	{
	TBool found = EFalse;
    CSIPConcreteProfile* profile = NULL;
	for (TInt i=0; i< iRegisteringQueue.Count() && !found; i++)
		{
		CSIPConcreteProfile& tmp = iRegisteringQueue[ i ].iConcreteProfile;
		found = ( tmp.Id() == aSIPConcreteProfile.Id() );
		profile = NULL;
		if ( found )
			{
			profile = &tmp;
			iRegisteringQueue.Remove( i );
			iRegisteringQueue.Compress();
			}					
		}
	return profile;
	}

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::IsEmpty()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPProfileQueueHandling::IsEmpty() const
    {
    return (iRegisteringQueue.Count() == 0  && 
            iDeregisteringQueue.Count() == 0);
    }

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::Cleanup()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C void CSIPProfileQueueHandling::Cleanup( 
    CSIPConcreteProfile& aOldProfile,
    CSIPConcreteProfile* aNewProfile )
    {
    QueueCleanup( iRegisteringQueue, aOldProfile, aNewProfile );
    QueueCleanup( iDeregisteringQueue, aOldProfile, aNewProfile );
    }

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::IsInQueue()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
EXPORT_C TBool CSIPProfileQueueHandling::IsInQueue( 
    CSIPConcreteProfile& aSIPConcreteProfile )
    {
    for (TInt i=0; i<iRegisteringQueue.Count();i++)
        {
        if (iRegisteringQueue[i].iConcreteProfile.Id() == 
        						aSIPConcreteProfile.Id() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::IsRegistrationAllowed
// -----------------------------------------------------------------------------
//	   
EXPORT_C TBool CSIPProfileQueueHandling::IsRegistrationAllowed(
    CSIPConcreteProfile& /*aSIPConcreteProfile*/, 
    TBool /*aOnHomeNetwork*/ )
    {
  	// This function call and execution has been removed, but still in place as it is Exported and for future Use.
	return EFalse;
    }


// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::AddToQueueNoDuplicatesL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
TBool CSIPProfileQueueHandling::AddToQueueNoDuplicatesL( 
    RArray<TSIPProfileQueueItem>& aProfileArray,
    TSIPProfileQueueItem& aProfileItem )
    {
    for (TInt i=0; i<aProfileArray.Count();i++)
        {
        if ( aProfileArray[i].iConcreteProfile.Id() == 
             aProfileItem.iConcreteProfile.Id() )
            {
            return EFalse;
            }
        }
    aProfileArray.AppendL( aProfileItem );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPProfileQueueHandling::QueueCleanup()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//     
void CSIPProfileQueueHandling::QueueCleanup( 
    RArray<TSIPProfileQueueItem>& aProfileArray,
    CSIPConcreteProfile& aOldProfile,
    CSIPConcreteProfile* aNewProfile )
    {
	TInt index = KErrNotFound;
	for ( TInt i=0; i< aProfileArray.Count() && index == KErrNotFound; i++ )
		{
		if ( &(aProfileArray[i].iConcreteProfile) == &aOldProfile )
		    {
		    index = i;
		    }
		}
    
    if ( index == KErrNotFound )
        {
        return;
        }
        
    TBool isAutoRegOnForOld( aOldProfile.IsAutoRegistrationEnabled() );
    
    if ( ( !isAutoRegOnForOld &&
           !aOldProfile.IsEnabled() ) ||
         ( aNewProfile && 
           !aNewProfile->IsAutoRegistrationEnabled() &&
           !aNewProfile->IsEnabled() &&
           isAutoRegOnForOld ) )
        {
        aProfileArray.Remove( index );
        aProfileArray.Compress();
        }
    }
