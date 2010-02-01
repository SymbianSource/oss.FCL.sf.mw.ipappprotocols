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
// Name          : CSipRefreshBase.cpp
// Part of       : SIPRefreshes
// Version       : SIP/4.0 
//



#include "SipStackServerDefs.h"
#include "MTransactionUser.h"
#include "MRefreshOwner.h"
#include "CSipRefreshMgr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcontactheader.h"
#include "sipaddress.h"
#include "sipcseqheader.h"
#include "sipexpiresheader.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "SipLogs.h"
#include "sipcallidheader.h"
#include "uricontainer.h"
#include "siprouteheader.h"
#include "siprecordrouteheader.h"
#include "sipheaderbase.h"
#include "siperr.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"
#include "CSipRefreshBase.h"
#include "sipsec.h"


const TInt KOneSecInMilliSecs = 1000; // one second is 1000 millisecond


// -----------------------------------------------------------------------------
// CSipRefreshBase::CSipRefreshBase
// -----------------------------------------------------------------------------
//
CSipRefreshBase::CSipRefreshBase (MTransactionUser& aTU,
                                  MTimerManager& aTimerMgr,
                                  CSIPSec& aSIPSec,
				                  CSipRefreshMgr& aRefreshMgr,
				                  TRegistrationId aRegistrationId,
                                  MRefreshOwner* aOwner,
                                  MSIPSecUser& aSIPSecUser)
 : iRefreshMgr              (aRefreshMgr),
   iRefreshId               (aRefreshMgr.NextRefreshId()),
   iCurrentTransactionId    (KEmptyTransactionId),
   iFirstRequest            (ETrue),
   iSIPSecUser              (aSIPSecUser), 
   iOwner                   (aOwner),
   iTU                      (aTU),
   iTimerMgr                (aTimerMgr),
   iSIPSec                  (aSIPSec),
   iTransactionOwnerCleared (EFalse),
   iIntervalUpdated         (EFalse),
   iRegistrationId          (aRegistrationId),
   iOwnRequest              (ETrue),
   iRemoveRouteHeaders      (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::BaseConstructL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::BaseConstructL(CURIContainer& aRemoteTarget,
                                     CSIPRequest& aRequest)
	{
	iRemoteTarget = CURIContainer::NewL(aRemoteTarget);
	TInt expiresInSeconds = ExpirationTimeFromRequestL(aRequest);
	iIntervalInMilliSeconds = CalculateInterval(expiresInSeconds);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::~CSipRefreshBase
// -----------------------------------------------------------------------------
//
CSipRefreshBase::~CSipRefreshBase ()
	{
	if (iTimerMgr.IsRunning(iTimerId)) 
		{
		iTimerMgr.Stop(iTimerId);
		}

	if (!iTransactionOwnerCleared)
		{
		iTU.ClearTransactionOwner (this);
		}

	if (iOwnRequest)
		{
		delete iRequest;
		}

	delete iRemoteTarget;
	delete iUpdatedHeader;
    iSIPSec.ClearCache(this);
	iUpdatedRouteHeaders.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::ClearTransactionOwner
// Needs to be called before the refresh is put to DeleteMgr, 
// so that TU will not call this refresh anymore.
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::ClearTransactionOwner()
	{
	iTU.ClearTransactionOwner (this);
	iTransactionOwnerCleared = ETrue;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::Owner
// -----------------------------------------------------------------------------
//
MRefreshOwner* CSipRefreshBase::Owner()
	{
	return iOwner;
	}
    
// -----------------------------------------------------------------------------
// CSipRefreshBase::Id
// -----------------------------------------------------------------------------
//
const TRefreshId& CSipRefreshBase::Id() const
	{
	return iRefreshId;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::CurrentTransactionId
// -----------------------------------------------------------------------------
//	
TTransactionId CSipRefreshBase::CurrentTransactionId() const
    {
    return iCurrentTransactionId;
    }	
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::Request
// -----------------------------------------------------------------------------
//
CSIPRequest* CSipRefreshBase::Request()
	{
	return iRequest;
	}
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::Request
// -----------------------------------------------------------------------------
//	
void CSipRefreshBase::SetIntervalL(TInt aInterval)
    {
    __ASSERT_ALWAYS(aInterval > 0, User::Leave(KErrArgument));
    
    TInt tmp = CalculateInterval(aInterval);    
	if (iTimerMgr.IsRunning(iTimerId)) 
		{
		iTimerMgr.Stop(iTimerId);
        StartTimerL(tmp);		
		}
    iIntervalInMilliSeconds = tmp;
	iIntervalUpdated = ETrue;
    }
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::Request
// -----------------------------------------------------------------------------
//	
TInt CSipRefreshBase::Interval() const
    {
    if (iIntervalInMilliSeconds < KOneSecInMilliSecs)
        {
        return 1;
        }
    return iIntervalInMilliSeconds/KOneSecInMilliSecs;
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::ReceiveL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::ReceiveL (TUint32        /*aIapId*/,
								TTransactionId /*aTransactionId*/, 
								CSIPRequest*   /*aRequest*/)
	{
	 // Must not ever receive SIP requests from TU.
	__SIP_ASSERT_LEAVE (EFalse, KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::ReceiveL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::ReceiveL (TTransactionId aTransactionId, 
								CSIPResponse*  aResponse)
	{
	__SIP_ASSERT_LEAVE (iRequest, KErrArgument);

	__SIP_ASSERT_LEAVE (aResponse, KErrArgument);

    __SIP_ASSERT_LEAVE (aTransactionId == iCurrentTransactionId, KErrArgument);

	__SIP_MESSAGE_LOG ("Refreshes", *aResponse)
  
	if (aResponse->Type() == CSIPResponse::E1XX)
		{
		ReceivedProvisonalResponseL (aResponse);
		}
	else if (aResponse->Type() == CSIPResponse::E2XX)
		{
		Received200ClassResponseL (aResponse);
		}		
	else // error response
		{
		ReceivedErrorResponseL (aResponse);	
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::TransactionEnded
// -----------------------------------------------------------------------------
//
TInt CSipRefreshBase::TransactionEnded(TUint32 /*aIapId*/,
                                       TTransactionId aTransactionId, 
									   TInt aReason)
	{
	__SIP_ASSERT_RETURN_VALUE (aTransactionId == iCurrentTransactionId, 
		                       KErrArgument);

	TInt err = KErrNone;
	if (aReason != KErrNone) 
		{
		RefreshErrorOccured(aTransactionId,aReason);
		err = iRefreshMgr.RemoveRefreshBy (this);
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::NextCSeq
// -----------------------------------------------------------------------------
//
TInt CSipRefreshBase::NextCSeq (TUint& aCSeq)
	{
	return iOwner->NextRefreshCSeq (aCSeq);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SIPSecUser
// -----------------------------------------------------------------------------
//	
const MSIPSecUser* CSipRefreshBase::SIPSecUser() const
    {
    return this;
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::TimerExpiredL (TTimerId aTimerId, TAny* /*aTimerParam*/)
	{
	__ASSERT_ALWAYS (aTimerId == iTimerId, User::Leave (KErrNotFound));

	if(NULL == iRequest)
	{
		StopTimer();
		return;
	}

	UpdateCSeqValueL(*iRequest); 

	TRAPD (err, SendL(iCurrentTransactionId, iRequest));

	if (err != KErrNone)
		{
        // Convert network states to a SIP stack error
        if (err == CSIPConnection::ESuspended ||
            err == CSIPConnection::EInactive ||
            err == CSIPConnection::EUnavailable)
            {
            err = KErrSIPTransportFailure;
            }
		User::LeaveIfError(RefreshErrorOccured(iCurrentTransactionId, err));
		User::LeaveIfError(iRefreshMgr.RemoveRefreshBy(this));
		}
	}
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::PassOnlyRealmsToUser
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
TBool CSipRefreshBase::PassOnlyRealmsToUser() const
    {
    return iSIPSecUser.PassOnlyRealmsToUser();
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::RequestCredentialsL
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::RequestCredentialsL(const TDesC8& aRealm)
    {
    iSIPSecUser.RequestCredentialsL(aRealm);
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::RequestCredentialsL
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::RequestCredentialsL(
    CSIPResponse& aResponse,
    TTransactionId aTransactionId,
    TRefreshId /*aRefreshId*/)
    {
    iSIPSecUser.RequestCredentialsL(aResponse,aTransactionId,iRefreshId);
    }	

// -----------------------------------------------------------------------------
// CSipRefreshBase::TrustedUser
// From MSIPSecUser
// -----------------------------------------------------------------------------
//	
const MSIPSecUser* CSipRefreshBase::TrustedUser(TRegistrationId aRegistrationId)
    {
    return iSIPSecUser.TrustedUser(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::ByPassSIPSec
// From MSIPSecUser
// -----------------------------------------------------------------------------
//
TBool CSipRefreshBase::ByPassSIPSec() const
    {
    return iSIPSecUser.ByPassSIPSec();
    }
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::SetRequest
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetRequest (CSIPRequest* aRequest,
								  TBool aDeleteRequest)
	{
	if (iOwnRequest)
		{
		delete iRequest;
		}
	iRequest = aRequest;
	iOwnRequest = aDeleteRequest;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SendL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SendL (TTransactionId& aTransactionId, 
							 CSIPRequest* aRequest)
	{
	if (iUpdatedHeader)
		{
		UpdateHeaderForL(*aRequest);
		}
	if (iRemoveRouteHeaders)
		{
		RemoveRouteHeadersFor(*aRequest);
		}
	if (iUpdatedRouteHeaders.Count() > 0)
		{
		UpdateRouteHeadersForL(*aRequest);
		}

	iTU.SendL (aTransactionId, iRegistrationId, aRequest,
		       this, *iRemoteTarget, 
		       iOwner->TransportParams(iRegistrationId), EFalse);
		       
	iIntervalUpdated = EFalse;      
	iCurrentTransactionId = aTransactionId;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SendAndGetHeadersL
// -----------------------------------------------------------------------------
//
MTransactionHeaders* CSipRefreshBase::SendAndGetHeadersL (
	TTransactionId& aTransactionId, 
	CSIPRequest*    aRequest)
	{
	MTransactionHeaders* headers = iTU.SendAndGetHeadersL(
		aTransactionId, iRegistrationId, aRequest, this,
		*iRemoteTarget, iOwner->TransportParams(iRegistrationId), EFalse);
	iCurrentTransactionId = aTransactionId;
	return headers;
	}
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::UpdateRemoteTargetL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::UpdateRemoteTargetL(CURIContainer& aRemoteTarget)
	{
	CURIContainer* tempRemoteTarget = CURIContainer::NewL(aRemoteTarget);	
	delete iRemoteTarget;
	iRemoteTarget = tempRemoteTarget;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SetUpdatedHeader
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetUpdatedHeader(CSIPHeaderBase* aHeader)
	{
	delete iUpdatedHeader;
	iUpdatedHeader = aHeader;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SetUpdatedRouteHeadersL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetUpdatedRouteHeadersL (
	const RPointerArray<CSIPRouteHeader>& aHeaderArray)
	{
	RPointerArray<CSIPHeaderBase> tempHeaderArray;
	CSIPHeaderBase::PushLC(&tempHeaderArray);

	for (TInt i = 0; i < aHeaderArray.Count(); i++)
		{
		CSIPHeaderBase* tempHeader = (aHeaderArray[i])->CloneL();
		CleanupStack::PushL(tempHeader);
		User::LeaveIfError(tempHeaderArray.Append(tempHeader));
		CleanupStack::Pop(tempHeader);
		}

	CleanupStack::Pop(); // tempHeaderArray
    iUpdatedRouteHeaders.ResetAndDestroy();
	iUpdatedRouteHeaders = tempHeaderArray;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::RemoveRouteHeaders
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::RemoveRouteHeaders ()
	{
	// reset the route headers array
	iUpdatedRouteHeaders.ResetAndDestroy();
	iRemoveRouteHeaders = ETrue;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SendUpdateRequestL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SendUpdateRequestL (TTransactionId& aTransactionId,
		                                  CSIPRequest*    aRequest)
	{
    CheckUpdateRequestMethodL(*aRequest);
    CheckUpdateRequestHeadersL(*aRequest);	
	
	delete iUpdatedHeader; 
	iUpdatedHeader = NULL;

	iUpdatedRouteHeaders.ResetAndDestroy();

	if (iTimerMgr.IsRunning(iTimerId)) 
		{
		iTimerMgr.Stop(iTimerId);
		}

	iTU.ClearTransactionOwner(iCurrentTransactionId);

	SendL(aTransactionId,aRequest);

	iFirstRequest = ETrue;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::UpdateRequestL 
// -----------------------------------------------------------------------------
//
void 
CSipRefreshBase::UpdateRequestL (TTransactionId& aTransactionId,
		                         CSIPRequest* aRequest)
	{
	CheckUpdateRequestExpiresValueL (*aRequest);
	SendUpdateRequestL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::TerminateRequestL 
// -----------------------------------------------------------------------------
//
void 
CSipRefreshBase::TerminateRequestL (TTransactionId& aTransactionId,
								    CSIPRequest* aRequest)
	{
	CheckTerminateRequestExpiresValueL(*aRequest);
	SendUpdateRequestL(aTransactionId, aRequest);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::IAPId
// -----------------------------------------------------------------------------
//
TUint CSipRefreshBase::IAPId () const
	{
	return iOwner->TransportParams(iRegistrationId).IapId();
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::ResetRegistrationId
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::ResetRegistrationId()
	{
	iRegistrationId = KEmptyRegistrationId;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::Received200ClassResponseL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::Received200ClassResponseL (CSIPResponse* aResponse)
	{
	TInt expirationTimeInRequest = ExpirationTimeFromRequestL(*iRequest);
	// if is terminate, expires in request is 0
	if (expirationTimeInRequest == 0)
		{	
		User::LeaveIfError(iRefreshMgr.RemoveRefreshBy (this));
		RefreshReceivedL(iCurrentTransactionId, aResponse);
		iTU.ClearTransactionOwner(this);
		return;
		}
		
    TInt expirationTimeInResponse = ExpirationTimeFromResponse(*aResponse);
	
	// response expire value lowered to 0
	if (expirationTimeInResponse == 0)
		{
		User::LeaveIfError(RefreshErrorOccured(iCurrentTransactionId, 
			               KErrSIPMalformedMessage));
		User::LeaveIfError(iRefreshMgr.RemoveRefreshBy (this));
		delete aResponse;
		return;
		}

	// no expires time from response
	if (expirationTimeInResponse == KErrNotFound)
		{
		// copy the expires value from request to response
		SetExpirationTimeToResponseL(*aResponse, expirationTimeInRequest);
		expirationTimeInResponse = ExpirationTimeFromResponse(*aResponse);
		}

	if (!iIntervalUpdated)
		{
		if(expirationTimeInResponse <= expirationTimeInRequest) 
			iIntervalInMilliSeconds = CalculateInterval(expirationTimeInResponse);
		else
			iIntervalInMilliSeconds = CalculateInterval(expirationTimeInRequest);	
		}
			
	StartTimerL(iIntervalInMilliSeconds);

	TRAPD(err, Forward2xxResponseToCallerL(aResponse));

	if (err)
		{
		iTimerMgr.Stop(iTimerId);
		User::Leave(err);
		}

	iTU.ClearTransactionOwner(iCurrentTransactionId);

	iFirstRequest = EFalse;
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::ReceivedErrorResponseL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::ReceivedErrorResponseL (CSIPResponse* aResponse)
	{
	User::LeaveIfError(iRefreshMgr.RemoveRefreshBy (this));
	RefreshReceivedL(iCurrentTransactionId, aResponse);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::ReceivedProvisonalResponseL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::ReceivedProvisonalResponseL (CSIPResponse* aResponse)
	{
	// if is first request, send response.
	if (iFirstRequest)
		{
		RefreshReceivedL(iCurrentTransactionId,aResponse);
		}
	else
		{
		delete aResponse;
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::UpdateCSeqValueL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::UpdateCSeqValueL(CSIPRequest& aRequest)
	{
	CSIPCSeqHeader* cseqHeader = aRequest.CSeq();

	__SIP_ASSERT_LEAVE (cseqHeader, KErrArgument);

	TUint cseq = cseqHeader->Seq();

	// ask Cseq value from refresh owner, leaves if error
	User::LeaveIfError(iOwner->NextRefreshCSeq(cseq));

	cseqHeader->SetSeq(cseq);
	}
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::UpdateHeaderForL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::UpdateHeaderForL(CSIPRequest& aRequest)
	{
	if (aRequest.HasHeader(iUpdatedHeader->Name()))
		{
		CSIPHeaderBase* oldHeader = aRequest.Header(iUpdatedHeader->Name(), 0);
		// ownership of iUpdatedHeader is passed.
		// the oldHeader is deleted.
		aRequest.ReplaceHeaderL(oldHeader, iUpdatedHeader);
		oldHeader = NULL; 
		}
	else
		{
		// the ownership of iUpdatedHeader header is transferred.
		aRequest.AddHeaderL(iUpdatedHeader);
		}
	iUpdatedHeader = NULL; // detach the iUpdatedHeader.
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::UpdateRouteHeadersForL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::UpdateRouteHeadersForL(CSIPRequest& aRequest)
	{
	if (aRequest.HasHeader(SIPStrings::StringF(SipStrConsts::ERouteHeader)))
		{
		aRequest.ReplaceHeadersL(iUpdatedRouteHeaders);
		}
	else
		{			 
		for (TInt i = 0; i < iUpdatedRouteHeaders.Count(); i++)
			{
			CSIPHeaderBase* header = (iUpdatedRouteHeaders[i])->CloneL();
			CleanupStack::PushL(header);
			aRequest.AddHeaderL(header);
			CleanupStack::Pop(header);
			}
		}
	// clear the route headers array after update those to aRequest
	iUpdatedRouteHeaders.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::RemoveRouteHeadersFor
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::RemoveRouteHeadersFor (CSIPRequest& aRequest)
	{	
	if (aRequest.HasHeader(SIPStrings::StringF(SipStrConsts::ERouteHeader)))
		{
		aRequest.DeleteHeaders(SIPStrings::StringF(SipStrConsts::ERouteHeader));
		}

	// change the flag to EFalse after remove route headers from aRequest
	iRemoveRouteHeaders = EFalse; 
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SetToFromOriginalRequestL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetToFromOriginalRequestL(CSIPRequest& aRequest)
	{
	CSIPToHeader* originalTo = 	iRequest->To();
    __SIP_ASSERT_LEAVE (originalTo, KErrGeneral);
	CSIPToHeader* newTo = CSIPToHeader::NewLC(*originalTo);
	aRequest.AddHeaderL(newTo);
	CleanupStack::Pop(newTo);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SetFromFromOriginalRequestL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetFromFromOriginalRequestL(CSIPRequest& aRequest)
	{
	CSIPFromHeader* originalFrom = iRequest->From();
    __SIP_ASSERT_LEAVE (originalFrom, KErrGeneral);	
	CSIPFromHeader* newFrom = CSIPFromHeader::NewLC(*originalFrom);
	aRequest.AddHeaderL(newFrom);
	CleanupStack::Pop(newFrom);
	}
	
// -----------------------------------------------------------------------------
// CSipRefreshBase::SetCallIdFromOriginalRequestL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetCallIdFromOriginalRequestL(CSIPRequest& aRequest)
	{
	const CSIPCallIDHeader* originalCallId = iRequest->CallID();
	__SIP_ASSERT_LEAVE (originalCallId != 0, KErrGeneral);	
	CSIPHeaderBase* newCallId = originalCallId->CloneL();
	CleanupStack::PushL(newCallId);
	aRequest.AddHeaderL(newCallId);
	CleanupStack::Pop(newCallId);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::SetCSeqFromOriginalRequestL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetCSeqFromOriginalRequestL(CSIPRequest& aRequest)
	{
	const CSIPCSeqHeader* originalCSeq = iRequest->CSeq();
	__SIP_ASSERT_LEAVE (originalCSeq != 0, KErrGeneral);

	CSIPCSeqHeader* newCseq = 
	    static_cast<CSIPCSeqHeader*>(originalCSeq->CloneL());
	CleanupStack::PushL(newCseq);    
	// ask the next CSeq number
	TUint cseq = newCseq->Seq();
	User::LeaveIfError(iOwner->NextRefreshCSeq(cseq));
	newCseq->SetSeq(cseq);

	aRequest.AddHeaderL(newCseq);
	CleanupStack::Pop(newCseq);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::RefreshErrorOccured
// -----------------------------------------------------------------------------
//
TInt CSipRefreshBase::RefreshErrorOccured(TTransactionId aTransactionId, 
										  TInt           aError)
	{
	return iOwner->RefreshError(iRefreshId, aTransactionId, aError);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::RefreshReceivedL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::RefreshReceivedL(TTransactionId aTransactionId, 
		                               CSIPResponse*  aResponse)
	{
    iOwner->RefreshReceivedL(aTransactionId, iRefreshId, aResponse);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::StartTimerL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::StartTimerL(TInt aMilliSeconds)
	{	
	iTimerId = iTimerMgr.StartL(this,aMilliSeconds);
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::ExpireTimeFromResponseExpiresHeader
// -----------------------------------------------------------------------------
//
TInt CSipRefreshBase::ExpireTimeFromResponseExpiresHeader (
    CSIPResponse& aResponse)
	{
	if(!aResponse.HasHeader(SIPStrings::StringF(SipStrConsts::EExpiresHeader)))
		{
		return KErrNotFound;
		}
	CSIPHeaderBase* resHeader = 
	    aResponse.Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader), 0);
	CSIPExpiresHeader* resContact = static_cast<CSIPExpiresHeader*>(resHeader);
	return resContact->Value();
	}


// -----------------------------------------------------------------------------
// CSipRefreshBase::SetExpireTimeToResponseExpiresHeaderL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::SetExpireTimeToResponseExpiresHeaderL (
    CSIPResponse& aResponse,
	TUint         aTimeInSeconds)
	{
	CSIPExpiresHeader* expires = static_cast<CSIPExpiresHeader*>(
	    aResponse.Header(SIPStrings::StringF(SipStrConsts::EExpiresHeader),0));	
	
	if(expires)
		{
		expires->SetValue(aTimeInSeconds);
		}
	else
		{
		expires = new(ELeave)CSIPExpiresHeader(aTimeInSeconds);
		CleanupStack::PushL(expires);
		aResponse.AddHeaderL(expires);
		CleanupStack::Pop(expires);
		}
	}

// -----------------------------------------------------------------------------
// CSipRefreshBase::CheckUpdateRequestMethodL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::CheckUpdateRequestMethodL(CSIPRequest& aRequest)
    {
    if (aRequest.Method().DesC().Length() == 0) 
		{
		// copy method from original request
		aRequest.SetMethodL(iRequest->Method()); 
		}
	else
		{
		if(aRequest.Method() != iRequest->Method())
			{
			User::Leave(KErrArgument); // methods are different
			}
		}
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::CheckUpdateRequestHeadersL
// -----------------------------------------------------------------------------
//
void CSipRefreshBase::CheckUpdateRequestHeadersL(CSIPRequest& aRequest)
    {
    if (!aRequest.To())
		{
		SetToFromOriginalRequestL(aRequest);
		}
	if (!aRequest.From())
		{
		SetFromFromOriginalRequestL(aRequest);
		}
	if (!aRequest.CallID())
		{
		SetCallIdFromOriginalRequestL(aRequest);
		}
	if (!aRequest.CSeq())
		{
		SetCSeqFromOriginalRequestL(aRequest);
		}
    }
    
// -----------------------------------------------------------------------------
// CSipRefreshBase::CalculateInterval
// -----------------------------------------------------------------------------
//  
TInt CSipRefreshBase::CalculateInterval(TInt aValueInSeconds) const
    {
    TInt interval = 0; // milliseconds
    if (aValueInSeconds > 0)
        {
	    const TInt KDelimiter1 = 1200;
	    const TInt KDelimiter2 = 600;
	    const TInt KDivider = 2;

	    // If expiration time is greater than 1200 seconds, 
	    // the interval is set to 600 seconds lower than expiration time.
	    // Otherwise, interval is set to half of the expiration time
	
	    if (aValueInSeconds > KDelimiter1)
		    {
		    interval = KOneSecInMilliSecs*(aValueInSeconds-KDelimiter2);
	
		    }
	    else 
		    {
		    interval = (KOneSecInMilliSecs*aValueInSeconds)/KDivider;
		    }
        }
	return interval;
    }

// -----------------------------------------------------------------------------
// CSipRefreshBase::StopTimer
// -----------------------------------------------------------------------------
//  
void CSipRefreshBase::StopTimer()
{
	if( iTimerMgr.IsRunning(iTimerId) )
	{
		iTimerMgr.Stop(iTimerId);
	}
}
