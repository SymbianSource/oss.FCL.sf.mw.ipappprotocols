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
// Name          : CancelUAC.cpp
// Part of       : TransactionUser
// Version       : SIP/6.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "sipfromheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "CancelUAC.h"
#include "InviteUAC.h"
#include "CTransactionStore.h"
#include "SIPMessageUtility.h"
#include "SIPRequestUtility.h"


// -----------------------------------------------------------------------------
// CCancelUAC::NewL
// -----------------------------------------------------------------------------
//
CCancelUAC* CCancelUAC::NewL(CUserAgentCreateParams& aParams,							 
                             MSipConnectionMgr& aConnectionMgr,                             
                             MSipUriResolver& aResolver,							 
							 CSIPSec& aSIPSec,
							 TUint32 aCSeqNumber)
	{
	CCancelUAC* self =
		new (ELeave) CCancelUAC(aParams, aResolver, aSIPSec, aCSeqNumber);
	CleanupStack::PushL(self);
	self->CUserAgent::ConstructL(aConnectionMgr);
	self->CUserAgentClient::ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CCancelUAC::CCancelUAC
// -----------------------------------------------------------------------------
//
CCancelUAC::CCancelUAC(CUserAgentCreateParams& aParams,
					   MSipUriResolver& aResolver,
					   CSIPSec& aSIPSec,
					   TUint32 aCSeqNumber) :
	CNormalUAC(aParams, aResolver, aSIPSec, aCSeqNumber)
	{
	}

// -----------------------------------------------------------------------------
// CCancelUAC::CreateAndSendCancelL
// -----------------------------------------------------------------------------
//
void CCancelUAC::CreateAndSendCancelL(TTransactionId aInviteTaId)
	{
	CUserAgent* ua = iTransactionStore.SearchById(aInviteTaId);

	// CTransactionUser has checked aInviteTaId is a TransactionId of an INVITE
	// client transaction
	__SIP_ASSERT_LEAVE(ua, KErrNotFound);
    __SIP_ASSERT_LEAVE(ua->IsInviteUAC(), KErrGeneral);    
	CInviteUAC& inviteUac = CInviteUAC::Ptr(*ua);
	__ASSERT_ALWAYS(!inviteUac.IsCanceled(),
    				User::Leave(KErrSIPInvalidTransactionState));

	CreateCancelRequestL(aInviteTaId, inviteUac);
	SendCancelRequestL(inviteUac);
	}

// -----------------------------------------------------------------------------
// CCancelUAC::CreateCancelRequestL
// Use Route headers of INVITE, in CANCEL.
// -----------------------------------------------------------------------------
//
void CCancelUAC::CreateCancelRequestL(TTransactionId aInviteTaId,
									  CInviteUAC& aInviteUac)
	{
	__SIP_ASSERT_LEAVE(!iOutgoingMsg, KErrNotFound);

	iOutgoingMsg =
		CSIPRequest::NewL(SIPStrings::StringF(SipStrConsts::ECancel));
	CSIPRequest* cancel = static_cast<CSIPRequest*>(iOutgoingMsg);

	iTransactionStore.CopyHeadersToRequestL(aInviteTaId,
											*cancel,
											ETrue,  // copy Request-URI
											ETrue); // copy Via
	SIPRequestUtility::FillNewMaxForwardsL(*cancel);
	CheckContactHeadersL(*cancel, cancel->From());	
	StoreRequestUriL(*cancel);

    aInviteUac.CancelInvite();
    aInviteUac.CopyRouteHeadersToCancelL(*cancel);

	CopyRouteSetAndRemoteTargetL(aInviteUac);
	}

// -----------------------------------------------------------------------------
// CCancelUAC::SendCancelRequestL
// -----------------------------------------------------------------------------
//
void CCancelUAC::SendCancelRequestL(CUserAgentClient& aInviteUac)
	{
	__SIP_ASSERT_LEAVE(iOutgoingMsg, KErrNotFound);

    CreateTransactionL();

	// Update CANCEL's and transaction's info
	UpdateInfoToStoreL(static_cast<CSIPRequest*>(iOutgoingMsg));

	// Obtain the current remote address used by INVITE transaction
    CopyRemoteAddress(aInviteUac);

	// Don't use SelectNextRemoteAddressL, as CancelUAC's address and transport
	// are in iRemoteAddr and iTransportProtocol, not in iResolvingResults.
	CSIPMessageUtility::UpdateViaTransportL(*iOutgoingMsg, iTransportProtocol);

	SendRequestToNetworkL();
	}

// -----------------------------------------------------------------------------
// CCancelUAC::Ptr
// -----------------------------------------------------------------------------
//
CCancelUAC& CCancelUAC::Ptr(CUserAgent& aUserAgent)
	{
	return static_cast<CCancelUAC&>(aUserAgent);
	}
