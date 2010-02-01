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
// Name          : Establishing.cpp
// Part of       : SIPDialogs
// Version       : SIP/6.0 
//




#include "Establishing.h"
#include "MDialogContext.h"
#include "TransactionItemBase.h"
#include "siperr.h"
#include "SipAssert.h"
#include "MTransactionUser.h"
#include "MTransactionHeaders.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "CSigCompMgr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

_LIT8(KEstablishing, "Establishing");
const TUint K100Response = 100;


// -----------------------------------------------------------------------------
// TEstablishing::TEstablishing
// -----------------------------------------------------------------------------
//
TEstablishing::TEstablishing (MDialogContext& aDialog,
                              TDialogStateBase& aStateEarly,
	                          TDialogStateBase& aStateConfirmed,
	                          TDialogStateBase& aStateTerminated)
 : TDialogStateBase(aDialog),
   iTagParamName(SIPStrings::StringF(SipStrConsts::ETag)),
   iStateEarly(aStateEarly),
   iStateConfirmed(aStateConfirmed),
   iStateTerminated(aStateTerminated)
	{
	}

// -----------------------------------------------------------------------------
// TEstablishing::Name
// -----------------------------------------------------------------------------
//
const TDesC8& TEstablishing::Name () const
	{
	return KEstablishing;
	}
	
// -----------------------------------------------------------------------------
// TEstablishing::RequestsWithinDialogAllowed
// -----------------------------------------------------------------------------
//
TBool TEstablishing::RequestsWithinDialogAllowed () const
    {
    return EFalse; 
    }
    
// -----------------------------------------------------------------------------
//TEstablishing::Accept
// -----------------------------------------------------------------------------
//
TBool TEstablishing::Accept (CSIPRequest& aRequest) const
    {
    return (aRequest.Method() == SIPStrings::StringF(SipStrConsts::ENotify));
    }

// -----------------------------------------------------------------------------
// TEstablishing::SendL
// -----------------------------------------------------------------------------
//
void TEstablishing::SendL (TTransactionId& aTransactionId,
                           CSIPRequest* aRequest,
                           CTransactionItemBase& aTransactionItem,
                           CURIContainer& aRemoteTarget)

	{
	TSIPTransportParams params(
	    iDialog.TransportParams(iDialog.RegistrationId()));
	
	if (iDialog.CallId())
	    {
	    // The dialog state has been reseted, the data already exists
	    aRequest->From()->SetParamL(SIPStrings::StringF(SipStrConsts::ETag),
	                                iDialog.LocalTag());
	    iDialog.AddCallIdHeaderInDialogL(*aRequest);
	    iDialog.AddCSeqHeaderInDialogL(*aRequest); 
        aTransactionItem.SendL(params,aTransactionId,iDialog.RegistrationId(),
                               aRequest,aRemoteTarget);    
	    }
	else
	    {
    	MTransactionHeaders* transactionHeaders = 
            aTransactionItem.SendAndGetHeadersL(params,aTransactionId,
                                                iDialog.RegistrationId(),
                                                aRequest,aRemoteTarget);
     
        TCleanupItem cleanupItem(DeleteTransactionHeaders,transactionHeaders);
        CleanupStack::PushL(cleanupItem);

        __ASSERT_ALWAYS(aTransactionId != KEmptyTransactionId,
                        User::Leave(KErrSIPInvalidDialogRequest));
     
        // Local CSeq
        iDialog.SetLocalSeqNum(transactionHeaders->CSeqNumber());
        
        // Call-ID
    	CSIPCallIDHeader* callId = transactionHeaders->CallIDHeader();
        __ASSERT_ALWAYS(callId, User::Leave(KErrSIPInvalidDialogRequest));
        iDialog.SetCallId(callId);

        // Local tag
    	CSIPFromHeader* from = transactionHeaders->FromHeader();
        __ASSERT_ALWAYS(from, User::Leave(KErrSIPInvalidDialogRequest));
        CleanupStack::PushL(from);
        __ASSERT_ALWAYS(from->HasParam(iTagParamName),
                        User::Leave(KErrSIPInvalidDialogRequest));
    	iDialog.SetLocalTag(from->ParamValue(iTagParamName));
    	CleanupStack::PopAndDestroy(from);
    	
    	CleanupStack::PopAndDestroy(); // transactionHeaders
	    }
	}

