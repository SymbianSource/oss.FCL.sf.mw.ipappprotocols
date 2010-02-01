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
// Name          : UserAgent.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0 
//



#include "siperr.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "siphostport.h"
#include "sipaddress.h"
#include "sipfromtoheaderbase.h"
#include "sipcontactheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SipLogs.h"
#include "DeleteMgr.h"
#include "Transmitter.h"

#include "MSipRegistrationContact.h"
#include "MTransactionOwner.h"
#include "CUserAgent.h"
#include "UserAgentState.h"
#include "UserAgentCreateParams.h"
#include "CTransactionStore.h"
#include "SIPMessageUtility.h"
#include "MTransactionUser.h"


// -----------------------------------------------------------------------------
// CUserAgent::CUserAgent
// aObserver is NULL for UAS
// -----------------------------------------------------------------------------
//
CUserAgent::CUserAgent(const CUserAgentCreateParams& aParams) :	
	iObserver(aParams.iObserver),	
	iTimers(aParams.iTimers),
	iRegistrations(aParams.iRegistrations),
    iRegistrationContact(aParams.iRegistrationContact),
    iSigComp(aParams.iSigComp),
	iTransactionStore(aParams.iTransactionStore),
	iTransactionMgr(aParams.iTransactionMgr),	
	iTransportProtocol(SIPStrings::StringF(SipStrConsts::EUDP)),
	iTransportParams(aParams.iTransportParams),
	iTimerValues(aParams.iTimerValues),
	iOwnsOutgoingMsg(aParams.iOwnsOutgoingMsg),
	iDeleteMgr(aParams.iDeleteMgr),
	iSIPMsgUtility(aParams.iSIPMsgUtility),    
	iState(aParams.iInitialUaState),
	iTransactionId(aParams.iTransactionId)
	{
	__SIP_ASSERT_RETURN(iTransactionId != KEmptyTransactionId, KErrArgument);
    __SIP_ASSERT_RETURN(iState != NULL, KErrArgument);
	}

// -----------------------------------------------------------------------------
// CUserAgent::ConstructL
// -----------------------------------------------------------------------------
//
void CUserAgent::ConstructL(MSipConnectionMgr& aConnectionMgr)
	{
	iTransmitter = CTransmitter::NewL(aConnectionMgr);
	}

// -----------------------------------------------------------------------------
// CUserAgent::~CUserAgent
// -----------------------------------------------------------------------------
//
CUserAgent::~CUserAgent()
	{
	delete iTransmitter;
	delete iTransaction;

	if (iOwnsOutgoingMsg)
		{
		delete iOutgoingMsg;
		}

	delete iIncomingMsg;
	iTransportProtocol.Close();	
	}

// -----------------------------------------------------------------------------
// CUserAgent::TransportParams
// -----------------------------------------------------------------------------
//
const TSIPTransportParams& CUserAgent::TransportParams() const
    {
    __TEST_INVARIANT;
    return iTransportParams;
    }

// -----------------------------------------------------------------------------
// CUserAgent::CleanupSilently
// -----------------------------------------------------------------------------
//
void CUserAgent::CleanupSilently(TAny* aUserAgent)
	{
	__SIP_ASSERT_RETURN(aUserAgent, KErrArgument);

	CUserAgent* ua = reinterpret_cast<CUserAgent*>(aUserAgent);
	ua->ClearTransactionOwner(); // Prevent MTransactionOwner::TransactionEnded
	ua->Stop(KErrGeneral); 		 // End silently, error code doesn't matter
	}

// -----------------------------------------------------------------------------
// CUserAgent::SendRequestL
// -----------------------------------------------------------------------------
//
void CUserAgent::SendRequestL(CSIPRequest* aReq,
							  TRegistrationId aRegisterId,
							  const CURIContainer& aRemoteTarget)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aReq, KErrArgument);
    __ASSERT_ALWAYS(!iStopped, User::Leave(KErrSIPInvalidTransactionState));

	iState->SendRequestL(*this, aReq, aRegisterId, aRemoteTarget);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::SendResponseL
