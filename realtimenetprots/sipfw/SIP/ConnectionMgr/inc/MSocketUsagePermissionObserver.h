/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSocketUsagePermissionObserver.h
* Part of       : ConnectionMgr
* See class definition below
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef MSOCKETUSAGEPERMISSIONOBSERVER_H
#define MSOCKETUSAGEPERMISSIONOBSERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RSocket;

// CLASS DEFINITION
/**
 *
 */
class MSocketUsagePermissionObserver
	{
	public: // Abstract methods

		virtual void UsagePermissionChanged( RSocket& aSocket, 
		                                     TBool aPermissionToUse ) = 0;

	};
	
#endif // MSOCKETUSAGEPERMISSIONOBSERVER_H

