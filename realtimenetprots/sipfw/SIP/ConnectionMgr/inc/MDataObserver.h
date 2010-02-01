/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MDataObserver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MDATAOBSERVER_H__
#define __MDATAOBSERVER_H__

#include <e32base.h>

class TInetAddr;

class MDataObserver
	{
public:
	virtual ~MDataObserver() {}

	virtual void IncomingDataL(HBufC8* aData, TInetAddr aRemoteAddr) = 0;
	};

#endif // end of __MDATAOBSERVER_H__

// End of File
