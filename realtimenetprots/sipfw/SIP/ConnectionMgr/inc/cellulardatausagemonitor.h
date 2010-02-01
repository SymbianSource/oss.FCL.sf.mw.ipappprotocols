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
* Name          : cellulardataUsagemonitor.h
* Part of       : ConnectionMgr
* See class definition below.
*/

/**
 @internalComponent
*/

#ifndef CSIPCELLULARDATAUSAGEMONITOR_H
#define CSIPCELLULARDATAUSAGEMONITOR_H

#include <e32base.h>
#include "MSipCellularDataUsageObserver.h"


class CRepository;
//class MSipCellularDataUsageObserver;

/*This class is used to monitor the cellular Data Usage permission from the CenRep.
* The CenRep Key that is monitored is KCurrentCellularDataUsage. 
* The value of this key will determine whether the Data usage Status for the Current Network.
*/

class CSipCellularDataUsageMonitor: public CActive
{
public:

static CSipCellularDataUsageMonitor* NewL(MSipCellularDataUsageObserver & aObserver);
~CSipCellularDataUsageMonitor(); //D'tor
MSipCellularDataUsageObserver::TCellularDataUsageStatus CellularUsageStatus();

protected: // From CActive
	
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);
private:
		void ConstructL();
		CSipCellularDataUsageMonitor(MSipCellularDataUsageObserver & aObserver); //Ctor
		void MonitorStatus();
		void SetCellularUsageStatus(TInt aVal);
		
private:
CRepository* iRepository;
MSipCellularDataUsageObserver::TCellularDataUsageStatus iPermissionStatus;
MSipCellularDataUsageObserver & iObserver; 
private: // For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSipCellularUsageObserver_Test;
#endif

};
#endif
