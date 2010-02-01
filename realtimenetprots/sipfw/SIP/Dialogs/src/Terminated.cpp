// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : Terminated.cpp
// Part of       : SIPDialogs
// Version       : SIP/6.0 
//



#include "Terminated.h"
#include "MDialogContext.h"
#include "TransactionItemBase.h"
#include "siperr.h"
#include "sipinternalstates.h"

_LIT8(KTerminated, "Terminated");

// -----------------------------------------------------------------------------
// TTerminated::TTerminated
// -----------------------------------------------------------------------------
//
TTerminated::TTerminated (MDialogContext& aDialog)
 : TDialogStateBase(aDialog)
	{
	}

// -----------------------------------------------------------------------------
// TTerminated::Name
// -----------------------------------------------------------------------------
//
const TDesC8& TTerminated::Name () const
	{
	return KTerminated;
	}
	
// -----------------------------------------------------------------------------
// TTerminated::RequestsWithinDialogAllowed
// -----------------------------------------------------------------------------
//
TBool TTerminated::RequestsWithinDialogAllowed () const
    {
    return EFalse; 
    }		

// -----------------------------------------------------------------------------
// TTerminated::Accept
// -----------------------------------------------------------------------------
//
TBool TTerminated::Accept(CSIPRequest& /*aRequest*/) const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TTerminated::SendL
// -----------------------------------------------------------------------------
//    
void TTerminated::SendL (TTransactionId& /*aTransactionId*/,
                         CSIPRequest* /*aRequest*/,
                         CTransactionItemBase& /*aTransactionItem*/,
                         CURIContainer& /*aRemoteTarget*/)
    {
    User::Leave(KErrSIPInvalidDialogState);
    }

// -----------------------------------------------------------------------------
// TTerminated::SendL
// -----------------------------------------------------------------------------
//
void TTerminated::SendL (CTransactionItemBase& aTransactionItem,
						 CSIPResponse* aSipResponse) 
	{
	TSIPTransportParams params(iDialog.TransportParams(KEmptyRegistrationId));
    aTransactionItem.SendL(params,aSipResponse,EFalse);
	}

// -----------------------------------------------------------------------------
// TTerminated::ReceiveL
// -----------------------------------------------------------------------------
//
void TTerminated::ReceiveL (CTransactionItemBase& aTransactionItem,
                            CSIPResponse* aResponse)
	{
	PassToOwnerL(aTransactionItem,aResponse);
	}

// -----------------------------------------------------------------------------
// TTerminated::ReceiveL
// -----------------------------------------------------------------------------
//  
void TTerminated::ReceiveL (TTransactionId /*aTransactionId*/,
                            CSIPRequest* /*aRequest*/)
    {
    User::Leave(KErrSIPInvalidDialogState);
    }


// -----------------------------------------------------------------------------
// TTerminated::EndTransaction
// -----------------------------------------------------------------------------
//
void TTerminated::EndTransaction (CTransactionItemBase& aTransactionItem,
                                  TInt aReason)
	{ 
	if (aReason)
        {
		PassToOwner(aTransactionItem,aReason);
        }
	}

// -----------------------------------------------------------------------------
// TTerminated::GetState
// -----------------------------------------------------------------------------
//
void TTerminated::GetState (CSIPInternalStates::TState& aState)
    {
    aState = CSIPInternalStates::EDialogTerminated;
    }
