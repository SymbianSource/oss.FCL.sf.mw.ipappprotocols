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
// Name        : sipplugindirector
// Part of     : sip agent server
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include	"sipplugindirector.h"
#include	"sipprofileagent.h"
#include	"sipprofileagentinitparams.h"
#include	"SipProfileLog.h"
#include 	<sipprofileagentextensionparams.h>

const TInt KPluginMicroSecInSec = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPPluginDirector::CSIPPluginDirector
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPPluginDirector::CSIPPluginDirector(MSIPProfileAgentObserver& aObserver) 
 : iProfileAgentObserver(aObserver),
   iDeltaTimerCallBack(CleanupTimerExpired, this)
    {
    TUid KSIPProfileServerUid = {0x0};
	iServerId = KSIPProfileServerUid;
	iDeltaTimerEntry.Set(iDeltaTimerCallBack);
    }

// -----------------------------------------------------------------------------
// CSIPPluginDirector::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::ConstructL()
    {
    iDeltaTimer = CDeltaTimer::NewL(CActive::EPriorityStandard);
    iConfigExtension = CSipProfileAgentConfigExtension::NewL();
    }

// -----------------------------------------------------------------------------
// CSIPPluginDirector::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPPluginDirector* CSIPPluginDirector::NewLC(
	MSIPProfileAgentObserver& aObserver)
    {
    CSIPPluginDirector* self = new(ELeave)CSIPPluginDirector(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPPluginDirector::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPPluginDirector* CSIPPluginDirector::NewL(
	MSIPProfileAgentObserver& aObserver)
    {
    CSIPPluginDirector* self = CSIPPluginDirector::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }
 
// -----------------------------------------------------------------------------
// CSIPPluginDirector::~CSIPPluginDirector
// -----------------------------------------------------------------------------
//
CSIPPluginDirector::~CSIPPluginDirector()
    {
	iSIPProfileAgents.ResetAndDestroy();
	delete iDeltaTimer;
	delete iConfigExtension;
	REComSession::FinalClose();
    }

// -----------------------------------------------------------------------------
// CSIPPluginDirector::FindImplementation 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPProfileAgent* CSIPPluginDirector::FindImplementation(
	const TSIPProfileTypeInfo& aType)
	{
	PROFILE_DEBUG6("CSIPPluginDirector::FindImplementation", 
		aType.iSIPProfileName)
	for (TInt i=0; i < iSIPProfileAgents.Count(); i++)
		{
		if (iSIPProfileAgents[i]->Type().iSIPProfileName == 
			aType.iSIPProfileName && 
			iSIPProfileAgents[i]->Type().iSIPProfileClass == 
			aType.iSIPProfileClass)
			{
			return iSIPProfileAgents[i];
			}
			
		}
	PROFILE_DEBUG2("CSIPPluginDirector::FindImplementation", "Not found!")
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::CreateImplementationL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPProfileAgent& CSIPPluginDirector::CreateImplementationL(
	const TSIPProfileTypeInfo& aType)
	{
	PROFILE_DEBUG6("CSIPPluginDirector::CreateImplementationL", 
		aType.iSIPProfileName)
	TSIPProfileAgentInitParams initParams(*iDeltaTimer,
			iProfileAgentObserver);
	TBuf8<30> plugin;
	plugin.Format(_L8("%u_"), aType.iSIPProfileClass);
	plugin.Append(aType.iSIPProfileName);

	CSIPProfileAgent* sipProfileAgent = 
	    CSIPProfileAgent::NewL(plugin,initParams);
	CleanupStack::PushL( sipProfileAgent);
	sipProfileAgent->Extension(KSipProfileAgentConfigExtension,
			iConfigExtension);
	User::LeaveIfError(iSIPProfileAgents.Append(sipProfileAgent));
	CleanupStack::Pop(sipProfileAgent);
	return *sipProfileAgent;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::ProvideImplementationL
// provides correct implementation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPProfileAgent& CSIPPluginDirector::ProvideImplementationL(
	const TSIPProfileTypeInfo& aType)
	{
	PROFILE_DEBUG6("CSIPPluginDirector::ProvideImplementationL", aType.iSIPProfileName)

	CSIPProfileAgent* implementation = FindImplementation(aType);
	if (implementation)
		return *implementation;
	else
		return CreateImplementationL(aType);
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::DeltaTimer
// returns iDeltaTimer as reference
// -----------------------------------------------------------------------------
//
CDeltaTimer& CSIPPluginDirector::DeltaTimer()
	{
	return *iDeltaTimer;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::RegisterL
// registers profile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::RegisterL(
	CSIPConcreteProfile& aSIPProfile)
	{
	PROFILE_DEBUG3("CSIPPluginDirector::RegisterL", aSIPProfile.Id())
	ProvideImplementationL(aSIPProfile.ProfileType()).RegisterL(aSIPProfile);
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::DeregisterL
// deregisters profile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::DeregisterL(
	CSIPConcreteProfile& aSIPProfile)
	{
	PROFILE_DEBUG3("CSIPPluginDirector::DeregisterL", aSIPProfile.Id())
	CSIPProfileAgent* profileagent = FindImplementation(aSIPProfile.ProfileType());
	
	__ASSERT_ALWAYS (profileagent, User::Leave(KErrArgument));
	profileagent->DeregisterL(aSIPProfile);
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::UpdateRegistrationL
// deregisters profile
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::UpdateRegistrationL(
	CSIPConcreteProfile& aNewProfile,
	CSIPConcreteProfile& aOldProfile)
	{
	PROFILE_DEBUG3("CSIPPluginDirector::UpdateRegistrationL", aNewProfile.Id())
	__ASSERT_ALWAYS(aNewProfile.Id() == aOldProfile.Id(), 
	                User::Leave(KErrArgument));
	CSIPProfileAgent* profileagent = FindImplementation(aOldProfile.ProfileType());
	__ASSERT_ALWAYS(profileagent, User::Leave(KErrArgument));
	profileagent->UpdateL(aNewProfile, aOldProfile);
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::ListImplementationsL
// Lists all implementations
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::ListImplementationsL(
	RImplInfoPtrArray& aImplementations)
	{
	PROFILE_DEBUG1("CSIPPluginDirector::ListImplementationsL")
	CSIPProfileAgent::ListAllImplementationsL(aImplementations);
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::IsRegisterPending
// -----------------------------------------------------------------------------
//
TBool CSIPPluginDirector::IsRegisterPending(CSIPConcreteProfile& aSIPProfile)
	{
	PROFILE_DEBUG3("CSIPPluginDirector::IsRegisterPending", aSIPProfile.Id())
	CSIPProfileAgent* implementation = 
	    FindImplementation(aSIPProfile.ProfileType());
	if (implementation)
		{
        return implementation->RegisterPending(aSIPProfile);
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::State
// -----------------------------------------------------------------------------
//
TInt CSIPPluginDirector::State(CSIPConcreteProfile::TStatus& aStatus,
							   CSIPConcreteProfile& aSIPProfile)
	{
	TInt err(KErrNotFound);
	aStatus = CSIPConcreteProfile::EUnregistered;
	CSIPProfileAgent* implementation =
	    FindImplementation(aSIPProfile.ProfileType());
	if (implementation)
		{
		err = implementation->GetProfileState(aStatus, aSIPProfile);
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::TerminateHandling
// -----------------------------------------------------------------------------
//
TInt CSIPPluginDirector::TerminateHandling(CSIPConcreteProfile& aSIPProfile)
	{
	CSIPProfileAgent* implementation = 
	    FindImplementation(aSIPProfile.ProfileType());
	if (implementation)
		{
		return implementation->TerminateHandling(aSIPProfile);
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::Cleanup
// initiate cleanup; idle agent will be deleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::Cleanup()
	{
	CSIPProfileAgent* agent;

	for (TInt i=iSIPProfileAgents.Count()-1; i >= 0; i-- )
		{
		agent = iSIPProfileAgents[i];
		if ( agent->IsIdle() )
			{
			iSIPProfileAgents.Remove(i);
			delete agent;
			agent = 0;
			}//if

		}//for
	iSIPProfileAgents.Compress();
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::StartTimerForCleanup
// -----------------------------------------------------------------------------
//
void CSIPPluginDirector::StartTimerForCleanup ()
	{
	PROFILE_DEBUG1("CSIPPluginDirector::StartTimerForCleanup")
	iDeltaTimer->Remove(iDeltaTimerEntry);
	TTimeIntervalMicroSeconds32 interval(KPluginMicroSecInSec);
	iDeltaTimer->Queue(interval, iDeltaTimerEntry);
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::CleanupTimerExpired
// -----------------------------------------------------------------------------
//
TInt CSIPPluginDirector::CleanupTimerExpired(TAny* aPtr)
	{
	PROFILE_DEBUG1("CSIPPluginDirector::CleanupTimerExpired")
	CSIPPluginDirector* self = reinterpret_cast<CSIPPluginDirector*>(aPtr);
  	self->Cleanup();
  	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPPluginDirector::CreateProfileL
// -----------------------------------------------------------------------------
//
CSIPConcreteProfile* CSIPPluginDirector::CreateProfileL (
	const TSIPProfileTypeInfo& aType)
	{
	PROFILE_DEBUG1("CSIPPluginDirector::CreateProfileL")
	return ProvideImplementationL(aType).CreateL();
	}
