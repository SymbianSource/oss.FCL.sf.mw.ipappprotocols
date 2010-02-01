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
// Name        : SipProfileStorageSecureBackup.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include <connect/sbdefs.h>
#include "SipProfileStorageSecureBackup.h"
#include "SipProfileServerCore.h"
#include "SipProfileLog.h"

// Strange PC_Lint warning silenced
/*lint -e57 */
const TInt KBackupBeginning = conn::EBackupBase | conn::EBURBackupFull;
const TInt KRestoreBeginning = conn::EBackupBase | conn::EBURRestoreFull;
const TInt KBackupIncrementBeginning = conn::EBackupIncrement | conn::EBURBackupFull;
const TInt KRestoreIncrementBeginning = conn::EBackupIncrement | conn::EBURRestoreFull;
const TInt KBackupRestoreCompleted = conn::EBURNormal;

// -----------------------------------------------------------------------------
// CSIPProfileStorageSecureBackup::NewL
// -----------------------------------------------------------------------------
//
CSIPProfileStorageSecureBackup* CSIPProfileStorageSecureBackup::NewL (
	CSIPProfileServerCore* aObserver)
    {
    CSIPProfileStorageSecureBackup* self =
		new (ELeave) CSIPProfileStorageSecureBackup(aObserver);
    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageSecureBackup::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageSecureBackup::ConstructL ()
    {
	PROFILE_DEBUG1("SecureBackup, subscribing backup event")
	User::LeaveIfError (iProperty.Attach(KUidSystemCategory,
		conn::KUidBackupRestoreKey));
	iProperty.Subscribe(iStatus);
	SetActive();
	PROFILE_DEBUG1("SecureBackup, subscribing backup event, complete")
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageSecureBackup::CSIPProfileStorageSecureBackup
// -----------------------------------------------------------------------------
//
CSIPProfileStorageSecureBackup::CSIPProfileStorageSecureBackup (
	CSIPProfileServerCore* aObserver):
	CActive(EPriorityStandard), iObserver(aObserver), iRestoreOngoing(EFalse)
    {
	CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageSecureBackup::~CSIPProfileStorageSecureBackup
// -----------------------------------------------------------------------------
//
CSIPProfileStorageSecureBackup::~CSIPProfileStorageSecureBackup ()
    {
	CActive::Cancel();
	iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageSecureBackup::RunL
// -----------------------------------------------------------------------------
//
void CSIPProfileStorageSecureBackup::RunL ()
    {
	iProperty.Subscribe(iStatus);
	SetActive();
	TInt status(0);
	if (iProperty.Get(status) == KErrNone)
		{
		switch (status)
			{
			case KBackupBeginning:
				PROFILE_DEBUG1("SecureBackup, KBackupBeginning, releasing")
				iRestoreOngoing = EFalse;
                iObserver->ReleaseStorage(iRestoreOngoing);
				break;
			case KRestoreBeginning:
				PROFILE_DEBUG1("SecureBackup, KRestoreBeginning, releasing")
				iRestoreOngoing = ETrue;
                iObserver->ReleaseStorage(iRestoreOngoing);
				break;
			case KBackupIncrementBeginning:
				PROFILE_DEBUG1("SecureBackup, KBackupIncrementBeginning, releasing")
				iRestoreOngoing = EFalse;
                iObserver->ReleaseStorage(iRestoreOngoing);
				break;
			case KRestoreIncrementBeginning:
				PROFILE_DEBUG1("SecureBackup, KRestoreIncrementBeginning, releasing")
				iRestoreOngoing = ETrue;
                iObserver->ReleaseStorage(iRestoreOngoing);
				break;
			case KBackupRestoreCompleted:
				PROFILE_DEBUG1("SecureBackup, KBackupRestoreCompleted, reserving")
                iObserver->ReserveStorage(iRestoreOngoing);
                iRestoreOngoing = EFalse;
				break;
			default:
				PROFILE_DEBUG1("SecureBackup, Unknown event")
				break;
			}
		}
    }

// -----------------------------------------------------------------------------
// CSIPProfileStorageSecureBackup::DoCancel
// -----------------------------------------------------------------------------
//	
void CSIPProfileStorageSecureBackup::DoCancel ()
    {
	iProperty.Cancel();
    }
