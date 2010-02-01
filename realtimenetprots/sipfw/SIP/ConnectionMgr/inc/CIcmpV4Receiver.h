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
* Name          : CIcmpV4Receiver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __CICMPV4RECEIVER_H__
#define __CICMPV4RECEIVER_H__

#include <e32base.h>
#include <in_sock.h>


// Forward declarations
class MIcmpErrorObserver;


class CIcmpV4Receiver : public CActive
	{
public:
	static CIcmpV4Receiver* NewL(MIcmpErrorObserver& aObserver,
								 RSocketServ& aServer);
	void RunL();
	void DoCancel();
	~CIcmpV4Receiver();

private:
	CIcmpV4Receiver();
	void Receive();
	void ConstructL(MIcmpErrorObserver& aObserver, RSocketServ& aServer);
	void CheckError();

	//data
	RSocket iSocket;
	RSocketServ* iServer;
	
	//Buffer where data is received from the socket
	TBuf8<64> iData;
	TInetAddr iAddress;
	MIcmpErrorObserver* iObserver;

	RConnection* iConnection;

private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CIcmprecvTest;
#endif
	};


#endif // end of __CICMPV4RECEIVER_H__

// End of File
