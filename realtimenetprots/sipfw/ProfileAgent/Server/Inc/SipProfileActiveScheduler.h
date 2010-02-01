/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofileactivescheduler.h
* Part of     : SIP Profile Server
* Interface   : private
* The class provides scheduling for profile server
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEACTIVESCHEDULER_H
#define CSIPPROFILEACTIVESCHEDULER_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * Class provides scheduler for profile server
 */
class CSIPProfileActiveScheduler: public CActiveScheduler
    {
	public:

		/**
		* Default constructor
		*/
		CSIPProfileActiveScheduler();

		/**
		* Destructor
		*/
		~CSIPProfileActiveScheduler();

	public: // From CActiveScheduler
		
		/**
		* Error occurred while scheduling
		* @param aError error code
		*/
		void Error(TInt aError) const;
    };

#endif
