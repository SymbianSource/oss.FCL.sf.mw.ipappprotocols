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
* Name          : TransactionItem.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CTRANSACTIONITEM_H
#define CTRANSACTIONITEM_H

#include "TransactionItemBase.h"
class MTransactionUser;
class MTransactionOwner;

class CTransactionItem : public CTransactionItemBase
	{
public:

    static CTransactionItem* NewL (MTransactionUser& aTU,
                                   MTransactionOwner* aTransactionOwner,
                                   TTransactionId aTransactionId);

    static CTransactionItem* NewLC (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    TTransactionId aTransactionId);

    static CTransactionItem* NewL (MTransactionUser& aTU,
                                   MTransactionOwner* aTransactionOwner,
                                   RStringF aMethod);

    static CTransactionItem* NewLC (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    RStringF aMethod);

    static CTransactionItem* NewL (MTransactionUser& aTU,
                                   MTransactionOwner* aTransactionOwner,
                                   TTransactionId aTransactionId,
                                   RStringF aMethod);

    static CTransactionItem* NewLC (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    TTransactionId aTransactionId,
                                    RStringF aMethod);

    static CTransactionItem* NewL (MTransactionUser& aTU,
                                   MTransactionOwner* aTransactionOwner,
                                   const CTransactionItemBase& aItem);

    static CTransactionItem* NewLC (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    const CTransactionItemBase& aItem);
	
    ~CTransactionItem ();

public: // From CTransactionItemBase

    TBool IsRefreshed() const;

	void ResponseReceived (TUint aResponseCode);

    MTransactionHeaders* SendAndGetHeadersL (
        const TSIPTransportParams& aTransportParams,
        TTransactionId& aTransactionId,
        TRegistrationId aRegistrationId,
        CSIPRequest* aRequest,
        CURIContainer& aRemoteTarget);

    void SendL (const TSIPTransportParams& aTransportParams,
                TTransactionId& aTransactionId,
                TRegistrationId aRegistrationId,
                CSIPRequest* aRequest,
                CURIContainer& aRemoteTarget);
   
    void SendL (const TSIPTransportParams& aTransportParams,
                CSIPResponse* aResponse,
                TBool aAddOwner);

private: // Second phase constructors

    CTransactionItem (MTransactionUser& aTU,
                      MTransactionOwner* aTransactionOwner,
                      const CTransactionItemBase& aItem);

    CTransactionItem (MTransactionUser& aTU,
                      MTransactionOwner* aTransactionOwner,
                      TTransactionId aTransactionId);
                      
    CTransactionItem (MTransactionUser& aTU,
                      MTransactionOwner* aTransactionOwner,
                      TTransactionId aTransactionId,
                      RStringF aMethod);                      
                      
private: // Data 

    MTransactionUser& iTU;
    MTransactionOwner* iTransactionOwner;
	};

#endif // CTRANSACTIONITEM_H

// End of File
