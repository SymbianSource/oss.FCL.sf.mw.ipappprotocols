/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef MWLANCONNUSAGEPERMISSIONOBSERVER_H
#define MWLANCONNUSAGEPERMISSIONOBSERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
 * Callback interface for receiving connection usage permission change events.
 *
 */
class MWLanConnUsagePermissionObserver
	{
	public: // Abstract methods

		virtual void UsagePermissionChanged( TBool aPermissionToUse, 
		                                     TInt aError ) = 0;

	};
	
#endif // MWLANCONNUSAGEPERMISSIONOBSERVER_H

