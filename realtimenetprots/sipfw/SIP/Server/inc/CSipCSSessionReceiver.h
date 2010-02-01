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
* Name          : CSipCSSessionReceiver.h
* Part of       : SIPServerCore
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCSSESSIONRECEIVER_H
#define CSIPCSSESSIONRECEIVER_H

#include "CSipCSReceiverBase.h"
#include "MSIPSecUser.h"

class MSipCSSession;


class CSipCSSessionReceiver : public CSipCSReceiverBase,
                              public MSIPSecUser
	{
public: // Constructors and destructor 
    
    static CSipCSSessionReceiver* NewL(CSipCSServerITC& aITC,
                                       MSipCSSession& aSession);
                                       
    static CSipCSSessionReceiver* NewLC(CSipCSServerITC& aITC,
                                        MSipCSSession& aSession);
  
    ~CSipCSSessionReceiver();
    
public: // New functions
    
    void SetHttpDigestObserver(TSipHttpDigestObserverType aType);
    
    void MovePendingErrorsL(TUint32 aIapId,
                            CSipCSReceiverBase& aReceiver);

public: // From MTransactionOwner

    void ReceiveL(TUint32 aIapId,
                  TTransactionId aTransactionId,
                  CSIPRequest* aRequest);

	void ReceiveL(TTransactionId aTransactionId,
	              CSIPResponse* aResponse);
	              
	TInt TransactionEnded(TUint32 aIapId, 
	                      TTransactionId aTransactionId,
	                      TInt aReason);	              

public: // From MSIPSecUser

	TBool PassOnlyRealmsToUser() const;
	
    void RequestCredentialsL(const TDesC8& aRealm);
	
    void RequestCredentialsL(CSIPResponse& aResponse,
	                         TTransactionId aTransactionId,
	                         TRefreshId aRefreshId);
	                         
	const MSIPSecUser* TrustedUser(TRegistrationId aRegistrationId);                       
    
    TBool ByPassSIPSec() const;
    
private: // Constructors

    CSipCSSessionReceiver(CSipCSServerITC& aITC,
                          MSipCSSession& aSession);
    
private: // Data
    
    TSipHttpDigestObserverType iHttpDigestObserverType;
    MSipCSSession& iSession;
    RArray<TSipCSReceiveItem> iPendingErrorItems;

private: // For testing purposes

	UNIT_TEST(CSipCSSessionReceiverTest)
	UNIT_TEST(CSipCSSessionTest)
	};

#endif // CSIPCSSESSIONRECEIVER_H

// End of File
