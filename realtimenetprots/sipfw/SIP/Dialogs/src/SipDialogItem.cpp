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
// Name          : SipDialogItem.cpp
// Part of       : SIPDialogs
// Version       : SIP/6.0 
//




#include "CSipDialogItem.h"
#include "MSipRefreshMgr.h"
#include "MTransactionUser.h"
#include "sipsec.h"
#include "DeleteMgr.h"
#include "MDialogOwner.h"
#include "SipLogs.h"
#include "SipAssert.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcontactheader.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcseqheader.h"
#include "sipcallidheader.h"
#include "siprecordrouteheader.h"
#include "siprouteheader.h"
#include "sipaddress.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "Establishing.h"
#include "Early.h"
#include "Confirmed.h"
#include "Terminated.h"
#include "DialogsStore.h"
#include "TransactionItemStore.h"
#include "TransactionItemBase.h"
#include "RefreshItem.h"
#include "TransactionItem.h"
#include "CSigCompMgr.h"
#include "MSigComp.h"
#include "sipcodecutils.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecerr.h"
#include "siperr.h"
#include "TSIPTransportParams.h"

const TUint K100Response = 100;
const TUint K400Response = 400;
const TUint K500Response = 500;


// -----------------------------------------------------------------------------
// CSipDialogItem::NewLC
// -----------------------------------------------------------------------------
//
CSipDialogItem* CSipDialogItem::NewLC(CDialogsStore& aDialogStore,
                                      MDialogOwner* aDialogOwner,
                                      MSIPSecUser& aSIPSecUser,
                                      MTransactionUser& aTransactionUser, 
                                      MSipRefreshMgr& aRefreshMgr,
                                      CSIPSec& aSIPSec,
                                      CDeleteMgr& aDeleteMgr,
                                      MSigComp& aSigComp,
                                      TRegistrationId aRegistrationId)
	{
	CSipDialogItem* self = new(ELeave)CSipDialogItem(aDialogStore,
	                                                 aSIPSecUser,
                                                     aTransactionUser,
                                                     aRefreshMgr,
                                                     aSIPSec,
                                                     aDeleteMgr,
                                                     aSigComp,
                                                     aRegistrationId);
	CleanupStack::PushL(self);	
	self->ConstructL(aDialogOwner);
	return self;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::CSipDialogItem
// -----------------------------------------------------------------------------
//
CSipDialogItem::CSipDialogItem (CDialogsStore& aDialogStore,
                                MSIPSecUser& aSIPSecUser,
								MTransactionUser& aTransactionUser, 
								MSipRefreshMgr& aRefreshMgr,
								CSIPSec& aSIPSec,
								CDeleteMgr& aDeleteMgr,
								MSigComp& aSigComp,
								TRegistrationId aRegistrationId)
: iRegistrationId(aRegistrationId),
  iDialogStore(aDialogStore),
  iTU(aTransactionUser),
  iRefreshMgr(aRefreshMgr),
  iSIPSec(aSIPSec), 
  iDeleteMgr(aDeleteMgr),
  iSigComp(aSigComp),
  iSIPSecUser(aSIPSecUser),
  iTerminated(*this),
  iConfirmed(*this,iTerminated),
  iEarly(*this,iConfirmed,iTerminated),
  iEstablishing(*this,iEarly,iConfirmed,iTerminated),    
  iRemoteTagSet(EFalse)
  	{
  	iTagParamName = SIPStrings::StringF(SipStrConsts::ETag);
  	iTransportParamName = SIPStrings::StringF(SipStrConsts::ETransport);
  	iContactHeaderName = SIPStrings::StringF(SipStrConsts::EContactHeader);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::ConstructL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::ConstructL (MDialogOwner* aDialogOwner)
	{
    __SIP_ASSERT_LEAVE(aDialogOwner, KErrArgument);

    iDialogOwner = aDialogOwner;
    iTransactionItemStore = CTransactionItemStore::NewL();
    iSigCompMgr = CSigCompMgr::NewL(
        aDialogOwner->TransportParams(KEmptyRegistrationId).IapId(),iSigComp);

    iDialogId = iDialogStore.NewDialogId();
	ChangeState(iEstablishing);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::~CSipDialogItem
// -----------------------------------------------------------------------------
//
CSipDialogItem::~CSipDialogItem()
	{
    iTU.ClearTransactionOwner(this);
    delete iSigCompMgr;
	delete iCallId;
	iLocalTag.Close();
	iRemoteTag.Close();
	delete iLocalAddress;
	delete iRemoteAddress;
	delete iLocalTarget;
	delete iRemoteTarget;
	iRouteSet.ResetAndDestroy();
    delete iTransactionItemStore;
    iSIPSec.ClearCache(this);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::IapId
// -----------------------------------------------------------------------------
//
TUint32 CSipDialogItem::IapId()
    {
    return TransportParams(KEmptyRegistrationId).IapId();
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::RegistrationId
// -----------------------------------------------------------------------------
//
TRegistrationId CSipDialogItem::RegistrationId() const
    {
    return iRegistrationId;
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::Offset
// -----------------------------------------------------------------------------
//
TInt CSipDialogItem::Offset ()
	{
    return _FOFF(CSipDialogItem,iLink);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::ResetStateL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::ResetStateL()
    {
    CTransactionItemStore* transactionStore = CTransactionItemStore::NewLC();
    CSigCompMgr* sigCompMgr = 
        CSigCompMgr::NewL(
            iDialogOwner->TransportParams(KEmptyRegistrationId).IapId(),
            iSigComp);
    CleanupStack::Pop(transactionStore);
    iTU.ClearTransactionOwner(this);
    iSIPSec.ClearCache(this);
    delete iTransactionItemStore;
    iTransactionItemStore = transactionStore;  
    delete iSigCompMgr;
    iSigCompMgr = sigCompMgr;
    iRemoteTag = SIPStrings::StringF(SipStrConsts::EEmpty);
    iRemoteTagSet = EFalse;
    ChangeState(iEstablishing);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::DialogOwner
// -----------------------------------------------------------------------------
//
MDialogOwner* CSipDialogItem::DialogOwner()
	{
	return iDialogOwner;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::DeleteMgr
// -----------------------------------------------------------------------------
//
CDeleteMgr& CSipDialogItem::DeleteMgr()
	{
	return iDeleteMgr;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::Match
// -----------------------------------------------------------------------------
//	
TBool CSipDialogItem::Match(CSIPRequest& aRequest) const
    {
	CSIPCallIDHeader* callId = aRequest.CallID();
	CSIPToHeader* to = aRequest.To();	
	CSIPFromHeader* from = aRequest.From();
	
	if ((callId && *callId == *iCallId) &&
		(to && 
		 to->HasParam(iTagParamName) && 
	     iLocalTag == to->ParamValue(iTagParamName)))
		{
		if (iRemoteTagSet)
			{
			if (from && 
			    from->HasParam(iTagParamName) &&
                iRemoteTag == from->ParamValue(iTagParamName))
                {
                return iCurrentState->Accept(aRequest);
                }
            }
        else
            {
			return iCurrentState->Accept(aRequest);
            }
		}
		
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::ContactOk
// -----------------------------------------------------------------------------
//    
TBool CSipDialogItem::ContactOk(const CSIPContactHeader& aContact)
    {		
	if (aContact.Star() || !aContact.SIPAddress())
        {
        return EFalse;
        }
    return (aContact.SIPAddress()->URI().IsSIPURI());   
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::SetFirstTransactionIdId
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetFirstTransactionId(const TTransactionId& aTransactionId)
	{
	iFirstTransactionId = aTransactionId;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::FirstTransactionId
// -----------------------------------------------------------------------------
//
TTransactionId CSipDialogItem::FirstTransactionId()
	{
	return iFirstTransactionId;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::DialogId
// -----------------------------------------------------------------------------
//
TDialogId CSipDialogItem::DialogId() const
	{
	return iDialogId;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SelfDestruct
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SelfDestruct ()
    {
	iTU.ClearTransactionOwner(this);
	ChangeState(iTerminated);
    iDialogStore.Remove(this);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::TerminateForkedDialogs
// -----------------------------------------------------------------------------
//
void CSipDialogItem::TerminateForkedDialogs (TTransactionId aTransactionId)
    {
    iDialogStore.RemoveAllEarlyDialogsWith(aTransactionId);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::IsEarly
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::IsEarly () const
    {
    return (iCurrentState == &iEarly);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::TransactionItems
// -----------------------------------------------------------------------------
//
CTransactionItemStore& CSipDialogItem::TransactionItems ()
    {
    return *iTransactionItemStore;
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::HasTransactionItem
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::HasTransactionItem (TTransactionId aTransactionId)
    {
    return iTransactionItemStore->HasItem(aTransactionId);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::RemoveTransactionItem
// -----------------------------------------------------------------------------
//
TInt CSipDialogItem::RemoveTransactionItem(TTransactionId& aTransactionId)
    {
    return iTransactionItemStore->RemoveItem(aTransactionId);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::ChangeState
// -----------------------------------------------------------------------------
//
void CSipDialogItem::ChangeState(TDialogStateBase& aState)
	{
	if (iCurrentState != &aState)
		{
		iCurrentState = &aState;
		__SIP_INT_LOG1( "CSipDialogItem::ChangeState: ID", iDialogId )
		__SIP_DES8_LOG( "CSipDialogItem::ChangeState: state", aState.Name() )
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::CurrentState
// -----------------------------------------------------------------------------
//
TDialogStateBase& CSipDialogItem::CurrentState()
	{
	return *iCurrentState;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::CallId
// -----------------------------------------------------------------------------
//
CSIPCallIDHeader* CSipDialogItem::CallId()
	{
	return iCallId;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetCallId
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetCallId (CSIPCallIDHeader* aCallId)
	{
    delete iCallId;
	iCallId = aCallId;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::LocalTag
// -----------------------------------------------------------------------------
//
RStringF CSipDialogItem::LocalTag ()
	{
	return iLocalTag;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetLocalTag
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetLocalTag (RStringF aTag)
	{
	iLocalTag.Close();
	iLocalTag = aTag.Copy();
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::RemoteTag
// -----------------------------------------------------------------------------
//
RStringF CSipDialogItem::RemoteTag ()
	{
	return iRemoteTag;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::RemoteTag
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::RemoteTagSet ()
	{
	return iRemoteTagSet;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRemoteTagL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRemoteTagL (CSIPFromToHeaderBase* aFromTo)
	{
    if (aFromTo->HasParam(iTagParamName))
        {
	    if (iRemoteTagSet)
            {
            if (iRemoteTag != aFromTo->ParamValue(iTagParamName))
                {
                User::Leave (KErrAlreadyExists);
                }
            }
        else
            {
            RStringF tmp = aFromTo->ParamValue(iTagParamName);
			UpdateRemoteTagForRefreshesL(tmp);
	        iRemoteTag.Close();
            iRemoteTag = tmp.Copy();
			iRemoteTagSet = ETrue;
            }
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::LocalSeqNum
// -----------------------------------------------------------------------------
//
TUint CSipDialogItem::LocalSeqNum()
	{
	return iLocalSeqNum;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetLocalSeqNum
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetLocalSeqNum(TUint aNum)
	{
	iLocalSeqNum = aNum;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::RemoteSeqNum
// -----------------------------------------------------------------------------
//
TUint CSipDialogItem::RemoteSeqNum()
	{
	return iRemoteSeqNum;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRemoteSeqNum
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRemoteSeqNum(TUint aNum)
	{
	iRemoteSeqNum = aNum;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::LocalAddress
// -----------------------------------------------------------------------------
//
const CSIPAddress* CSipDialogItem::LocalAddress()
	{
	return iLocalAddress;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetLocalAddressL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetLocalAddressL (const CSIPAddress& aAddr)
	{
	CSIPAddress* tmp = CSIPAddress::NewL(aAddr);
	delete iLocalAddress;
	iLocalAddress = tmp;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::RemoteAddress
// -----------------------------------------------------------------------------
//
const CSIPAddress* CSipDialogItem::RemoteAddress()
	{
	return iRemoteAddress;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRemoteAddressL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRemoteAddressL (const CSIPAddress& aAddr)
	{
	CSIPAddress* tmp = CSIPAddress::NewL(aAddr);
	delete iRemoteAddress;
	iRemoteAddress = tmp;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::RemoteTarget
// -----------------------------------------------------------------------------
//
CURIContainer* CSipDialogItem::RemoteTarget()
	{
	return iRemoteTarget;
	}
	
// -----------------------------------------------------------------------------
// CSipDialogItem::SetInitialRemoteTargetL
// -----------------------------------------------------------------------------
//	
void CSipDialogItem::SetInitialRemoteTargetL (CURIContainer& aUri)
    {
    CURIContainer* tmp = CURIContainer::NewL(aUri);
	delete iRemoteTarget;
    iRemoteTarget = tmp;    
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRemoteTargetL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRemoteTargetL (CURIContainer& aUri)
	{
	__ASSERT_ALWAYS(aUri.IsSIPURI(), User::Leave(KErrSIPInvalidURIType));
	if (iLocalTarget)
		{
		CSIPURI* localTargetURI = iLocalTarget->SIPAddress()->URI().SIPURI();
		iSigCompMgr->UpdateL(aUri,iRouteSet,*localTargetURI);
		}
    CURIContainer* tmp = CURIContainer::NewL(aUri);
	if (iTransactionItemStore->HasRefreshedItems())
		{
		CleanupStack::PushL(tmp);
		iRefreshMgr.UpdateRemoteTargetL(this,*tmp);
		CleanupStack::Pop(tmp);
		}
	delete iRemoteTarget;
    iRemoteTarget = tmp;
 	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRemoteTargetWithMsgL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRemoteTargetWithMsgL (CSIPMessage& aMsg)
	{
	CSIPContactHeader* contact = static_cast<CSIPContactHeader*> 
                                   (aMsg.Header(iContactHeaderName,0));
	if (contact && contact->SIPAddress())
        {
		SetRemoteTargetL(contact->SIPAddress()->URI());
		// Contact is updated only for outgoing requests
		UpdateTransportForNextHopAndLocalTargetL(NULL);
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::LocalTarget
// -----------------------------------------------------------------------------
//
CSIPContactHeader* CSipDialogItem::LocalTarget()
	{
	return iLocalTarget;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetLocalTargetL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetLocalTargetL(CSIPContactHeader& aContact,
                                     TBool aUpdateSigCompParam)
	{
	__ASSERT_ALWAYS(aContact.SIPAddress(), 
	                User::Leave(KErrSIPInvalidURIType));
	CURIContainer& contactURI = aContact.SIPAddress()->URI();
	__ASSERT_ALWAYS(contactURI.IsSIPURI(), User::Leave(KErrSIPInvalidURIType));
    if (iRouteSet.Count() > 0 && iRouteSet[0] != NULL &&
        iRouteSet[0]->SIPAddress().URI().IsSIPURI() && 
        iRouteSet[0]->SIPAddress().URI().SIPURI()->IsSIPSURI())
        {
        // Next hop has sips-scheme
		aContact.SIPAddress()->URI().SIPURI()->SetSIPS(ETrue);
		}	
	if (iRemoteTarget)
		{
		iSigCompMgr->UpdateL(*iRemoteTarget, iRouteSet,
		                     *contactURI.SIPURI(), aUpdateSigCompParam);
		}
    CSIPContactHeader* tmp = static_cast<CSIPContactHeader*>(aContact.CloneL());
	delete iLocalTarget;
	iLocalTarget = tmp;
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::SetLocalTargetWithMsgL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetLocalTargetWithMsgL(CSIPMessage& aMsg, 
											TBool aUpdateSigCompParam)
	{
	CSIPContactHeader* contact = 
	    static_cast<CSIPContactHeader*>(aMsg.Header(iContactHeaderName,0));
	if (contact)
        {       
		SetLocalTargetL(*contact,aUpdateSigCompParam);
		UpdateTransportForNextHopAndLocalTargetL(contact);
		if (aMsg.IsRequest())
		    {
		    // UpdateTransportForNextHopAndLocalTargetL 
		    // may have updated the next hop transport
		    aMsg.DeleteHeaders(SIPStrings::StringF(SipStrConsts::ERouteHeader));
		    AddRouteToMessageL(aMsg);
		    }
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRouteSetL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRouteSetL(RPointerArray<CSIPRouteHeaderBase>& aRoute)
	{
	RPointerArray<CSIPRouteHeader>* routeSet = 
        new(ELeave)RPointerArray<CSIPRouteHeader>;
    TCleanupItem cleanupItem(CleanupRouteSet,routeSet);
	CleanupStack::PushL (cleanupItem);

	for (TInt i=0; i < aRoute.Count(); i++)
		{
		CSIPRouteHeader* routeHeader = 
		    SIPCodecUtils::CreateRouteHeaderLC(*(aRoute[i]));    
		User::LeaveIfError(routeSet->Append(routeHeader));
		CleanupStack::Pop(routeHeader);
		}

	/* Ownership is transferred for routeset, so popping routset before passing 
	 * to SetRouteSet()
	 */
	CleanupStack::Pop(1); // cleanupItem
	SetRouteSet(routeSet); // ownership transferred
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRouteSetUsingRouteL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRouteSetUsingRouteL (CSIPMessage& aMessage)
	{
	TSglQueIter<CSIPHeaderBase> iter = 
        aMessage.Headers(SIPStrings::StringF(SipStrConsts::ERouteHeader));
	
	RPointerArray<CSIPRouteHeader>* routeSet = 
        new(ELeave)RPointerArray<CSIPRouteHeader>;
    TCleanupItem cleanupItem(CleanupRouteSet,routeSet);
	CleanupStack::PushL (cleanupItem);

	while (iter)
		{
		CSIPHeaderBase* header = iter++;
		CSIPRouteHeader* newRoute = 
		    static_cast<CSIPRouteHeader*>(header->CloneL());
		CleanupStack::PushL(newRoute);
        User::LeaveIfError(routeSet->Append(newRoute));
        CleanupStack::Pop(newRoute);
		}

	/* Ownership is transferred for routeset, so popping routset before passing 
	 * to SetRouteSet()
	 */
	CleanupStack::Pop(1); // cleanupItem
	SetRouteSet(routeSet); // ownership transferred
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRouteSetUsingRecordRouteL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRouteSetUsingRecordRouteL (CSIPMessage& aMessage, 
												   TBool aIsUAC,
												   TInt aLeaveCode)
	{
	TSglQueIter<CSIPHeaderBase> iter = 
        aMessage.Headers(SIPStrings::StringF(SipStrConsts::ERecordRouteHeader));
	
	if (iter)
		{
		RPointerArray<CSIPRouteHeader>* routeSet = 
            new(ELeave)RPointerArray<CSIPRouteHeader>;
		TCleanupItem cleanupItem(CleanupRouteSet,routeSet);
		CleanupStack::PushL (cleanupItem);
	
	    while (iter)
    		{
	    	CSIPHeaderBase* header = iter++;
		    CSIPRecordRouteHeader* recRoute = 
                static_cast<CSIPRecordRouteHeader*>(header);
	    	CSIPRouteHeader* newRoute = 
	    	    SIPCodecUtils::CreateRouteHeaderLC(*recRoute);
            if (aIsUAC)
                {
		    	User::LeaveIfError(routeSet->Insert(newRoute, 0));
                }
	    	else
                {
	    		User::LeaveIfError(routeSet->Append(newRoute));
                }
		    CleanupStack::Pop(newRoute);
		    }
		CSIPRouteHeader* firstRoute = (*routeSet)[0];
		if (!firstRoute->SIPAddress().URI().IsSIPURI())
		    {
		    User::Leave(aLeaveCode);
		    }           
		iRefreshMgr.UpdateRouteSetsL(this,*routeSet);

		/* Ownership is transferred for routeset, so popping routset before passing 
	 	* to SetRouteSet()
	 	*/
		CleanupStack::Pop(1); // cleanupItem
		SetRouteSet(routeSet); // ownership transferred
    	
	    }
    else
		{
		iRefreshMgr.RemoveRouteSets(this);
		iRouteSet.ResetAndDestroy();
		}
	// Route updated, tell Refreshes not to include Registration Id 
	// for the calls to TU anymore.
	iRefreshMgr.SetRegistrationIdsEmpty(this);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SetRouteSet
// -----------------------------------------------------------------------------
//
void CSipDialogItem::SetRouteSet (RPointerArray<CSIPRouteHeader>* aRouteSet)
	{
	iRouteSet.ResetAndDestroy();
	if (aRouteSet)
		{
		iRouteSet = *aRouteSet;
		
		delete aRouteSet;
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::HasRouteSet
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::HasRouteSet() const
    {
    return (iRouteSet.Count() > 0);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::AddRouteToMessageL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::AddRouteToMessageL(CSIPMessage& aMessage)
	{
	for (TInt i=0; i < iRouteSet.Count(); i++)
		{
		CSIPRouteHeader* route = 
		    static_cast<CSIPRouteHeader*>(iRouteSet[i]->CloneL());
		CleanupStack::PushL(route);		    
		aMessage.AddHeaderL(route);
		CleanupStack::Pop(route);
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::AddRecordRouteToMessageL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::AddRecordRouteToMessageL(CSIPMessage& aMessage)
	{
	for (TInt i=0; i < iRouteSet.Count(); i++)
		{
		CSIPRecordRouteHeader* route = 
		    SIPCodecUtils::CreateRecordRouteHeaderLC(*(iRouteSet[i]));
		aMessage.AddHeaderL(route);
		CleanupStack::Pop(route);
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::AddToHeaderInDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::AddToHeaderInDialogL(CSIPRequest& aRequest)
	{
    if (!aRequest.To())
        {
	    CSIPAddress* address = CSIPAddress::NewLC(*iRemoteAddress);
	    CSIPToHeader* to = CSIPToHeader::NewL(address);
	    CleanupStack::Pop(address);
	    CleanupStack::PushL(to);
	    if (iRemoteTagSet)
            {
    		to->SetParamL(iTagParamName,iRemoteTag);
			}
    	aRequest.AddHeaderL(to);
    	CleanupStack::Pop(to);
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::AddFromHeaderInDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::AddFromHeaderInDialogL(CSIPRequest& aRequest)
	{
    if (!aRequest.From())
        {
    	CSIPAddress* address = CSIPAddress::NewLC(*iLocalAddress);
    	CSIPFromHeader* from = CSIPFromHeader::NewL(address);
    	CleanupStack::Pop(address);
    	CleanupStack::PushL(from);
    	from->SetParamL(iTagParamName,iLocalTag);
    	aRequest.AddHeaderL(from);
    	CleanupStack::Pop(from);
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::AddCallIdHeaderInDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::AddCallIdHeaderInDialogL(CSIPMessage& aMessage)
	{    
    if (!aMessage.CallID())
        {
    	CSIPCallIDHeader* callId = 
    	    static_cast<CSIPCallIDHeader*>(iCallId->CloneL());
    	CleanupStack::PushL(callId);
    	aMessage.AddHeaderL(callId);
    	CleanupStack::Pop(callId);
        }
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::AddCSeqHeaderInDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::AddCSeqHeaderInDialogL(CSIPRequest& aRequest)
	{
    if (!aRequest.CSeq())
        {
    	CSIPCSeqHeader* cseq = 
    	    CSIPCSeqHeader::NewLC(iLocalSeqNum+1,aRequest.Method());
    	aRequest.AddHeaderL(cseq);
    	CleanupStack::Pop(cseq);
    	iLocalSeqNum++;
    	}
    }
    
// -----------------------------------------------------------------------------
// CSipDialogItem::ReceiveL
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
void CSipDialogItem::ReceiveL (TTransactionId aTransactionId,
                               CSIPResponse* aResponse)
	{
    __SIP_ASSERT_LEAVE(aTransactionId, KErrSIPInvalidDialogResponse);
	__SIP_ASSERT_LEAVE(aResponse, KErrSIPInvalidDialogResponse);
    CTransactionItemBase* transactionItem = 
        iTransactionItemStore->FindItem(aTransactionId);
    __SIP_ASSERT_LEAVE (transactionItem, KErrNotFound);
    
	__SIP_MESSAGE_LOG("Dialogs", *aResponse)
	
	if (transactionItem->TargetRefresh())
	    {
	    CheckContactsL(aTransactionId,*aResponse);
	    }

    CSIPToHeader* to = aResponse->To();
	if (!CheckResponse(aResponse) || 
        (aResponse->ResponseCode() > K100Response &&
         iRemoteTagSet && iRemoteTag != to->ParamValue(iTagParamName) && 
         aTransactionId != FirstTransactionId()))
		{
        iDialogOwner->DialogsError(aTransactionId,KEmptyRefreshId,iDialogId,
                                   KErrSIPInvalidDialogResponse);
		delete aResponse;
		return;
		}

	if (aResponse->ResponseCode() > K100Response && 
	    !aResponse->IsErrorResponse() &&
		iRemoteTagSet && 
		iRemoteTag != to->ParamValue(iTagParamName))
		{
		if(to->HasParam(iTagParamName))
			{		
			CSipDialogItem* dialog = 
                iDialogStore.FindDialogItemForPass(aTransactionId,aResponse);
			if(!dialog)
				{
				dialog = CreateForkingDialogLC(); 
                dialog->SetRemoteTagL(to);
				dialog->ReceiveL(aTransactionId, aResponse);
            	iDialogStore.AddDialogItem(dialog);
	            CleanupStack::Pop(dialog);
				}
            else
                {
				dialog->ReceiveL(aTransactionId, aResponse);
                }
            return;
			}
		}
		
	iCurrentState->ReceiveL(*transactionItem,aResponse);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::ReceiveL
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
void CSipDialogItem::ReceiveL (TUint32 aIapId,
                               TTransactionId aTransactionId,
                               CSIPRequest* aRequest)
	{
	__SIP_ASSERT_LEAVE(aTransactionId, KErrSIPInvalidDialogResponse);
	__SIP_ASSERT_LEAVE(aRequest, KErrSIPInvalidDialogRequest);
	__SIP_ASSERT_LEAVE(CheckRequest(aRequest), KErrSIPMalformedMessage);
	__SIP_ASSERT_LEAVE(aIapId == IapId(), KErrArgument);
	__SIP_MESSAGE_LOG("Dialogs", *aRequest)

    TSIPTransportParams transportParams(TransportParams(KEmptyRegistrationId));
	if (!CheckContacts(*aRequest))
		{
		if (aRequest->Method() == SIPStrings::StringF(SipStrConsts::EAck))
			{
			iDialogOwner->DialogsError(aTransactionId, KEmptyRefreshId,
                                       iDialogId, KErrSIPNoAckReceived);
			delete aRequest;
			return;
			}
		CSIPResponse* response = 
		    CSIPResponse::NewLC(K400Response,
		        SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
		iTU.SendL(aTransactionId, response, this, transportParams);
		CleanupStack::Pop(response);
		delete aRequest;
		return;
		}

	// check cseq compared to iRemoteSeqNum
	if (aRequest->Method() != SIPStrings::StringF(SipStrConsts::EAck))
		{	
		if (iRemoteSeqNum > aRequest->CSeq()->Seq())
			{
			CSIPResponse* response = 
			    CSIPResponse::NewLC(K500Response,
			        SIPStrings::StringF(
			            SipStrConsts::EPhraseServerInternalError));
			iTU.SendL(aTransactionId, response, this, transportParams);
			CleanupStack::Pop(response);
			delete aRequest;
			return;
			}
		}

	iRemoteSeqNum = aRequest->CSeq()->Seq();
    CTransactionItem* transactionItem = 
        CTransactionItem::NewLC(iTU,this,aTransactionId,aRequest->Method());
    iCurrentState->ReceiveL(aTransactionId, aRequest);
	iTransactionItemStore->AddItem(transactionItem);
	CleanupStack::Pop(transactionItem);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::TransactionEnded
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
TInt CSipDialogItem::TransactionEnded (TUint32 /*aIapId*/,
                                       TTransactionId aTransactionId, 
                                       TInt aReason)
	{
    TInt err = KErrNone;

    CTransactionItemBase* item = 
        iTransactionItemStore->FindItem(aTransactionId);
    if (item && 
        aReason == KErrNone && 
        item->Method() == SIPStrings::StringF(SipStrConsts::EInvite))
        {
        err = iDialogOwner->InviteTransactionCompleted(aTransactionId);
        }

    iDialogStore.TransactionEnded(aTransactionId,aReason);

	return err;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::NextCSeq
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//
TInt CSipDialogItem::NextCSeq(TUint& aCSeq)
	{
	aCSeq = ++iLocalSeqNum;
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CSipDialogItem::SIPSecUser
// From MTransactionOwner:
// -----------------------------------------------------------------------------
//    
const MSIPSecUser* CSipDialogItem::SIPSecUser() const
    {
    return this;
    }    

// -----------------------------------------------------------------------------
// CSipDialogItem::RefreshReceivedL
// From MRefreshOwner:
// -----------------------------------------------------------------------------
//
void CSipDialogItem::RefreshReceivedL (TTransactionId aTransactionId,
                                       TRefreshId aRefreshId,
                                       CSIPResponse* aResponse)
	{
    __SIP_ASSERT_LEAVE(aTransactionId, KErrSIPInvalidDialogResponse);
    __SIP_ASSERT_LEAVE(aResponse, KErrSIPInvalidDialogResponse);
	__SIP_MESSAGE_LOG("Dialogs", *aResponse)

	// Check Call-ID, CSeq and From-tag
	if (!CheckResponse(aResponse))
		{
		iDialogOwner->DialogsError(aTransactionId, aRefreshId, iDialogId,
                                   KErrSIPInvalidDialogResponse);
		delete aResponse;
		return;
		}

	CRefreshItem* item = iTransactionItemStore->FindRefreshItem(aRefreshId);
	if (!item)
		{
		iDialogOwner->DialogsError(aTransactionId,aRefreshId,iDialogId,
                                   KErrSIPInvalidTransactionState);
		delete aResponse;
		return;
		}

	if (iRemoteTagSet)
		{
		if (iRemoteTag != aResponse->To()->ParamValue(iTagParamName))
			{
			iDialogOwner->DialogsError(aTransactionId,aRefreshId,iDialogId, 
                                       KErrSIPInvalidDialogResponse);
			delete aResponse;
			return;
			}
		}

	if (item->TargetRefresh())
	    {
	    CheckContactsL(aTransactionId,*aResponse);
	    }

	if (aResponse->IsErrorResponse())
		{
		item->SetTerminated();
		}

    item->SetTransactionId(aTransactionId);
	iCurrentState->ReceiveL(*item, aResponse);
	}
	
// -----------------------------------------------------------------------------
// CSipDialogItem::RefreshError
// From MRefreshOwner:
// -----------------------------------------------------------------------------
//
TInt CSipDialogItem::RefreshError (TRefreshId aRefreshId,
                                   TTransactionId aTransactionId,
                                   TInt aReason)
	{
	CTransactionItemBase* refreshItem = 
		iTransactionItemStore->FindItem(aTransactionId);
	if (!refreshItem)
		{
		refreshItem = iTransactionItemStore->FindRefreshItem(aRefreshId);
		if (!refreshItem)
			{
			return KErrArgument;
			}
		}
    refreshItem->SetTransactionId(aTransactionId);
    iDialogStore.TransactionEnded(refreshItem->Id(),aReason);
	return KErrNone;
	}	
	
// -----------------------------------------------------------------------------
// CSipDialogItem::NextRefreshCSeq
// From MRefreshOwner:
// -----------------------------------------------------------------------------
//
TInt CSipDialogItem::NextRefreshCSeq(TUint& aCSeq)
	{
	return NextCSeq(aCSeq);
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::TransportParams
// From MRefreshOwner:
// -----------------------------------------------------------------------------
//
TSIPTransportParams CSipDialogItem::TransportParams(
    const TRegistrationId& aRegistrationId)
	{
	TSIPTransportParams transportParams(
	    iDialogOwner->TransportParams(aRegistrationId));
	if (aRegistrationId == KEmptyRegistrationId)
	    {
	    // Registration id is set empty after receiving the first
	    // response to the refreshed initial request.
	    // See function SetRouteSetUsingRecordRouteL.
	    transportParams.SetCompartmentId(iSigCompMgr->CompartmentId());
	    }
	return transportParams;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::SIPSecUser
// From MRefreshOwner:
// -----------------------------------------------------------------------------
//	
const MSIPSecUser* CSipDialogItem::SIPSecUser(
    const TRegistrationId& aRegistrationId)
    {
    return iDialogOwner->SIPSecUser(aRegistrationId);
    }
	
// -----------------------------------------------------------------------------
// CSipDialogItem::RefreshDurationL
// -----------------------------------------------------------------------------
//
TUint CSipDialogItem::RefreshDurationL()
	{
	User::Leave(KErrNotFound);
	return 0;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::PassOnlyRealmsToUser
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//    
TBool CSipDialogItem::PassOnlyRealmsToUser() const
    {
    return iSIPSecUser.PassOnlyRealmsToUser();
    }
    
// -----------------------------------------------------------------------------
// CSipDialogItem::RequestCredentialsL
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//
void CSipDialogItem::RequestCredentialsL(const TDesC8& aRealm)
    {
    iSIPSecUser.RequestCredentialsL(aRealm);
    }    	
	
// -----------------------------------------------------------------------------
// CSipDialogItem::RequestCredentialsL
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//	
void CSipDialogItem::RequestCredentialsL(CSIPResponse& aResponse,
	                                     TTransactionId aTransactionId,
	                                     TRefreshId aRefreshId)
    {
    iSIPSecUser.RequestCredentialsL(aResponse,aTransactionId,aRefreshId);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::TrustedUser
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//    
const MSIPSecUser* CSipDialogItem::TrustedUser(
    TRegistrationId /*aRegistrationId*/)
    {
    return iDialogOwner->SIPSecUser(iRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::ByPassSIPSec
// From MSIPSecUser:
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::ByPassSIPSec() const
    {
    const MSIPSecUser* trustedUser = iDialogOwner->SIPSecUser(iRegistrationId);
    if (trustedUser)
        {
        // Use the same value as the related registration, if possible
        return trustedUser->ByPassSIPSec();
        }
    return iSIPSecUser.ByPassSIPSec();
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::EndTransaction
// -----------------------------------------------------------------------------
//
void CSipDialogItem::EndTransaction (TTransactionId aTransactionId,
									 TInt aReason)
	{
    CTransactionItemBase* transactionItem = 
        iTransactionItemStore->FindItem(aTransactionId);
	if (transactionItem)
        {
	    iCurrentState->EndTransaction(*transactionItem, aReason);
	    iTransactionItemStore->RemoveItem(transactionItem->Id());
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::CheckResponse
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::CheckResponse(CSIPResponse* aResponse)
	{
	CSIPCallIDHeader* callId = aResponse->CallID();
    if (!callId)
        {
        return EFalse;
        }
    if (!(*iCallId == *callId))
        {
        return EFalse;
        }
	CSIPFromHeader* from = aResponse->From();
    if (!from)
        {
        return EFalse;
        }   
    if (iLocalTag != from->ParamValue(iTagParamName))
        {
        return EFalse;
        }
    if (!aResponse->CSeq())
        {
        return EFalse;
        }
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::CheckRequest
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::CheckRequest(CSIPRequest* aRequest)
	{
	CSIPCallIDHeader* callId = aRequest->CallID();
    if (!callId)
        {
        return EFalse;
        }
    if (!(*iCallId == *callId))
        {
        return EFalse;
        }
	CSIPToHeader* to = aRequest->To();
    if (!to)
        {
        return EFalse;
        }   
    if (iLocalTag != to->ParamValue(iTagParamName))
        {
        return EFalse;
        }
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::UpdateRemoteTagForRefreshesL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::UpdateRemoteTagForRefreshesL (RStringF aRemoteTag)
	{
    RArray<TRefreshId> ids = iTransactionItemStore->RefreshItemIdsL();
    CleanupClosePushL(ids);
    for (TInt i=0; i<ids.Count(); i++)
		{					
		TRefreshId refreshId = ids[i];
		CSIPRequest* refreshReq = iRefreshMgr.Request(refreshId);
		if (refreshReq)
			{
			CSIPToHeader* refreshReqTo = refreshReq->To();
			if (refreshReqTo && !refreshReqTo->HasParam(iTagParamName))
				{
				refreshReqTo->SetParamL(iTagParamName,aRemoteTag);
				}
			}
		}
    CleanupStack::PopAndDestroy(1); // ids
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::UpdateLocalTargetL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::UpdateLocalTargetL(CSIPMessage& aMessage)
	{
	SetLocalTargetWithMsgL(aMessage);
	CSIPContactHeader* contact = 
	    static_cast<CSIPContactHeader*>(aMessage.Header(iContactHeaderName,0));
	
    RArray<TRefreshId> ids = iTransactionItemStore->RefreshItemIdsL();
    CleanupClosePushL(ids);
    for (TInt i=0; i<ids.Count(); i++)
		{
		CSIPContactHeader* newContact = 
		    static_cast<CSIPContactHeader*>(contact->CloneL());
		CleanupStack::PushL(newContact);
		iRefreshMgr.UpdateHeaderL(ids[i],this,newContact);
		CleanupStack::Pop(newContact);
		}
    CleanupStack::PopAndDestroy(1); // ids
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::CheckContacts
// -----------------------------------------------------------------------------
//
TBool CSipDialogItem::CheckContacts (CSIPMessage& aMessage, 
                                     TBool aContactRequired) const
	{
	TInt contactCount = aMessage.HeaderCount(iContactHeaderName);
    if (contactCount == 1)
        {
		CSIPContactHeader* contact = 
            static_cast<CSIPContactHeader*>(
                aMessage.Header(iContactHeaderName,0));
		if (!ContactOk(*contact))
            {
			return EFalse;
            }
        }
    if (aContactRequired)
        {
        return (contactCount == 1);
        }
    return (contactCount <= 1);
	}
	
// -----------------------------------------------------------------------------
// CSipDialogItem::CheckContactsL
// -----------------------------------------------------------------------------
//	
void CSipDialogItem::CheckContactsL (TTransactionId aTransactionId,
                                     CSIPResponse& aResponse) const
    {
    if (aResponse.ResponseCode() > K100Response && 
        !aResponse.IsErrorResponse())
        {        
        TBool contactRequired = 
            (aResponse.Type() == CSIPResponse::E2XX &&
             aTransactionId == iFirstTransactionId);
        if (!CheckContacts(aResponse,contactRequired))
            {
            User::Leave(KErrSIPInvalidDialogResponse);
            }   
        }
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::CreateForkingDialogLC
// -----------------------------------------------------------------------------
//
CSipDialogItem* CSipDialogItem::CreateForkingDialogLC ()
	{
	CSipDialogItem* dialog = 
        CSipDialogItem::NewLC(iDialogStore,iDialogOwner,iSIPSecUser,iTU,
                              iRefreshMgr,iSIPSec,iDeleteMgr,iSigComp);

	//copy the first TransactionItem from parent dialog
    TTransactionId firstTransactionId = iFirstTransactionId;
	CTransactionItemBase* firstTransactionItem = 
        iTransactionItemStore->FindItem(firstTransactionId);
    __ASSERT_ALWAYS (firstTransactionItem, User::Leave(KErrGeneral));

    CTransactionItemBase* forkingTransactionItem =
        CTransactionItem::NewL(iTU,this,*firstTransactionItem);
    
    dialog->TransactionItems().AddItem(forkingTransactionItem);

	dialog->SetFirstTransactionId(iFirstTransactionId);

	CSIPCallIDHeader* callIdHeader = 
	    static_cast<CSIPCallIDHeader*>(iCallId->CloneL());
	dialog->SetCallId(callIdHeader);
	dialog->SetLocalTag(iLocalTag);

	__SIP_ASSERT_LEAVE(iLocalAddress, KErrArgument);
	dialog->SetLocalAddressL(*iLocalAddress);

	__SIP_ASSERT_LEAVE(iRemoteAddress, KErrArgument);
	dialog->SetRemoteAddressL(*iRemoteAddress);

	dialog->SetLocalSeqNum(iLocalSeqNum);

	__SIP_ASSERT_LEAVE(iLocalTarget, KErrArgument);
	dialog->SetLocalTargetL(*iLocalTarget);

	return dialog;
	}

// -----------------------------------------------------------------------------
// CSipDialogItem::UpdateTransportForNextHopAndLocalTargetL
// -----------------------------------------------------------------------------
//
void CSipDialogItem::UpdateTransportForNextHopAndLocalTargetL(
    CSIPContactHeader* aContactInOutgoingRequest)
    {
    CSIPURI& localTarget = ExtractUriL(iLocalTarget);
    RStringF localTargetTransport = localTarget.ParamValue(iTransportParamName);
   
    CSIPURI* nextHop = NULL;
    if (iRouteSet.Count() > 0)
        {
        CURIContainer& uri = iRouteSet[0]->SIPAddress().URI();
        __ASSERT_ALWAYS(uri.IsSIPURI(), User::Leave(KErrSIPInvalidURIType));
        nextHop = uri.SIPURI();
        }
    else
        {
        __ASSERT_ALWAYS(iRemoteTarget, User::Leave(KErrNotFound));
        __ASSERT_ALWAYS(iRemoteTarget->IsSIPURI(), 
                        User::Leave(KErrSIPInvalidURIType));
        nextHop = iRemoteTarget->SIPURI();        
        }
    RStringF nextHopTransport = nextHop->ParamValue(iTransportParamName);
    
    if (nextHopTransport.DesC().Length() > 0)
        {
        if (localTargetTransport.DesC().Length() == 0 &&
            !localTarget.IsSIPSURI())
            {
            localTarget.SetParamL(iTransportParamName,nextHopTransport);
            if (aContactInOutgoingRequest)
                {
                CSIPURI& contactUri = ExtractUriL(aContactInOutgoingRequest);
                contactUri.SetParamL(iTransportParamName,nextHopTransport);
                }
            }        
        }
    else
        {
        if (localTargetTransport.DesC().Length() > 0 &&
            !nextHop->IsSIPSURI())
            {
            nextHop->SetParamL(iTransportParamName,localTargetTransport);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::ExtractUriL
// -----------------------------------------------------------------------------
//
CSIPURI& CSipDialogItem::ExtractUriL(CSIPContactHeader* aContact)
    {
    __ASSERT_ALWAYS(aContact, User::Leave(KErrNotReady));
    __ASSERT_ALWAYS(aContact->SIPAddress(), User::Leave(KErrNotReady));
    __ASSERT_ALWAYS(aContact->SIPAddress()->URI().IsSIPURI(), 
                    User::Leave(KErrSIPInvalidURIType));
    return *(aContact->SIPAddress()->URI().SIPURI()); 
    }

// -----------------------------------------------------------------------------
// CSipDialogItem::CleanupRouteSet
// -----------------------------------------------------------------------------
//
void CSipDialogItem::CleanupRouteSet (TAny* aRouteSet)
	{
    RPointerArray<CSIPRouteHeader>* routeSet =
        reinterpret_cast<RPointerArray<CSIPRouteHeader>*>(aRouteSet);
    if (routeSet)
        {
        routeSet->ResetAndDestroy();
        }
    delete routeSet;
	}
