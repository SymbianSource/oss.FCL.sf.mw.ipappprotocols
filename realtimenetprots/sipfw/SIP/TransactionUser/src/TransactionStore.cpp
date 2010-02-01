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
// Name          : TransactionStore.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include <centralrepository.h>
#include "sipprivatecrkeys.h"
#include "SipAssert.h"
#include "sipfromheader.h"
#include "sipviaheader.h"
#include "sipcseqheader.h"
#include "siphostport.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "CTransactionStore.h"
#include "InviteUAS.h"
#include "SIPMessageUtility.h"


// -----------------------------------------------------------------------------
// CTransactionStore::NewL
// -----------------------------------------------------------------------------
//
CTransactionStore* CTransactionStore::NewL()
	{	
	CTransactionStore* self = new (ELeave) CTransactionStore();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::CTransactionStore
// -----------------------------------------------------------------------------
//
CTransactionStore::CTransactionStore() :
	iList(_FOFF(CTransactionInfo, iLink)),
	iTransactionIdCounter(KMaxTransactionId),
	iMaxServerTransactions(KMaxTInt)
	{
	}
// -----------------------------------------------------------------------------
// CTransactionStore::ConstructL
// Read upper limit of simultaneous server transactions from central repository
// -----------------------------------------------------------------------------
//
void CTransactionStore::ConstructL()
	{
	TInt maxServerTransactions(0);
	CRepository* repository = CRepository::NewL(KCRUidSIP);
	if ((repository->Get(KSIPMaxPendingServerTransactions,
						 maxServerTransactions) == KErrNone) &&
		maxServerTransactions >= 0)
		{
		iMaxServerTransactions = maxServerTransactions;
		}
	delete repository;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::~CTransactionStore
// -----------------------------------------------------------------------------
//
CTransactionStore::~CTransactionStore()
	{
	CTransactionInfo* taInfo = iList.First();
	while (iList.IsFirst(taInfo) && !iList.IsEmpty())
		{
		DeleteItem(taInfo);		
		taInfo = iList.First();
		}
	}

// -----------------------------------------------------------------------------
// CTransactionStore::DeleteItem
// -----------------------------------------------------------------------------
//
void CTransactionStore::DeleteItem(CTransactionInfo* aItem)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(aItem, KErrArgument);
	
	RemoveListItem(*aItem);
	aItem->FreeContents();
	delete aItem;
	
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::NewTransactionId
// -----------------------------------------------------------------------------
//
TTransactionId CTransactionStore::NewTransactionId()
	{
	__TEST_INVARIANT;

	if (iTransactionIdCounter < KMaxTransactionId)
		{
		++iTransactionIdCounter;
		}
	else
		{
		iTransactionIdCounter = KMinTransactionId;
		}

	__TEST_INVARIANT;
	return iTransactionIdCounter;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::AddL
// -----------------------------------------------------------------------------
//
void CTransactionStore::AddL(TTransactionId aTransactionId,
							 CUserAgent* aUserAgent,
							 CTransactionBase* aTransaction,
							 CSIPMessage* aMsg,
							 CTransactionBase::TTransactionType aType)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aTransactionId != KEmptyTransactionId, KErrArgument);
	__SIP_ASSERT_LEAVE(aUserAgent, KErrArgument);

    CTransactionInfo* newItem =
    	CTransactionInfo::NewL(aType, aUserAgent->TransportParams().IapId());
    CleanupStack::PushL(newItem);
	newItem->UpdateMessageHeadersL(aMsg);
    CleanupStack::Pop(newItem);
	if (aUserAgent->IsUAS())
		{
		++iServerTransactionCount;
		}

    iList.AddLast(*newItem);

	// Attach UA when can't leave
	newItem->SetUserAgent(aUserAgent);
	newItem->SetTransaction(aTransaction);
	newItem->SetTransactionId(aTransactionId);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::StoreRecordRouteHeadersL
// -----------------------------------------------------------------------------
//
void CTransactionStore::StoreRecordRouteHeadersL(TTransactionId aTransactionId,
												 CSIPRequest& aReq)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
    __SIP_ASSERT_LEAVE(taInfo, KErrNotFound);
	taInfo->StoreRecordRouteHeadersL(aReq);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::FreeRecordRouteHeaders
// -----------------------------------------------------------------------------
//
void CTransactionStore::FreeRecordRouteHeaders(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	__SIP_ASSERT_RETURN(taInfo, KErrNotFound);
	taInfo->FreeRecordRouteHeaders();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::StoreContactHeadersL
// -----------------------------------------------------------------------------
//
void CTransactionStore::StoreContactHeadersL(TTransactionId aTransactionId,
											 CSIPRequest& aReq)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	__SIP_ASSERT_LEAVE(taInfo, KErrNotFound);
	taInfo->StoreContactHeadersL(aReq);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::FreeContactHeaders
// -----------------------------------------------------------------------------
//
void CTransactionStore::FreeContactHeaders(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	__SIP_ASSERT_RETURN(taInfo, KErrNotFound);
	taInfo->FreeContactHeaders();

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::Search
// UAC sets the magic cookie, so a response without it is broken and dropped.
// Incoming request without branch or ACK to a 2xx: search using headers.
// -----------------------------------------------------------------------------
//
MReceiverObserver* CTransactionStore::Search(CSIPMessage& aMsg)
	{
	__TEST_INVARIANT;

	CTransactionBase::TTransactionType taType =
        CSIPMessageUtility::TransactionType(aMsg, ETrue);

    if (CSIPMessageUtility::HasViaMagicCookie(aMsg))
        {
        MReceiverObserver* receiver = SearchByBranch(aMsg, taType, NULL);
        if (receiver || !CSIPMessageUtility::IsAck(aMsg))
            {
            __TEST_INVARIANT;
			return receiver;
            }
        }

	if (aMsg.IsRequest())
		{
		return SearchServerByHeaders(
            static_cast<CSIPRequest&>(aMsg), taType, NULL);
		}

    __TEST_INVARIANT;
	return NULL;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::SearchUasToCancel
// -----------------------------------------------------------------------------
//
CUserAgentServer*
CTransactionStore::SearchUasToCancel(CSIPRequest& aCancel,
									 const CUserAgent& aCancelUAS)
	{
	__TEST_INVARIANT;

	CTransactionBase::TTransactionType taType =
        CSIPMessageUtility::TransactionType(aCancel, ETrue);
	MReceiverObserver* receiver(NULL);

	if (CSIPMessageUtility::HasViaMagicCookie(aCancel))
		{
		receiver = SearchByBranch(aCancel, taType, &aCancelUAS);
		}
	else
		{
		receiver = SearchServerByHeaders(aCancel, taType, &aCancelUAS);
		}

	CUserAgentServer* uas = static_cast<CUserAgentServer*>(receiver);

	// Must not return the same UAS that initiated the search
	__ASSERT_DEBUG(uas != &aCancelUAS,
				   User::Panic(_L("TaStore:SearchUasToCancel"), KErrGeneral));
	__TEST_INVARIANT;
	return uas;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::SearchServerByHeaders
// Use CompareTransactionTypes before CompareHeaders, so CompareHeaders is not
// used on UAC's taInfo as it has no sent-by part in Via. UAC uses magic cookie.
// -----------------------------------------------------------------------------
//
MReceiverObserver*
CTransactionStore::SearchServerByHeaders(CSIPRequest& aReq,
									 CTransactionBase::TTransactionType aType,
									 const CUserAgent* aUserAgent)
	{
	__TEST_INVARIANT;

	MReceiverObserver* res(NULL);
	if (RequiredHeadersPresent(aReq))
		{
	    TBool searchCanceledUas = (aUserAgent != NULL);
	    TSglQueIter<CTransactionInfo> iter(iList);

	    for (CTransactionInfo* taInfo = iter++; !res && taInfo; taInfo = iter++)
		    {
		    if (taInfo->RequiredHeadersPresent() &&
			    CompareTransactionTypes(*taInfo, aType, searchCanceledUas) &&
				taInfo->CompareHeaders(aReq) &&
				taInfo->CompareRequestLine(*(aReq.RequestURI()),
										   aReq.Method(),
										   searchCanceledUas))
				{
				res = CheckResult(*taInfo, aUserAgent);
				}
		    }
        }

	__TEST_INVARIANT;
	return res;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::RequiredHeadersPresent
// -----------------------------------------------------------------------------
//
TBool CTransactionStore::RequiredHeadersPresent(CSIPMessage& aMsg) const
	{
	__TEST_INVARIANT;
	return aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader))  &&
		   aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)) &&
		   aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::EToHeader))   &&
		   aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)) &&
		   (!aMsg.IsRequest() || static_cast<CSIPRequest&>(aMsg).RequestURI());
	}

