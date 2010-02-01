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
* Name          : CListener.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CLISTENER_H__
#define __CLISTENER_H__

#include <e32base.h>
#include <in_sock.h>
class MListenerOwner;


class CListener : public CActive
	{
public:
	static CListener* NewL(MListenerOwner& aContext,
						   RSocketServ& aServer,
						   TUint aPort);
	static CListener* NewLC(MListenerOwner& aContext,
							RSocketServ& aServer,
							TUint aPort);	

	static CListener* NewL(MListenerOwner& aContext,
						   RSocketServ& aServer,
						   TUint aPort,
						   RConnection& aConnection);
	static CListener* NewLC(MListenerOwner& aContext,
							RSocketServ& aServer,
							TUint aPort,
							RConnection& aConnection);

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	void Accept(RSocket& aSocket);
	~CListener();
private:
	CListener();
	void ConstructL(MListenerOwner& aContext,
					RSocketServ& aServer,
					TUint aPort);					
	void ConstructL(MListenerOwner& aContext,
					RSocketServ& aServer,
					TUint aPort,
					RConnection& aConnection);
					
	MListenerOwner* iContext;
	RSocket iListen;
	RSocketServ* iServer;
	TUint iPort;

	RConnection* iConnection;
	};
#endif // end of __CLISTENER_H__

// End of File
