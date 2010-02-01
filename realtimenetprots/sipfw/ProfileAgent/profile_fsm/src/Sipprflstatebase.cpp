// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprflstatebase.cpp
// Part of     : sip profile fsm
// implementation
// Version     : %version: 2.1.1 %
//



// INCLUDE FILES
#include "sipprflstatebase.h"
#include "sipconcreteprofile.h"
#include "sipprofileagentobserver.h"
#include "sipregistrationbinding.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::CSIPPrflStateBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPPrflStateBase::CSIPPrflStateBase(
    MSIPProfileFSMUser& aUser,
    MSIPProfileContext::TSIPProfileState aName)
    : iUser(aUser), 
      iName(aName)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::CSIPPrflStateBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPPrflStateBase::~CSIPPrflStateBase()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPPrflStateBase::Name()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C MSIPProfileContext::TSIPProfileState CSIPPrflStateBase::Name() const
	{
	return iName;
	}	

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::RegisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::RegisterL(
	MSIPProfileContext& /*aContext*/)
	{
	User::Leave(KErrTotalLossOfPrecision);
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::DeregisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::DeregisterL(
	MSIPProfileContext& /*aContext*/)
	{
	User::Leave(KErrTotalLossOfPrecision);
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::UpdateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::UpdateL(
	MSIPProfileContext& /*aContext*/,
	CSIPConcreteProfile& /*aNewProfile*/)
	{
	User::Leave(KErrTotalLossOfPrecision);
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::ResponseReceived()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::ResponseReceived(
	MSIPProfileContext& /*aContext*/,
	CSIPClientTransaction& /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::ErrorOccurred()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::ErrorOccured(
	MSIPProfileContext& /*aContext*/,
	TInt /*aError*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::ConnectionStateChanged()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::ConnectionStateChanged(
	MSIPProfileContext& /*aContext*/,
	CSIPConnection::TState /*aState*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::ChangeRegistrationStateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::ChangeRegistrationStateL(
	MSIPProfileContext& /*aContext*/)
	{
	User::Leave(KErrTotalLossOfPrecision);
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::IsRegisterPending()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TBool CSIPPrflStateBase::IsRegisterPending() const
    {
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CSIPPrflStateBase::ProxyResolvingRequestCompleteL()
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::ProxyResolvingRequestCompleteL(
    MSIPProfileContext& /*aContext*/,
    MDesC8Array& /*aProxies*/)
    {
    User::Leave(KErrTotalLossOfPrecision);
    }

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::ProxyResolvingRequestFailed()
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::ProxyResolvingRequestFailed(
    MSIPProfileContext& /*aContext*/,
    TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::IsIdle()
// -----------------------------------------------------------------------------
//
TBool CSIPPrflStateBase::IsIdle( MSIPProfileContext& /*aContext*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::HandleError()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPrflStateBase::HandleError(
	MSIPProfileContext& aContext,
	TInt aError,
	CSIPPrflStateBase* aNextState)
	{
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	TUint32	contextId(0);
	CSIPConcreteProfile& profile = DoCleanup(aContext,contextId);
	aContext.SetNextState(*aNextState);
	aContext.AgentObserver().SIPProfileErrorEvent(profile,aError);
	}

// -----------------------------------------------------------------------------
// CSIPPrflStateBase::DoCleanup()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile& CSIPPrflStateBase::DoCleanup(
	MSIPProfileContext& aContext, 
	TUint32& aContextId)
	{
	__ASSERT_DEBUG(aContext.Profile()!=0, User::Invariant());
	CSIPConcreteProfile& profile = *(aContext.Profile());
	aContext.ForgetProfile();
	aContextId = 0;
	if (aContext.Registration())
	    {
	    aContextId = aContext.Registration()->ContextId();
	    }
	aContext.DestroyRegistration();
	return profile;
	}