// -----------------------------------------------------------------------------
// CTransactionStore::SearchByBranch
// -----------------------------------------------------------------------------
//
MReceiverObserver*
CTransactionStore::SearchByBranch(CSIPMessage& aMsg,
								  CTransactionBase::TTransactionType aType,
								  const CUserAgent* aUserAgent)
	{
	__TEST_INVARIANT;

    CSIPViaHeader* topVia = CSIPMessageUtility::TopVia(aMsg);
    __ASSERT_DEBUG(topVia != NULL,
        		   User::Panic(_L("TaStore:SearchByBranch"), KErrArgument));

    TBool searchCanceledUas = (aUserAgent != NULL);
	RStringF method = CSIPMessageUtility::MessageMethod(aMsg);
	RStringF branch = SIPStrings::StringF(SipStrConsts::EBranch);		

	MReceiverObserver* result(NULL);
	TSglQueIter<CTransactionInfo> iter(iList);
	for (CTransactionInfo* taInfo = iter++; taInfo && !result; taInfo = iter++)
		{
		// Skip transactions not associated with a SIP request.
        // Type must match, e.g. a client transaction receives only responses.
		if (taInfo->RequiredHeadersPresent() &&
			CompareTransactionTypes(*taInfo, aType, searchCanceledUas))
			{
			CSIPViaHeader& storedVia = taInfo->TopVia();
			if (storedVia.HasParam(branch) &&
				(storedVia.ParamValue(branch) == topVia->ParamValue(branch)) &&
				(searchCanceledUas || taInfo->CompareMethod(method)) &&
				(aType == CTransactionBase::KClientTransaction ||
				 aType == CTransactionBase::KClientInviteTransaction ||
                 // Server transaction: sent-by of top via must match.
                 // Client transaction: sent-by can be missing from storedVia.
				 (storedVia.SentByHostPort() == topVia->SentByHostPort())))
				{
				result = IgnoreAckTo2xx(aMsg, *taInfo,	aUserAgent);
				}
			}
		}

	__TEST_INVARIANT;
	return result;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::CompareTransactionTypes
// If searching UAS to cancel, accept only server transactions.
// -----------------------------------------------------------------------------
//
TBool CTransactionStore::CompareTransactionTypes(const CTransactionInfo& aInfo,
								   CTransactionBase::TTransactionType aType,
								   TBool aSearchCanceledUas) const
	{
    __TEST_INVARIANT;

	if (aSearchCanceledUas)
		{
		__ASSERT_DEBUG(aType == CTransactionBase::KServerTransaction,
					   User::Panic(_L("TaStore:CompTaTypes"), KErrArgument));		
		return aInfo.CompareType(CTransactionBase::KServerTransaction) ||
               aInfo.CompareType(CTransactionBase::KServerInviteTransaction);
		}

	return aInfo.CompareType(aType);
	}

// -----------------------------------------------------------------------------
// CTransactionStore::IgnoreAckTo2xx
// Ignore Via branch of ACK to a 2xx. INVITE may've forked, one UAS sent 2xx,
// other 4xx. Even if UAC sends ACK to both with a same branch as in INVITE, UAS
// must ignore the ACK targeted to the other UAS. To-tag is the difference.
// -----------------------------------------------------------------------------
//
MReceiverObserver*
CTransactionStore::IgnoreAckTo2xx(const CSIPMessage& aMsg,
                                  CTransactionInfo& aInfo,
								  const CUserAgent* aUserAgent) const
    {
    __TEST_INVARIANT;

	if (CSIPMessageUtility::IsAck(aMsg) && !aUserAgent && aInfo.UserAgent())
        {
        // aMsg is ACK so UA must be InviteUAS
        __ASSERT_DEBUG(aInfo.UserAgent()->IsUAS() &&
            aInfo.CompareType(CTransactionBase::KServerInviteTransaction),
            User::Panic(_L("TaStore:IgnoreAckTo2xx"), KErrGeneral));

        if (CInviteUAS::Ptr(*(aInfo.UserAgent())).IsSending2xx())
            {
            return NULL;
            }
        }

    return CheckResult(aInfo, aUserAgent);
    }

// -----------------------------------------------------------------------------
// CTransactionStore::CheckResult
// -----------------------------------------------------------------------------
//
MReceiverObserver*
CTransactionStore::CheckResult(CTransactionInfo& aInfo,
							   const CUserAgent* aUserAgent) const
	{
    __TEST_INVARIANT;

	if (!aUserAgent)
		{
		__ASSERT_DEBUG(aInfo.Receiver(),
					   User::Panic(_L("TaStore:ChkResult"), KErrNotFound));
		return aInfo.Receiver();
		}

	// Ignore result if it is the same UA that makes the search
    if (aInfo.UserAgent() == aUserAgent)
        {
        return NULL;
        }
	return aInfo.UserAgent();
	}

// -----------------------------------------------------------------------------
// CTransactionStore::SearchById
// -----------------------------------------------------------------------------
//
CUserAgent* CTransactionStore::SearchById(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		__ASSERT_DEBUG(taInfo->UserAgent(),
					   User::Panic(_L("TaStore:SearchById"), KErrNotFound));
		return taInfo->UserAgent();
		}

	return NULL;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::FindTransactionInfo
// -----------------------------------------------------------------------------
//
CTransactionInfo*
CTransactionStore::FindTransactionInfo(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;
    __ASSERT_DEBUG(aTransactionId != KEmptyTransactionId,
        		   User::Panic(_L("TaStore:FindTaInfo"), KErrArgument));

    TSglQueIter<CTransactionInfo> iter(iList);
    for (CTransactionInfo* taInfo = iter++; taInfo; taInfo = iter++)
    	{
    	if (taInfo->TransactionId() == aTransactionId)
			{
			__TEST_INVARIANT;
			return taInfo;
			}
    	if (taInfo->TransactionId() == KEmptyTransactionId)
			{
            // TransactionInfo marked for deletion. Delete it.
			DeleteItem(taInfo);			
			}
    	}

	__TEST_INVARIANT;
	return NULL;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::IcmpErrorL
// -----------------------------------------------------------------------------
//
void CTransactionStore::IcmpErrorL(const TInetAddr& aAddress,
								   CSipConnectionMgr::TICMPError aError)
	{
	__TEST_INVARIANT;

	TSglQueIter<CTransactionInfo> iter(iList);
	for (CTransactionInfo* taInfo = iter++; taInfo; taInfo = iter++)
		{
		if (taInfo->Receiver())
			{
			taInfo->Receiver()->IcmpErrorL(aAddress, aError);
			}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::ClearUserAgent
// -----------------------------------------------------------------------------
//
void CTransactionStore::ClearUserAgent(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		taInfo->SetUserAgent(NULL);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::ClearTransaction
// -----------------------------------------------------------------------------
//
void CTransactionStore::ClearTransaction(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		taInfo->SetTransaction(NULL);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::UpdateTransactionId
// -----------------------------------------------------------------------------
//
TInt CTransactionStore::UpdateTransactionId(TTransactionId aOldTransactionId,
										    TTransactionId aNewTransactionId)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_RETURN_VALUE(!(aOldTransactionId == aNewTransactionId),
        					  KErrArgument);
    TInt status(KErrNotFound);
	CTransactionInfo* taInfo = FindTransactionInfo(aOldTransactionId);
	if (taInfo)
		{
		taInfo->SetTransactionId(aNewTransactionId);
		status = KErrNone;
		}

    __TEST_INVARIANT;
	return status;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::UpdateL
// -----------------------------------------------------------------------------
//
void CTransactionStore::UpdateL(TTransactionId aTransactionId,
								CTransactionBase* aTransaction,
								CSIPMessage* aMsg)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	__ASSERT_ALWAYS(taInfo, User::Leave(KErrNotFound));
	taInfo->SetTransaction(aTransaction);
	taInfo->UpdateMessageHeadersL(aMsg);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::UpdateToTagL
// -----------------------------------------------------------------------------
//
void
CTransactionStore::UpdateToTagL(TTransactionId aTransactionId, RStringF aTag)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	__ASSERT_ALWAYS(taInfo, User::Leave(KErrNotFound));
	taInfo->UpdateToTagL(aTag);

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::Remove
// -----------------------------------------------------------------------------
//
void CTransactionStore::Remove(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		RemoveListItem(*taInfo);
		delete taInfo;
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::ClearTransactionOwner
// -----------------------------------------------------------------------------
//
TInt
CTransactionStore::ClearTransactionOwner(const MTransactionOwner* aObserver)
	{
	__TEST_INVARIANT;
    __SIP_ASSERT_RETURN_VALUE(aObserver != NULL, KErrArgument);

	TInt status(KErrNotFound);
	TSglQueIter<CTransactionInfo> iter(iList);
    CUserAgent* ua(NULL);

	for (CTransactionInfo* taInfo = iter++; taInfo; taInfo = iter++)
		{
		ua = taInfo->UserAgent();
		if (ua && ua->TransactionOwner() == aObserver)
			{
			ua->ClearTransactionOwner();
			status = KErrNone;
			// Continue the loop as many transactions may use the same aObserver
			}
		}

	__TEST_INVARIANT;
	return status;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::EndResolvingTransactions
// Detached transactions (ua == NULL), are never resolving.
// -----------------------------------------------------------------------------
//
void CTransactionStore::EndResolvingTransactions(TUint32 aIapId, TInt aReason)
	{
	__TEST_INVARIANT;

	TSglQueIter<CTransactionInfo> iter(iList);
	for (CTransactionInfo* taInfo = iter++; taInfo; taInfo = iter++)
		{
        CUserAgent* ua = taInfo->UserAgent();
        if (ua && !ua->IsUAS())
            {
			if ((ua->TransportParams().IapId() == aIapId) && ua->IsResolving())
				{
				ua->Stop(aReason);
				}
            }
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::IsMergedRequest
// -----------------------------------------------------------------------------
//
TBool CTransactionStore::IsMergedRequest(const CUserAgent& aUserAgent,
										 CSIPRequest& aReq)
	{
	__TEST_INVARIANT;

	CSIPFromHeader* from = aReq.From();
	__ASSERT_DEBUG(from, User::Panic(_L("TaStore:IsMerg !from"), KErrArgument));

	if (!aReq.HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)) ||
		!aReq.HasHeader(SIPStrings::StringF(SipStrConsts::ECallIDHeader)) ||
		!aReq.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)) ||
		!from->HasParam(SIPStrings::StringF(SipStrConsts::ETag)))
		{
		return EFalse;
		}

    CSIPCSeqHeader* cseq = aReq.CSeq();
    __ASSERT_DEBUG(cseq, User::Panic(_L("TaStore:IsMerg !cseq"), KErrArgument));

	TBool merged(EFalse);
	TSglQueIter<CTransactionInfo> iter(iList);
	for (CTransactionInfo* taInfo = iter++; taInfo && !merged; taInfo = iter++)
		{
		merged = taInfo->IsMergedRequest(aUserAgent,
										 *from,
										 *aReq.CallID(),
										 cseq->Seq(),
										 cseq->Method());
		}

	__TEST_INVARIANT;
	return merged;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::CopyHeadersToResponseL
// -----------------------------------------------------------------------------
//
void CTransactionStore::CopyHeadersToResponseL(TTransactionId aTransactionId,
											   CSIPResponse& aResp)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_RETURN(aTransactionId != KEmptyTransactionId, KErrArgument);
	__SIP_ASSERT_RETURN(
		!aResp.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader)),
		KErrArgument);

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		taInfo->CopyToFromCallIDCSeqToMsgL(aResp);
		taInfo->CopyViaHeadersToMsgL(aResp);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::CopyHeadersToRequestL
// -----------------------------------------------------------------------------
//
void CTransactionStore::CopyHeadersToRequestL(TTransactionId aTransactionId,
										      CSIPRequest& aReq,
											  TBool aCopyRequestURI,
										      TBool aCopyViaHeaders)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aTransactionId != KEmptyTransactionId, KErrArgument);

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		taInfo->CopyToFromCallIDCSeqToMsgL(aReq);

		if (aCopyRequestURI)
			{
			taInfo->CopyRequestUriToRequestL(aReq);
			}

		if (aCopyViaHeaders)
			{
			taInfo->CopyViaHeadersToMsgL(aReq);
			}
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::TransactionHeadersL
// -----------------------------------------------------------------------------
//
MTransactionHeaders*
CTransactionStore::TransactionHeadersL(TTransactionId aTransactionId)
	{
	__TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	if (taInfo)
		{
		return taInfo->TransactionHeadersL();
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::RandomTaInfoL
// -----------------------------------------------------------------------------
//
void CTransactionStore::RandomTaInfoL(TTransactionId& aTaId,
								      CUserAgent** aUserAgent,
								      CSIPMessage** aMsg)
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(aUserAgent && aMsg, KErrArgument);

	TSglQueIter<CTransactionInfo> iter(iList);
	for (CTransactionInfo* taInfo = iter++; taInfo; taInfo = iter++)
		{
		if (aTaId == KEmptyTransactionId)
			{
			aTaId = taInfo->TransactionId();
			}

		if (taInfo->UserAgent())
			{
			*aUserAgent = taInfo->UserAgent();
			*aMsg = taInfo->BuildRequestFromStoredInfoL();

            __TEST_INVARIANT;
			return;
			}
		}

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::RequestMethod
// -----------------------------------------------------------------------------
//
RStringF CTransactionStore::RequestMethod(TTransactionId aTransactionId)
	{
    __TEST_INVARIANT;

	CTransactionInfo* taInfo = FindTransactionInfo(aTransactionId);
	__ASSERT_DEBUG(taInfo, User::Panic(_L("TaStore:ReqMethod"), KErrNotFound));
	return taInfo->RequestMethod();
	}

// -----------------------------------------------------------------------------
// CTransactionStore::RemoveItemsByIapId
// -----------------------------------------------------------------------------
//
void CTransactionStore::RemoveItemsByIapId(TUint32 aIapId)
    {
    __TEST_INVARIANT;

	TSglQueIter<CTransactionInfo> iter(iList);
    for (CTransactionInfo* taInfo = iter++; taInfo; taInfo = iter++)
        {
        if (taInfo->IapId() == aIapId)
            {
            DeleteItem(taInfo);
            }
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CTransactionStore::AllowMoreServerTransactions
// -----------------------------------------------------------------------------
//
TBool CTransactionStore::AllowMoreServerTransactions() const
	{
	__TEST_INVARIANT;
	return iServerTransactionCount < iMaxServerTransactions;
	}

// -----------------------------------------------------------------------------
// CTransactionStore::RemoveListItem
// -----------------------------------------------------------------------------
//
void CTransactionStore::RemoveListItem(CTransactionInfo& aItem)
	{
	__TEST_INVARIANT;
	if (aItem.CompareType(CTransactionBase::KServerTransaction) ||
        aItem.CompareType(CTransactionBase::KServerInviteTransaction))
		{
		--iServerTransactionCount;
		}
	iList.Remove(aItem);
	__TEST_INVARIANT;
	}
// CTransactionStore::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CTransactionStore::__DbgTestInvariant() const
	{
	if (iTransactionIdCounter < KMinTransactionId)
		{
		User::Invariant();
		}
	}

