/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofilecsserverclosetimer.h
* Part of     : SIP Profile Server
* Interface   : private
* The class for stopping server in startup
* if server not needed
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILECSSERVERCLOSETIMER_H
#define CSIPPROFILECSSERVERCLOSETIMER_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Class for stopping server in startup if server not needed.
 */
class CSipProfileCSServerCloseTimer : public CActive
    {
	public:

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSipProfileCSServerCloseTimer* NewL();

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSipProfileCSServerCloseTimer* NewLC();

		/**
		* Destructor
		*/
		~CSipProfileCSServerCloseTimer();

	    /**
        * Start shutdown timer
		* @param aSeconds time in seconds
        */
		void StopActiveSchedulerAfter(TUint aSeconds);

	    /**
        * Stop shutdown timer
        */
		void DoCancel();

	private:

	    /**
        * Default constructor
        */
		CSipProfileCSServerCloseTimer();

	    /**
        * Symbian 2nd phase constructor
        */
		void ConstructL ();

	    /**
        * Method executed when timeout occurs
        */
		void RunL();

	private:

		RTimer iTimer;
    };

#endif
