/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  See class definition below.
*
*/

#ifndef MSIPNETWORKOBSERVER_H
#define MSIPNETWORKOBSERVER_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
* @publishedPartner
*
* MSIPNetworkObserver defines an interface for observing network state changes.
*/
class MSIPNetworkObserver
	{
	public: // Enumerations

		/// Network states
		enum TNetworkState
			{
			ENetworkUnknown,
			ENetworkConnected,
			ENetworkInactive,
			ENetworkActive,
			ENetworkSuspended,
			EConnectionLost
			};

	protected: // Constructors and destructors

		/// Virtual destructor. Prohibit deletion through this interface.
		virtual ~MSIPNetworkObserver() {};

	public: // Abstract methods

		/**
		 * Called by a when the network state has changed.
		 *
		 * @param aState Current network state
		 * @param aError KErrNone if state was changed succesfully, otherwise
		 *				 one of the system-wide error codes.
		 */
		virtual void NetworkStateChanged( 
		    MSIPNetworkObserver::TNetworkState aState,
		    TInt aError ) = 0;
	};

#endif // MSIPNETWORKOBSERVER_H
