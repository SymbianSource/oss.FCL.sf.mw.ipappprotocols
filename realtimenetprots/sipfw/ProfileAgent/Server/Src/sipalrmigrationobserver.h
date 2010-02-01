/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipalrmigrationobserver.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPALRMIGRATIONOBSERVER_H
#define MSIPALRMIGRATIONOBSERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* Callback implemented by the users of CSipAlrMigrationController
*
* @lib sipsrofilesrv.exe
*/
class MSipAlrMigrationObserver
	{
	public:
		/**
		* A new IAP has became available.
		* A class implementing MSipAlrMigrationObserver must not ignore this
		* callback. It must either use
		* CSipAlrMigrationController::SetMigrationPermission, or forward the
		* IapAvailable event to client.
		* @param aSnapId a SNAP id.
		* @param aNewIapId an IAP id. 
		*/
		virtual void IapAvailable(TUint32 aSnapId, TUint32 aNewIapId) = 0;

		/**
        * The overall result to migrating to aIapId is allowed.
        * @param aIapId IAP id
        */
        virtual void MigrationIsAllowedL(TUint32 aIapId) = 0;

		/**
        * The overall result to migrating to aIapId is disallowed.
        * @param aIapId IAP id
        */
        virtual void MigrationIsDisallowedL(TUint32 aIapId) = 0;

		/**
        * Leave occurred from one of the callbacks of MSipAlrMigrationObserver.       
        * @param aError Leave code
        */
        virtual void ErrorOccurred(TInt aError) = 0;

        /**
        * No new IAP available in the SNAP
        */
        virtual void NoNewIapAvailable() = 0;

		/**
        * Refreshing IAPs of the SNAP failed.
        */
        virtual void RefreshIAPsFailed() = 0;

		/**
        * The user has rejected the offered IAP
        */
        virtual void OfferedIapRejected() = 0;
    };

#endif // MSIPALRMIGRATIONOBSERVER_H
