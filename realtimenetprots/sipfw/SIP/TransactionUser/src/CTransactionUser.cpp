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
// Name          : CTransactionUser.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipsec.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TransactionMgr.h"

#include "CTransactionUser.h"
#include "CTransactionStore.h"
#include "DeleteMgr.h"
#include "NormalUAC.h"
#include "InviteUAC.h"
#include "CancelUAC.h"
#include "NormalUAS.h"
#include "InviteUAS.h"
#include "CancelUAS.h"
#include "NormalUACStates.h"
#include "InviteUACStates.h"
#include "CancelUACStates.h"
#include "NormalUASStates.h"
#include "InviteUASStates.h"
#include "CancelUASStates.h"
#include "UserAgentCreateParams.h"
#include "TimerValueRetriever.h"
#include "SIPMessageUtility.h"
#include "MTransactionHeaders.h"

using namespace CommsDat;

// CSeq sequence number is a 32 bit integer, and must be less than 2**31
const TUint32 KMaxCSeqValue = 0x7fffffff;


// -----------------------------------------------------------------------------
// CTransactionUser::NewL
// -----------------------------------------------------------------------------
//
CTransactionUser* CTransactionUser::NewL(MSIPRequestRouter* aRouter,
									     MTimerManager* aTimers,
									     MConnectionStateObserver& aObserver)
	{
	__SIP_ASSERT_LEAVE(aRouter && aTimers, KErrArgument);

	CTransactionUser* self = new (ELeave) CTransactionUser(aRouter,
                                                           aTimers,
                                                           aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::CTransactionUser
// -----------------------------------------------------------------------------
//
CTransactionUser::CTransactionUser(MSIPRequestRouter* aRouter,
								   MTimerManager* aTimers,
                                   MConnectionStateObserver& aStateObserver) :
	iRouter(aRouter),
	iTimers(aTimers),
    iStateObserver(aStateObserver)
	{
	iCSeqCounter = User::TickCount() & KMaxCSeqValue;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::ConstructL
// -----------------------------------------------------------------------------
//
void CTransactionUser::ConstructL()
	{
	__SIP_ASSERT_LEAVE(iTimers, KErrGeneral);

	iTransactionStore = CTransactionStore::NewL();
	iTimerValueRetriever = CTimerValueRetriever::NewL();
	TUint t1 = iTimerValueRetriever->HighestT1();
    iConnectionMgr = CSipConnectionMgr::NewL(*iTransactionStore,
											 *this,
											 *iTimers,
											 t1,
			                                 *this);
	iDeleteMgr = CDeleteMgr::NewL();
	iSIPMsgUtility = new (ELeave) CSIPMessageUtility;

	ConstructUaStateMachinesL();

	iTransactionMgr = CTransactionMgr::NewL(*iTimers, *iTransactionStore);
	iSIPSec = CSIPSec::NewL(*iTimers, *iConnectionMgr, t1);
    }

// -----------------------------------------------------------------------------
// CTransactionUser::ConstructUaStateMachinesL
// -----------------------------------------------------------------------------
//
void CTransactionUser::ConstructUaStateMachinesL()
	{
	iInviteUAC_Start = new (ELeave) CInviteUAC_Start;
	iInviteUAC_ResolveAddress = new (ELeave) CInviteUAC_ResolveAddress;
	iInviteUAC_WaitResponse = new (ELeave) CInviteUAC_WaitResponse;
	iInviteUAC_WaitAckFromApp = new (ELeave) CInviteUAC_WaitAckFromApp;
	iInviteUAC_ResolveAckAddress = new (ELeave) CInviteUAC_ResolveAckAddress;
	iInviteUAC_SendingAck = new (ELeave) CInviteUAC_SendingAck;
	iInviteUAC_WaitTransactionToEnd =
		new (ELeave) CInviteUAC_WaitTransactionToEnd;
	iInviteUAC_WaitAuthentication = new (ELeave) CInviteUAC_WaitAuthentication;

	iInviteUAC_Start->SetNeighbourStates(*iInviteUAC_ResolveAddress);
	iInviteUAC_ResolveAddress->SetNeighbourStates(*iInviteUAC_WaitResponse);
	iInviteUAC_WaitResponse->SetNeighbourStates(
										*iInviteUAC_WaitAckFromApp,
										*iInviteUAC_WaitTransactionToEnd,
										*iInviteUAC_ResolveAddress,
										*iInviteUAC_WaitAuthentication);
	iInviteUAC_WaitAckFromApp->SetNeighbourStates(
										*iInviteUAC_ResolveAckAddress);
	iInviteUAC_ResolveAckAddress->SetNeighbourStates(*iInviteUAC_SendingAck);
	iInviteUAC_SendingAck->SetNeighbourStates(*iInviteUAC_ResolveAckAddress);
	iInviteUAC_WaitAuthentication->SetNeighbourStates(
											*iInviteUAC_ResolveAddress,
											*iInviteUAC_WaitResponse,
											*iInviteUAC_WaitAckFromApp,
											*iInviteUAC_WaitTransactionToEnd);


	iNormalUAC_Start = new (ELeave) CNormalUAC_Start;
	iNormalUAC_ResolveAddress = new (ELeave) CNormalUAC_ResolveAddress;
	iNormalUAC_WaitResponse = new (ELeave) CNormalUAC_WaitResponse;
	iNormalUAC_WaitAuthentication = new (ELeave) CNormalUAC_WaitAuthentication;

	iNormalUAC_Start->SetNeighbourStates(*iNormalUAC_ResolveAddress);
	iNormalUAC_ResolveAddress->SetNeighbourStates(*iNormalUAC_WaitResponse);
	iNormalUAC_WaitResponse->SetNeighbourStates(*iNormalUAC_ResolveAddress,
												*iNormalUAC_WaitAuthentication);
	iNormalUAC_WaitAuthentication->SetNeighbourStates(
										*iNormalUAC_ResolveAddress,
										*iNormalUAC_WaitResponse);


	iCancelUAC_Start = new (ELeave) CCancelUAC_Start;
	// CancelUAC uses rest of NormalUAC's states
	iCancelUAC_Start->SetNeighbourStates(*iNormalUAC_WaitResponse);


	iInviteUAS_Start = new (ELeave) CInviteUAS_Start;
	iInviteUAS_GetTxOwner = new (ELeave) CInviteUAS_GetTxOwner;
	iInviteUAS_WaitRespFromApp = new (ELeave) CInviteUAS_WaitRespFromApp;
	iInviteUAS_Accepted = new (ELeave) CInviteUAS_Accepted;
	iInviteUAS_Rejected = new (ELeave) CInviteUAS_Rejected;	

	iInviteUAS_Start->SetNeighbourStates(*iInviteUAS_GetTxOwner,
										 *iInviteUAS_WaitRespFromApp,
										 *iInviteUAS_Rejected);
	iInviteUAS_GetTxOwner->SetNeighbourStates(*iInviteUAS_WaitRespFromApp,
											  *iInviteUAS_Rejected);
	iInviteUAS_WaitRespFromApp->SetNeighbourStates(*iInviteUAS_Accepted,
												   *iInviteUAS_Rejected);


	iNormalUAS_Start = new (ELeave) CNormalUAS_Start;
	iNormalUAS_GetTxOwner = new (ELeave) CNormalUAS_GetTxOwner;
	iNormalUAS_WaitRespFromApp = new (ELeave) CNormalUAS_WaitRespFromApp;
	iNormalUAS_FinalRespSent = new (ELeave) CNormalUAS_FinalRespSent;

	iNormalUAS_Start->SetNeighbourStates(*iNormalUAS_GetTxOwner,
 									     *iNormalUAS_WaitRespFromApp,
										 *iNormalUAS_FinalRespSent);
	iNormalUAS_GetTxOwner->SetNeighbourStates(*iNormalUAS_WaitRespFromApp,
											  *iNormalUAS_FinalRespSent);
	iNormalUAS_WaitRespFromApp->SetNeighbourStates(*iNormalUAS_FinalRespSent);


	iCancelUAS_Start = new (ELeave) CCancelUAS_Start;
	iCancelUAS_FinalRespSent = new (ELeave) CCancelUAS_FinalRespSent;

	iCancelUAS_Start->SetNeighbourStates(*iCancelUAS_FinalRespSent);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::~CTransactionUser
// Deleting iTransactionStore and iDeleteMgr can lead deleting CTransmitter
// (cancels request to iConnectionMgr) and UAC (cancels request to SIPSec).
// -----------------------------------------------------------------------------
//
CTransactionUser::~CTransactionUser()
	{
	delete iTransactionStore;
	delete iDeleteMgr;
	delete iTransactionMgr;
	delete iTimerValueRetriever;
	delete iConnectionMgr;
	delete iSIPMsgUtility;
	delete iSIPSec;

	delete iInviteUAC_Start;
	delete iInviteUAC_ResolveAddress;
	delete iInviteUAC_WaitResponse;
	delete iInviteUAC_WaitAckFromApp;
	delete iInviteUAC_ResolveAckAddress;
	delete iInviteUAC_SendingAck;
	delete iInviteUAC_WaitTransactionToEnd;
	delete iInviteUAC_WaitAuthentication;

	delete iNormalUAC_Start;
	delete iNormalUAC_ResolveAddress;
	delete iNormalUAC_WaitResponse;
	delete iNormalUAC_WaitAuthentication;

	delete iCancelUAC_Start;

	delete iInviteUAS_Start;
	delete iInviteUAS_GetTxOwner;
	delete iInviteUAS_WaitRespFromApp;
	delete iInviteUAS_Accepted;
	delete iInviteUAS_Rejected;	

	delete iNormalUAS_Start;
	delete iNormalUAS_GetTxOwner;
	delete iNormalUAS_WaitRespFromApp;
	delete iNormalUAS_FinalRespSent;

	delete iCancelUAS_Start;
	delete iCancelUAS_FinalRespSent;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SIPSec
// -----------------------------------------------------------------------------
//
CSIPSec& CTransactionUser::SIPSec()
	{
	return *iSIPSec;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SipConnectionMgr
// -----------------------------------------------------------------------------
//
CSipConnectionMgr& CTransactionUser::SipConnectionMgr()
	{
	return *iConnectionMgr;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SetDialogs
// No invariant as iDialogs/iRegistrations/iRegistrationContact aren't set yet.
// -----------------------------------------------------------------------------
//
void CTransactionUser::SetDialogs(MSipDialogs* aDialogs)
	{
	__SIP_ASSERT_RETURN(aDialogs, KErrArgument);

	iDialogs = aDialogs;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SetRegistrations
// No invariant as iDialogs/iRegistrations/iRegistrationContact aren't set yet.
// -----------------------------------------------------------------------------
//
void CTransactionUser::SetRegistrations(
                                MSipRegistrations* aRegistrations,
								MSipRegistrationContact* aRegistrationContact)
	{
	__SIP_ASSERT_RETURN(aRegistrations && aRegistrationContact, KErrArgument);

	iRegistrations = aRegistrations;
	iRegistrationContact = aRegistrationContact;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::GetStateL
// -----------------------------------------------------------------------------
//
void CTransactionUser::GetStateL(TTransactionId aTransactionId,
                            	 CSIPInternalStates::TState& aState) const
    {
    __TEST_INVARIANT;

    CUserAgent* ua = iTransactionStore->SearchById(aTransactionId);
	__ASSERT_ALWAYS(ua && ua->Transaction(), User::Leave(KErrNotFound));
    ua->Transaction()->GetState(aState);
    }

// -----------------------------------------------------------------------------
// CTransactionUser::NewCSeqValue
// -----------------------------------------------------------------------------
//
TUint32 CTransactionUser::NewCSeqValue()
	{
	__TEST_INVARIANT;

	if (iCSeqCounter >= KMaxCSeqValue)
		{
		iCSeqCounter = 0;
		}

	return ++iCSeqCounter;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SendL
// Send ACK using an existing INVITE-UAC.
// -----------------------------------------------------------------------------
//
void CTransactionUser::SendL(TTransactionId& aTransactionId,
							 TRegistrationId aRegistrationId,
							 CSIPRequest* aRequest,
							 MTransactionOwner* aObserver,
                             CURIContainer& aRemoteTarget,
                             const TSIPTransportParams& aTransportParams,
							 TBool aDeleteRequest)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRequest && aObserver, KErrArgument);
	__SIP_MESSAGE_LOG("TransactionUser", *aRequest)

	if (CSIPMessageUtility::IsAck(*aRequest))
		{
		CUserAgent* ua = iTransactionStore->SearchById(aTransactionId);
		__ASSERT_ALWAYS(ua && ua->IsInviteUAC(),
					    User::Leave(KErrSIPInvalidTransactionState));
		CInviteUAC::Ptr(*ua).SendAckL(aRequest,
								*aObserver,
								aTransportParams,
								aRegistrationId,
								aRemoteTarget,
								aDeleteRequest,
								SIPStrings::StringF(SipStrConsts::EEmpty));
		}
	else
		{
		CTransactionBase::TTransactionType type =
            CTransactionBase::KClientTransaction;
        if (aRequest->Method() == SIPStrings::StringF(SipStrConsts::EInvite))
			{
			type = CTransactionBase::KClientInviteTransaction;
			}

		aTransactionId = iTransactionStore->NewTransactionId();
		CUserAgent* ua = CreateUacL(aTransactionId,
									aRequest->Method(),
									*aObserver,
									aDeleteRequest,
									NewCSeqValue(),
                                    aTransportParams);
        TCleanupItem cleanupItem(CUserAgent::CleanupSilently, ua);
        CleanupStack::PushL(cleanupItem);
		iTransactionStore->AddL(aTransactionId, ua, NULL, NULL, type);
		ua->SendRequestL(aRequest, aRegistrationId, aRemoteTarget);
		CleanupStack::Pop(); // cleanupItem
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SendAndGetHeadersL
// -----------------------------------------------------------------------------
//
MTransactionHeaders*
CTransactionUser::SendAndGetHeadersL(TTransactionId& aTransactionId,
								 TRegistrationId aRegistrationId,
								 CSIPRequest* aRequest,
								 MTransactionOwner* aObserver,
                                 CURIContainer& aRemoteTarget,
                                 const TSIPTransportParams& aTransportParams,
								 TBool aDeleteRequest)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aRequest && aObserver, KErrArgument);

	SendL(aTransactionId,
		  aRegistrationId,
		  aRequest,
		  aObserver,
		  aRemoteTarget,
          aTransportParams,
		  EFalse); // Don't take aRequest's ownership yet as leave can occur

	CUserAgent* ua = iTransactionStore->SearchById(aTransactionId);
	__SIP_ASSERT_LEAVE(ua, KErrNotFound);

    TCleanupItem cleanupItem(CUserAgent::CleanupSilently, ua);
    CleanupStack::PushL(cleanupItem);
	MTransactionHeaders* headers =
        iTransactionStore->TransactionHeadersL(aTransactionId);
	CleanupStack::Pop(); // cleanupItem

	static_cast<CUserAgentClient*>(ua)->SetRequestOwnership(aDeleteRequest);

	__TEST_INVARIANT;
	return headers;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SendL
// -----------------------------------------------------------------------------
//
void CTransactionUser::SendL(TTransactionId aTransactionId,
							 CSIPResponse* aResp,
							 const TSIPTransportParams& aTransportParams) const
	{
	__TEST_INVARIANT;

	SendL(aTransactionId, aResp, NULL, aTransportParams);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SendL
// -----------------------------------------------------------------------------
//
void CTransactionUser::SendL(TTransactionId aTransactionId,
							 CSIPResponse* aResp,
							 MTransactionOwner* aNewObserver,
							 const TSIPTransportParams& aTransportParams) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aTransactionId != KEmptyTransactionId, KErrArgument);
	__SIP_ASSERT_LEAVE(aResp, KErrArgument);
	__SIP_MESSAGE_LOG("TransactionUser", *aResp)

	CUserAgent* ua = iTransactionStore->SearchById(aTransactionId);
	__ASSERT_ALWAYS(ua && ua->IsUAS(),
					User::Leave(KErrSIPInvalidTransactionState));
	if (aNewObserver)
		{
		ua->UpdateTransactionOwner(aNewObserver);
		}

	ua->SendResponseL(aResp, aTransportParams);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::SendCancelL
// CANCEL uses INVITE's TransportParams.
// -----------------------------------------------------------------------------
//
void CTransactionUser::SendCancelL(TTransactionId aInviteTaId,
								   TTransactionId& aCancelTaId,
								   MTransactionOwner* aObserver)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aObserver, KErrArgument);

	CUserAgent* ua = iTransactionStore->SearchById(aInviteTaId);
	__ASSERT_ALWAYS(ua &&
					ua->Transaction() &&
					iTransactionMgr->IsPossibleToSendCancel(*ua->Transaction()),
					User::Leave(KErrSIPInvalidTransactionState));
    aCancelTaId = iTransactionStore->NewTransactionId();
	CCancelUAC* cancelUa = CreateCancelUacL(aCancelTaId,
									        *aObserver,
                                            NewCSeqValue(),
                                            ua->TransportParams());
    TCleanupItem cleanupItem(CUserAgent::CleanupSilently, cancelUa);
    CleanupStack::PushL(cleanupItem);
	iTransactionStore->AddL(aCancelTaId,
                            cancelUa,
                            NULL,
                            NULL,
							CTransactionBase::KClientTransaction);
	cancelUa->SendCancelL(aInviteTaId);
	CleanupStack::Pop(); // cleanupItem

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::ClearTransactionOwner
// -----------------------------------------------------------------------------
//
TInt
CTransactionUser::ClearTransactionOwner(TTransactionId aTransactionId) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN_VALUE(aTransactionId != KEmptyTransactionId,
                              KErrArgument);
	CUserAgent* ua = iTransactionStore->SearchById(aTransactionId);
	if (ua)
		{
		ua->ClearTransactionOwner();
		return KErrNone;
		}

	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::ClearTransactionOwner
// -----------------------------------------------------------------------------
//
TInt CTransactionUser::ClearTransactionOwner(MTransactionOwner* aObserver) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN_VALUE(aObserver, KErrArgument);

	return iTransactionStore->ClearTransactionOwner(aObserver);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::TransactionHeadersL
// -----------------------------------------------------------------------------
//
MTransactionHeaders*
CTransactionUser::TransactionHeadersL(TTransactionId aTransactionId) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aTransactionId != KEmptyTransactionId, KErrArgument);

	return iTransactionStore->TransactionHeadersL(aTransactionId);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::MakeTagL
// -----------------------------------------------------------------------------
//
void CTransactionUser::MakeTagL(TDes8& aTag) const
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(aTag.Length() != aTag.MaxLength(),
					User::Leave(KErrArgument));
	TTransactionId taId = KEmptyTransactionId;
	CUserAgent* ua = NULL;
    CSIPMessage* message = NULL;
	iTransactionStore->RandomTaInfoL(taId, &ua, &message);

	CleanupStack::PushL(message);
	iSIPMsgUtility->AddRandomStringL(aTag,
									 aTag.MaxLength() - aTag.Length(),
									 EFalse,
									 message,
									 taId,
									 ua);
	CleanupStack::PopAndDestroy(message);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::NewTransactionL
// -----------------------------------------------------------------------------
//
MReceiverObserver*
CTransactionUser::NewTransactionL(CSIPRequest& aRequest,
                                  const TSIPTransportParams& aParams,
								  TTransactionId& aTransactionId)
	{
	__TEST_INVARIANT;	

	if (CSIPMessageUtility::IsAck(aRequest) ||
		!iTransactionStore->AllowMoreServerTransactions())
		{
		return NULL;
		}

	aTransactionId = iTransactionStore->NewTransactionId();
	CUserAgent* ua = CreateUasL(aTransactionId, aRequest.Method(), aParams);
	MReceiverObserver* receiver = ua->Transaction();

	CleanupStack::PushL(ua);
	iTransactionStore->AddL(aTransactionId,
			        ua,
			        ua->Transaction(),
                    &aRequest,
                    CSIPMessageUtility::TransactionType(aRequest, ETrue));
	CleanupStack::Pop(ua);

	__TEST_INVARIANT;
	return receiver;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::FillUserAgentParamsLC
// -----------------------------------------------------------------------------
//
CUserAgentCreateParams*
CTransactionUser::FillUserAgentParamsLC(TTransactionId aTransactionId,
									MTransactionOwner* aObserver,
									TBool aDeleteOutgoingMsg,
		                            const TSIPTransportParams& aParams) const
	{
	__TEST_INVARIANT;
	CTimerValueRetriever::BearerType bearerType = 
		(CTimerValueRetriever::BearerType)TimerRetrieverMappedBearerTypeL(aParams.IapId());
	CUserAgentCreateParams* uaParams =
		CUserAgentCreateParams::NewLC(*iTransactionMgr,
                                      *iTimers,
									  *iSIPMsgUtility,
									  iTimerValueRetriever->TimerValues(bearerType),
									  *iRegistrations,
                                      *iRegistrationContact,
                                      *iConnectionMgr,
                                      *iTransactionStore,
                                      *iDeleteMgr,
									  aDeleteOutgoingMsg,
                                      aParams);
	uaParams->iTransactionId = aTransactionId;
	uaParams->iObserver = aObserver;

	return uaParams;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::CreateUasL
// -----------------------------------------------------------------------------
//
CUserAgent* CTransactionUser::CreateUasL(TTransactionId aTaId,
									 RStringF aMethod,
		                             const TSIPTransportParams& aParams) const
	{
	__TEST_INVARIANT;

	CUserAgent* ua = NULL;
	CUserAgentCreateParams* uaParams =
        FillUserAgentParamsLC(aTaId,
							  NULL,  // Observer not yet known
							  ETrue, // UAS sends responses
                              aParams);
	if (aMethod == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		uaParams->iInitialUaState = iInviteUAS_Start;
		ua = CInviteUAS::NewL(*uaParams, *iConnectionMgr, *iRouter, *iDialogs);
		}
	else
		{
		if (aMethod == SIPStrings::StringF(SipStrConsts::ECancel))
			{
			uaParams->iInitialUaState = iCancelUAS_Start;
			ua = CCancelUAS::NewL(*uaParams,
								  *iConnectionMgr,
								  *iRouter,
								  *iDialogs);
			}
		else
			{
			uaParams->iInitialUaState = iNormalUAS_Start;
			ua = CNormalUAS::NewL(*uaParams,
								  *iConnectionMgr,
								  *iRouter,
                                  *iDialogs);
			}
		}
	CleanupStack::PopAndDestroy(uaParams);
	return ua;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::CreateUacL
// -----------------------------------------------------------------------------
//
CUserAgent* CTransactionUser::CreateUacL(TTransactionId aTaId,
									 RStringF aMethod,
									 MTransactionOwner& aObserver,
									 TBool aDeleteRequest,
									 TUint32 aCSeqNumber,
		                             const TSIPTransportParams& aParams) const
	{
	__TEST_INVARIANT;	

	CUserAgent* ua = NULL;
	CUserAgentCreateParams* uaParams = FillUserAgentParamsLC(aTaId,
															 &aObserver,
															 aDeleteRequest,
                                                             aParams);
	if (aMethod == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		uaParams->iInitialUaState = iInviteUAC_Start;
		ua = CInviteUAC::NewL(*uaParams,
							  *iConnectionMgr,
                              *iConnectionMgr,
							  *iSIPSec,
							  aCSeqNumber,							  
							  CSIPMessageUtility::IsPrivateAddressL(
							  	*iConnectionMgr, aParams.IapId()),
							  iInviteUAC_WaitAckFromApp);
		}
	else
		{
		uaParams->iInitialUaState = iNormalUAC_Start;
		ua = CNormalUAC::NewL(*uaParams,
							  *iConnectionMgr,
                              *iConnectionMgr,
							  *iSIPSec,
							  aCSeqNumber);
		}

	CleanupStack::PopAndDestroy(uaParams);
	return ua;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::CreateCancelUacL
// -----------------------------------------------------------------------------
//
CCancelUAC* CTransactionUser::CreateCancelUacL(TTransactionId aTaId,
						           MTransactionOwner& aObserver,
		                           TUint32 aCSeqNumber,
		                           const TSIPTransportParams& aParams) const
	{
	__TEST_INVARIANT;	

	CUserAgentCreateParams* uaParams = FillUserAgentParamsLC(aTaId,
									                         &aObserver,
									                         ETrue,
                                                             aParams);
    uaParams->iInitialUaState = iCancelUAC_Start;

	CCancelUAC* ua = CCancelUAC::NewL(*uaParams,
									  *iConnectionMgr,
                                      *iConnectionMgr,
									  *iSIPSec,
									  aCSeqNumber);
    CleanupStack::PopAndDestroy(uaParams);
    return ua;
	}

// -----------------------------------------------------------------------------
// CTransactionUser::GetLocalAddress
// No invariant as iDialogs/iRegistrations/iRegistrationContact aren't set yet.
// -----------------------------------------------------------------------------
//
void CTransactionUser::GetLocalAddress(TUint32 aIapId, TInetAddr& aAddr) const
    {
    __SIP_ASSERT_RETURN(iConnectionMgr, KErrGeneral);

    iConnectionMgr->GetLocalAddress(aIapId, aAddr);
    }

// -----------------------------------------------------------------------------
// CTransactionUser::FreeResources
// No invariant as iDialogs/iRegistrations/iRegistrationContact aren't set yet.
// -----------------------------------------------------------------------------
//
void
CTransactionUser::FreeResources(MSIPNATBindingObserver& aSIPNATBindingObserver)
	{
    __SIP_ASSERT_RETURN(iConnectionMgr, KErrGeneral);

    iConnectionMgr->FreeResources(aSIPNATBindingObserver);
	}

// -----------------------------------------------------------------------------
// CTransactionUser::ConnectionStateChangedL
// EInit/EActive: no action. If application opens an IAP that has been opened by
// another application, ConnectionMgr passes connection's current state.
// No invariant as iDialogs/iRegistrations/iRegistrationContact aren't set yet.
// -----------------------------------------------------------------------------
//
void CTransactionUser::ConnectionStateChangedL(TUint32 aIapId,
                                               CSIPConnection::TState aState)
	{
    __SIP_ASSERT_LEAVE(iTransactionStore, KErrGeneral);

	iStateObserver.ConnectionStateChangedL(aIapId, aState);

	switch (aState)
		{
		case CSIPConnection::ESuspended:
	        iTransactionStore->EndResolvingTransactions(aIapId,
	                                                KErrSIPTransportFailure);
			break;

		case CSIPConnection::EInactive:
	    case CSIPConnection::EUnavailable:
	        iTransactionStore->RemoveItemsByIapId(aIapId);
			break;

		default:
	        break;
		}
	}

// -----------------------------------------------------------------------------
// CTransactionUser::TimerRetrieverMappedBearerType
// -----------------------------------------------------------------------------
//
TInt CTransactionUser::TimerRetrieverMappedBearerTypeL(TUint32 aIapID) const
{
	CTimerValueRetriever::BearerType mappedBearerType(CTimerValueRetriever::EModemBearer);

	if (aIapID)
	{
	   	CMDBSession* cdb = CMDBSession::NewLC( KCDVersion1_1 ); 
	    
	   	cdb->SetAttributeMask( ECDHidden );
	    
		// Create an iap record
		CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>( 
	            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
		CleanupStack::PushL( iapRecord );
	    
		iapRecord->SetRecordId( aIapID );

		iapRecord->LoadL( *cdb );

		if (!iapRecord->iBearerType.IsNull())
		{
	       		TPtrC bearerType(iapRecord->iBearerType);
			if(bearerType.CompareF(TPtrC(KCDTypeNameLANBearer)) == 0)
				mappedBearerType = CTimerValueRetriever::ELanBearer;
	   	}
		
	
		CleanupStack::PopAndDestroy( iapRecord );
		CleanupStack::PopAndDestroy( cdb );
	}

	return mappedBearerType;
}

// -----------------------------------------------------------------------------
// CTransactionUser::NextHopIP
// -----------------------------------------------------------------------------
//
TInt CTransactionUser::NextHopIP(
		const TTransactionId& aTransactionId, TInetAddr& aAddr )
    {
    TInt error = KErrNone;

    CUserAgent* ua = iTransactionStore->SearchById(aTransactionId);
    if (!ua || ua->IsUAS())
        {
        error = CSIPClient::ENonExistingRequestId;
        }
    else
        {
    	aAddr = static_cast<CUserAgentClient*>(ua)->NextHopIP();
        }
    
    return error;
    
    }

// -----------------------------------------------------------------------------
// CTransactionUser::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CTransactionUser::__DbgTestInvariant() const
	{
	if (!iRouter ||
		!iTimers ||
		!iTransactionStore ||
		!iConnectionMgr ||
		!iDialogs ||
		!iRegistrations ||
		!iRegistrationContact ||
		!iDeleteMgr ||
		!iTimerValueRetriever ||		
        iCSeqCounter > KMaxCSeqValue)
		{
		User::Invariant();
		}
	}
