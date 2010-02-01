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
* Name          : sipsystemstateobserver.h
* Part of       : SIP
* Version       : SIP/6.0 
*
*/



#ifndef MSIPSYSTEMSTATEOBSERVER_H
#define MSIPSYSTEMSTATEOBSERVER_H

// INCLUDES
#include "sipsystemstatemonitor.h"
#include <e32std.h>

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Callback to receive notifications about changes 
* in the values of system variables.
*/
class MSipSystemStateObserver
	{
	public:

		/**
		* The value of a system variable has changed.
		* @param aVariable the identifier of a system variable that has changed
        * @param aObjectId identifies the monitored object		
		* @param aValue the value of the variable
		*/
		virtual void SystemVariableUpdated( 
		    CSipSystemStateMonitor::TSystemVariable aVariable,
		    TInt aObjectId,
		    TInt aValue ) = 0;
	};

#endif // MSIPSYSTEMSTATEOBSERVER_H
