/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipCSSubSessionReceiver.h
* Part of       : SIPServerCore
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSSUBSESSIONRECEIVER_H
#define CSIPCSSUBSESSIONRECEIVER_H

#include "CSipCSReceiverBase.h"
#include "MRefreshOwner.h"
#include "MRegistrationOwner.h"
#include "MDialogOwner.h"
#include "sipconnection.h"
#include "TSIPTransportParams.h"

class CSipCSServerITC;
class MSIPCSReceiverOwner;


class CSipCSSubSessionReceiver : public CSipCSReceiverBase,
                                 public MRefreshOwner,
                                 public MRegistrationOwner,
                                 public MDialogOwner
    {
public:
    
    static CSipCSSubSessionReceiver* NewL(
        CSipCSServerITC& aITC,
        MSIPSecUser& aSIPSecUser,
        MSIPCSReceiverOwner& aOwner);

    static CSipCSSubSessionReceiver* NewLC(
        CSipCSServerITC& aITC,
        MSIPSecUser& aSIPSecUser,
        MSIPCSReceiverOwner& aOwner);
  
    ~CSipCSSubSessionReceiver();

public: // From MTransactionOwner

    void ReceiveL(TUint32 aIapId,
                  TTransactionId aTransactionId,
                  CSIPRequest* aRequest);

    void ReceiveL(TTransactionId aTransactionId,
                  CSIPResponse* aResponse);                 
                   
public: // From MRefreshOwner and MDialogOwner

    TSIPTransportParams TransportParams(const TRegistrationId& aRegistrationId);

    const MSIPSecUser* SIPSecUser(const TRegistrationId& aRegistrationId);

public:    // From MRefreshOwner
    
    void RefreshReceivedL(TTransactionId aTransactionId,
                          TRefreshId aRefreshId,
                          CSIPResponse* aResponse);

    TInt RefreshError(TRefreshId aRefreshId,
                      TTransactionId aTransactionId,
                      TInt aReason);

    TInt NextRefreshCSeq(TUint& aCSeq);
    
    TUint RefreshDurationL();

public: // MRegistrationOwner
    
    void RegistrationReceivedL(TTransactionId aTransactionId,
                               TRefreshId aRefreshId,
                               TRegistrationId aRegistrationId,
                               CSIPResponse* aSIPResponse);
 
    TInt RegistrationError(TTransactionId aTransactionId,
                           TRegistrationId aRegistrationId,
                           TInt aReason);
                            
    const TSIPTransportParams& TransportParams() const;

public: // From MDialogOwner

    void DialogsReceivedL(TTransactionId aTransactionId,
                          TDialogId aDialogId,
                          CSIPRequest* aRequest);

    void DialogsReceivedL(TTransactionId aTransactionId,
                          CSIPResponse* aResponse);


    void DialogsReceivedL(TTransactionId aTransactionId,
                          TRefreshId aRefreshId,
                          TDialogId aDialogId,
                          CSIPResponse* aResponse);

    TInt DialogsError(TTransactionId aTransactionId,
                      TInt aReason);

    TInt DialogsError(TTransactionId aTransactionId,
                      TRefreshId aRefreshId,
                      TDialogId aDialogId,
                      TInt aReason);
    
    TInt InviteTransactionCompleted(TTransactionId aId); 

public: // New functions

    void ConnectionStateChangedL(CSIPConnection::TState aState);

private: // Second phase constructors

    CSipCSSubSessionReceiver(
        CSipCSServerITC& aITC,
        MSIPSecUser& aSIPSecUser,
        MSIPCSReceiverOwner& aOwner);

private: // Data

    MSIPCSReceiverOwner& iOwner;

private: // The unit test class is declared as a friend:
#ifdef CPPUNIT_TEST
    friend class CSipCSSubSessionReceiverTest;
    friend class CSipCSSubSessionTest;
#endif
    };

#endif // CSIPCSSUBSESSIONRECEIVER_H

// End of File
