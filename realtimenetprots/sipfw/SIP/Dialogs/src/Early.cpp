// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : Early.cpp
// Part of       : SIPDialogs
// Version       : SIP/5.0 
//




#include "siperr.h"
#include "Early.h"
#include "MDialogContext.h"
#include "TransactionItemBase.h"
#include "SipAssert.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "CSigCompMgr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"

_LIT8(KEarly, "Early");
const TUint K100Response = 100;

// -----------------------------------------------------------------------------
// TEarly::TEarly
// -----------------------------------------------------------------------------
//
TEarly::TEarly (MDialogContext& aDialog,
                TDialogStateBase& aStateConfirmed,
	            TDialogStateBase& aStateTerminated)
 : TDialogStateBase(aDialog),
   iStateConfirmed(aStateConfirmed),
   iStateTerminated(aStateTerminated)
	{
	}

// -----------------------------------------------------------------------------
// TEarly::Name
// -----------------------------------------------------------------------------
//
const TDesC8& TEarly::Name () const
	{
	return KEarly;
	}

// -----------------------------------------------------------------------------
// TEarly::SendL
// -----------------------------------------------------------------------------
//
void TEarly::SendL (TTransactionId& aTransactionId,
                    CSIPRequest* aRequest,
                    CTransactionItemBase& aTransactionItem,
                    CURIContainer& aRemoteTarget)
	{
	TSIPTransportParams params(iDialog.TransportParams(KEmptyRegistrationId));
	params.SetSkipNaptrAndSrv(iDialog.HasRouteSet());
    aTransactionItem.SendL(params,aTransactionId,KEmptyRegistrationId,
                           aRequest,aRemoteTarget);
	}

// -----------------------------------------------------------------------------
// TEarly::SendL
// -----------------------------------------------------------------------------
//
void TEarly::SendL (CTransactionItemBase& aTransactionItem,
                    CSIPResponse* aResponse)
	{
	TSIPTransportParams params(iDialog.TransportParams(KEmptyRegistrationId));
	// Check that 2xx response is for correct request
    if(aResponse->Type() == CSIPResponse::E2XX &&
       iDialog.FirstTransactionId() == aTransactionItem.Id())
		{
		iDialog.ChangeState(iStateConfirmed);
		}
    aTransactionItem.SendL(params,aResponse,ETrue);
	}

// -----------------------------------------------------------------------------
// TEarly::ReceiveL
// -----------------------------------------------------------------------------
//
void TEarly::ReceiveL (CTransactionItemBase& aTransactionItem, 
                       CSIPResponse* aResponse)
	{
	if (aResponse->IsErrorResponse())
		{
        if (iDialog.FirstTransactionId() == aTransactionItem.Id())
            {
		    iDialog.TerminateForkedDialogs(aTransactionItem.Id());
		    iDialog.ChangeState(iStateTerminated);
            }
        }
    else // 100-299 response
        {
		if (aResponse->ResponseCode() > K100Response)
			{
			iDialog.SetRemoteTagL(aResponse->To());
			if (aTransactionItem.TargetRefresh())
				{
				// Only the final response to the initial transaction 
				// updates the route-set
				if (aResponse->Type() == CSIPResponse::E2XX && 
				    iDialog.FirstTransactionId() == aTransactionItem.Id())
					{
					iDialog.SetRouteSetUsingRecordRouteL(
					    *aResponse, ETrue, KErrSIPInvalidDialogResponse);
					}
				iDialog.SetRemoteTargetWithMsgL(*aResponse);
				}
            }

		if (aResponse->Type() == CSIPResponse::E2XX)
			{
		    if (aTransactionItem.OverlappingRestricted())
                {
			    aTransactionItem.SetOverlappingRestricted(EFalse);
                }
		    if (iDialog.FirstTransactionId() == aTransactionItem.Id())
                {
			    iDialog.ChangeState(iStateConfirmed);
                }
            }
		}
    PassToOwnerL(aTransactionItem,aResponse);
	}

// -----------------------------------------------------------------------------
// TEarly::ReceiveL
// -----------------------------------------------------------------------------
//
void TEarly::ReceiveL (TTransactionId aTransactionId, 
                       CSIPRequest* aRequest)
	{
    CSIPFromHeader* from = aRequest->From();
    __SIP_ASSERT_LEAVE (from != 0, KErrArgument);
	if (iDialog.RemoteTagSet())
		{
		RStringF tagParamName = SIPStrings::StringF(SipStrConsts::ETag);
		__ASSERT_ALWAYS(iDialog.RemoteTag() == from->ParamValue(tagParamName), 
                        User::Leave(KErrSIPInvalidDialogRequest));
		}
	else
        {
		iDialog.SetRemoteTagL(aRequest->From());
        }
    CSIPCSeqHeader* cseq = aRequest->CSeq();
    __SIP_ASSERT_LEAVE (cseq != 0, KErrArgument);
	iDialog.SetRemoteSeqNum(cseq->Seq());
	PassToOwnerL(aTransactionId,aRequest);
	}

// -----------------------------------------------------------------------------
// TEarly::EndTransaction
// -----------------------------------------------------------------------------
//
void TEarly::EndTransaction (CTransactionItemBase& aTransactionItem, 
                             TInt aReason)
	{
	if (aReason)
        {
		PassToOwner(aTransactionItem,aReason);
        }
	if (iDialog.FirstTransactionId() == aTransactionItem.Id())
		{
		iDialog.SelfDestruct();
		}
	}

// -----------------------------------------------------------------------------
// TEarly::GetState
// -----------------------------------------------------------------------------
//
void TEarly::GetState (CSIPInternalStates::TState& aState)
    {
    aState = CSIPInternalStates::EDialogEarly;
    }
