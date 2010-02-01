// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CancelUAS.cpp
// Part of       : TransactionUser
// Version       : SIP/4.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "siptoheader.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "CancelUAS.h"
#include "InviteUAS.h"
#include "CTransactionStore.h"

// -----------------------------------------------------------------------------
// CCancelUAS::NewL
// -----------------------------------------------------------------------------
//
CCancelUAS* CCancelUAS::NewL(CUserAgentCreateParams& aParams,							
							 MSipConnectionMgr& aConnectionMgr,
							 MSIPRequestRouter& aRouter,
                             MSipDialogs& aDialogs)
	{
	CCancelUAS* self = new (ELeave) CCancelUAS(aParams, aDialogs, aRouter);
	CleanupStack::PushL(self);	
	self->ConstructL(aConnectionMgr);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CCancelUAS::CCancelUAS
// -----------------------------------------------------------------------------
//
CCancelUAS::CCancelUAS(CUserAgentCreateParams& aParams,
					   MSipDialogs& aDialogs,
					   MSIPRequestRouter& aRouter) :
	CNormalUAS(aParams, aDialogs, aRouter)
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAS::ConstructL
// -----------------------------------------------------------------------------
//
void CCancelUAS::ConstructL(MSipConnectionMgr& aConnectionMgr)
	{
	CUserAgent::ConstructL(aConnectionMgr);
	CreateTransactionL();
	}

// -----------------------------------------------------------------------------
// CCancelUAS::CancelReceivedL
// Can't cancel ACK or CANCEL. Send 200 to CANCEL before 487 to INVITE. If
// INVITE UAS has no To tag yet, the To tags of 200 and 487 will differ.
// -----------------------------------------------------------------------------
//
void CCancelUAS::CancelReceivedL(CSIPRequest* aCancel,
								 const CUserAgentState& aFinalRespSent)
	{
	__SIP_ASSERT_LEAVE(aCancel, KErrArgument);

	if (!UpdateTransportProtocol(*aCancel))
		{
		// Drop the broken CANCEL
		delete aCancel;
		Stop(KErrNone);
		return;
		}

    StoreToTag(*aCancel);

	TInt responseCode = 0;
	RStringF reasonPhrase;
	if (CheckReceivedRequest(*aCancel, responseCode, reasonPhrase))
		{
		CUserAgentServer* uas =
            iTransactionStore.SearchUasToCancel(*aCancel, *this);
		if (uas)
			{
			iTransportParams = uas->TransportParams();
			RStringF method =
                iTransactionStore.RequestMethod(uas->TransactionId());

			if (method == SIPStrings::StringF(SipStrConsts::EAck) ||
                method == SIPStrings::StringF(SipStrConsts::ECancel))
				{                
				SendErrorResponseL(400,
					SIPStrings::StringF(SipStrConsts::EPhraseBadRequest),
					aFinalRespSent);
				}
			else
				{
				Send200L(aFinalRespSent, uas->ToTag());
				// If INVITE UAS has sent 2xx, it no longer has transaction
				if (method == SIPStrings::StringF(SipStrConsts::EInvite))
					{
					CInviteUAS::Ptr(*uas).CancelInviteL();
					}
				}
			}
		else
			{
			SendErrorResponseL(481,
				SIPStrings::StringF(
					SipStrConsts::EPhraseCallTransactionDoesNotExist),
                aFinalRespSent);
			}
		}
	else
		{
		__SIP_ASSERT_LEAVE(responseCode >= 300 && responseCode < 700,
                           KErrSIPMalformedMessage);

		SendErrorResponseL(responseCode, reasonPhrase, aFinalRespSent);		
		}

	delete aCancel;
	}

// -----------------------------------------------------------------------------
// CCancelUAS::Send200L
// If the canceled transaction has a To-tag, it's put in the response to CANCEL
// -----------------------------------------------------------------------------
//
void CCancelUAS::Send200L(const CUserAgentState& aFinalRespSent,
						  RStringF aToTag)
    {    
	CSIPResponse* resp =
		CSIPResponse::NewLC(200, SIPStrings::StringF(SipStrConsts::EPhraseOk));
	FillResponseL(*resp);

	if (aToTag.DesC().Length() > 0)
		{
        CSIPToHeader* to = resp->To();
        __SIP_ASSERT_LEAVE(to != NULL, KErrSIPMalformedMessage);

		to->SetParamL(SIPStrings::StringF(SipStrConsts::ETag), aToTag);
		}

	ChangeState(aFinalRespSent);
	SendResponseToTransactionL(resp);
	CleanupStack::Pop(resp);
	}

// -----------------------------------------------------------------------------
// CCancelUAS::Ptr
// -----------------------------------------------------------------------------
//
CCancelUAS& CCancelUAS::Ptr(CUserAgent& aUserAgent)
	{
	return static_cast<CCancelUAS&>(aUserAgent);
	}
