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
* Name          : CSipCSServer.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSSERVER_H
#define CSIPCSSERVER_H

#include <e32base.h>
#include "SipCSServer.pan"
#include "sipconnection.h"

class CSipServerCore;
class MTransactionOwner;


class CSipCSServer : public CPolicyServer
	{
public : 
    
    static CSipCSServer* NewL(CSipServerCore& aServerCore);
    static CSipCSServer* NewLC(CSipServerCore& aServerCore);
    ~CSipCSServer();

	void ConnectionStateChangedL(TUint32 aIapId, 
                                 CSIPConnection::TState aState);

	MTransactionOwner* FindOwner(TUint32 aIapId, const TUid& aUid);

protected: // From CActive

	TInt RunError(TInt aError);

protected: // From CServer2

    CSession2* NewSessionL(const TVersion &aVersion,
                           const RMessage2& aMessage) const;

protected: // From CPolicyServer

    TCustomResult CustomSecurityCheckL(const RMessage2 &aMsg,
                                       TInt &aAction,
                                       TSecurityInfo &aMissing);

private: // Constructors

    CSipCSServer(TInt aPriority, CSipServerCore& aServerCore);
    void ConstructL();

private: // Data

	CSipServerCore& iServerCore;
	TSecurityInfo iSecurityInfo;
	};

#endif // CSIPCSSERVER_H

// End of File
