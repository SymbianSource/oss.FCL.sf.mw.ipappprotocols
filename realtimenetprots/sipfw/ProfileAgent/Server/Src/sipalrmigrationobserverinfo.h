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
* Name          : sipalrmigrationobserverinfo.h
* Part of       : SIP / SIP Profile Server
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPALRMIGRATIONOBSERVERINFO_H
#define TSIPALRMIGRATIONOBSERVERINFO_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSipAlrMigrationObserver;


// CLASS DECLARATION
/**
* Information about the observer, listening for IAP changes
*
* @lib sipsrofilesrv.exe
*/
class TSipAlrMigrationObserverInfo	
	{
	public: // Constructor

    	TSipAlrMigrationObserverInfo(MSipAlrMigrationObserver& aObserver);

	public: // New functions
	
		static TBool
			CompareObservers(const TSipAlrMigrationObserverInfo& aItem,
							 const TSipAlrMigrationObserverInfo& aItem2);

    public: // Data

		// Observer to receive notifications of new IAPs.
		// Not owned.
		MSipAlrMigrationObserver& iObserver;

		// ETrue if waiting migration permission from iObserver.
		TBool iIsWaitingForPermission;

		// ETrue if waiting information if the IAP works (NewIapAccepted or
		// NewIapRejected), from iObserver.
		TBool iIsWaitingForAcceptance;
    };

#endif // TSIPALRMIGRATIONOBSERVERINFO_H
