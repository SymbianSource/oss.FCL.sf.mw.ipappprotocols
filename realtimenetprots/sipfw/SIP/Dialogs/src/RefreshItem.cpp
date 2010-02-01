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
// Name          : RefreshItem.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//



#include "RefreshItem.h"
#include "MSipRefreshMgr.h"
#include "siprequest.h"
#include "sipexpiresheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

const TUint K200Response = 200;

// -----------------------------------------------------------------------------
// CRefreshItem::NewL
// -----------------------------------------------------------------------------
//
CRefreshItem* CRefreshItem::NewL (MSipRefreshMgr& aRefreshMgr,
                                  MRefreshOwner* aRefreshOwner,
                                  MSIPSecUser& aSIPSecUser,
                                  RStringF aMethod)
    {
	CRefreshItem* self = 
        CRefreshItem::NewLC(aRefreshMgr,aRefreshOwner,aSIPSecUser,aMethod);
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CRefreshItem::NewLC
// -----------------------------------------------------------------------------
//
CRefreshItem* CRefreshItem::NewLC (MSipRefreshMgr& aRefreshMgr,
                                   MRefreshOwner* aRefreshOwner,
                                   MSIPSecUser& aSIPSecUser,
                                   RStringF aMethod)
    {
	CRefreshItem* self = 
        new(ELeave)CRefreshItem(aRefreshMgr,aRefreshOwner,aSIPSecUser,aMethod);
	CleanupStack::PushL(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CRefreshItem::CRefreshItem
// -----------------------------------------------------------------------------
//
CRefreshItem::CRefreshItem (MSipRefreshMgr& aRefreshMgr,
                            MRefreshOwner* aRefreshOwner,
                            MSIPSecUser& aSIPSecUser,
                            RStringF aMethod)
 : CTransactionItemBase(KEmptyTransactionId,aMethod),
   iRefreshMgr(aRefreshMgr),
   iRefreshOwner(aRefreshOwner),
   iSIPSecUser(aSIPSecUser),
   iRefreshId(KEmptyRefreshId),
   iFirstRequestSent(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CRefreshItem::~CRefreshItem
// -----------------------------------------------------------------------------
//
CRefreshItem::~CRefreshItem ()
	{
    iRefreshMgr.TerminateRefresh(iRefreshId,iRefreshOwner);
	}

// -----------------------------------------------------------------------------
// CRefreshItem::RefreshId
// -----------------------------------------------------------------------------
//
TRefreshId CRefreshItem::RefreshId()
	{
	return iRefreshId;
	}

// -----------------------------------------------------------------------------
// CRefreshItem::SetTerminated
// -----------------------------------------------------------------------------
//
void CRefreshItem::SetTerminated ()
    {
    iTerminate = ETrue;
    }

// -----------------------------------------------------------------------------
// CRefreshItem::IsRefreshed
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
TBool CRefreshItem::IsRefreshed () const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CRefreshItem::ResponseReceived
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
void CRefreshItem::ResponseReceived (TUint aResponseCode)
	{
	if (aResponseCode >= K200Response)
		{
		iPassResponseToOwner = EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CRefreshItem::SendAndGetHeadersL
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
MTransactionHeaders* CRefreshItem::SendAndGetHeadersL (
    const TSIPTransportParams& /*aTransportParams*/,
    TTransactionId& aTransactionId,
    TRegistrationId aRegistrationId,
    CSIPRequest* aRequest,
    CURIContainer& aRemoteTarget)
    {

    MTransactionHeaders* headers = 
        iRefreshMgr.RefreshAndGetHeadersL(aTransactionId,iRefreshId,
                                          aRegistrationId,
                                          aRequest,iRefreshOwner,
                                          iSIPSecUser,aRemoteTarget);
    iFirstRequestSent = ETrue;
    SetTransactionId (aTransactionId);
    return headers;
    }


// -----------------------------------------------------------------------------
// CRefreshItem::SendL
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
void CRefreshItem::SendL (const TSIPTransportParams& /*aTransportParams*/,
                          TTransactionId& aTransactionId,
                          TRegistrationId aRegistrationId,
                          CSIPRequest* aRequest,
                          CURIContainer& aRemoteTarget)
    {
    if (!iFirstRequestSent)
        {
        iRefreshMgr.RefreshL(aTransactionId,iRefreshId,aRegistrationId,aRequest,
                             iRefreshOwner,iSIPSecUser,aRemoteTarget,
                             ETrue,ETrue);
        iFirstRequestSent= ETrue;
        }
    else
        {
        TBool terminate = EFalse;
        CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>
            (aRequest->Header(
                SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));
        if (expires && expires->Value() == 0)
            {
            terminate = ETrue;
            }
        iRefreshMgr.UpdateRefreshL(aTransactionId,iRefreshId,aRequest,
                                   iRefreshOwner,ETrue);
        if (terminate)
            {
            SetTerminated();
            }
		// After a refresh update pass the next response from Refreshes to the
		// dialog owner
		iPassResponseToOwner = ETrue;
        }
    SetTransactionId (aTransactionId);
    }

// -----------------------------------------------------------------------------
// CTransactionItem::SendL
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
void CRefreshItem::SendL (const TSIPTransportParams& /*aTransportParams*/,
                          CSIPResponse* /*aResponse*/,
                          TBool /*aAddOwner*/)
    {
    User::Leave(KErrNotSupported);
    }
