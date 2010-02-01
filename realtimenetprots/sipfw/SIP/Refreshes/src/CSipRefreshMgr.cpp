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
// Name          : CSipRefreshMgr.cpp
// Part of       : SIPRefreshes
// Version       : SIP/5.0 
//



#include "CSipRefreshMgr.h"
#include "MTransactionUser.h"
#include "CSipRegisterRefresh.h"
#include "CSipAnyRefresh.h"
#include "MTransactionHeaders.h"
#include "TSIPTransportParams.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipfromheader.h"
#include "sipcontactheader.h"
#include "sipexpiresheader.h"
#include "sipcseqheader.h"
#include "siptoheader.h"
#include "SipLogs.h"
#include "DeleteMgr.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSipRefreshMgr::NewL
// -----------------------------------------------------------------------------
//
CSipRefreshMgr* CSipRefreshMgr::NewL (MTransactionUser& aTU, 
                                      MTimerManager&    aTimerMgr,
                                      CSIPSec&          aSIPSec)
	{
	CSipRefreshMgr* self = new(ELeave)CSipRefreshMgr(aTU,aTimerMgr,aSIPSec);
	CleanupStack::PushL(self);	
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::CSipRefreshMgr
// -----------------------------------------------------------------------------
//
CSipRefreshMgr::CSipRefreshMgr (
    MTransactionUser& aTU,
    MTimerManager& aTimerMgr,
    CSIPSec& aSIPSec)
 : iTU(aTU),
   iTimerMgr(aTimerMgr),
   iSIPSec(aSIPSec),
   iNextRefreshId (KMinRefreshId)
	{
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::ConstructL
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::ConstructL ()
	{
	iDeleteMgr = CDeleteMgr::NewL();
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::~CSipRefreshMgr
// -----------------------------------------------------------------------------
//
CSipRefreshMgr::~CSipRefreshMgr ()
	{
	delete iDeleteMgr;
	iRefreshes.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RefreshL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::RefreshL (TTransactionId& aTransactionId,
                               TRefreshId& aRefreshId,
							   TRegistrationId aRegistrationId,
							   CSIPRequest* aRequest,
							   MRefreshOwner* aOwner,
							   MSIPSecUser& aSIPSecUser,
							   CURIContainer& aRemoteTarget,
							   TBool aDeleteRequest,
							   TBool aPassAllResponsesToOwner)
	{
    __SIP_ASSERT_LEAVE (aRequest, KErrArgument);

    __SIP_ASSERT_LEAVE (aRequest->Method().DesC().Length() > 0, KErrArgument);

    __SIP_ASSERT_LEAVE (aRequest->From(), KErrArgument);

    __SIP_ASSERT_LEAVE (aRequest->To(), KErrArgument);

    __SIP_ASSERT_LEAVE (aOwner, KErrArgument);
    
	__SIP_MESSAGE_LOG ("Refreshes", *aRequest)

	CSipRefreshBase* refresh = NULL;

	// method is register
	if (aRequest->Method() == SIPStrings::StringF(SipStrConsts::ERegister)) 
		{
		refresh = CSipRegisterRefresh::NewLC(iTU,iTimerMgr,iSIPSec,*this, 
		                                     aRegistrationId,aRequest,aOwner,
		                                     aSIPSecUser,aRemoteTarget);
		}
	else
		{
		refresh = CSipAnyRefresh::NewLC(iTU,iTimerMgr,iSIPSec,*this,
		                                aRegistrationId,aRequest,aOwner,
		                                aSIPSecUser,aRemoteTarget,
		                                aPassAllResponsesToOwner);
		}

	refresh->SendL(aTransactionId, aRequest);

	User::LeaveIfError(iRefreshes.Append(refresh));
	CleanupStack::Pop(refresh);

	refresh->SetRequest(aRequest,aDeleteRequest);
	aRefreshId = refresh->Id();
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RefreshAndGetHeadersL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
MTransactionHeaders* CSipRefreshMgr::RefreshAndGetHeadersL (
	TTransactionId& aTransactionId,
	TRefreshId& aRefreshId,
	TRegistrationId aRegistrationId,
	CSIPRequest* aRequest,
	MRefreshOwner* aOwner,
	MSIPSecUser& aSIPSecUser,
	CURIContainer&  aRemoteTarget)
	{
    __SIP_ASSERT_LEAVE (aRequest, KErrArgument);

    __SIP_ASSERT_LEAVE (aRequest->Method().DesC().Length() != 0, KErrArgument);

    __SIP_ASSERT_LEAVE (aRequest->From(), KErrArgument);

    __SIP_ASSERT_LEAVE (aRequest->To(), KErrArgument);

    __SIP_ASSERT_LEAVE (aOwner, KErrArgument);

	__SIP_MESSAGE_LOG ("Refreshes", *aRequest)

	__ASSERT_ALWAYS (aRequest->Method() != 
	                 SIPStrings::StringF(SipStrConsts::ERegister), 
		             User::Leave(KErrNotSupported));

	// Only Dialogs uses this function, so any refresh is always created
	// with flag telling the refresh to pass all the responses up to the owner.
	CSipRefreshBase* refresh = CSipAnyRefresh::NewLC (iTU, iTimerMgr, iSIPSec,
	                                                  *this, aRegistrationId, 
	                                                  aRequest, aOwner, 
	                                                  aSIPSecUser, 
	                                                  aRemoteTarget, ETrue);

	MTransactionHeaders* headers = 
		refresh->SendAndGetHeadersL (aTransactionId,aRequest);

    TCleanupItem cleanupItem(DeleteTransactionHeaders,headers);
	CleanupStack::PushL (cleanupItem);

	User::LeaveIfError(iRefreshes.Append(refresh));

	CleanupStack::Pop(1); // cleanupItem
	CleanupStack::Pop(refresh);

	refresh->SetRequest(aRequest,ETrue);
	
    aRefreshId = refresh->Id();

	return headers;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::UpdateRefreshL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::UpdateRefreshL (TTransactionId& aTransactionId,
							         const TRefreshId& aRefreshId,
							         CSIPRequest* aRequest,
							         const MRefreshOwner* aOwner,
							         TBool aDeleteRequest)
	{
    __SIP_ASSERT_LEAVE (aRequest, KErrArgument);

	__SIP_MESSAGE_LOG ("Refreshes", *aRequest)

	CSipRefreshBase& refresh = FindRefreshL(aRefreshId,aOwner);

	// increase the request CSeq number if it is none REGISTER request
	// and if there is CSeqHeader in the request.
	// since Registration subsystem update CSeq itself for update Request
	if(aRequest->Method() != SIPStrings::StringF(SipStrConsts::ERegister) &&
	   aRequest->CSeq() != 0)
		{
		refresh.UpdateCSeqValueL(*aRequest);
		}

	refresh.UpdateRequestL(aTransactionId,aRequest);

	refresh.SetRequest(aRequest,aDeleteRequest);
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::TerminateRefreshL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::TerminateRefreshL (TTransactionId& aTransactionId,
								        const TRefreshId& aRefreshId,
								        CSIPRequest* aRequest,
								        const MRefreshOwner* aOwner,
								        TBool aDeleteRequest)
	{
    __SIP_ASSERT_LEAVE (aRequest, KErrArgument);

	__SIP_MESSAGE_LOG ("Refreshes", *aRequest)

	CSipRefreshBase& refresh = FindRefreshL(aRefreshId,aOwner);

	// increase the request CSeq number if it is none REGISTER request
	// and if there is CSeqHeader in the request.
	// since Registration subsystem update CSeq itself for terminate Request
	if(aRequest->Method() != SIPStrings::StringF(SipStrConsts::ERegister) &&
	   aRequest->CSeq() != 0)
		{
		refresh.UpdateCSeqValueL(*aRequest);
		}

	refresh.TerminateRequestL(aTransactionId,aRequest);

	refresh.SetRequest(aRequest,aDeleteRequest);
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::TerminateRefresh
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
TInt CSipRefreshMgr::TerminateRefresh (const TRefreshId& aRefreshId,
                                       const MRefreshOwner* aOwner)
	{
	TInt index = FindIndex(aRefreshId,aOwner);
	if (index < 0 || index >= iRefreshes.Count()) 
		{
		return KErrNotFound;
		}
	return RemoveRefreshBy(iRefreshes[index]);
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RemoveRefreshesFor
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
TInt CSipRefreshMgr::RemoveRefreshesFor (const MRefreshOwner* aOwner)
	{
	if (!aOwner)
		{
		return KErrNotFound;
		}

    for (TInt i=iRefreshes.Count()-1; i>=0; i--)
		{
        if (iRefreshes[i]->Owner() == aOwner)
			{
			iRefreshes[i]->StopTimer();	
			iRefreshes[i]->SetRequest(NULL,EFalse);
			TInt err = RemoveRefreshBy (iRefreshes[i]);
			if (err)
				{
				return err;
				}
			}
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::Request
// From MSipRefreshMgr:
// const CSIPRequest* CSipRefreshMgr::Request (const TRefreshId& aRefreshId) const
// -----------------------------------------------------------------------------
//
CSIPRequest* CSipRefreshMgr::Request (const TRefreshId& aRefreshId) 
	{
	if (aRefreshId == KEmptyRefreshId)
		{
		return NULL;
		}
	for (TInt i=0; i<iRefreshes.Count(); i++)
		{
		if (iRefreshes[i]->Id() == aRefreshId)
			{
			return (iRefreshes[i]->Request());
			}
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RemoveRefreshBy
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::SetIntervalL  (const TRefreshId& aRefreshId,
                                    TInt aInterval)
    {
    CSipRefreshBase& refresh = FindRefreshL(aRefreshId,NULL,EFalse);
    refresh.SetIntervalL(aInterval);
    }
	
// -----------------------------------------------------------------------------
// CSipRefreshMgr::RemoveRefreshBy
// -----------------------------------------------------------------------------
//	
TInt CSipRefreshMgr::IntervalL (const TRefreshId& aRefreshId)
    {
    CSipRefreshBase& refresh = FindRefreshL(aRefreshId,NULL,EFalse);
    return refresh.Interval();    
    }

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RemoveRefreshBy
// -----------------------------------------------------------------------------
//
TInt CSipRefreshMgr::RemoveRefreshBy (CSipRefreshBase* aRefresh)
	{
	for (TInt i=0; i < iRefreshes.Count(); i++)
		{
        if (iRefreshes[i] == aRefresh)
			{
			// Transaction owner needs to be cleared so that TU will not
			// call the refresh while it is in DeleteMgr 
			aRefresh->ClearTransactionOwner();
			TInt err = iDeleteMgr->AddDeleteRequest(aRefresh);
			if (err == KErrNone) 
				{
				iRefreshes.Remove (i);
				}
			return err;
			}
		}
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::CurrentTransactionId
// -----------------------------------------------------------------------------
//
TTransactionId CSipRefreshMgr::CurrentTransactionId(
    const TRefreshId& aRefreshId) const
    {
    TTransactionId transactionId = KEmptyTransactionId;
	TInt index = FindIndex(aRefreshId,NULL,EFalse);
	if (index >= 0) 
		{
		transactionId = iRefreshes[index]->CurrentTransactionId();
		}
	return transactionId;   
    }

// -----------------------------------------------------------------------------
// CSipRefreshMgr::SIPSecUser
// -----------------------------------------------------------------------------
//        
const MSIPSecUser* CSipRefreshMgr::SIPSecUser(
    const TRefreshId& aRefreshId) const
    {
    const MSIPSecUser* sipSecUser = NULL;
    TInt index = FindIndex(aRefreshId,NULL,EFalse);
    if (index >= 0)
        {
        sipSecUser = iRefreshes[index]->SIPSecUser();
        }
    return sipSecUser;
    }

// -----------------------------------------------------------------------------
// CSipRefreshMgr::NextRefreshId
// -----------------------------------------------------------------------------
//
TRefreshId CSipRefreshMgr::NextRefreshId()
	{
	TRefreshId refreshId = iNextRefreshId;
    if (iNextRefreshId == KMaxRefreshId)
		{
		iNextRefreshId = KMinRefreshId;
		}
	else
		{
		iNextRefreshId++;
		}
	return refreshId;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::FindIndex
// -----------------------------------------------------------------------------
//
TInt CSipRefreshMgr::FindIndex (const TRefreshId& aRefreshId,
                                const MRefreshOwner* aOwner,
                                TBool aCheckOwner) const
	{
	for (TInt i=0; i < iRefreshes.Count(); i++)
		{
		CSipRefreshBase* refresh = iRefreshes[i];
		if (refresh->Id() == aRefreshId) 
			{
			if (!aCheckOwner || refresh->Owner() == aOwner)
			    {
			    return i;
			    }
			}
		}
    return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::FindRefreshL
// -----------------------------------------------------------------------------
//
CSipRefreshBase& CSipRefreshMgr::FindRefreshL (const TRefreshId& aRefreshId,
                                               const MRefreshOwner* aOwner,
                                               TBool aCheckOwner)
	{
	TInt index = FindIndex(aRefreshId,aOwner,aCheckOwner);
	if (index < 0) 
		{
		User::Leave (KErrNotFound);
		}
	return *iRefreshes[index];
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RemoveRefreshByIAPId
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::RemoveRefreshByIAPId(const TUint aIAPId)
	{
	for (TInt i=iRefreshes.Count() - 1; i >= 0; i--)
		{
		CSipRefreshBase* refresh = iRefreshes[i];
        if (refresh->IAPId() == aIAPId)
			{
			iRefreshes.Remove (i);
			delete refresh;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::ConnectionStateChangedL
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::ConnectionStateChangedL (TUint32                aIapId,
                                              CSIPConnection::TState aState)
	{
	if (aState == CSIPConnection::EInactive || 
        aState == CSIPConnection::EUnavailable)
		{
		RemoveRefreshByIAPId(aIapId);
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::UpdateRemoteTargetL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::UpdateRemoteTargetL (const MRefreshOwner* aOwner, 
										  CURIContainer& aRemoteTarget)
	{
    __SIP_ASSERT_LEAVE (aOwner != 0, KErrArgument);

    for (TInt i=iRefreshes.Count()-1; i>=0; i--)
		{
        if (iRefreshes[i]->Owner() == aOwner)
			{
			iRefreshes[i]->UpdateRemoteTargetL(aRemoteTarget);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::UpdateHeaderL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::UpdateHeaderL (const TRefreshId& aRefreshId,
                                    const MRefreshOwner* aOwner,
									CSIPHeaderBase* aSIPHeader)
	{
	TInt index = FindIndex(aRefreshId,aOwner);
	if (index >= 0) 
		{
	    CSipRefreshBase* refresh = iRefreshes[index];
	    refresh->SetUpdatedHeader(aSIPHeader);
		}
	// if no refresh could found by the Id, aSIPHeader is must be deleted
	// since the ownersip is transferred.
	else 
		{
		delete aSIPHeader; 
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::UpdateRouteSetsL
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::UpdateRouteSetsL (
	const MRefreshOwner* aOwner, 
	const RPointerArray<CSIPRouteHeader>& aRouteSet)
	{
	for (TInt i=0; i < iRefreshes.Count(); i++)
		{
		if (iRefreshes[i]->Owner() == aOwner) 
			{
			iRefreshes[i]->SetUpdatedRouteHeadersL(aRouteSet);
			}
		}	
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::RemoveRouteSets
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::RemoveRouteSets (const MRefreshOwner* aOwner)
	{
	for (TInt i=0; i < iRefreshes.Count(); i++)
		{
		if (iRefreshes[i]->Owner() == aOwner) 
			{
			iRefreshes[i]->RemoveRouteHeaders();
			}
		}	
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::SetRegistrationIdsEmpty
// From MSipRefreshMgr:
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::SetRegistrationIdsEmpty (const MRefreshOwner* aOwner)
	{
	for (TInt i=iRefreshes.Count()-1; i>=0; i--)
		{
		// iRefreshes[i]->Owner() never could be NULL, aOwner could be NULL
        if (iRefreshes[i]->Owner() == aOwner)
			{
			iRefreshes[i]->ResetRegistrationId();
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshMgr::DeleteTransactionHeaders
// -----------------------------------------------------------------------------
//
void CSipRefreshMgr::DeleteTransactionHeaders (TAny* aTransactionHeaders)
	{
	MTransactionHeaders* transactionHeaders = 
        reinterpret_cast<MTransactionHeaders*>(aTransactionHeaders);
    delete transactionHeaders;
	}
