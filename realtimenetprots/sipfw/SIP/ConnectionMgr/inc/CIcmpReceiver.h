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
* Name          : CIcmpReceiver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __CICMPRECEIVER_H__
#define __CICMPRECEIVER_H__

#include <e32base.h>

// Forward declarations
class MIcmpErrorObserver;
class RSocketServ;
class CIcmpV4Receiver;
class CIcmpV6Receiver;


class CIcmpReceiver : public CBase
	{
public:
	static CIcmpReceiver* NewL(MIcmpErrorObserver& aObserver,
							   RSocketServ& aServer);
	~CIcmpReceiver();

private:
	CIcmpReceiver();
	void ConstructL(MIcmpErrorObserver& aObserver, RSocketServ& aServer);

	//data
	CIcmpV4Receiver* iIcmpV4Receiver;
	CIcmpV6Receiver* iIcmpV6Receiver;
	};


#endif // end of __CICMPRECEIVER_H__

// End of File
