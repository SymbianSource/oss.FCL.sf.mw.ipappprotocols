// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : DialogStateBase.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//



#include "DialogStateBase.h"
#include "MDialogContext.h"
#include "MDialogOwner.h"
#include "TransactionItem.h"
#include "RefreshItem.h"
#include "sip.h"
#include "sipresponse.h"

// -----------------------------------------------------------------------------
// TDialogStateBase::TDialogStateBase
// -----------------------------------------------------------------------------
//
TDialogStateBase::TDialogStateBase(MDialogContext& aDialog)
 : iDialog(aDialog)
    {
    }
    
// -----------------------------------------------------------------------------
// TDialogStateBase::~TDialogStateBase
// -----------------------------------------------------------------------------
//
TDialogStateBase::~TDialogStateBase()
    {
    }   

// -----------------------------------------------------------------------------
// TDialogStateBase::RequestsWithinDialogAllowed
// -----------------------------------------------------------------------------
//
TBool TDialogStateBase::RequestsWithinDialogAllowed () const
    {
    return ETrue; 
    }

// -----------------------------------------------------------------------------
// TDialogStateBase::Accept
// -----------------------------------------------------------------------------
//
TBool TDialogStateBase::Accept(CSIPRequest& /*aRequest*/) const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// TDialogStateBase::PassToOwnerL
// -----------------------------------------------------------------------------
// 
void TDialogStateBase::PassToOwnerL (CTransactionItemBase& aTransactionItem,
                                     CSIPResponse* aResponse)
    {
    MDialogOwner* owner = iDialog.DialogOwner();
    TDialogId dialogId = iDialog.DialogId();
    TTransactionId transactionId = aTransactionItem.Id();
    TRefreshId refreshId = KEmptyRefreshId;
    if (aTransactionItem.IsRefreshed())
        {
        refreshId = static_cast<CRefreshItem&>(aTransactionItem).RefreshId();
        if (aTransactionItem.Terminate() && 
            aResponse->Type() != CSIPResponse::E1XX)
            {
            iDialog.RemoveTransactionItem(transactionId);
			owner->DialogsReceivedL(transactionId,refreshId,dialogId,aResponse);
			return;
            }
        }
	if (aTransactionItem.PassResponseToOwner())
		{
		TUint responseCode = aResponse->ResponseCode();
		owner->DialogsReceivedL(transactionId,refreshId,dialogId,aResponse);
		aTransactionItem.ResponseReceived(responseCode);
		}
	else
		{
		delete aResponse;
		}
    }
    
// -----------------------------------------------------------------------------
// TDialogStateBase::PassToOwnerL
// -----------------------------------------------------------------------------
// 
void TDialogStateBase::PassToOwnerL (TTransactionId aTransactionId,
                                     CSIPRequest* aRequest)
    {
    TDialogId dialogId = iDialog.DialogId();
    iDialog.DialogOwner()->DialogsReceivedL(aTransactionId,dialogId,aRequest);
    }

// -----------------------------------------------------------------------------
// TDialogStateBase::PassToOwner
// -----------------------------------------------------------------------------
// 
TInt TDialogStateBase::PassToOwner (CTransactionItemBase& aTransactionItem,
                                    TInt aErr)
    {
    MDialogOwner* owner = iDialog.DialogOwner();
    TDialogId dialogId = iDialog.DialogId();
    TTransactionId transactionId = aTransactionItem.Id();
    TRefreshId refreshId = KEmptyRefreshId;
    if (aTransactionItem.IsRefreshed())
        {
        refreshId = static_cast<CRefreshItem&>(aTransactionItem).RefreshId();
        }
    return owner->DialogsError(transactionId,refreshId,dialogId,aErr);
    }

// -----------------------------------------------------------------------------
// TDialogStateBase::PassToOwnerL
// -----------------------------------------------------------------------------
// 
void TDialogStateBase::PassToOwnerL (CTransactionItemBase& aTransactionItem,
                                     TInt aErr)
    {
    TInt err = PassToOwner(aTransactionItem,aErr);
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    }
