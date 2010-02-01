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
* Name          : TransactionItemStore.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CTRANSACTIONITEMSTORE_H
#define CTRANSACTIONITEMSTORE_H

#include <e32base.h>
#include <stringpool.h>
#include "TransactionItemBase.h"  

class CRefreshItem;


class CTransactionItemStore : public CBase
	{
public:

    static CTransactionItemStore* NewL ();
    static CTransactionItemStore* NewLC ();
    ~CTransactionItemStore();

    void AddItem (CTransactionItemBase* aTransactionItem);
    TBool HasItem (TTransactionId aTransactionId);
    CTransactionItemBase* FindItem (TTransactionId aTransactionId);
	TInt RemoveItem (TTransactionId aTransactionId);

    TBool OverlappingRestricted (RStringF aMethod);

    // CRefreshItem related functions:
    TBool HasRefreshedItems ();
    CRefreshItem* FindRefreshItem (RStringF aMethod);
    CRefreshItem* FindRefreshItem (TRefreshId aRefreshId);
    RArray<TRefreshId> RefreshItemIdsL ();
    TRefreshId RefreshId (TTransactionId aTransactionId);

private:
      
	CTransactionItemStore ();

    // data
   	TSglQue<CTransactionItemBase> iList;
	};

#endif // CTRANSACTIONITEMSTORE_H

// End of File



