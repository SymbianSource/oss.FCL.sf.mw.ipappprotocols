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
// Name          : Confirmed.cpp
// Part of       : SIPDialogs
// Version       : SIP/6.0 
//



#include "Confirmed.h"
#include "MDialogContext.h"
#include "TransactionItemBase.h"
#include "sipresponse.h"
#include "CSigCompMgr.h"
#include "sip.h"
#include "TSIPTransportParams.h"

_LIT8(KConfirmed, "Confirmed");

// -----------------------------------------------------------------------------
// TConfirmed::TConfirmed
// -----------------------------------------------------------------------------
//
TConfirmed::TConfirmed (MDialogContext& aDialog,
                        TDialogStateBase& aStateTerminated)
 : TDialogStateBase(aDialog),
   iStateTerminated(aStateTerminated)
	{
	}

// -----------------------------------------------------------------------------
// TConfirmed::Name
// -----------------------------------------------------------------------------
//
const TDesC8& TConfirmed::Name () const
	{
	return KConfirmed;
	}

// -----------------------------------------------------------------------------
// TConfirmed::SendL
// -----------------------------------------------------------------------------
//
void TConfirmed::SendL (TTransactionId& aTransactionId,
						CSIPRequest* aRequest,
						CTransactionItemBase& aTransactionItem,
						CURIContainer& aRemoteTarget)

	{
	// When a dialog is in Confirmed state, its route must be fixed. Using any
	// other registration id than KEmptyRegistrationId potentially causes
	// the transaction layer to add an outbound proxy to the route. To prevent
	// this, the registration id is forced to be KEmptyRegistrationId
	TSIPTransportParams params(iDialog.TransportParams(KEmptyRegistrationId));
	params.SetSkipNaptrAndSrv(iDialog.HasRouteSet());	
    aTransactionItem.SendL(params,aTransactionId,KEmptyRegistrationId,
                           aRequest,aRemoteTarget);
	}


// -----------------------------------------------------------------------------
// TConfirmed::SendL
// -----------------------------------------------------------------------------
//
void TConfirmed::SendL (CTransactionItemBase& aTransactionItem,
						CSIPResponse* aResponse) 
	{
	TSIPTransportParams params(iDialog.TransportParams(KEmptyRegistrationId));
    aTransactionItem.SendL(params,aResponse,EFalse);
	}

// -----------------------------------------------------------------------------
// TConfirmed::ReceiveL
// -----------------------------------------------------------------------------
//
void TConfirmed::ReceiveL (TTransactionId aTransactionId, 
                           CSIPRequest* aRequest)
	{
    PassToOwnerL (aTransactionId,aRequest);
	}

// -----------------------------------------------------------------------------
// TConfirmed::ReceiveL
// -----------------------------------------------------------------------------
//
void TConfirmed::ReceiveL (CTransactionItemBase& aTransactionItem, 
                           CSIPResponse* aResponse)
	{
    if (aResponse->Type() != CSIPResponse::E1XX &&
        aTransactionItem.OverlappingRestricted())
        {
        aTransactionItem.SetOverlappingRestricted(EFalse);
        }
    if (aResponse->Type() == CSIPResponse::E2XX &&
        aTransactionItem.TargetRefresh())
        {
        iDialog.SetRemoteTargetWithMsgL(*aResponse);
        }
    PassToOwnerL(aTransactionItem,aResponse);
    }

// -----------------------------------------------------------------------------
// TConfirmed::EndTransaction
// -----------------------------------------------------------------------------
//
void TConfirmed::EndTransaction (CTransactionItemBase& aTransactionItem, 
                                 TInt aReason)
	{
	if (aReason)
		{
		PassToOwner(aTransactionItem,aReason);
		}
	}

// -----------------------------------------------------------------------------
// TConfirmed::GetState
// -----------------------------------------------------------------------------
//
void TConfirmed::GetState (CSIPInternalStates::TState& aState)
    {
    aState = CSIPInternalStates::EDialogConfirmed;
    }
