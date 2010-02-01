/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : CAccessNetworkMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CACCESSNETWORKMONITOR_H__
#define __CACCESSNETWORKMONITOR_H__

// INCLUDES
#include <etelmm.h>
#include "sipaccessnetworkinfo.h"

// FORWARD DECLARATIONS
class MAccessNetworkInfoObserver;


// CLASS DEFINITION
/**
 * Monitors the changes of cell info.
 */
class CAccessNetworkMonitor : public CActive
	{
	public:	// Constructors and destructor
		/**
		 * Static constructor.
		 *
		 * @param aObserver Callback to owner
		 * @return An initialized instance of this class.
		 */
		static CAccessNetworkMonitor*
			NewL( MAccessNetworkInfoObserver& aObserver );

		// Destructor
		~CAccessNetworkMonitor();
	
	protected: // From CActive

		void DoCancel();
		void RunL();

	private: // Constructors

		// Constructor
		CAccessNetworkMonitor( MAccessNetworkInfoObserver& aObserver );

		// Default constructor, not implemented.
		CAccessNetworkMonitor();

		// 2nd phase constructor
		void ConstructL();

	private: // New functions, for internal use
		/**
		 * Request notification when network information changes.
		 *
		 * @pre !IsActive()
		 *		 
		 * @return An initialized instance of this class.
		 */
		void MonitorForChanges();

	private: // Data

		//Observer to notify when information has changed
		MAccessNetworkInfoObserver& iObserver;
		RTelServer iTelServer;
		RMobilePhone iPhone;	

		//Buffers where the notifications are received from RMobilePhone
		TPckgBuf<RMobilePhone::TMobilePhoneNetworkInfoV2> iNetworkInfoPckg;		
		RMobilePhone::TMobilePhoneLocationAreaV1 iLocationArea;

		//Access network information extracted from iNetworkInfoPckg and
		//iLocationArea.
		TSIPAccessNetworkInfo iAccessNetworkInfo;
	};

#endif // __CACCESSNETWORKMONITOR_H__
