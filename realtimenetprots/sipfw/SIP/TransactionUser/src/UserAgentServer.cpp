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
// Name          : UserAgentServer.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "siperr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "siptoheader.h"
#include "sipmaxforwardsheader.h"
#include "uricontainer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MSipRegistrations.h"
#include "MSipDialogs.h"
#include "MSIPRequestRouter.h"
#include "Transmitter.h"

#include "UserAgentServer.h"
#include "UserAgentState.h"
#include "UserAgentTimer.h"
#include "TimerValues.h"
#include "CTransactionStore.h"
#include "SIPMessageUtility.h"


// -----------------------------------------------------------------------------
// CUserAgentServer::CUserAgentServer
// -----------------------------------------------------------------------------
//
CUserAgentServer::CUserAgentServer(CUserAgentCreateParams& aParams,
                                   MSipDialogs& aDialogs,
                                   MSIPRequestRouter& aRouter) :
	CUserAgent(aParams),
    iDialogs(aDialogs),
    iRouter(aRouter)
	{
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::~CUserAgentServer
// If transaction exists or InviteUAS sent a 2xx, inform ConnectionMgr to stop
// using the responses.
// -----------------------------------------------------------------------------
//
CUserAgentServer::~CUserAgentServer()
	{
	CancelGetOwnerRequest();
	iToTag.Close();

	if (iTransmitter)
		{
        iTransmitter->CancelSendResponses(CUserAgent::TransactionId(), ETrue);        
		}
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::IsUAS
// -----------------------------------------------------------------------------
//
TBool CUserAgentServer::IsUAS() const
	{
	__TEST_INVARIANT;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::RegistrationId
// UAS doesn't have a registration ID.
// -----------------------------------------------------------------------------
//
TRegistrationId CUserAgentServer::RegistrationId() const
	{
	__TEST_INVARIANT;

	return KEmptyRegistrationId;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::OwnerFoundL
// -----------------------------------------------------------------------------
//
void
CUserAgentServer::OwnerFoundL(TUint32 aRequestId, MTransactionOwner* aOwner)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aOwner && aRequestId == iRouterRequestId, KErrArgument);

	if (!HasStopped())
		{				
		State().OwnerFoundL(*this, aOwner);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::OwnerNotFoundL
// -----------------------------------------------------------------------------
//
void CUserAgentServer::OwnerNotFoundL(TUint32 aRequestId, CSIPResponse* aResp)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResp && aRequestId == iRouterRequestId, KErrArgument);
	
	if (HasStopped())
		{
		delete aResp;
		}
	else
		{
		State().OwnerNotFoundL(*this, aResp);
		}	

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgentServer::ErrorOccurred
// -----------------------------------------------------------------------------
//
void CUserAgentServer::ErrorOccurred(TUint32 aRequestId, TInt aError)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRequestId == iRouterRequestId, KErrArgument);

	StopTimerOwnerResolver();
	if (aError == KErrNoMemory)
		{
		Stop(aError);
		}
	else
		{
		TRAPD(err, RequestRouterErrorL());
		if (err != KErrNone)
			{
			Stop(aError);
			}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::RequestRouterErrorL
// -----------------------------------------------------------------------------
//
void CUserAgentServer::RequestRouterErrorL()
    {
    __TEST_INVARIANT;

	if (!HasStopped())
		{
		State().RequestRouterErrorL(*this);
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgentServer::InitialRequestReceivedL
// -----------------------------------------------------------------------------
//
void CUserAgentServer::InitialRequestReceivedL(CSIPRequest* aReq,
									  const CUserAgentState& aGetTxOwner,
									  const CUserAgentState& aWaitRespFromApp,
									  const CUserAgentState& aErrorRespSent)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aReq, KErrArgument);

	if (!UpdateTransportProtocol(*aReq))
		{
		delete aReq;
		Stop(KErrNone);
		return;
		}

    StoreToTag(*aReq);

	TInt responseCode = 0;
	RStringF reasonPhrase;
	if (CheckReceivedRequest(*aReq, responseCode, reasonPhrase))
		{
		iTransactionStore.StoreContactHeadersL(TransactionId(), *aReq);        

		if (DoesDialogExistForRequestL(*aReq))
			{
			//Enter next state before passing request. If upper layer sends
			//response synchronously in PassMsgToTransactionOwnerL, UAS is in a
			//correct state to handle it.
			ChangeState(aWaitRespFromApp);
			PassMsgToTransactionOwnerL(aReq);
			}
		else
			{
			RequestOutsideDialogL(aReq, aGetTxOwner, aErrorRespSent);
			}
		}
	else
		{
		SendErrorResponseL(responseCode, reasonPhrase, aErrorRespSent);
		delete aReq;
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::CheckReceivedRequest
// If no To-tag, compare request to ongoing transactions to see if it is a
// merged request.
// -----------------------------------------------------------------------------
//
TBool CUserAgentServer::CheckReceivedRequest(CSIPRequest& aReq,
											 TInt& aResponseCode,
											 RStringF& aReasonPhrase) const
	{
	__TEST_INVARIANT;
    __ASSERT_ALWAYS(aReq.To() != NULL,
					User::Panic(_L("UAS:ChkRcvReq to"), KErrArgument));

	if (aReq.HasHeader(SIPStrings::StringF(SipStrConsts::EMaxForwardsHeader)))
		{
		TSglQueIter<CSIPHeaderBase> iter =
            aReq.Headers(SIPStrings::StringF(SipStrConsts::EMaxForwardsHeader));
		CSIPMaxForwardsHeader& maxForw =
            static_cast<CSIPMaxForwardsHeader&>(*iter);

		if (maxForw.Value() == 0)
			{
			aReasonPhrase =
				SIPStrings::StringF(SipStrConsts::EPhraseTooManyHops);
			aResponseCode = 483;
			return EFalse;
			}
        }

	if (!aReq.To()->HasParam(SIPStrings::StringF(SipStrConsts::ETag)) &&
		iTransactionStore.IsMergedRequest(*this, aReq))
		{
		aReasonPhrase =
				SIPStrings::StringF(SipStrConsts::EPhraseLoopDetected);		
		aResponseCode = 482;
		return EFalse;
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::DoesDialogExistForRequestL
// -----------------------------------------------------------------------------
//
TBool CUserAgentServer::DoesDialogExistForRequestL(CSIPRequest& aReq)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iObserver && !iOutgoingMsg, KErrAlreadyExists);

	CSIPResponse* resp = NULL;
	iObserver = iDialogs.TransactionOwnerL(aReq, &resp);

    if (!iObserver && resp)
		{
		iOutgoingMsg = resp;
		}

    __TEST_INVARIANT;
    return iObserver != NULL;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::RequestOutsideDialogL
// If registration subsystem does not recognize the Request-URI, a 404 is sent.
// Record-Routes are only stored from requests outside dialog
// -----------------------------------------------------------------------------
//
void
CUserAgentServer::RequestOutsideDialogL(CSIPRequest* aReq,
										const CUserAgentState& aGetTxOwner,
										const CUserAgentState& aErrorRespSent)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aReq, KErrArgument);

	if (iOutgoingMsg)
		{
		HandleResponseFromDialogsL(aErrorRespSent);
		}
	else
		{		
		CURIContainer* uri = aReq->RequestURI();			
		if (uri && iRegistrations.CheckRequestURI(*uri))
			{
			TRAPD(err, iRouterRequestId =
		        iRouter.TransactionOwnerL(*aReq,
										  TransportParams().IapId(),
									      *this));
			if (err == KErrNone)
				{
	            StartTimerOwnerResolverL();
				iTransactionStore.StoreRecordRouteHeadersL(TransactionId(),
	                	                                   *aReq);
				__SIP_ASSERT_LEAVE(!iIncomingMsg, KErrAlreadyExists);
				iIncomingMsg = aReq;

				ChangeState(aGetTxOwner);
				return; //aReq is not deleted
				}

			SendErrorResponseL(603,
						   SIPStrings::StringF(SipStrConsts::EPhraseDecline),
						   aErrorRespSent);
			}
		else
			{
			SendErrorResponseL(404,
							SIPStrings::StringF(SipStrConsts::EPhraseNotFound),
							aErrorRespSent);
			}
	    }

	delete aReq;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::SendErrorResponseL
// -----------------------------------------------------------------------------
//
void CUserAgentServer::SendErrorResponseL(TInt aResponseCode,
										  RStringF aReasonPhrase,
										  const CUserAgentState& aErrorRespSent)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResponseCode >= 300, KErrArgument);

	CSIPResponse* resp = CSIPResponse::NewLC(aResponseCode, aReasonPhrase);
	FillResponseL(*resp);

	ChangeState(aErrorRespSent);
	SendResponseToTransactionL(resp);
	CleanupStack::Pop(resp);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::CheckResponseL
// Via headers should not be present yet. UAS copies them from the request.
// -----------------------------------------------------------------------------
//
void CUserAgentServer::CheckResponseL(const CSIPResponse& aResp) const
	{
	__TEST_INVARIANT;

    if (aResp.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader)))
        {
        User::Leave(KErrSIPMalformedMessage);
        }
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::FillResponseL
// If the To-header doesn't yet have a tag, it's added unless the response is
// 100. If a tag has already been generated, it is used for all responses.
// -----------------------------------------------------------------------------
//
void CUserAgentServer::FillResponseL(CSIPResponse& aResp)
	{
	__TEST_INVARIANT;

	iTransactionStore.CopyHeadersToResponseL(TransactionId(), aResp);
    CheckContactHeadersL(aResp, aResp.To());

	RStringF tag = SIPStrings::StringF(SipStrConsts::ETag);

	if (aResp.ResponseCode() > 100)
		{
        CSIPToHeader* to = aResp.To();
        __SIP_ASSERT_LEAVE(to != NULL, KErrArgument);

        if (iToTag.DesC().Length() > 0)
            {
            if (!to->HasParam(tag))
                {
                to->SetParamL(tag, iToTag);
                }
            }
        else
            {
            if (!to->HasParam(tag))
                {
                //Generate the tag after CopyHeadersToResponseL so SIP headers
                //can be used as random input for tag.
				AddTagL(*to);
                }

			StoreToTag(aResp);

			//Put the tag to store. Routing of an ACK needs it.
			iTransactionStore.UpdateToTagL(TransactionId(), iToTag);
            }
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::StoreToTag
// -----------------------------------------------------------------------------
//
void CUserAgentServer::StoreToTag(CSIPMessage& aMsg)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(iToTag.DesC().Length() == 0,
    				User::Panic(_L("StoreToTag exist"), KErrAlreadyExists));

    CSIPToHeader* to = aMsg.To();
    __ASSERT_ALWAYS(to != NULL,
    				User::Panic(_L("StoreToTag !to"), KErrArgument));

    if (to->HasParam(SIPStrings::StringF(SipStrConsts::ETag)))
        {
        iToTag.Close();
		iToTag = to->ParamValue(SIPStrings::StringF(SipStrConsts::ETag)).Copy();
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgentServer::SendResponseToTransactionL
// -----------------------------------------------------------------------------
//
void CUserAgentServer::SendResponseToTransactionL(CSIPResponse* aResp) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iTransaction, KErrNotFound);
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);

	iTransaction->SendResponseL(aResp, iTransportProtocol, TransportParams());
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::HandleResponseFromDialogsL
// -----------------------------------------------------------------------------
//
void
CUserAgentServer::HandleResponseFromDialogsL(const CUserAgentState& aNextState)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

	CSIPResponse* resp = static_cast<CSIPResponse*>(iOutgoingMsg);
    __SIP_ASSERT_LEAVE(resp->IsErrorResponse(), KErrSIPMalformedMessage);

	FillResponseL(*resp);

	ChangeState(aNextState);
	SendResponseToTransactionL(resp);
	iOutgoingMsg = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::HandleOwnerFoundL
// Enter next state before passing request to MTransactionOwner. If upper layer
// sends response synchronously in MTransactionOwner::ReceiveL, UAS must be in a
// correct state to handle it.
// -----------------------------------------------------------------------------
//
void
CUserAgentServer::HandleOwnerFoundL(MTransactionOwner* aOwner,
								    const CUserAgentState& aWaitRespFromApp)
	{
	__TEST_INVARIANT;
	//Observer can be set only once by MSIPRequestRouter. But when application
	//sends a response, it can change MTransactionOwner with every SendL call.
	__SIP_ASSERT_LEAVE(!iObserver, KErrAlreadyExists);
	__SIP_ASSERT_LEAVE(aOwner, KErrArgument);

	iObserver = aOwner;

    StopTimerOwnerResolver();

	ChangeState(aWaitRespFromApp);
	PassStoredRequestToTransactionOwnerL();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::HandleOwnerNotFoundL
// Response can not be 2xx for INVITE since it has different state in the
// InviteUAS than the error responses.
// -----------------------------------------------------------------------------
//
void
CUserAgentServer::HandleOwnerNotFoundL(CSIPResponse* aResp,
 									   const CUserAgentState& aFinalRespSent)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iIncomingMsg, KErrNotFound);	
	__SIP_ASSERT_LEAVE(aResp && aResp->ResponseCode() >= 200, KErrArgument);

	if (static_cast<CSIPRequest*>(iIncomingMsg)->Method() ==
		SIPStrings::StringF(SipStrConsts::EInvite))
		{
		__SIP_ASSERT_LEAVE(aResp->ResponseCode() >= 300, KErrArgument);
		}

    StopTimerOwnerResolver();
	CheckResponseL(*aResp);
	FillResponseL(*aResp);
	ChangeState(aFinalRespSent);
	SendResponseToTransactionL(aResp);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::HandleRequestRouterErrorL
// -----------------------------------------------------------------------------
//
void CUserAgentServer::HandleRequestRouterErrorL(
									const CUserAgentState& aErrorRespSent)
    {
    __TEST_INVARIANT;    

    CancelGetOwnerRequest();
    SendErrorResponseL(603,
    				   SIPStrings::StringF(SipStrConsts::EPhraseDecline),
    				   aErrorRespSent);
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgentServer::HandleSendResponseL
// Free stored Record-Route and Contact headers when a final response is sent.
// Always keep the TransportId from original TSIPTransportParams.
// -----------------------------------------------------------------------------
//
void CUserAgentServer::HandleSendResponseL(CSIPResponse* aResp,
										   const TSIPTransportParams& aParams)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);

	CheckResponseL(*aResp);
	FillResponseL(*aResp);

	if (CSIPMessageUtility::IsFinalResponse(*aResp))
        {
	    iTransactionStore.FreeRecordRouteHeaders(TransactionId());
	    iTransactionStore.FreeContactHeaders(TransactionId());
        }

	TSIPTransportParams newParams(aParams);
	newParams.SetTransportId(iTransportParams.TransportId());
	iTransportParams = newParams;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::StartTimerOwnerResolverL
// Timer duration is selected to be shorter than timer F2, so if this timer
// expires, the server transaction still exists and a response can be sent.
// -----------------------------------------------------------------------------
//
void CUserAgentServer::StartTimerOwnerResolverL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iTimerOwnerResolver, KErrAlreadyExists);

	iTimerOwnerResolver =
        CTimerOwnerResolver::NewL(iTimers,
								  this,
                                  (iTimerValues.Duration64xT1() / 10) * 9);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::StopTimerOwnerResolver
// -----------------------------------------------------------------------------
//
void CUserAgentServer::StopTimerOwnerResolver()
	{
	__TEST_INVARIANT;

	delete iTimerOwnerResolver;
	iTimerOwnerResolver = NULL;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::CancelGetOwnerRequest
// -----------------------------------------------------------------------------
//
void CUserAgentServer::CancelGetOwnerRequest() const
	{
	__TEST_INVARIANT;    

	iRouter.Cancel(iRouterRequestId);
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::PassStoredRequestToTransactionOwnerL
// iIncomingMsg is not passed as parameter to PassMsgToTransactionOwnerL(),
// since iIncomingMsg would be set to NULL only after PassMsgToTransactionOwnerL
// returns. If upper layer deletes the request, accessing iIncomingMsg causes
// crash in TU's invariants.
// -----------------------------------------------------------------------------
//
void CUserAgentServer::PassStoredRequestToTransactionOwnerL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iIncomingMsg, KErrNotFound);

	CSIPMessage* msg = iIncomingMsg;
	iIncomingMsg = NULL;
	CleanupStack::PushL(msg);
	PassMsgToTransactionOwnerL(msg);
	CleanupStack::Pop(msg);
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::ToTag
// -----------------------------------------------------------------------------
//
RStringF CUserAgentServer::ToTag() const
	{
	__TEST_INVARIANT;
	return iToTag;
	}

// -----------------------------------------------------------------------------
// CUserAgentServer::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CUserAgentServer::__DbgTestInvariant() const
	{
	//For CCancelUAS, iOwnerResolver is always NULL
	if ((iIncomingMsg && !iIncomingMsg->IsRequest()) ||
		(iOutgoingMsg && iOutgoingMsg->IsRequest()) ||
		!iOwnsOutgoingMsg)
		{
		User::Invariant();
		}
	}

