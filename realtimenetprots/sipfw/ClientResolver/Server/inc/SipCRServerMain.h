/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SipCRServerMain.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPCRSERVERMAIN_H
#define SIPCRSERVERMAIN_H

//  INCLUDES
#include <e32base.h>
#include "SipCRServer.pan"


// CLASS DECLARATION

/**
* This class is C/S server main class which starts server process
* launched by client's server starter.
* 
* @lib siprsvsrv.exe
*/

class SipCRServerMain
	{
public : 
    
    static TInt ThreadFunction (TAny* aNone);

private:

    SipCRServerMain ();

    static void PanicServer (TSipCRServerPanic aReason);
    static void ThreadFunctionL (RSemaphore& aSemaphore);
	};

#endif // end of SIPCRSERVERMAIN_H

// End of File
