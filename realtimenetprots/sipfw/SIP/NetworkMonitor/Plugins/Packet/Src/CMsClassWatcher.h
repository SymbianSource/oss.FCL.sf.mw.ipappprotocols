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
* Name        : CMsClassWatcher.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CMSCLASSWATCHER_H__
#define __CMSCLASSWATCHER_H__

// INCLUDES
#include <etelpckt.h>

// FORWARD DECLARATIONS
class CPacketServiceMonitor;

// CLASS DEFINITION
/**
 *
 */
class CMsClassWatcher
	: public CActive
	{
	public:	// Constructors and destructor

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CMsClassWatcher* NewL( CPacketServiceMonitor& aParent,
								      RPacketService& aPacketService );

		/// Destructor
		~CMsClassWatcher();

	public: // New methods

		TBool IsLineMonitoringNeeded() const;
		TBool IsSupportedMSClass() const;

	protected: // From CActive

		void DoCancel();
		void RunL();

	private: // Constructors

		/// Constructor
		CMsClassWatcher( CPacketServiceMonitor& aParent,
						 RPacketService& aPacketService );

		/// Default constructor. Not implemented.
		CMsClassWatcher();

		/// 2nd phase constructor
		void ConstructL();
	
	private: // New methods
		
		void UpdateMSClassInfoAvailability();
		
		TBool IsMSClassInfoAvailable() const;

	private: // Data

		CPacketServiceMonitor& iParent;
		RPacketService& iPacketService;
		RPacketService::TMSClass iMsClass;
		RPacketService::TMSClass iMaxMsClass;
		TBool iMSClassInfoAvailable;

	};

#endif // __CMSCLASSWATCHER_H__
