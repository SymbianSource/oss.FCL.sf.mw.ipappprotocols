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
* Name          : CSipCSReceiverBase.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSRECEIVERBASE_H
#define CSIPCSRECEIVERBASE_H

#include <e32base.h>
#include "sipclientserver.h"
#include "MTransactionOwner.h"
#include "_sipcodecdefs.h"

class CSipCSReceiveQueue;
class CSipCSServerITC;
class CSIPMessage;
class MSIPSecUser;
class TSipCSReceiveItem;


class CSipCSReceiverBase : public CBase,
                           public MTransactionOwner
	{
public:
  
    ~CSipCSReceiverBase();

    void WriteSipMessageToClientL(const RMessage2& aMessage);

    void ClientReadyToReceiveL(const RMessage2& aMessage);
    
    void CancelClientReceiveL();
    
    void AddItemL(const TSipCSReceiveItem& aReceiveItem);

public: // From MTransactionOwner

	virtual TInt TransactionEnded(TUint32 aIapId, 
	                              TTransactionId aTransactionId,
	                              TInt aReason);

    TInt NextCSeq(TUint& aCSeq);

    const MSIPSecUser* SIPSecUser() const;

protected: // Constructors

    CSipCSReceiverBase(CSipCSServerITC& aITC, MSIPSecUser& aSIPSecUser);
    void ConstructL();

protected: // New functions

	void SipRequestReceivedL(TUint32 aIapId,
                             TTransactionId aTransactionId,
                             CSIPRequest* aRequest);
                  
    void SipMessageReceivedL(const TSIPIds& aIds,
                             CSIPMessage* aSipMessage);
    
    void HandleNewReceiveItemL(const TSipCSReceiveItem& aItem);    
    
    TInt ErrorReceived(const TSIPIds& aIds, TInt aError);

protected: // Data

    CSipCSServerITC&    iITC;

private: // Data

    MSIPSecUser&        iSIPSecUser;
    TBool               iClientReadyToReceive;
    CSipCSReceiveQueue* iReceiveQueue;
    RMessage2           iReceiveRMessage;    

private: // For testing purposes

	UNIT_TEST(CSipCSReceiverBaseTest)
	UNIT_TEST(CSipCSSessionTest)
	};

#endif // CSIPCSRECEIVERBASE_H

// End of File
