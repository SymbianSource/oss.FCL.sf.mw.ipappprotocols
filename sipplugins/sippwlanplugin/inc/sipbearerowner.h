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

#ifndef MSIPBEAREROWNER_H
#define MSIPBEAREROWNER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CSIPBearerMonitor;

// CLASS DECLARATION
/**
* @publishedPartner
*
* MSIPBearerOwner defines an internal interface 
* for observing bearer monitor state changes.
*/
class MSIPBearerOwner
	{
	protected: // Constructors and destructors

		/// Virtual destructor. Prohibit deletion through this interface.
		virtual ~MSIPBearerOwner() {};

	public: // Abstract methods

		/**
		 * Called by a CSIPBearerMonitor implementation when its state changes
		 *
		 * @param aBearer "this" pointer of the reporting implementation.
		 * @param aError KErrNone if state was changed succesfully, otherwise
		 *				 one of the system-wide error codes.
		 */
		virtual void BearerStateChanged( CSIPBearerMonitor* aBearer,
										 TInt aError ) = 0;

	};

#endif // MSIPBEAREROWNER_H
