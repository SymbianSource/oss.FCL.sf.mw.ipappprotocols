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
* Name          : TResponseRoute.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __TRESPONSEROUTE_H__
#define __TRESPONSEROUTE_H__

#include "SipStackServerDefs.h"
#include <e32base.h>
#include <in_sock.h>


class TResponseRoute
	{
public:
	TResponseRoute(TTransactionId& aTAId, const TInetAddr& aAddr);
	TInetAddr& Address();
	TTransactionId& TAId();

private:
	TResponseRoute();
	TTransactionId iTAId;
	TInetAddr iAddr;
	};

#endif // end of __TRESPONSEROUTE_H__

// End of File
