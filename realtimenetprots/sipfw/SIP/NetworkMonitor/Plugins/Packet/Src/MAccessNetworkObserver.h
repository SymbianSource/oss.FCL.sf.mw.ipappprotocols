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
* Name        : MAccessNetworkObserver.h
* Part of     : NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __MACCESSNETWORKOBSERVER_H__
#define __MACCESSNETWORKOBSERVER_H__

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class TSIPAccessNetworkInfo;

// CLASS DEFINITION
/**
 * MAccessNetworkInfoObserver defines an internal interface for observing
 * access network information changes.
 */
class MAccessNetworkInfoObserver
	{
	protected: // Constructors and destructors

		/// Virtual destructor. Prohibit deletion through this interface.
		virtual ~MAccessNetworkInfoObserver() {};

	public: // Abstract methods

		/**
		 * Called when the access network info has changed.
		 *
		 * @param aInfo Access network info
		 */
		virtual void InfoChanged( const TSIPAccessNetworkInfo& aInfo ) = 0;
	};

#endif // __MACCESSNETWORKOBSERVER_H__
