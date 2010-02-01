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
* Name          : SipServerMain.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef SIPSERVERMAIN_H
#define SIPSERVERMAIN_H

#include <e32base.h>
#include "SipCSServer.pan"


class SipServerMain
	{
public : 
    
    static TInt ThreadFunction (TAny* aNone);

private:

    SipServerMain ();

    static void PanicServer (TSipCSServerPanic aReason);
    static void ThreadFunctionL (RSemaphore& aSemaphore);
	};

#endif // SIPSERVERMAIN_H

// End of File
