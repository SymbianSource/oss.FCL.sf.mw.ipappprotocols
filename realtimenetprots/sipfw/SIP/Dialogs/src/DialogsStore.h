/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : DialogsStore.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef CDIALOGSSTORE_H
#define CDIALOGSSTORE_H

#include <e32base.h>
#include <stringpool.h>
#include "CSipDialogItem.h"


class CDialogsStore: public CBase
	{
public:

	static CDialogsStore* NewL();
	
	void AddDialogItem(CSipDialogItem* aDialogItem);
	
	TDialogId NewDialogId();
	
	TInt Remove(CSipDialogItem* aDialogItem);
	
	TInt RemoveAllWith(const MDialogOwner* aDialogOwner);
	
	TInt RemoveAllEarlyDialogsWith(TTransactionId aTransactionId);
	
	CSipDialogItem* FindDialogItemForPass(
	    TTransactionId aTransactionId, 
	    CSIPResponse* aResponse);
	
	CSipDialogItem* FindDialogItem(const TDialogId& aDialogId);
	
	CSipDialogItem* FindDialogItem(
	    const TDialogId& aDialogId,
	    const MDialogOwner* aDialogOwner);
	
	CSipDialogItem* FindDialogItemByTransaction(
	    const TTransactionId& aTransactionId);
	
	CSipDialogItem* FindDialogItemByCallId (CSIPMessage* aMessage);
	
	CSipDialogItem* DialogItemForRequest (CSIPRequest* aRequest);
	
	void TransactionEnded (TTransactionId aTransactionId,
                           TInt aReason);
	
	void DeleteItemsWithThisIapId (TUint32 aIapId);

	~CDialogsStore();

private:
    
	CDialogsStore();
	TInt DialogItemsCount();
	
private: // Data

	TDialogId iDialogIdCounter;
	TSglQue<CSipDialogItem> iList;
	RStringF iTagParamName; 

private: // For testing purposes

	UNIT_TEST(CDialogStoreTest)
	};

#endif // CDIALOGSSTORE_H

// End of File
