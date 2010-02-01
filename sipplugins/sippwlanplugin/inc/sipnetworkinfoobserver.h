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

#ifndef MSIPNETWORKINFOOBSERVER_H
#define MSIPNETWORKINFOOBSERVER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class TSIPAccessNetworkInfo;

// CLASS DEFINITION
/**
 * MSIPNetworkInfoObserver defines an interface for observing access network
 * information and its changes.
 */
class MSIPNetworkInfoObserver
	{
	protected: // Constructors and destructors

		/// Virtual destructor. Prohibit deletion through this interface.
		virtual ~MSIPNetworkInfoObserver() {}

	public: // Abstract methods

		/**
		 * Called when new access network information is available.
		 *		 
		 * @param aInfo Access network info
		 */
		virtual void InfoChanged( const TSIPAccessNetworkInfo& aInfo ) = 0;
	};

#endif // MSIPNETWORKINFOOBSERVER_H
