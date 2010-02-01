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
* Name        : sipprofileserverstarter.h
* Part of     : SIP Profile Client
* Interface   : private
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef TSIPPROFILESERVERSTARTER_H
#define TSIPPROFILESERVERSTARTER_H

//  INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  The class for starting profile server if not yet running.
*
*  @lib sipprofilecli.lib
*/
class TSIPProfileServerStarter
    {
	public:

	    /**
        * Start the profile server if not already running.
		* @return KErrNone if started
        */
		static TInt Start();

	private:

	    /**
        * Start the profile server.
		* @return KErrNone if started
        */
		static TInt CreateServerProcess();
    };

#endif
