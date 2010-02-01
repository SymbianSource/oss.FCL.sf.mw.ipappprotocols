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
* Name          : CReceiverTcp.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CRECEIVERTCP_H
#define CRECEIVERTCP_H

#include <e32base.h>
#include <in_sock.h>
#include "MSIPMsgAssemblerOwner.h"

class MContext;
class CSIPMsgAssembler;


class CReceiverTcp : public CActive, public MSIPMsgAssemblerOwner
	{
	
public:

    enum TState
        {
        EIdle,
        EConnecting,
        EConnected,
        EShuttingDown
        };
        
public:

	static CReceiverTcp* NewL(MContext& aContext);
	static CReceiverTcp* NewLC(MContext& aContext);
	
	~CReceiverTcp();
	
public: // From CActive

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

public: // From MSIPMsgAssemblerOwner

    void MessageCompleteL(CSIPMessage* aSIPMessage,
						  TInt aError,
						  TBool aCompressed);
						  
public:

	void ReceiveDataL();
	void Connect(TInetAddr& aRemoteAddr);
    void Shutdown();
	
	TInetAddr iAddr;	
	
private:

	void ConstructL();
	CReceiverTcp(MContext& aContext);

private:

	void RemoveReceived();
	
private: // Data

	MContext& iContext;
	TPtr8 iReceivedMsgPtr;
	
	HBufC8* iReceivedMsg;	
	
	TState iReceiverState;
	TSockXfrLength iSockLenght;
	TUint iNextLength;
	CSIPMsgAssembler* iMsgAssembler;
	
	};
#endif // end of CRECEIVERTCP_H

// End of File
