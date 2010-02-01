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
* Name        : sipbearermonitorobserver.h
* Part of     : SIP NetworkMonitor
* See class definition below.
* Version     : SIP/4.0
*
*/



#ifndef MSIPBEARERMONITOROBSERVER_H
#define MSIPBEARERMONITOROBSERVER_H

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* MSIPBearerMonitorObserver defines an internal interface for observing 
* network monitor objects within bearer monitor plugins.
*/
class MSIPBearerMonitorObserver
	{
	protected: // Constructors and destructors

		/// Virtual destructor. Prohibit deletion through this interface.
		virtual ~MSIPBearerMonitorObserver() {};

	public: // Abstract methods

		/**
		 * Called when a monitor changes its state. It is the responsibility
		 * of the observer to find out which observer was involved and
		 * what is the new state.
		 *
		 * @param aError System-wide error code if something failed
		 *				 when monitoring (or trying to monitor),
		 *				 KErrNone otherwise.
		 */
		virtual void MonitorStateChanged( TInt aError ) = 0;

	};

#endif // MSIPBEARERMONITOROBSERVER_H
