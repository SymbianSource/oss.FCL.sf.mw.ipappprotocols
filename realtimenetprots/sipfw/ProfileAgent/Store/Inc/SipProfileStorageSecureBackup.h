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
* Name        : sipprofilestoragesecurebackup.h
* Part of     : SIP Profile Server
* Interface   : private
* The class for providing secure backup/restore
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILESTORAGESECUREBACKUP_H
#define CSIPPROFILESTORAGESECUREBACKUP_H

//  INCLUDES

#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS

class CSIPProfileServerCore;

// CLASS DECLARATION

/**
 * Class for subscribing events from secure backup server.
 */
class CSIPProfileStorageSecureBackup : public CActive
    {
	public:

	    /**
        * Two-phased constructor.
		* @return new instance
        */
		static CSIPProfileStorageSecureBackup* NewL(
			CSIPProfileServerCore* aObserver);

		/**
		* Destructor
		*/
		~CSIPProfileStorageSecureBackup();

	private:

	    /**
        * Default constructor
        */
		CSIPProfileStorageSecureBackup(CSIPProfileServerCore* aObserver);

	    /**
        * Symbian 2nd phase constructor
        */
		void ConstructL ();

	    /**
        * Method executed when backup property modified
        */
		void RunL();

	    /**
        * Stop subscribing backup property
        */
		void DoCancel();

	private:

		RProperty				iProperty;
		CSIPProfileServerCore*	iObserver;
		// ETrue if restore is being done
		TBool					iRestoreOngoing;
    };

#endif
