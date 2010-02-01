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
// Name        : sipprofileitcreceiver
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include <s32mem.h>
#include <eikenv.h>

#include "SIPProfileITCReceiver.h"
#include "SIPRemoteProfile.h"
#include "sipconcreteprofile.h"
#include "sipconcreteprofileobserver.h"
#include "sipprofileslots.h"
#include "sipprofilealrobserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileITCReceiver* CSIPProfileITCReceiver::NewL(
	RSIPProfile& aSipProfile, MSIPConcreteProfileObserver& aObserver)
	{
    CSIPProfileITCReceiver* self = 
		CSIPProfileITCReceiver::NewLC(aSipProfile, aObserver);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileITCReceiver* CSIPProfileITCReceiver::NewLC(
	RSIPProfile& aSipProfile, MSIPConcreteProfileObserver& aObserver)
	{
	CSIPProfileITCReceiver* self = new (ELeave)
		CSIPProfileITCReceiver(aSipProfile, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::CSIPProfileITCReceiver
// -----------------------------------------------------------------------------
//
CSIPProfileITCReceiver::CSIPProfileITCReceiver(
	RSIPProfile& aSipProfile, MSIPConcreteProfileObserver& aObserver) 
	: CActive(EPriorityStandard),
	iSip(aSipProfile),
	iObserver(aObserver)
	{
    CActiveScheduler::Add(this);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCReceiver::ConstructL()
	{
    iITCMsgArgs.Set(ESipProfileItcArgSlots, &iIdsPckg);
	Receive();
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::~CSIPProfileITCReceiver
// -----------------------------------------------------------------------------
//
CSIPProfileITCReceiver::~CSIPProfileITCReceiver() 
	{
	CActive::Cancel();
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::Receive
// -----------------------------------------------------------------------------
//
void CSIPProfileITCReceiver::Receive()
	{
	if (!IsActive())
		{
    	iSip.Receive(iITCMsgArgs, iStatus);
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::RunL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCReceiver::RunL()
	{
	TInt err = iStatus.Int();
	TSIPProfileSlots ids = iIdsPckg();
	
	if (err == KErrServerTerminated)
		{
		User::LeaveIfError(iSip.Connect());
		}

	Receive();

	if (err == KErrNone)
		{
		IncomingEventL(ids);
		}
	else
		{
		ErrorOccuredL(err,ids);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPProfileITCReceiver::RunError(TInt aError)
	{
	if (aError == KErrServerTerminated)
		{
		// to avoid infinite loop
		return aError;
		}
	Receive();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::DoCancel
// -----------------------------------------------------------------------------
//
void CSIPProfileITCReceiver::DoCancel()
	{
    iSip.CancelReceive();
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::IncomingEventL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCReceiver::IncomingEventL(TSIPProfileSlots& aIds)
    {
	switch (aIds.iEventId)
		{
		case ESipProfileItcOpRegistered:
			iObserver.RegistrationStatusChangedL(
				aIds.iProfileId, 
				CSIPConcreteProfile::ERegistered, 
				aIds.iSlot1);
			break;
		case ESipProfileItcOpRegistrationError:
			iObserver.ErrorOccurredL(
				aIds.iProfileId,
				CSIPConcreteProfile::ERegistrationInProgress, 
				aIds.iSlot1);
			break;
		case ESipProfileItcOpUnregistrationError:
			iObserver.ErrorOccurredL(
				aIds.iProfileId, 
				CSIPConcreteProfile::EUnregistrationInProgress, 
				aIds.iSlot1);
			break;
		case ESipProfileItcOpDeregistered:
			iObserver.RegistrationStatusChangedL(
				aIds.iProfileId, 
				CSIPConcreteProfile::EUnregistered, 
				0);
			break;
		case ESipProfileItcOpProfileAdded:
		case ESipProfileItcOpProfileAddedDefault:
			iObserver.AddedL(aIds.iProfileId);
			break;
		case ESipProfileItcOpProfileUpdated:
			iObserver.UpdatedL(aIds.iProfileId, aIds.iSlot1);
			break;
		case ESipProfileItcOpProfileRemoved:
			iObserver.RemovedL(aIds.iProfileId);
			break;
		case ESipProfileItcOpIapAvailable:
			if (iALRObserver)
				{
				iALRObserver->AlrEvent(
					MSipProfileAlrObserver::EIapAvailable, 
					aIds.iProfileId,
					aIds.iSlot1, 
					aIds.iSlot2);
				}
			break;
		case ESipProfileItcOpMigrationStarted:
			if (iALRObserver)
				{
				iALRObserver->AlrEvent(
					MSipProfileAlrObserver::EMigrationStarted, 
					aIds.iProfileId,
					aIds.iSlot1, 
					aIds.iSlot2);
				}
			break; 
		case ESipProfileItcOpMigrationCompleted:
			if (iALRObserver)
				{
				iALRObserver->AlrEvent(
					MSipProfileAlrObserver::EMigrationCompleted, 
					aIds.iProfileId,
					aIds.iSlot1, 
					aIds.iSlot2);
				}
			break;
		case ESipProfileItcOpALRError:
			if (iALRObserver)
				{
				iALRObserver->AlrError(
					aIds.iError, 
					aIds.iProfileId,
					aIds.iSlot1,
					aIds.iSlot2);
				}
			break;
		case ESipProfileItcOpProfileForciblyDisabled:
			{
			iObserver.RegistrationStatusChangedL(
				aIds.iProfileId, 
				CSIPConcreteProfile::EUnregistered, 
				aIds.iEventId);
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::ErrorOccuredL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCReceiver::ErrorOccuredL(TInt aError, TSIPProfileSlots& aIds)
	{
	iObserver.ErrorOccurredL(aIds.iProfileId, 
							 CSIPConcreteProfile::ERegistrationInProgress, 
							 aError);
	}
	
// -----------------------------------------------------------------------------
// CSIPProfileITCReceiver::SetALRObserver
// -----------------------------------------------------------------------------
//
TInt
CSIPProfileITCReceiver::SetALRObserver(MSipProfileAlrObserver* aALRObserver)
	{
	if (aALRObserver && iALRObserver)
		{
		return KErrAlreadyExists;
		}
	iALRObserver = aALRObserver;
	return KErrNone;
	}
