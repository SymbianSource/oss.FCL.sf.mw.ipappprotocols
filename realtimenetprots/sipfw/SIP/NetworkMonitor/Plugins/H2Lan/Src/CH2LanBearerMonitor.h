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
* Name        : CH2LanBearerMonitor.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef CH2LANBEARERMONITOR_H
#define CH2LANBEARERMONITOR_H

// INCLUDES
#include <sipbearermonitor.h>

// CLASS DEFINITION
/**
 * CH2LanBearerMonitor implements a dummy plugin for
 * testing SIP in H2 reference HW.
 */
class CH2LanBearerMonitor
	: public CSIPBearerMonitor
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor
		 *
		 * @param aParams ECom instantiation parameters
		 * @return An initialized instance of this class.
		 */
		static CH2LanBearerMonitor* NewL( TAny* aParams );

		/// Destructor
		~CH2LanBearerMonitor();

	public: // from MSIPBearerMonitorObserver

		void MonitorStateChanged( TInt aError );
		

	private: // Constructors

		/**
		 * Constructor.
		 *
		 * @param aParams ECom instantiation parameters
		 */
		CH2LanBearerMonitor( TAny* aParams );

		/// Default constructor. Not implemented
		CH2LanBearerMonitor();

		/// 2nd phase constructor
		void ConstructL();
		
	private: // Data
	
	};

#endif // CH2LANBEARERMONITOR_H

