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
* Name          : MHandler.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MHANDLER_H__
#define __MHANDLER_H__

#include <e32base.h>

class CTransportBase;


class MHandler
	{
	public:	
		virtual void AddL(CTransportBase* aTransport) = 0;
	
		virtual void RemoveAndDelete(CTransportBase* aTransport) = 0;
	};
	
#endif // end of __MHANDLER_H__

// End of File