// -----------------------------------------------------------------------------
//
void CUserAgent::SendResponseL(CSIPResponse* aResp,
							   const TSIPTransportParams& aParams)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __ASSERT_ALWAYS(!iStopped, User::Leave(KErrSIPInvalidTransactionState));
	
    iState->SendResponseL(*this, aResp, aParams);	

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::SendCancelL
// -----------------------------------------------------------------------------
//
void CUserAgent::SendCancelL(TTransactionId aInviteTaId)
	{
	__TEST_INVARIANT;
    __ASSERT_ALWAYS(!iStopped, User::Leave(KErrSIPInvalidTransactionState));

	iState->SendCancelL(*this, aInviteTaId);
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::TransactionOwner
// -----------------------------------------------------------------------------
//
const MTransactionOwner* CUserAgent::TransactionOwner() const
	{
	__TEST_INVARIANT;
	return iObserver;
	}

// -----------------------------------------------------------------------------
// CUserAgent::ClearTransactionOwner
// Detach iOutgoingMsg unless owned, as the upper layer can delete it.
// -----------------------------------------------------------------------------
//
void CUserAgent::ClearTransactionOwner()
	{
	__TEST_INVARIANT;

	iObserver = NULL;

	if (!iOwnsOutgoingMsg)
        {
        DetachOutgoingMsg();
        }

	if (iState->ShouldUASStop())
		{
		Stop(KErrNone);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::DetachOutgoingMsg
// No action.
// -----------------------------------------------------------------------------
//
void CUserAgent::DetachOutgoingMsg()
	{
	}

// -----------------------------------------------------------------------------
// CUserAgent::ReceiveL
// Write request to log, if it came directly from ConnectionMgr.
// -----------------------------------------------------------------------------
//
void CUserAgent::ReceiveL(CSIPRequest* aRequest)
    {
    __TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);

	if (iStopped)
		{
		delete aRequest;
		}
	else
        {
		if (!iTransaction)
			{
			__SIP_MESSAGE_LOG("TransactionUser", *aRequest)
			}

		iState->ReceiveL(*this, aRequest);
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgent::ReceiveL
// Write response to log, if it came directly from ConnectionMgr.
// -----------------------------------------------------------------------------
//
void CUserAgent::ReceiveL(CSIPResponse* aResponse)
    {
    __TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aResponse, KErrArgument);

	if (iStopped)
		{
		delete aResponse;
		}
	else
		{
		if (!iTransaction)
			{
			__SIP_MESSAGE_LOG("TransactionUser", *aResponse)
			}

		iState->ReceiveL(*this, aResponse);
		}

	__TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgent::LeaveOccurred
// If Dialog drops a response, it leaves with KErrSIPInvalidDialogResponse.
// -----------------------------------------------------------------------------
//
void CUserAgent::LeaveOccurred(TInt aReason)
	{
	__TEST_INVARIANT;
	
	__SIP_INT_LOG1( "CUser Agent :: LeaveOccured with reason", 
   			                aReason )

	if (aReason != KErrSIPInvalidDialogResponse)
		{
		Stop(aReason);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::SendCompleteL
// -----------------------------------------------------------------------------
//
void CUserAgent::SendCompleteL()
	{
	__TEST_INVARIANT;

	if (!iStopped)
		{
		iState->SendCompleteL(*this);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::SendFailedL
// -----------------------------------------------------------------------------
//
void CUserAgent::SendFailedL(TInt aError)
	{
	__TEST_INVARIANT;
	__SIP_INT_LOG1( "CUserAgent Sending failed with Error",
					aError )

	if (!iStopped)
        {
        iState->SendFailedL(*this, aError);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::LeaveFromTransmitter
// -----------------------------------------------------------------------------
//
void CUserAgent::LeaveFromTransmitter(TInt aReason)
	{
	__TEST_INVARIANT;
	__ASSERT_DEBUG(aReason != KErrNone,
				   User::Panic(_L("UA:LeaveFromTransmitter"), KErrArgument));
	Stop(aReason);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CUserAgent::TimerExpiredL(TTimerId aTimerId, TAny* aTimerParam)
	{
	__TEST_INVARIANT;

	if (!iStopped)
		{
		iState->TimerExpiredL(*this, aTimerId, aTimerParam);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::DeleteTimer
// The concrete UA must implement DeleteTimer.
// -----------------------------------------------------------------------------
//
void CUserAgent::DeleteTimer(const CUserAgentTimer& /*aTimer*/) 
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_RETURN(EFalse, KErrGeneral);
	}

// -----------------------------------------------------------------------------
// CUserAgent::StoreOutgoingMsg
// -----------------------------------------------------------------------------
//
void CUserAgent::StoreOutgoingMsg(CSIPMessage* aMsg)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(aMsg, KErrArgument);

	if (iOwnsOutgoingMsg)
		{
		delete iOutgoingMsg;
		}

	iOutgoingMsg = aMsg;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::IsInviteUAC
// -----------------------------------------------------------------------------
//
TBool CUserAgent::IsInviteUAC() const
	{
	__TEST_INVARIANT;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgent::IsSubInviteUAC
// Don't call __TEST_INVARIANT here to avoid recursion
// -----------------------------------------------------------------------------
//
TBool CUserAgent::IsSubInviteUAC() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgent::State
// -----------------------------------------------------------------------------
//
const CUserAgentState& CUserAgent::State() const
    {
    __TEST_INVARIANT;
    return *iState;
    }

// -----------------------------------------------------------------------------
// CUserAgent::ChangeState
// -----------------------------------------------------------------------------
//
void CUserAgent::ChangeState(const CUserAgentState& aNewState)
	{
	__TEST_INVARIANT;

	iState = &aNewState;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::IsResolving
// -----------------------------------------------------------------------------
//
TBool CUserAgent::IsResolving() const
	{
	__TEST_INVARIANT;
	return iState->IsResolving(); 
	}
	
// -----------------------------------------------------------------------------
// CUserAgent::UpdateTransactionOwner
// -----------------------------------------------------------------------------
//
void CUserAgent::UpdateTransactionOwner(MTransactionOwner* aNewObserver)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(aNewObserver, KErrArgument);

	iObserver = aNewObserver;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::TransactionEndsL
// -----------------------------------------------------------------------------
//
void CUserAgent::TransactionEndsL(TInt aReason)
	{
	__TEST_INVARIANT;

	iState->TransactionEndsL(*this, aReason);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::PassMsgToTransactionOwnerL
// -----------------------------------------------------------------------------
//
void CUserAgent::PassMsgToTransactionOwnerL(CSIPMessage* aMsg) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aMsg, KErrArgument);

	if (iObserver)
		{
		__SIP_INT_LOG1( "TU passes SIP message to owner, transaction ID", 
		                 iTransactionId )

		if (aMsg->IsRequest())
			{
			CSIPRequest* request = static_cast<CSIPRequest*>(aMsg);

			__SIP_DES8_LOG( "TU SIP request method", 
			                request->Method().DesC() )
			iObserver->ReceiveL(iTransportParams.IapId(),
                                iTransactionId,
								request);
			}
		else
			{
			CSIPResponse* response = static_cast<CSIPResponse*>(aMsg);

			__SIP_INT_LOG1( "TU SIP response code", 
			                response->ResponseCode() )
			iObserver->ReceiveL(iTransactionId, response);
			}
		}
	else
        {
    	__SIP_INT_LOG1(
    	    "TU msg not passed to owner, iObserver==NULL, transaction ID",
    	    iTransactionId )

		delete aMsg;
		}
	}

// -----------------------------------------------------------------------------
// CUserAgent::UpdateTransportProtocol
// -----------------------------------------------------------------------------
//
TBool CUserAgent::UpdateTransportProtocol(CSIPMessage& aMsg)
	{
	__TEST_INVARIANT;
	return CSIPMessageUtility::TransportProtocol(aMsg, iTransportProtocol);
	}

// -----------------------------------------------------------------------------
// CUserAgent::AddTagL
// -----------------------------------------------------------------------------
//
void CUserAgent::AddTagL(CSIPFromToHeaderBase& aToFromHeader) const
	{
	__TEST_INVARIANT;

	TBuf8<MTransactionUser::KTagLength> tagBuf;
	AddRandomStringL(tagBuf, MTransactionUser::KTagLength, EFalse);
	
	RStringF tag = SIPStrings::Pool().OpenFStringL(tagBuf);
	CleanupClosePushL(tag);
	aToFromHeader.SetParamL(SIPStrings::StringF(SipStrConsts::ETag), tag);
	CleanupStack::PopAndDestroy(); // tag
	}

// -----------------------------------------------------------------------------
// CUserAgent::AddRandomStringL
// -----------------------------------------------------------------------------
//
void CUserAgent::AddRandomStringL(TDes8& aBuf,
								  TInt aLength,
								  TBool aCaseSensitive) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aLength <= aBuf.MaxLength() - aBuf.Length(),
                       KErrArgument);

	iSIPMsgUtility.AddRandomStringL(aBuf,
									aLength,
									aCaseSensitive,
									iOutgoingMsg,
									iTransactionId,
									this);
	}

// -----------------------------------------------------------------------------
// CUserAgent::IcmpErrorL
// -----------------------------------------------------------------------------
//
void CUserAgent::IcmpErrorL(const TInetAddr& aAddress,
							CSipConnectionMgr::TICMPError aError)
	{
	__TEST_INVARIANT;

	if (!iStopped)
		{
		iState->IcmpErrorL(*this, aAddress, aError);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::CheckContactHeadersL
// -----------------------------------------------------------------------------
//
void CUserAgent::CheckContactHeadersL(CSIPMessage& aMsg,
                              const CSIPFromToHeaderBase* aFromToHeader) const
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aFromToHeader != NULL, KErrArgument);

	const RStringF KContact = SIPStrings::StringF(SipStrConsts::EContactHeader);
	if (aMsg.HasHeader(KContact))
		{
		TSglQueIter<CSIPHeaderBase> iter = aMsg.Headers(KContact);
		for (CSIPHeaderBase* header = iter++; header; header = iter++)
			{
			CSIPContactHeader* contact =
				static_cast<CSIPContactHeader*>(header);
			if (!contact->Star())
                {
                __SIP_ASSERT_LEAVE(contact->SIPAddress(), KErrArgument);
				if (contact->SIPAddress()->URI().IsSIPURI())
					{
					CSIPURI* sipUri = contact->SIPAddress()->URI().SIPURI();
					if (sipUri->HostPort().Host().CompareF(
						SIPStrings::StringF(SipStrConsts::ELocalHost).DesC())
						== 0)
						{
						iRegistrationContact.ContactL(iTransportParams.IapId(),
                                    RegistrationId(),
                                    aFromToHeader,
						            static_cast<CSIPContactHeader&>(*header));
						}
	                CheckSigCompL(contact->SIPAddress()->URI());
					}
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CUserAgent::RemoveFromStore
// -----------------------------------------------------------------------------
//
void CUserAgent::RemoveFromStore() const
	{
	__TEST_INVARIANT;

	iTransactionStore.Remove(iTransactionId);
	}

// -----------------------------------------------------------------------------
// CUserAgent::RequestDeletionOfTransactionL
// Leave if AddDeleteRequest fails. Don't change TransactionId, to avoid using
// RequestDeletionOfUserAgent after changing id. Changing id twice fails causing
// "delete this" and call stack likely has UA/transaction code, causing a crash.
// -----------------------------------------------------------------------------
//
void CUserAgent::RequestDeletionOfTransactionL()
    {
    __TEST_INVARIANT;

	if (iTransaction)
		{
		User::LeaveIfError(iDeleteMgr.AddDeleteRequest(iTransaction));		
		iTransaction = NULL;
		iTransactionStore.ClearTransaction(iTransactionId);
		}

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgent::RequestDeletionOfUserAgent
// -----------------------------------------------------------------------------
//
TInt CUserAgent::RequestDeletionOfUserAgent()
	{
	__TEST_INVARIANT;

    TInt status = iDeleteMgr.AddDeleteRequest(this);
	if (status == KErrNone)
        {
        // DeleteMgr took UA's ownership
		RemoveFromStore();
		}
	else
		{
        // Other errors mean e.g. trying to delete object twice
        if (status == KErrNoMemory)
            {
			// Set id empty, to be freed later. Fails if UA isn't stored, but it
			// occurs only in CTransactionUser::SendL/SendAndGetHeadersL/
			// SendCancelL so UA isn't in call stack, and can "delete this".
			status = iTransactionStore.UpdateTransactionId(iTransactionId,
				            						       KEmptyTransactionId);
            if (status != KErrNone)
                {
                delete this;
                return KErrDied; // Skip __TEST_INVARIANT
                }
            }
        }

	__TEST_INVARIANT;
	return status;
	}

// -----------------------------------------------------------------------------
// CUserAgent::Stop
// "sub InviteUAC" doesn't use observer or request its own deletion.
// -----------------------------------------------------------------------------
//
void CUserAgent::Stop(TInt aReason)
	{
	__TEST_INVARIANT;

	if (!iStopped)
		{
		iStopped = ETrue;

		if (!IsSubInviteUAC())
			{
			if (iObserver)
				{
				if (IsInviteUAC() && IsCanceled() && aReason == KErrTimedOut)
					{
					aReason = KErrCancel;
					}
				__SIP_INT_LOG2( "TU TransactionEnded (reason, transaction ID)",
				                aReason, iTransactionId )
				iObserver->TransactionEnded(iTransportParams.IapId(), 
				                            iTransactionId,
				                            aReason);
				}
			else
				{
				__SIP_INT_LOG1( "TU UA:Stop(), iObserver==NULL, transaction ID",
				                iTransactionId )
				}

            if (RequestDeletionOfUserAgent() == KErrDied)
                {
                // Skip __TEST_INVARIANT as UA was deleted synchronously
                return;
                }
			}
		}

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgent::TransactionId
// -----------------------------------------------------------------------------
//
TTransactionId CUserAgent::TransactionId() const
	{
    __TEST_INVARIANT;
	return iTransactionId;
	}

// -----------------------------------------------------------------------------
// CUserAgent::Transaction
// -----------------------------------------------------------------------------
//
CTransactionBase* CUserAgent::Transaction()
	{
    __TEST_INVARIANT;
	return iTransaction;
	}

// -----------------------------------------------------------------------------
// CUserAgent::CheckSigCompL
// -----------------------------------------------------------------------------
//
void CUserAgent::CheckSigCompL(const CURIContainer& aUri) const
    {
    __TEST_INVARIANT;
	
	if (CSIPMessageUtility::HasSigCompParam(aUri) && !iSigComp.IsSupported())
        {
        User::Leave(KErrNotSupported);
        }
    }

// -----------------------------------------------------------------------------
// CUserAgent::HasStopped
// -----------------------------------------------------------------------------
//
TBool CUserAgent::HasStopped() const
	{
	__TEST_INVARIANT;
	return iStopped;	
	}

// -----------------------------------------------------------------------------
// CUserAgent::IsCanceled
// -----------------------------------------------------------------------------
//
TBool CUserAgent::IsCanceled() const
	{
	__TEST_INVARIANT;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgent::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CUserAgent::__DbgTestInvariant() const
	{
	if (!iState || !iTransmitter || (IsUAS() && !iOwnsOutgoingMsg))
		{	
		User::Invariant();
		}
	}

