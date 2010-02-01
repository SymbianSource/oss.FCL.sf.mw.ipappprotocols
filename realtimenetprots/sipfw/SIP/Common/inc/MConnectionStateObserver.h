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
* Name          : MConnectionStateObserver.h
* Part of       : ConnectionMgr
* Version       : SIP/3.0 
*
*/




/**
 @internalComponent
*/


#ifndef __MCONNECTIONSTATEOBSERVER_H__
#define __MCONNECTIONSTATEOBSERVER_H__

#include "sipconnection.h"

class MConnectionStateObserver
	{
public:

    virtual ~MConnectionStateObserver() { }

	virtual void ConnectionStateChangedL (TUint32 aIapId,
                                          CSIPConnection::TState aState) = 0;
	};



#endif // end of __MCONNECTIONSTATEOBSERVER_H__

// End of File
