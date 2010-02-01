/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipCSSession.h
* Part of       : SIPServerCore
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCSSESSION_H
#define CSIPCSSESSION_H

#include "MConnectionStateObserver.h"
#include "MSipCSSession.h"
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class CSipServerCore;
class CSipCSSessionReceiver;
class CSipCSServerITC;
class MTransactionOwner;


class CSipCSSession : public CSession2,
                      public MConnectionStateObserver,
                      public MSipCSSession
	{
public: // Constructors and destructor

    static CSipCSSession* NewL(CSipServerCore& aServerCore);
    static CSipCSSession* NewLC(CSipServerCore& aServerCore);

    ~CSipCSSession();

public: // New functions

    CSipCSServerITC& ITC();
    CSipServerCore& ServerCore();
    TUid Uid();

public: // From MSipCSSession

    MTransactionOwner& FindOwner(TUint32 aIapId);
    const MSIPSecUser* SIPSecUser(const TRegistrationId& aRegistrationId);
    TBool ByPassSIPSec() const;

public: // From CSession2
    
    void ServiceL (const RMessage2& aMessage);

public: // From MConnectionStateObserver

	void ConnectionStateChangedL (TUint32 aIapId,
                                  CSIPConnection::TState aState);

private:

    CSipCSSession (CSipServerCore& aServerCore);

    void ConstructL ();

    void DoServiceL (TInt aFunction, const RMessage2& aMessage);

    // Supported services:
    void SetClientUidL (const RMessage2& aMessage);
    void SupportedSecurityMechanismsL (const RMessage2& aMessage);
    void NegotiatedSecurityMechanismL (const RMessage2& aMessage);
    void SetHttpDigestObserverL (const RMessage2& aMessage);
	void SetCredentialsL (const RMessage2& aMessage);
	void RemoveCredentialsL (const RMessage2& aMessage);
	void IgnoreChallengeL(const RMessage2& aMessage);
	void SetSecurityHandlingL(const RMessage2& aMessage);    
    void SigCompSupportedL ();
    void GetTransactionStateL (const RMessage2& aMessage);
    void GetDialogStateL (const RMessage2& aMessage);
    void GetCompartmentCountL (const RMessage2& aMessage);
    void OpenSubSessionL (TUint32 aIapId, const RMessage2& aMessage);
    void CloseSubSession (TInt aSubSessionHandle);
    void ReceiveSipMessageL (const RMessage2& aMessage);
    void ClientReadyToReceiveL (const RMessage2& aMessage);
    void CancelClientReceiveL ();
    void CallSubSessionL (TInt aSubSessionHandle, const RMessage2& aMessage);

    // helper functions:
    TBool Exists (TUint32 aIapId);

    const MSIPSecUser* FindSIPSecUser(
        TRefreshId aRefreshId, 
        TTransactionId aTransactionId);

    // Credentials enumeration
    enum TCredentialIndexes
        {
        EUserIndex=0,
        EPasswdIndex,
        EOutboundProxyIndex    
        };

private: // Data

    CSipServerCore&        iServerCore;
    TUid                   iClientUid;
    CSipCSSessionReceiver* iReceiver;
	CObjectCon*            iSubSessions;
	CObjectIx*             iSubSessionIndex;
	TBool                  iByPassSIPSec;

private: // For testing purposes

	UNIT_TEST(CSipCSSessionTest)
    UNIT_TEST(CSipCSSubSessionTest)
	};

#endif // CSIPCSSESSION_H

// End of File
