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
* Name          : multiaddrsenderowner.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MULTIADDRSENDEROWNER_H__
#define __MULTIADDRSENDEROWNER_H__

#include <in_sock.h>

class CSIPResponse;
class CMultipleAddrResSender;
class TSIPTransportParams;


class MMultiAddrSenderOwner
	{
public:
	virtual ~MMultiAddrSenderOwner() {}

	virtual void SendToNetL(const TSIPTransportParams& aParams,
	                        CSIPResponse& aResponse,
							const TInetAddr& aAddr,
							TRequestStatus& aStat) = 0;
	virtual TBool CancelRequest(TRequestStatus& aStat) = 0;
	virtual void Remove(CMultipleAddrResSender* aSender) = 0;
	};

#endif // end of __MULTIADDRSENDEROWNER_H__

// End of File
