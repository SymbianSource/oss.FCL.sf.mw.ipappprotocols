// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TransactionInfo.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "SipAssert.h"
#include "uricontainer.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipviaheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "siprecordrouteheader.h"
#include "sipcontactheader.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "CUserAgent.h"
#include "TransactionInfo.h"
#include "CTransactionHeaders.h"
#include "SIPMessageUtility.h"

// -----------------------------------------------------------------------------
// CTransactionInfo::NewL
// -----------------------------------------------------------------------------
//
CTransactionInfo*
CTransactionInfo::NewL(CTransactionBase::TTransactionType aType, TUint32 aIapId)
	{
	return new (ELeave) CTransactionInfo(aType, aIapId);	
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CTransactionInfo
// -----------------------------------------------------------------------------
//
CTransactionInfo::CTransactionInfo(CTransactionBase::TTransactionType aType,
                                   TUint32 aIapId) :
	iTransactionType(aType),
    iIapId(aIapId)
#ifdef CPPUNIT_TEST
    , iViaHeaders(1),
    iRecordRoute(1),
    iContact(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::~CTransactionInfo
// -----------------------------------------------------------------------------
//
CTransactionInfo::~CTransactionInfo()
	{
	ClearMessageHeaders();	
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::FreeContents
// UA owns transaction. Delete it only if it is detached from UA, or it differs
// from UA's transaction (=should be impossible).
// -----------------------------------------------------------------------------
//
void CTransactionInfo::FreeContents()
	{
	if (!iUserAgent || (iUserAgent->Transaction() != iTransaction))
		{
		delete iTransaction;
		iTransaction = NULL;
		}

	delete iUserAgent;
	iUserAgent = NULL;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::ClearMessageHeaders
// -----------------------------------------------------------------------------
//
void CTransactionInfo::ClearMessageHeaders()
	{
	delete iRequestURI;
	iRequestURI = NULL;

	delete iTo;
	iTo = NULL;

	delete iFrom;
	iFrom = NULL;

	delete iCallID;
	iCallID = NULL;

	iMethod.Close();
	iCSeq = 0;
    iViaHeaders.ResetAndDestroy();
	iRecordRoute.ResetAndDestroy();
	iContact.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::Receiver
// -----------------------------------------------------------------------------
//
MTransactionReceiver* CTransactionInfo::Receiver()
	{
	if (iTransaction)
		{
		return iTransaction;
		}
	return iUserAgent;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::UserAgent
// -----------------------------------------------------------------------------
//
CUserAgent* CTransactionInfo::UserAgent()
	{
	return iUserAgent;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::SetUserAgent
// -----------------------------------------------------------------------------
//
void CTransactionInfo::SetUserAgent(CUserAgent* aUserAgent)
	{
	iUserAgent = aUserAgent;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::SetTransaction
// -----------------------------------------------------------------------------
//
void CTransactionInfo::SetTransaction(CTransactionBase* aTransaction)
	{
	iTransaction = aTransaction;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::TransactionId
// -----------------------------------------------------------------------------
//
TTransactionId CTransactionInfo::TransactionId() const
	{
	return iTransactionId;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::SetTransactionId
// -----------------------------------------------------------------------------
//
void CTransactionInfo::SetTransactionId(TTransactionId aTransactionId)
	{
	iTransactionId = aTransactionId;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::IapId
// -----------------------------------------------------------------------------
//
TUint32 CTransactionInfo::IapId() const
    {
    return iIapId;
    }

// -----------------------------------------------------------------------------
// CTransactionInfo::TopVia
// -----------------------------------------------------------------------------
//
CSIPViaHeader& CTransactionInfo::TopVia() const
	{
	__ASSERT_ALWAYS(iViaHeaders.Count() > 0,
					User::Panic(_L("TaInfo:TopVia"), KErrNotFound));
	return *iViaHeaders[0];
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::UpdateMessageHeadersL
// -----------------------------------------------------------------------------
//
void CTransactionInfo::UpdateMessageHeadersL(CSIPMessage* aMsg)
	{
	if (aMsg)
		{
		ClearMessageHeaders();

		if (aMsg->IsRequest())
			{
			CURIContainer* uri = static_cast<CSIPRequest*>(aMsg)->RequestURI();
			if (uri)
				{
				iRequestURI = CURIContainer::NewL(*uri);
				}
			}

		iMethod.Close();
		iMethod = CSIPMessageUtility::MessageMethod(*aMsg).Copy();

		iTo = static_cast<CSIPToHeader*>
			(CSIPMessageUtility::CopyHeaderFromMsgL(*aMsg,
				SIPStrings::StringF(SipStrConsts::EToHeader)));
		iFrom = static_cast<CSIPFromHeader*>
			(CSIPMessageUtility::CopyHeaderFromMsgL(*aMsg,
                SIPStrings::StringF(SipStrConsts::EFromHeader)));
		iCallID = static_cast<CSIPCallIDHeader*>
			(CSIPMessageUtility::CopyHeaderFromMsgL(*aMsg,
				SIPStrings::StringF(SipStrConsts::ECallIDHeader)));

		if (aMsg->HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)))
			{
        	iCSeq = aMsg->CSeq()->Seq();
			}

		CopyViaHeadersFromMsgL(*aMsg);
		}
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::UpdateToTagL
// Server transaction has checked there is no To-tag yet.
// -----------------------------------------------------------------------------
//
void CTransactionInfo::UpdateToTagL(RStringF aTag)
	{
	__SIP_ASSERT_LEAVE(iTo, KErrNotFound);

	RStringF tag = SIPStrings::StringF(SipStrConsts::ETag);
    __SIP_ASSERT_LEAVE(!iTo->HasParam(tag), KErrAlreadyExists);

	iTo->SetParamL(tag, aTag);
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CopyViaHeadersFromMsgL
// Sent requests have empty host part, but response matching won't use sent-by.
// -----------------------------------------------------------------------------
//
void CTransactionInfo::CopyViaHeadersFromMsgL(CSIPMessage& aMsg)
	{
	const RStringF KVia = SIPStrings::StringF(SipStrConsts::EViaHeader);
	if (aMsg.HasHeader(KVia))
		{
		iViaHeaders.ResetAndDestroy();

		TSglQueIter<CSIPHeaderBase> iter = aMsg.Headers(KVia);
		for (CSIPHeaderBase* header = iter++; header; header = iter++)
			{
			CSIPViaHeader* via = dynamic_cast<CSIPViaHeader*>(header->CloneL());
			CleanupStack::PushL(via);
			iViaHeaders.AppendL(via);
			CleanupStack::Pop(via);
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::StoreRecordRouteHeadersL
// -----------------------------------------------------------------------------
//
void CTransactionInfo::StoreRecordRouteHeadersL(CSIPRequest& aReq)
	{
    __SIP_ASSERT_LEAVE(iRecordRoute.Count() == 0, KErrAlreadyExists);
	
	const RStringF KRecordRoute =
		SIPStrings::StringF(SipStrConsts::ERecordRouteHeader);
	if (aReq.HasHeader(KRecordRoute))
		{
		TSglQueIter<CSIPHeaderBase> iter = aReq.Headers(KRecordRoute);
		for (CSIPHeaderBase* header = iter++; header; header = iter++)
			{
			CSIPRecordRouteHeader* rr =
				static_cast<CSIPRecordRouteHeader*>(header->CloneL());
			CleanupStack::PushL(rr);
			iRecordRoute.AppendL(rr);
			CleanupStack::Pop(rr);
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::FreeRecordRouteHeaders
// -----------------------------------------------------------------------------
//
void CTransactionInfo::FreeRecordRouteHeaders()
	{
	iRecordRoute.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::StoreContactHeadersL
// -----------------------------------------------------------------------------
//
void CTransactionInfo::StoreContactHeadersL(CSIPRequest& aReq)
	{
    __SIP_ASSERT_LEAVE(iContact.Count() == 0, KErrAlreadyExists);

	const RStringF KContact = SIPStrings::StringF(SipStrConsts::EContactHeader);
	if (aReq.HasHeader(KContact))
		{
		TSglQueIter<CSIPHeaderBase> iter = aReq.Headers(KContact);
		for (CSIPHeaderBase* header = iter++; header; header = iter++)
			{
			CSIPContactHeader* contact =
				dynamic_cast<CSIPContactHeader*>(header->CloneL());
			CleanupStack::PushL(contact);
			iContact.AppendL(contact);
			CleanupStack::Pop(contact);
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::FreeContactHeaders
// -----------------------------------------------------------------------------
//
void CTransactionInfo::FreeContactHeaders()
	{
	iContact.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::RequiredHeadersPresent
// -----------------------------------------------------------------------------
//
TBool CTransactionInfo::RequiredHeadersPresent() const
	{
	return iViaHeaders.Count() > 0 && iFrom && iTo && iCallID && MethodExists();
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::MethodExists
// -----------------------------------------------------------------------------
//
TBool CTransactionInfo::MethodExists() const
	{
	return iMethod.DesC().Length() > 0;
	}
	
// -----------------------------------------------------------------------------
// CTransactionInfo::CompareType
// -----------------------------------------------------------------------------
//
TBool
CTransactionInfo::CompareType(CTransactionBase::TTransactionType aType) const
	{
	return aType == iTransactionType;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CompareMethod
// If the received message is ACK, iMethod should be INVITE.
// -----------------------------------------------------------------------------
//
TBool CTransactionInfo::CompareMethod(RStringF aMsgMethod) const
	{
	if (aMsgMethod == SIPStrings::StringF(SipStrConsts::EAck))
		{
		return iMethod == SIPStrings::StringF(SipStrConsts::EInvite);
		}

	return aMsgMethod == iMethod;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CompareRequestLine
// Don't compare ACK's Request-URI, as it can be different than in INVITE.
// -----------------------------------------------------------------------------
//
TBool CTransactionInfo::CompareRequestLine(const CURIContainer& aReqUri,
										   RStringF aMethod,
										   TBool aSearchCanceledUas) const
	{
	if (iRequestURI)
		{
		if (aMethod == SIPStrings::StringF(SipStrConsts::EAck))
			{
			return CompareMethod(aMethod);
			}

		if (aReqUri == *iRequestURI)
			{
			return (aSearchCanceledUas || CompareMethod(aMethod));
			}
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CompareHeaders
// If ACK, don't compare top Via.
// -----------------------------------------------------------------------------
//
TBool CTransactionInfo::CompareHeaders(CSIPRequest& aReq) const
	{
	__ASSERT_DEBUG(aReq.RequestURI() != NULL &&
		aReq.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader)) &&
		aReq.HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)) &&
		aReq.HasHeader(SIPStrings::StringF(SipStrConsts::EToHeader)) &&
		aReq.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)) &&
    	aReq.HasHeader(SIPStrings::StringF(SipStrConsts::ECallIDHeader)),
		User::Panic(_L("CTaInfo:Compare aReq"), KErrArgument));
	__ASSERT_DEBUG(RequiredHeadersPresent(),
		User::Panic(_L("CTaInfo:Compare headers not stored"), KErrArgument));

	
		
	if (CSIPMessageUtility::CompareTags(*iTo, *aReq.To()) &&
		    CSIPMessageUtility::CompareTags(*iFrom, *aReq.From()) &&
		    *iCallID == *aReq.CallID() &&
		    iCSeq == aReq.CSeq()->Seq())
		{
		if(CSIPMessageUtility::IsAck(aReq))
		    return ETrue;
		 else
		 	{
			 CSIPViaHeader* topvia = CSIPMessageUtility::TopVia(aReq);
		     if (topvia) 
			     return(*topvia == TopVia());		     
	         }
	    }
		 return EFalse;
	}
	

// -----------------------------------------------------------------------------
// CTransactionInfo::IsMergedRequest
// aCSeqMethod can't be ACK, as stray ACKs are dropped.
// -----------------------------------------------------------------------------
//
TBool CTransactionInfo::IsMergedRequest(const CUserAgent& aUserAgent,
										CSIPFromHeader& aFrom,
										CSIPCallIDHeader& aCallID,
										TUint aCSeqNbr,
										RStringF aCSeqMethod) const
	{
	__ASSERT_DEBUG(aCSeqMethod != SIPStrings::StringF(SipStrConsts::EAck),
		User::Panic(_L("CTaInfo:IsMergedRequest"), KErrArgument));
	return (iUserAgent != &aUserAgent) &&
		   iFrom &&
		   iCallID &&
		   MethodExists() &&
		   CSIPMessageUtility::CompareTags(aFrom, *iFrom) &&
		   (aCallID == *iCallID) &&
		   (aCSeqNbr == iCSeq) &&
		   iUserAgent->IsUAS() && // Allow application to send request to itself
		   // Allow receiving CANCEL
		   (iMethod != SIPStrings::StringF(SipStrConsts::EInvite) ||
		    aCSeqMethod != SIPStrings::StringF(SipStrConsts::ECancel));
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::TransactionHeadersL
// -----------------------------------------------------------------------------
//
MTransactionHeaders* CTransactionInfo::TransactionHeadersL()
	{
	return CTransactionHeaders::NewL(iRequestURI,
                                     iTo,
									 iFrom,
									 iCallID,
									 iCSeq,
									 iRecordRoute,
									 iContact);
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CopyToFromCallIDCSeqToMsgL
// Copy headers only if they don't exist.
// -----------------------------------------------------------------------------
//
void CTransactionInfo::CopyToFromCallIDCSeqToMsgL(CSIPMessage& aMsg) const
	{
	if (iTo && !aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::EToHeader)))
		{
		CSIPToHeader* to = CSIPToHeader::NewLC(*iTo);
		aMsg.AddHeaderL(to);
		CleanupStack::Pop(to);
		}

	if (iFrom &&
		!aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)))
		{
		CSIPFromHeader* from = CSIPFromHeader::NewLC(*iFrom);
		aMsg.AddHeaderL(from);
		CleanupStack::Pop(from);
		}

	if (iCallID &&
		!aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::ECallIDHeader)))
		{
		CSIPCallIDHeader* callID =
			static_cast<CSIPCallIDHeader*>(iCallID->CloneL());
		CleanupStack::PushL(callID);
		aMsg.AddHeaderL(callID);
		CleanupStack::Pop(callID);
		}

	CSIPMessageUtility::FillCSeqL(aMsg,
		iCSeq,
		aMsg.IsRequest() ? static_cast<CSIPRequest&>(aMsg).Method() : iMethod);
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CopyViaHeadersToMsgL
// -----------------------------------------------------------------------------
//
void CTransactionInfo::CopyViaHeadersToMsgL(CSIPMessage& aMsg) const
	{
	for (TInt i = 0; i < iViaHeaders.Count(); ++i)
		{
		CSIPViaHeader* via =
			static_cast<CSIPViaHeader*>(iViaHeaders[i]->CloneL());
		CleanupStack::PushL(via);
		aMsg.AddHeaderL(via);
		CleanupStack::Pop(via);
		}
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::CopyRequestUriToRequestL
// -----------------------------------------------------------------------------
//
void CTransactionInfo::CopyRequestUriToRequestL(CSIPRequest& aReq) const
	{
    __SIP_ASSERT_LEAVE(iRequestURI, KErrNotFound);

	CURIContainer* reqUri = CURIContainer::NewLC(*iRequestURI);
	aReq.SetRequestURIL(reqUri);
	CleanupStack::Pop(reqUri);
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::BuildRequestFromStoredInfoL
// -----------------------------------------------------------------------------
//
CSIPRequest* CTransactionInfo::BuildRequestFromStoredInfoL() const
	{
	CSIPRequest* req(NULL);
	if (iRequestURI && iTo && iFrom && MethodExists())
		{
		req = CSIPRequest::NewLC(iMethod);
		CopyToFromCallIDCSeqToMsgL(*req);
		CopyRequestUriToRequestL(*req);
		CleanupStack::Pop(req);
		}
	return req;
	}

// -----------------------------------------------------------------------------
// CTransactionInfo::RequestMethod
// -----------------------------------------------------------------------------
//
RStringF CTransactionInfo::RequestMethod() const
	{
	return iMethod;
	}
