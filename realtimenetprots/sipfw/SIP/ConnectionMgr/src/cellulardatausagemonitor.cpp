/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : cellulardatausagemonitor.cpp
* Part of       : ConnectionMgr
* See class definition below.
*/



#include <centralrepository.h>
#include "cellulardatausagemonitor.h"
#include <cmmanagerkeys.h>
#include <cmgenconnsettings.h>
#include "SipLogs.h"

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::NewLC
// -----------------------------------------------------------------------------
//
CSipCellularDataUsageMonitor* CSipCellularDataUsageMonitor::NewL(MSipCellularDataUsageObserver & aObserver)
{
	CSipCellularDataUsageMonitor* self = new (ELeave) CSipCellularDataUsageMonitor(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::CSipCellularDataUsageMonitor
// -----------------------------------------------------------------------------
//

CSipCellularDataUsageMonitor::CSipCellularDataUsageMonitor(MSipCellularDataUsageObserver & aObserver) :
	CActive(EPriorityStandard),
	iObserver(aObserver)
{
	CActiveScheduler::Add(this);
}
	
// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::~CSipCellularDataUsageMonitor
// -----------------------------------------------------------------------------
//


CSipCellularDataUsageMonitor::~CSipCellularDataUsageMonitor()
{
	Cancel();
	delete iRepository;
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::DoCancel
// -----------------------------------------------------------------------------
//

void CSipCellularDataUsageMonitor::DoCancel()
{
	if (iRepository)
		{
		iRepository->NotifyCancelAll();
		}
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::ConstructL
// -----------------------------------------------------------------------------
//

void CSipCellularDataUsageMonitor:: ConstructL()
{
iRepository = CRepository::NewL( KCRUidCmManager );
//Current Usage Status;
TInt iUsageStatus;
iRepository->Get( KCurrentCellularDataUsage, iUsageStatus );
__SIP_INT_LOG1( "CSipCellularDataUsageMonitor::ConstructL, Initial CenRepValue ", iUsageStatus )	
SetCellularUsageStatus(iUsageStatus);
MonitorStatus();
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::MonitorStatus
// -----------------------------------------------------------------------------
//

void CSipCellularDataUsageMonitor::MonitorStatus()
{
__SIP_LOG( "CSipCellularDataUsageMonitor::MonitorStatus, Requesting Notify" )
TInt err = iRepository->NotifyRequest( KCurrentCellularDataUsage, iStatus );
SetActive();
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::RunL
// -----------------------------------------------------------------------------
//

void CSipCellularDataUsageMonitor::RunL()
{
	__SIP_INT_LOG1( "CSipCellularDataUsageMonitor::RunL, Status ", iStatus.Int() )
	if (iStatus.Int() >=  KErrNone)
		{
		TInt iUsageStatus;
		TInt err = iRepository->Get( KCurrentCellularDataUsage, iUsageStatus );
		__SIP_INT_LOG1( "CSipCellularDataUsageMonitor::RunL, Notified CenRepValue ", iUsageStatus )
		SetCellularUsageStatus(iUsageStatus);
		iObserver.CellularDataUsageChangedL(iPermissionStatus);
		}

	MonitorStatus();
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::RunError
// -----------------------------------------------------------------------------
//

TInt CSipCellularDataUsageMonitor::RunError(TInt aError)
{
return aError;
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::CellularUsageStatus
// -----------------------------------------------------------------------------
//

MSipCellularDataUsageObserver::TCellularDataUsageStatus CSipCellularDataUsageMonitor::CellularUsageStatus()
{

__SIP_INT_LOG1( "CSipCellularDataUsageMonitor::CellularUsageStatus, Status is ", iPermissionStatus )
return iPermissionStatus;
}

// -----------------------------------------------------------------------------
// CSipCellularDataUsageMonitor::SetCellularUsageStatus
// -----------------------------------------------------------------------------
//

void CSipCellularDataUsageMonitor::SetCellularUsageStatus(TInt aVal)

{
if(aVal != ECmCellularDataUsageDisabled) 
	{
	// This means the connectivity Settings Value is set to either "Automatic" or "Always Ask" for  the current Network.
		iPermissionStatus = MSipCellularDataUsageObserver::EDataUsageAllowed;
	}
else
	{
	// This means the connectivity Settings Value is set to "WLAN Only" for  the current Network.
	iPermissionStatus = MSipCellularDataUsageObserver::EDataUsageNotAllowed;
	}
}
