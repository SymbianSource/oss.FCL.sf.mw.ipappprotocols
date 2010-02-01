// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).627

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
// Name          : UserAgentClient.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "MSipRegistrations.h"
#include "sipcseqheader.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "siphostport.h"
#include "sipaddress.h"
#include "sipcallidheader.h"
#include "sipviaheader.h"
#include "sipfromheader.h"
#include "siprouteheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipsec.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "Transmitter.h"

#include "UserAgentClient.h"
#include "UserAgentState.h"
#include "CTransactionStore.h"
#include "SIPMessageUtility.h"
#include "SIPRequestUtility.h"
#include "MTransactionUser.h"
#include "MTransactionOwner.h"
#include "RouteSet.h"
#include "ResolvingResults.h"
#include "StopUserAgent.h"
#include "RestoreUaState.h"
#include "MSIPSecUser.h"

// -----------------------------------------------------------------------------
// CUserAgentClient::CUserAgentClient
// -----------------------------------------------------------------------------
//
CUserAgentClient::CUserAgentClient(CUserAgentCreateParams& aParams,
								   MSipUriResolver& aResolver,
                                   CSIPSec& aSIPSec,
					               TUint32 aCSeqNumber) :
	CUserAgent(aParams),
	iSIPSec(aSIPSec),
	iCSeqNumber(aCSeqNumber),
	iResolver(aResolver),
	iSipSecError(KErrNone),
    iFinalRespPassed(EFalse),
    iOutboundProxyHasBeenTried(EFalse),
    iLastError(KErrNone),
    iIgnoreSIPSecCallback(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::ConstructL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::ConstructL()
	{
	iResolvingResults = CResolvingResults::NewL();	
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::~CUserAgentClient
// -----------------------------------------------------------------------------
//
CUserAgentClient::~CUserAgentClient()
	{
	iResolver.CancelGetByUri(this);
	CancelSIPSecRequest();
	delete iResolvingResults;	
    delete iRemoteTarget;
    delete iRouteSet;
    delete iRequestUri;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::IsUAS
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::IsUAS() const
	{
	__TEST_INVARIANT;
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::RegistrationId
// -----------------------------------------------------------------------------
//
TRegistrationId CUserAgentClient::RegistrationId() const
	{
	__TEST_INVARIANT;
	return iRegisterId;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::CompletedL
// Resolving succeeded so clear the stored error.
// -----------------------------------------------------------------------------
//
void CUserAgentClient::CompletedL()
	{
	__TEST_INVARIANT;

	if (!HasStopped())
		{
		iLastError = KErrNone;
		delete iIncomingMsg;
		iIncomingMsg = NULL;
		State().AddressResolvedL(*this);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::ErrorOccured
// -----------------------------------------------------------------------------
//
void CUserAgentClient::ErrorOccured(TInt aError)
	{
	__TEST_INVARIANT;

	if (!HasStopped())
		{
		if (aError == KErrSIPResolvingFailure)
			{
			// URI fully resolved. Old results not needed.
			iResolvingResults->ClearAll();
			State().ResolvingFailed(*this);
			}
		else
			{
			// Leave came in MSIPServerResolverObserver::CompletedL
			Stop(aError);
			}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SIPSecCacheUpdated
// Don't use __TEST_INVARIANT as destructor may lead here.
// -----------------------------------------------------------------------------
//
void CUserAgentClient::SIPSecCacheUpdated(TBool aSuccess)
	{
	if (!iIgnoreSIPSecCallback)
		{
		SIPSecReady(aSuccess);
		}
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SIPSecTimerExpiredL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::SIPSecTimerExpiredL()
	{
	__TEST_INVARIANT;

	TInt err = SIPSecReady(ETrue);
	if (err != KErrNone)
		{
		User::Leave(err);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SIPSecReady
// TRAP may delete UA. Check if UA exists before stopping. Don't use
// __TEST_INVARIANT after Stop.
// -----------------------------------------------------------------------------
//
TInt CUserAgentClient::SIPSecReady(TBool aSuccess)
	{
	__TEST_INVARIANT;

    TTransactionId transactionId = TransactionId();
    CTransactionStore& store = iTransactionStore;

	TRAPD(err, State().SIPSecCacheUpdatedL(*this, aSuccess));
	if (err != KErrNone && store.SearchById(transactionId))
		{
        Stop(err);
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::HandleSendRequestL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::HandleSendRequestL(CSIPRequest* aReq,
										  TRegistrationId aRegisterId,
										  const CURIContainer& aRemoteTarget,
                                          const CUserAgentState& aResolve)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aReq, KErrArgument);
    __SIP_ASSERT_LEAVE(!iRouteSet && !iRemoteTarget, KErrAlreadyExists);

	iRegisterId = aRegisterId;
    StoreRemoteTargetL(aRemoteTarget);
    iRouteSet = CRouteSet::NewL(*aReq, iRegistrations, iRegisterId);

    SIPRequestUtility::CheckOutgoingRequestL(*aReq);
	FillRequestL(*aReq);
	StoreRequestUriL(*aReq);
	
	TBool isRegMsg = (SIPStrings::StringF(SipStrConsts::ERegister) == aReq->Method());
	ResolveNextHopL(ETrue,!isRegMsg);

	UpdateInfoToStoreL(aReq);
	StoreOutgoingMsg(aReq);
    ChangeState(aResolve);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::FillRequestL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::FillRequestL(CSIPRequest& aReq) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iRouteSet && iRemoteTarget, KErrNotFound);

	CSIPMessageUtility::FillCSeqL(aReq, iCSeqNumber, aReq.Method());

	if (!aReq.HasHeader(SIPStrings::StringF(SipStrConsts::ECallIDHeader)))
		{
        FillNewCallIdL(aReq);
		}

	if (!aReq.HasHeader(SIPStrings::StringF(SipStrConsts::EMaxForwardsHeader)))
		{
		SIPRequestUtility::FillNewMaxForwardsL(aReq);
		}

	CheckContactHeadersL(aReq, aReq.From());
	SIPRequestUtility::FillRouteAndRequestUriL(aReq,
											   *iRouteSet,
											   *iRemoteTarget);
    FillNewViaL(aReq, SIPStrings::StringF(SipStrConsts::EEmpty));

    CSIPFromHeader* from = aReq.From();
    __SIP_ASSERT_LEAVE(from != NULL, KErrArgument);
	if (!from->HasParam(SIPStrings::StringF(SipStrConsts::ETag)))
		{
		AddTagL(*from);
		}
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::UpdateViaAndCSeqL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::UpdateViaAndCSeqL() const
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

	CSIPRequest* req = static_cast<CSIPRequest*>(iOutgoingMsg);
    FillNewViaL(*req, SIPStrings::StringF(SipStrConsts::EEmpty));

    CSIPCSeqHeader* cseqHeader = req->CSeq();
    __SIP_ASSERT_LEAVE(cseqHeader != NULL, KErrSIPMalformedMessage);
	TUint cseq = cseqHeader->Seq();

	if (iObserver)
		{
		User::LeaveIfError(iObserver->NextCSeq(cseq));
		}
	else
		{
		++cseq;
		}
	cseqHeader->SetSeq(cseq);
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::FillNewCallIdL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::FillNewCallIdL(CSIPRequest& aReq) const
	{
	__TEST_INVARIANT;

	TBuf8<MTransactionUser::KCallIDLength> callID;
	AddRandomStringL(callID, MTransactionUser::KCallIDLength, ETrue);
	CSIPCallIDHeader* callIdHeader = CSIPCallIDHeader::DecodeL(callID);
	CleanupStack::PushL(callIdHeader);
	aReq.AddHeaderL(callIdHeader);
	CleanupStack::Pop(callIdHeader);
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SetBranchL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::SetBranchL(CSIPViaHeader& aVia, RStringF aBranch) const
	{
	__TEST_INVARIANT;

	if (aBranch == SIPStrings::StringF(SipStrConsts::EEmpty))
		{
		const TInt KBranchLength = 30;
    	__ASSERT_DEBUG(
    		KBranchLength > CSIPMessageUtility::BranchMagicCookie().Length(),
			User::Panic(_L("UAC:SetBranchL"), KErrOverflow));
		TBuf8<KBranchLength> branchBuf;
		branchBuf.Append(CSIPMessageUtility::BranchMagicCookie());
		AddRandomStringL(branchBuf, KBranchLength - branchBuf.Length(), EFalse);

		RStringF branch = SIPStrings::Pool().OpenFStringL(branchBuf);
		CleanupClosePushL(branch);
		aVia.SetParamL(SIPStrings::StringF(SipStrConsts::EBranch), branch);	
		CleanupStack::PopAndDestroy(); // branch
		}
	else
		{
		aVia.SetParamL(SIPStrings::StringF(SipStrConsts::EBranch), aBranch);
		}
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::FillNewViaL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::FillNewViaL(CSIPRequest& aReq, RStringF aBranch) const
	{
	__TEST_INVARIANT;

	CSIPViaHeader* via = CSIPMessageUtility::TopVia(aReq);
	if (via)
		{
		SetBranchL(*via, aBranch);
		via->SetTransportL(iTransportProtocol);
		}
	else
		{
		CSIPHostPort* hp = CSIPHostPort::DecodeL(
			SIPStrings::StringF(SipStrConsts::ELocalHost).DesC());
		CleanupStack::PushL(hp);
		via = CSIPViaHeader::NewL(iTransportProtocol, hp);
		CleanupStack::Pop(hp);
		CleanupStack::PushL(via);
	
		SetBranchL(*via, aBranch);

		aReq.AddHeaderL(via);
		CleanupStack::Pop(via);
		}
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::StoreRequestUriL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::StoreRequestUriL(CSIPRequest &aReq)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aReq.RequestURI() != NULL, KErrArgument);

	CURIContainer* newUri = CURIContainer::NewL(*aReq.RequestURI());
	delete iRequestUri;
	iRequestUri = newUri;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::UpdateInfoToStoreL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::UpdateInfoToStoreL(CSIPRequest* aReq) const
	{
	__TEST_INVARIANT;

	iTransactionStore.UpdateL(TransactionId(), iTransaction, aReq);
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::ResolveNextHopL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::ResolveNextHopL(TBool aCheckSigComp,TBool aUseResolvedProxy)
	{
	__TEST_INVARIANT;

	CURIContainer& uri = NextHopL(aUseResolvedProxy);
	if (aCheckSigComp)
		{
		CheckSigCompL(uri);
		}

	iResolver.GetByURIL(uri,
						iResolvingResults->Addresses(),
						TransportParams(),
						this);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::CopyRemoteAddress
// Clear resolving results as CANCEL must use only one ip-address.
// -----------------------------------------------------------------------------
//
void CUserAgentClient::CopyRemoteAddress(const CUserAgentClient& aSrc)
	{
	__TEST_INVARIANT;

	iRemoteAddr = aSrc.iRemoteAddr;
	iForceUDP = aSrc.iForceUDP;

	iTransportProtocol.Close();
	if (aSrc.iTransaction)
		{
		iTransportProtocol = aSrc.iTransaction->TransportProtocol().Copy();
		}
	else
		{
		RStringF protocol = aSrc.iTransportProtocol;
		iTransportProtocol = protocol.Copy();
		}

	iResolvingResults->ClearAll();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::CopyRouteSetAndRemoteTargetL
// -----------------------------------------------------------------------------
//
void
CUserAgentClient::CopyRouteSetAndRemoteTargetL(const CUserAgentClient& aSrc)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(!iRouteSet && !iRemoteTarget, KErrAlreadyExists);
	__SIP_ASSERT_LEAVE(aSrc.iRouteSet && aSrc.iRemoteTarget, KErrNotFound);

	iRouteSet = CRouteSet::NewL(*aSrc.iRouteSet);
	iRemoteTarget = CURIContainer::NewL(*aSrc.iRemoteTarget);

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::StoreRemoteTargetL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::StoreRemoteTargetL(const CURIContainer& aRemoteTarget)
	{
	__TEST_INVARIANT;

	CURIContainer* newRemoteTarget = CURIContainer::NewL(aRemoteTarget);
	delete iRemoteTarget;
	iRemoteTarget = newRemoteTarget;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::NextHopL
// -----------------------------------------------------------------------------
//
CURIContainer& CUserAgentClient::NextHopL(TBool aUseResolvedProxy) const
	{
	__TEST_INVARIANT;
	__ASSERT_ALWAYS(iRouteSet && iRemoteTarget,
        			User::Panic(_L("UAC:NextHop"), KErrNotFound)); 
	
	if (aUseResolvedProxy && iRegistrations.HasOutboundProxy(iRegisterId))
	    {
	    if(iRegistrations.OutboundProxy(iRegisterId)->SIPAddress().URI() == 
	    	iRouteSet->NextHop(*iRemoteTarget))
	    	{
		    if(iRegistrations.IsCacheOutboundProxyIPEanbled(iRegisterId))
		    	{
		    	TRAPD(err,iRegistrations.OutboundProxyIPL(iRegisterId))
		    	if(KErrNone == err)
		    		{
		    		return iRegistrations.OutboundProxyIPL(iRegisterId).SIPAddress().URI();
		    		}
		    	}
		    }
	    }
	return iRouteSet->NextHop(*iRemoteTarget);
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SetRequestOwnership
// -----------------------------------------------------------------------------
//
void CUserAgentClient::SetRequestOwnership(TBool aOwnsRequest)
	{
	__TEST_INVARIANT;

	iOwnsOutgoingMsg = aOwnsRequest;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::StoreResponse
// -----------------------------------------------------------------------------
//
void CUserAgentClient::StoreResponse(CSIPResponse* aResponse)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(!iIncomingMsg, KErrAlreadyExists);

	iIncomingMsg = aResponse;

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SelectNextRemoteAddressL
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::SelectNextRemoteAddressL()
	{
	__TEST_INVARIANT;

	iSIPSecCounter = 0; // Remote address changes, reset counter
	if (!iResolvingResults->GetNext(iRemoteAddr, iTransportProtocol, iForceUDP))
		{
		return EFalse;
		}
	if(iRemoteAddr.IsUnspecified())
		{
		return EFalse;
		}

	__SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);
	CSIPMessageUtility::UpdateViaTransportL(*iOutgoingMsg, iTransportProtocol);

	__TEST_INVARIANT;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::RemoteAddressResolvedL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::RemoteAddressResolvedL(const CUserAgentState& aNextState)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(SelectNextRemoteAddressL(), KErrNotFound);

	if (FillSecurityParamsL())	
		{
	    CreateTransactionL();
		UpdateInfoToStoreL(NULL /* don't update message headers */);
		SendRequestToNetworkL();
		ChangeState(aNextState);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::HandleResolvingFailure
// -----------------------------------------------------------------------------
//
void
CUserAgentClient::HandleResolvingFailure(const CUserAgentState& aResolve,
				  					     const CUserAgentState& aWaitResponse)
	{
	__TEST_INVARIANT;

	TBool retry(ETrue);
	TRAPD(err, retry = TryNextAddressL(KErrSIPResolvingFailure,
							 		   aResolve,
							 		   aWaitResponse));
	if (err != KErrNone || !retry)
		{
		StopWithLastError();
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::FillSecurityParamsL
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::FillSecurityParamsL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg && iRemoteTarget, KErrNotFound);

	if (!iObserver)
		{
		Stop(KErrNone);
		return EFalse;
		}

	if (UseSIPSec())
		{
		CUri8* remoteTarget = iRemoteTarget->CloneAsUri8L();
		CleanupStack::PushL(remoteTarget);
		TRAPD(err, iSIPSec.AddSecurityParamsL(
					iTransportParams,
					static_cast<CSIPRequest&>(*iOutgoingMsg),
					RegistrationId(),
					TransactionId(),
					iRemoteAddr,
					CSIPMessageUtility::UriDescriptor(NextHopL()),
					*remoteTarget,
					ProxyL(),
					const_cast<MSIPSecUser&>(*iObserver->SIPSecUser())));
		CleanupStack::PopAndDestroy(remoteTarget);
		HandleSIPSecErrorL(err);
		}

	__TEST_INVARIANT;
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::HandleSIPSecErrorL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::HandleSIPSecErrorL(TInt aError)
	{
	__TEST_INVARIANT;
	
	if (aError != KErrNone)		
		{
		if (aError != KErrNoMemory)
			{
			aError = KErrSIPForbidden;
			}
		Stop(aError);
		User::Leave(aError);
		}
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SIPSecCacheUpdatedL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::SIPSecCacheUpdatedL(TBool aSuccess,
	const CUserAgentState& aResolve,
	const CUserAgentState& aWaitResponse,
	const CUserAgentState& aWaitAckFromApp,
	const CUserAgentState& aWaitTransactionToEnd)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iIncomingMsg, KErrNotFound);

	if (aSuccess)
		{
		CSIPResponse* resp = static_cast<CSIPResponse*>(iIncomingMsg);
		iIncomingMsg = NULL;
		CleanupStack::PushL(resp);
		ReceiveResponseL(resp,
						 aResolve,
					 	 aWaitResponse,
						 aWaitAckFromApp,
						 aWaitTransactionToEnd);
		CleanupStack::Pop(resp);
		}
	else
		{
        Stop(KErrSIPForbidden);
		}

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::ProxyL
// -----------------------------------------------------------------------------
//
const TDesC8& CUserAgentClient::ProxyL() const
	{
	__TEST_INVARIANT;

	CURIContainer& nextHop = NextHopL();
	if (iRegistrations.IsOutboundProxy(nextHop))
		{
		return CSIPMessageUtility::UriDescriptor(nextHop);
		}
	return KNullDesC8;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::SendRequestToNetworkL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::SendRequestToNetworkL()
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

	CUri8* proxy(NULL);
    const CSIPRouteHeader* route = iRegistrations.OutboundProxy(iRegisterId);
    if (route)
        {
        proxy = route->SIPAddress().URI().CloneAsUri8L();
		}
	CleanupStack::PushL(proxy);

	if (CSIPMessageUtility::IsAck(*iOutgoingMsg))
		{
    	__SIP_ASSERT_LEAVE(iTransmitter, KErrNotFound);
		iTransmitter->SendRequestL(static_cast<CSIPRequest&>(*iOutgoingMsg),
							   	   iRemoteAddr,
							   	   iForceUDP,
							   	   TransportParams(),
							   	   proxy,
							   	   this);
		CleanupStack::PopAndDestroy(proxy);
		}
	else
		{
		__SIP_ASSERT_LEAVE(iTransaction, KErrNotFound);
		iTransaction->SendRequestL(static_cast<CSIPRequest&>(*iOutgoingMsg),
							   	   iRemoteAddr,
							   	   iTransportProtocol,
							   	   iForceUDP,
							   	   TransportParams(),
							   	   proxy);
		CleanupStack::Pop(proxy);
		}
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::PassRespToTransactionOwnerL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::PassRespToTransactionOwnerL(CSIPResponse* aResp,
												   CUserAgent* aUserAgent,
												   CUserAgentTimer* aTimer)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);
    __SIP_ASSERT_LEAVE(!iFinalRespPassed, KErrAlreadyExists);

    if (aResp->ResponseCode() == 100 && !IsInviteUAC())
        {
        delete aResp;
        }
    else
    	{
        TRestoreUaState restoreState(iFinalRespPassed, aUserAgent, aTimer);
        CleanupStack::PushL(restoreState.CleanupItem());
    	iFinalRespPassed = CSIPMessageUtility::IsFinalResponse(*aResp);

		PassMsgToTransactionOwnerL(aResp);

        CleanupStack::Pop(); // cleanupItem
    	}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::FinalRespPassed
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::FinalRespPassed() const
	{
	__TEST_INVARIANT;
	return iFinalRespPassed;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::PassResponseToSIPSecL
// If no iObserver, ignore response and wait transaction to end.
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::PassResponseToSIPSecL(CSIPResponse& aResp)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg && iRemoteTarget, KErrNotFound);

	iResponseReceived = ETrue;
	TBool mustWait(EFalse);

	if (UseSIPSec() && CSIPMessageUtility::IsFinalResponse(aResp))  
		{
		CUri8* remoteTarget = iRemoteTarget->CloneAsUri8L();
		TRAPD(err, mustWait = iSIPSec.ResponseReceivedL(iTransportParams,
						 aResp,
					     static_cast<CSIPRequest&>(*iOutgoingMsg),
					     RegistrationId(),
					     TransactionId(),
	                     iRemoteAddr,
	                     CSIPMessageUtility::UriDescriptor(NextHopL()),              
	                     *remoteTarget,
	                   	 ProxyL(),
		                 const_cast<MSIPSecUser&>(*iObserver->SIPSecUser()),
		                 *this));
		delete remoteTarget;
		if( err != KErrNone )
			{
			User::Leave( KErrSIPForbidden );
			}
    	}

    __TEST_INVARIANT;
	return mustWait;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::ShouldUaTryAgainL
// If no iObserver ignore response. If send leaves, ConnectionMgr calls old
// transaction's MReceiverObserver::LeaveOccurred and cleanupItem stops UA.
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::ShouldUaTryAgainL(CSIPResponse* aResp,
										  const CUserAgentState& aResolve,
										  const CUserAgentState& aWaitResponse)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aResp, KErrArgument);

    TBool retry(EFalse);
	if (!FinalRespPassed() && iObserver)
		{
        TStopUserAgent stopUa(TransactionId(),
        					  iTransactionStore,
        					  KErrSIPForbidden);
        CleanupStack::PushL(stopUa.CleanupItem());

		TInt respCode = aResp->ResponseCode();
		if (respCode == 503 || respCode == 408)
			{
			retry =	TryNextAddressL(KErrNone, aResolve, aWaitResponse, aResp);
			}
		
		if ((respCode == 401 || respCode == 407 || respCode == 494) && UseSIPSec())
		    {
		    if(iSipSecError)
		        {
		        __SIP_LOG( "Request can't be resubmitted because of insufficient information" ) 
		        }
		    else if(FillSecurityParamsL())
		        {
		        if (iSIPSecCounter > SIPRequestUtility::EMaxForwardsValue)
				{
				User::Leave(KErrSIPForbidden);
				}
		        ++iSIPSecCounter;
		        UpdateAndSendRequestL(aWaitResponse);
		        delete aResp;
		        retry = ETrue;
		        }
		    }
        CleanupStack::Pop(); // cleanupItem
		}

	__TEST_INVARIANT;
	return retry;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::TransactionEndsWithoutFinalResponseL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::TransactionEndsWithoutFinalResponseL(TInt aReason,
										const CUserAgentState& aResolve,
										const CUserAgentState& aWaitResponse)
	{
	__TEST_INVARIANT;

	TStopUserAgent stopUa(TransactionId(), iTransactionStore, KErrNoMemory);
    CleanupStack::PushL(stopUa.CleanupItem());

	TBool retry(EFalse);
	if (!IsCanceled())
		{
		if (aReason == KErrTimedOut)
			{
			retry = HandleTimeoutL(aResolve, aWaitResponse);
			}
		if (aReason == KErrSIPTransportFailure ||
			aReason == KErrSIPICMPFailure)
			{
			retry = TryNextAddressL(aReason, aResolve, aWaitResponse);
			}
		}

    CleanupStack::Pop(); // cleanupItem
    if (!retry)
    	{
		Stop(aReason);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::HandleTimeoutL
// If SigComp used, delete the compartment and close TCP connection.
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::HandleTimeoutL(const CUserAgentState& aResolve,
									   const CUserAgentState& aWaitResponse)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

    if (CSIPMessageUtility::HasSigCompParam(NextHopL()))
        {
        UpdateTransportProtocol(*iOutgoingMsg);
        if (iTransportProtocol == SIPStrings::StringF(SipStrConsts::ETCP) ||
            iTransportProtocol == SIPStrings::StringF(SipStrConsts::ETLS))
            {
            iTransmitter->TcpDisconnect(iRemoteAddr, TransportParams());
            }
		CSIPURI* uri = NextHopL().SIPURI();
		__SIP_ASSERT_LEAVE(uri, KErrNotFound);

        iSigComp.SendFailedL(iTransportParams.CompartmentId());
        iTransportParams.SetCompartmentId(0);
        uri->DeleteParam(SIPStrings::StringF(SipStrConsts::EComp));
        SIPRequestUtility::FillRouteAndRequestUriL(
        							static_cast<CSIPRequest&>(*iOutgoingMsg),
									*iRouteSet,
									*iRemoteTarget);
        UpdateAndSendRequestL(aWaitResponse);

        __TEST_INVARIANT;
        return ETrue;
        }

    return TryNextAddressL(KErrTimedOut, aResolve, aWaitResponse);
    }

// -----------------------------------------------------------------------------
// CUserAgentClient::IgnoreResponseRetransmissionsL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::IgnoreResponseRetransmissionsL()
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(!iTransaction, KErrSIPInvalidTransactionState);

    RemoveFromStore();
    CleanupStack::PushL(this);

    CTransactionBase::TTransactionType taType =
        CTransactionBase::KClientTransaction;
    if (IsInviteUAC())
        {
        taType = CTransactionBase::KClientInviteTransaction;
        }

    // Set message NULL, to prevent responses being routed to UAC
    iTransactionStore.AddL(TransactionId(), this, NULL, NULL, taType);
    CleanupStack::Pop(this);

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CUserAgentClient::CancelSIPSecRequest
// No __TEST_INVARIANT as destructor may lead here.
// -----------------------------------------------------------------------------
//
void CUserAgentClient::CancelSIPSecRequest()
    {
    iIgnoreSIPSecCallback = ETrue;
    iSIPSec.CancelPendingOperations(*this);
    iIgnoreSIPSecCallback = EFalse;
    }

// -----------------------------------------------------------------------------
// CUserAgentClient::TryNextAddressL
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::TryNextAddressL(TInt aError,
										const CUserAgentState& aResolve,
										const CUserAgentState& aWaitResponse,
										CSIPResponse* aResp)
	{
	__TEST_INVARIANT;
	__SIP_INT_LOG1( "CUserAgentClient TryNextAddressL Error",
					aError )
	__SIP_ASSERT_LEAVE((aError != KErrNone && !aResp) ||
					   (aError == KErrNone && aResp), KErrArgument);

	if (SelectNextRemoteAddressL())
		{
		UpdateAndSendRequestL(aWaitResponse);
		delete aResp;
		return ETrue;
		}

	if (iResolvingResults->ContinueResolvingCurrentUri())
		{
		__SIP_ASSERT_LEAVE(!iIncomingMsg, KErrAlreadyExists);

		UpdateAndResolveL(ETrue, aResolve);
		iLastError = aError;
		iIncomingMsg = aResp;
		return ETrue;
		}

    if (iLastError != KErrNone)
        {
        aError = iLastError;
        }
    
	// If a stored response exists, next hop has responded.
	// Stateless proxy can result KErrTimedOut if its next hop doesn't respond,
	// even if the proxy is ok. So URIFailed isn't used for KErrTimedOut.	
	if ((aError == KErrSIPTransportFailure ||
		 aError == KErrSIPResolvingFailure ||
		 aError == KErrSIPICMPFailure) &&
		 !iIncomingMsg && !iResponseReceived)
		{
		iRegistrations.URIFailed(NextHopL());
		// Registrations may've used ClearTransactionOwner, stopping UA
		if (HasStopped())
			{
			return EFalse;
			}
		}

	TBool retry = TryOutboundProxyL(aResolve);
	if (retry)
		{
		delete aResp;
		}
	__TEST_INVARIANT;
	return retry;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::TryOutboundProxyL
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::TryOutboundProxyL(const CUserAgentState& aResolve)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iRouteSet && iRemoteTarget && iRequestUri, KErrNotFound);

    const CSIPRouteHeader* proxy = iRegistrations.OutboundProxy(iRegisterId);
	if (iRouteSet->PreconfigRouteExists() &&
        !iOutboundProxyHasBeenTried && proxy)
		{
        const CURIContainer& proxyUri = proxy->SIPAddress().URI();

      	if ( !((NextHopL() == proxyUri) || proxyUri == *iRequestUri) )
            {
			iRouteSet->AddToBeginningL(*proxy, EFalse /* append */);
			SIPRequestUtility::FillRouteAndRequestUriL(
							static_cast<CSIPRequest&>(*iOutgoingMsg),
							*iRouteSet,
							*iRemoteTarget);
			UpdateAndResolveL(ETrue, aResolve);
			iOutboundProxyHasBeenTried = ETrue;

			__TEST_INVARIANT;
			return ETrue;
            }
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::DetachCurrentTransactionL
// Request must've been updated or there'd be 2 transactions with a same branch
// -----------------------------------------------------------------------------
//
void CUserAgentClient::DetachCurrentTransactionL()
	{
	__TEST_INVARIANT;

	if (iTransaction)
		{
		if (iTransaction->HasTerminated())
			{
            // Cancel transaction's send so new transaction can send
			iTransmitter->Cancel();
            RequestDeletionOfTransactionL();
			RemoveFromStore();
			}
		else
			{			
			TTransactionId newId = iTransactionStore.NewTransactionId();
            PrepareTxForNewRequestL(newId);
			User::LeaveIfError(iTransactionStore.UpdateTransactionId(
										            TransactionId(), newId));
			if (IsInviteUAC())
				{
				__SIP_INT_LOG2( "TU taID changed, old -> new",
				                TransactionId(), newId )
                }
			iTransaction->DetachFromUserAgent();
			iTransaction->ClearSendBuffer();
			iTransaction = NULL;

			iTransactionStore.ClearUserAgent(newId);
			}

		CleanupStack::PushL(this);
        __SIP_ASSERT_LEAVE(!iTransaction, KErrAlreadyExists);
        __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

		iTransactionStore.AddL(TransactionId(),
				   this,
				   iTransaction,
				   iOutgoingMsg,
                   CSIPMessageUtility::TransactionType(*iOutgoingMsg, EFalse));
		CleanupStack::Pop(this);
		}
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::UpdateAndSendRequestL
// -----------------------------------------------------------------------------
//
void
CUserAgentClient::UpdateAndSendRequestL(const CUserAgentState& aWaitResponse)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

	UpdateViaAndCSeqL();
	if (!CSIPMessageUtility::IsAck(*iOutgoingMsg))
		{
		DetachCurrentTransactionL();
		CreateTransactionL();
		}

	UpdateInfoToStoreL(static_cast<CSIPRequest*>(iOutgoingMsg));
	SendRequestToNetworkL();
	ChangeState(aWaitResponse);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::UpdateAndResolveL
// -----------------------------------------------------------------------------
//
void CUserAgentClient::UpdateAndResolveL(TBool aCheckSigComp,
										 const CUserAgentState& aResolve)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

	UpdateViaAndCSeqL();	
	DetachCurrentTransactionL();
	UpdateInfoToStoreL(static_cast<CSIPRequest*>(iOutgoingMsg));
	ResolveNextHopL(aCheckSigComp);
	ChangeState(aResolve);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::StopWithLastError
// -----------------------------------------------------------------------------
//
void CUserAgentClient::StopWithLastError()
	{
	__TEST_INVARIANT;

	if (iIncomingMsg)
		{
		CSIPMessage* msg = iIncomingMsg;
		iIncomingMsg = NULL;
		TRAPD(err, PassMsgToTransactionOwnerL(msg));
		if (err != KErrNone)
			{
			delete msg;
			}
		}
	else
		{
		if (iLastError == KErrNone)
			{
			// Resolving URI for the first time fails
			iLastError = KErrSIPResolvingFailure;
			}
		}
	Stop(iLastError);
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::DetachOutgoingMsg
// If transaction can't proceed w/o the message, terminate it silently.
// -----------------------------------------------------------------------------
//
void CUserAgentClient::DetachOutgoingMsg()
	{
	__TEST_INVARIANT;

	iOutgoingMsg = NULL;
	iTransmitter->Cancel();
    CancelSIPSecRequest();

	if (iTransaction && !iTransaction->ClearSendBuffer())
		{
		iTransaction->DetachFromUserAgent();
		iTransaction->Terminated();
		}
	else
		{
		if (State().CanContinueWithoutOutgoingMsg(iFinalRespPassed))
			{
			return;
			}
		}

	Stop(KErrNone);
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::UseSIPSec
// -----------------------------------------------------------------------------
//
TBool CUserAgentClient::UseSIPSec() const
	{
	__TEST_INVARIANT;
	return (iObserver && 
	        iObserver->SIPSecUser() && 
	        !iObserver->SIPSecUser()->ByPassSIPSec());
	}

// -----------------------------------------------------------------------------
// CUserAgentClient::NextHopIP
// -----------------------------------------------------------------------------
//
const TInetAddr& CUserAgentClient::NextHopIP() const
    {
    return iRemoteAddr;
    }

// -----------------------------------------------------------------------------
// CUserAgentClient::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CUserAgentClient::__DbgTestInvariant() const
	{
	if ((iOutgoingMsg && !iOutgoingMsg->IsRequest()) ||
		(iIncomingMsg && iIncomingMsg->IsRequest()) ||
        (iSIPSecCounter > SIPRequestUtility::EMaxForwardsValue + 1) ||        
        !iResolvingResults)
		{
		User::Invariant();
		}
	}