// -----------------------------------------------------------------------------
// TEstablishing::SendL
// -----------------------------------------------------------------------------
//
void TEstablishing::SendL (CTransactionItemBase& aTransactionItem,
                           CSIPResponse* aResponse)
	{
	TSIPTransportParams params(iDialog.TransportParams(KEmptyRegistrationId));
	if (aResponse->Type() == CSIPResponse::E1XX)
		{
		iDialog.ChangeState(iStateEarly);
		}
	else		
		{
		iDialog.ChangeState(iStateConfirmed);
		}
    aTransactionItem.SendL(params,aResponse,ETrue);
	}

// -----------------------------------------------------------------------------
// TEstablishing::ReceiveL
// -----------------------------------------------------------------------------
//
void TEstablishing::ReceiveL (CTransactionItemBase& aTransactionItem, 
                              CSIPResponse* aResponse)
	{
    if (aResponse->IsErrorResponse())
		{
		TUint resCode = aResponse->ResponseCode();
		if( resCode == 494 || resCode == 401 || resCode == 407)
		    {
		    delete aResponse;
		    PassToOwner(aTransactionItem,KErrSIPInvalidDialogResponse);
		    }
		else
		    {
		    PassToOwnerL(aTransactionItem,aResponse);
		    }
		iDialog.ChangeState(iStateTerminated);
		}
    else if (aResponse->ResponseCode() == K100Response)
		{
		PassToOwnerL(aTransactionItem,aResponse);
		}
    else // 101-299 response
		{
		iDialog.SetRouteSetUsingRecordRouteL(
		    *aResponse, ETrue, KErrSIPInvalidDialogResponse); 
        iDialog.SetRemoteTargetWithMsgL(*aResponse);
		iDialog.SetRemoteTagL(aResponse->To());

        if (aResponse->Type() == CSIPResponse::E1XX) // Provisional response
			{
			PassToOwnerL(aTransactionItem,aResponse);
			iDialog.ChangeState(iStateEarly); 
			return;
			}

		if (aTransactionItem.OverlappingRestricted())
            {
			aTransactionItem.SetOverlappingRestricted(EFalse);
            }

		PassToOwnerL(aTransactionItem,aResponse);
		iDialog.ChangeState(iStateConfirmed);
		}
	}

// -----------------------------------------------------------------------------
// TEstablishing::ReceiveL
// -----------------------------------------------------------------------------
//
void TEstablishing::ReceiveL (TTransactionId aTransactionId, 
                              CSIPRequest* aRequest)
	{
    CSIPFromHeader* from = aRequest->From();
    __SIP_ASSERT_LEAVE (from != 0, KErrArgument);
	if (iDialog.RemoteTagSet())
		{
		__ASSERT_ALWAYS(iDialog.RemoteTag() == from->ParamValue(iTagParamName), 
                        User::Leave(KErrSIPInvalidDialogRequest));
		}
	else
        {
		iDialog.SetRemoteTagL(aRequest->From());
        }

	iDialog.SetRouteSetUsingRecordRouteL(
	    *aRequest, EFalse, KErrSIPInvalidDialogRequest);
	iDialog.SetRemoteTargetWithMsgL(*aRequest);	    
	    
    CSIPCSeqHeader* cseq = aRequest->CSeq();
    __SIP_ASSERT_LEAVE (cseq != 0, KErrArgument);
	
    iDialog.SetRemoteSeqNum(cseq->Seq());

    TBool isNotify = 
        (aRequest->Method() == SIPStrings::StringF(SipStrConsts::ENotify));
    
    PassToOwnerL(aTransactionId,aRequest);

    if (isNotify)
        {
        iDialog.ChangeState(iStateConfirmed);
        }
    }

// -----------------------------------------------------------------------------
// TEstablishing::EndTransaction
// -----------------------------------------------------------------------------
//
void TEstablishing::EndTransaction (CTransactionItemBase& aTransactionItem, 
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
// TEstablishing::DeleteTransactionHeaders
// -----------------------------------------------------------------------------
//
void TEstablishing::DeleteTransactionHeaders (TAny* aTransactionHeaders)
	{
	MTransactionHeaders* transactionHeaders = 
        reinterpret_cast<MTransactionHeaders*>(aTransactionHeaders);
    delete transactionHeaders;
	}

// -----------------------------------------------------------------------------
// TEstablishing::GetState
// -----------------------------------------------------------------------------
//
void TEstablishing::GetState (CSIPInternalStates::TState& aState)
    {
    aState = CSIPInternalStates::EDialogEstablishing;
    }

