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
// Name          : SipDialogMgr.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//




#include "CSipDialogMgr.h"
#include "sipfromheader.h"
#include "DialogStateBase.h"
#include "MSipRegistrationContact.h"
#include "MTransactionUser.h"
#include "MSipRefreshMgr.h"
#include "MDialogOwner.h"
#include "DeleteMgr.h"
#include "sipcontactheader.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "siptoheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "SipLogs.h"
#include "sipaddress.h"
#include "siphostport.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "sipcodecutils.h"
#include "sipinternalstates.h"
#include "MTransactionHeaders.h"
#include "siprouteheader.h"
#include "DialogsStore.h"
#include "SipAssert.h"
#include "RefreshItem.h"
#include "TransactionItem.h"
#include "TransactionItemStore.h"
#include "Establishing.h"
#include "Early.h"
#include "Confirmed.h"
#include "Terminated.h"
#include "CSigCompMgr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"
#include "TSIPTransportParams.h"

_LIT8(KLocalHostSIPURI, "sip:localhost");
const TUint K100Response = 100;
const TUint K400Response = 400;
const TUint K481Response = 481;


// -----------------------------------------------------------------------------
// CSipDialogMgr::NewL
// -----------------------------------------------------------------------------
//
CSipDialogMgr* CSipDialogMgr::NewL(
    MTransactionUser& aTU,
    MSipRefreshMgr& aRefreshMgr,
    MSipRegistrationContact& aRegistrationContact,
    CSIPSec& aSIPSec,
    MSigComp& aSigComp)
	{
	CSipDialogMgr* self = new(ELeave)CSipDialogMgr(aTU,aRefreshMgr, 
                                                   aRegistrationContact,
                                                   aSIPSec, aSigComp);	
	CleanupStack::PushL(self);	
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::CSipDialogMgr
// -----------------------------------------------------------------------------
//
CSipDialogMgr::CSipDialogMgr (MTransactionUser& aTU,  
                              MSipRefreshMgr& aRefreshMgr,
							  MSipRegistrationContact& aRegistratinonContact,
							  CSIPSec& aSIPSec,
							  MSigComp& aSigComp)
: iTU(aTU), 
  iRefreshMgr(aRefreshMgr), 
  iRegistrationContact(aRegistratinonContact),
  iSIPSec(aSIPSec),
  iSigComp(aSigComp)
	{
	iContactHeaderName = SIPStrings::StringF(SipStrConsts::EContactHeader);
	iRouteHeaderName = SIPStrings::StringF(SipStrConsts::ERouteHeader);
	iTagParamName = SIPStrings::StringF(SipStrConsts::ETag);
    iPhraseCallTransactionDoesNotExist = 
        SIPStrings::StringF(SipStrConsts::EPhraseCallTransactionDoesNotExist);
    iPhraseBadRequest = SIPStrings::StringF(SipStrConsts::EPhraseBadRequest);
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::ConstructL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::ConstructL ()
	{
	iDialogsStore = CDialogsStore::NewL();
	iDeleteMgr = CDeleteMgr::NewL();
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::~CSipDialogMgr
// -----------------------------------------------------------------------------
//
CSipDialogMgr::~CSipDialogMgr ()
	{
	delete iDialogsStore;
	delete iDeleteMgr;
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::SendAndCreateDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::SendAndCreateDialogL (TTransactionId& aTransactionId,
                                          TRefreshId& aRefreshId,
                                          TRegistrationId aRegistrationId,
                                          TDialogId& aDialogId,
                                          CSIPRequest* aRequest,
                                          CURIContainer& aRemoteTarget,
                                          MDialogOwner* aOwner,
                                          MSIPSecUser& aSIPSecUser,
                                          TBool aRefresh)	
	{
    __SIP_ASSERT_LEAVE(aOwner, KErrArgument);
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	__SIP_ASSERT_LEAVE(aRequest->To(), KErrSIPMalformedMessage);
	__SIP_ASSERT_LEAVE(aRequest->From(), KErrSIPMalformedMessage);
	__SIP_ASSERT_LEAVE(aRequest->Method().DesC().Length() > 0, 
                       KErrSIPMalformedMessage);
	__SIP_ASSERT_LEAVE(aRequest->HeaderCount(iContactHeaderName) <= 1,
                       KErrSIPMalformedMessage);
	__SIP_MESSAGE_LOG("Dialogs", *aRequest)

    TBool newDialogCreated = EFalse;
    CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
    if (!dialog)
        {
	    dialog = CSipDialogItem::NewLC(*iDialogsStore,aOwner,aSIPSecUser,iTU,
                                       iRefreshMgr,iSIPSec,*iDeleteMgr,
                                       iSigComp,aRegistrationId);
        newDialogCreated = ETrue;
        }

	dialog->SetLocalAddressL(aRequest->From()->SIPAddress());
	dialog->SetRemoteAddressL(aRequest->To()->SIPAddress());
	dialog->SetInitialRemoteTargetL(aRemoteTarget);
	dialog->SetRouteSetUsingRouteL(*aRequest);
	AddContactHeaderL(dialog->IapId(),aRegistrationId,
	                  *aRequest,aRequest->From());
	dialog->SetLocalTargetWithMsgL(*aRequest,EFalse);

    CTransactionItemBase* transactionItem = 0;
	if (aRefresh)
		{
        transactionItem = CRefreshItem::NewLC(iRefreshMgr,dialog,
                                              *dialog,aRequest->Method());
		}
	else
		{
        transactionItem = CTransactionItem::NewLC(iTU,dialog,
                                                  aRequest->Method());
        }
    transactionItem->SetTargetRefresh();

	dialog->CurrentState().SendL(aTransactionId,aRequest,*transactionItem,
	                             *dialog->RemoteTarget());
    
    dialog->TransactionItems().AddItem(transactionItem);
    CleanupStack::Pop(transactionItem);
    dialog->SetFirstTransactionId(aTransactionId);
    
    if (newDialogCreated)
        {
        aDialogId = dialog->DialogId();
	    iDialogsStore->AddDialogItem(dialog);
	    CleanupStack::Pop(dialog);
        }
	
	if (aRefresh)
	    {
	    aRefreshId = static_cast<CRefreshItem*>(transactionItem)->RefreshId();
	    }
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::SendWithinDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::SendWithinDialogL (TTransactionId& aTransactionId,
                                       TRefreshId& aRefreshId,
                                       const TDialogId& aDialogId,
                                       CSIPRequest* aRequest,
                                       const MDialogOwner* aOwner,
                                       TBool aRefresh,
                                       TBool aAllowOverlap, 
                                       TBool aTargetRefresh)
	{
	__SIP_ASSERT_LEAVE(aDialogId, KErrArgument);
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	__SIP_ASSERT_LEAVE(aRequest->Method().DesC().Length() != 0, 
                       KErrSIPMalformedMessage);
	__SIP_ASSERT_LEAVE(aRequest->HeaderCount(iContactHeaderName) <= 1, 
                       KErrSIPMalformedMessage);
	__ASSERT_ALWAYS(!aRequest->HasHeader(iRouteHeaderName), 
                    User::Leave(KErrSIPMalformedMessage));
	
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
	__ASSERT_ALWAYS(dialog, User::Leave(KErrSIPInvalidDialogState));
	__ASSERT_ALWAYS(dialog->CurrentState().RequestsWithinDialogAllowed(),
					User::Leave(KErrSIPInvalidDialogState));

	RStringF method = aRequest->Method();
	if (dialog->TransactionItems().OverlappingRestricted(method))
        {
		User::Leave(KErrSIPRequestPending);
        }

	__SIP_MESSAGE_LOG("Dialogs", *aRequest)
	
	dialog->AddToHeaderInDialogL(*aRequest);
	dialog->AddFromHeaderInDialogL(*aRequest);
	dialog->AddCallIdHeaderInDialogL(*aRequest);
	dialog->AddRouteToMessageL(*aRequest);
	AddContactHeaderL(*dialog,*aRequest,aRequest->From(),aTargetRefresh);
	if (aTargetRefresh)
		{
		dialog->UpdateLocalTargetL(*aRequest);
		}
	if (method != SIPStrings::StringF(SipStrConsts::EAck))
        {
		dialog->AddCSeqHeaderInDialogL(*aRequest);
        }		
	
    CTransactionItemBase* transactionItem = 0;
	if (aRefresh)
		{
        transactionItem = 
			CRefreshItem::NewLC(iRefreshMgr,dialog,*dialog,method);
		}
    else
        {
        transactionItem = CTransactionItem::NewLC(iTU,dialog,method);
        }
    transactionItem->SetOverlappingRestricted(!aAllowOverlap);
    transactionItem->SetTargetRefresh(aTargetRefresh);

	dialog->CurrentState().SendL(aTransactionId,aRequest,*transactionItem,
	                             *dialog->RemoteTarget());

    dialog->TransactionItems().AddItem(transactionItem);
	CleanupStack::Pop(transactionItem);
	if (aRefresh)
	    {
	    aRefreshId = static_cast<CRefreshItem*>(transactionItem)->RefreshId();
	    }
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::SendAndUpdateRefreshL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::SendAndUpdateRefreshL (TTransactionId& aTransactionId,
                                           const TRefreshId& aRefreshId,
                                           const TDialogId& aDialogId,
                                           CSIPRequest* aRequest,
                                           const MDialogOwner* aOwner)
    {
	__SIP_ASSERT_LEAVE(aRefreshId, KErrArgument);
	__SIP_ASSERT_LEAVE(aDialogId, KErrArgument);
	__SIP_ASSERT_LEAVE(aRequest, KErrArgument);
	__SIP_ASSERT_LEAVE(aRequest->HeaderCount(iContactHeaderName) <= 1, 
                       KErrSIPMalformedMessage);
	__ASSERT_ALWAYS (!aRequest->HasHeader(iRouteHeaderName), 
                     User::Leave(KErrSIPMalformedMessage));
	
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
	__ASSERT_ALWAYS(dialog, User::Leave(KErrSIPInvalidDialogState));
	__ASSERT_ALWAYS(dialog->CurrentState().RequestsWithinDialogAllowed(),
					User::Leave(KErrSIPInvalidDialogState));

    CTransactionItemBase* refreshItem = 
        dialog->TransactionItems().FindRefreshItem(aRefreshId);
    __ASSERT_ALWAYS(refreshItem,User::Leave(KErrSIPInvalidTransactionState));

	__SIP_MESSAGE_LOG("Dialogs", *aRequest)
	
	dialog->AddToHeaderInDialogL(*aRequest);
	dialog->AddFromHeaderInDialogL(*aRequest);
	dialog->AddCallIdHeaderInDialogL(*aRequest);
    if (aRequest->Method().DesC().Length() == 0)
        {
        aRequest->SetMethodL(refreshItem->Method());
        }
	dialog->AddRouteToMessageL(*aRequest);
    TBool targetRefresh = refreshItem->TargetRefresh();
	AddContactHeaderL(*dialog,*aRequest,aRequest->From(),targetRefresh);
	if (targetRefresh)
		{
		dialog->UpdateLocalTargetL(*aRequest);
		}
	dialog->CurrentState().SendL(aTransactionId,aRequest,*refreshItem,
	                             *dialog->RemoteTarget());
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::SendAndCreateDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::SendAndCreateDialogL (const TTransactionId& aTransactionId,
										  TRegistrationId aRegistrationId,
                                          TDialogId& aDialogId,
                                          CSIPResponse* aResponse,
                                          MDialogOwner* aOwner,
                                          MSIPSecUser& aSIPSecUser)
	{
	__SIP_ASSERT_LEAVE (aTransactionId != 0, KErrArgument);
	__SIP_ASSERT_LEAVE (!aDialogId, KErrNotSupported);
	__SIP_ASSERT_LEAVE (aResponse, KErrArgument);
	__SIP_ASSERT_LEAVE (aOwner, KErrArgument);
	__SIP_ASSERT_LEAVE (aResponse->ResponseCode() > K100Response, KErrArgument);

	__SIP_MESSAGE_LOG("Dialogs", *aResponse)
	
	
	CSipDialogItem* dialog = 
        CSipDialogItem::NewLC(*iDialogsStore,aOwner,aSIPSecUser,
                              iTU,iRefreshMgr,iSIPSec,*iDeleteMgr,iSigComp, aRegistrationId);
    TSIPTransportParams transportParams(
        dialog->TransportParams(aRegistrationId));
    
	MTransactionHeaders* trHeaders = iTU.TransactionHeadersL(aTransactionId);
	__ASSERT_ALWAYS(trHeaders, User::Leave(KErrSIPInvalidTransactionState));
    TCleanupItem cleanupItem(DeleteTransactionHeaders,trHeaders);
	CleanupStack::PushL (cleanupItem);

	// Remote target 
    CSIPContactHeader* remoteContact = 
        ExtractContactL(aTransactionId,*trHeaders,transportParams,ETrue);
    CleanupStack::PushL(remoteContact);
	dialog->SetRemoteTargetL(remoteContact->SIPAddress()->URI());
	CleanupStack::PopAndDestroy(remoteContact);

	// Remote CSeq
	dialog->SetRemoteSeqNum(trHeaders->CSeqNumber());

	// Call-ID 
	CSIPCallIDHeader* callId = trHeaders->CallIDHeader();
	__SIP_ASSERT_LEAVE(callId, KErrSIPInvalidDialogRequest);
	dialog->SetCallId(callId);

    // Route-set
	RPointerArray<CSIPRouteHeaderBase>* route = trHeaders->RecordRoute();
	if (route)
		{
		TCleanupItem routeCleanupItem(DeleteRouteSet,route);
		CleanupStack::PushL (routeCleanupItem);
		CheckRecordRouteL(aTransactionId,*route,transportParams);
		dialog->SetRouteSetL(*route);
		CleanupStack::PopAndDestroy(1);// routeCleanupItem
		}

    // From
	CSIPFromHeader* from = trHeaders->FromHeader();
	CleanupStack::PushL(from);
	__SIP_ASSERT_LEAVE(from, KErrSIPInvalidDialogRequest);
	dialog->SetRemoteAddressL(from->SIPAddress());
	if (from->HasParam(iTagParamName))
        {
		dialog->SetRemoteTagL(from);
        }
	aResponse->AddHeaderL(from);
	CleanupStack::Pop(from);

	// To
	CSIPToHeader* to = trHeaders->ToHeader();
	CleanupStack::PushL(to);
	__SIP_ASSERT_LEAVE(to, KErrSIPInvalidDialogRequest);    
	dialog->SetLocalAddressL(to->SIPAddress());
	if (!to->HasParam(iTagParamName))
        {
		HBufC8* tagBuf = HBufC8::NewLC(MTransactionUser::KTagLength);
		TPtr8 tagPtr = tagBuf->Des();
		iTU.MakeTagL(tagPtr);
		RStringF tag = SIPStrings::Pool().OpenFStringL(tagPtr);
		CleanupClosePushL(tag);
        to->SetParamL(iTagParamName,tag);
        CleanupStack::PopAndDestroy(); // tag
        CleanupStack::PopAndDestroy(tagBuf); 
		}
	dialog->SetLocalTag(to->ParamValue(iTagParamName));
	aResponse->AddHeaderL(to);
	CleanupStack::Pop(to);

	// Local target (Contact)
	CURIContainer* requestURI = trHeaders->RequestURI();
	__SIP_ASSERT_LEAVE(requestURI, KErrSIPInvalidDialogRequest);
	CleanupStack::PushL(requestURI);
	if (!aResponse->IsErrorResponse())
	    {
	    AddContactHeaderL(dialog->IapId(),*aResponse,aResponse->To(),
	                      *requestURI);
	    }
	CleanupStack::PopAndDestroy(requestURI);
	dialog->SetLocalTargetWithMsgL(*aResponse);

	dialog->AddRecordRouteToMessageL(*aResponse);
	dialog->AddCallIdHeaderInDialogL(*aResponse);
	dialog->SetFirstTransactionId(aTransactionId);

    CTransactionItem* transactionItem = 
        CTransactionItem::NewLC(iTU,dialog,aTransactionId);
	dialog->CurrentState().SendL(*transactionItem,aResponse);
    dialog->TransactionItems().AddItem(transactionItem);
	iDialogsStore->AddDialogItem(dialog);

    aDialogId = dialog->DialogId();

    CleanupStack::Pop(transactionItem); 
    CleanupStack::PopAndDestroy(); // trHeaders
    CleanupStack::Pop(dialog);
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::SendWithinDialogL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::SendWithinDialogL (const TTransactionId& aTransactionId,
                                       const TDialogId& aDialogId,
                                       CSIPResponse* aResponse,
                                       const MDialogOwner* aOwner,
                                       TBool aTargetRefresh)
	{
	__SIP_ASSERT_LEAVE (aTransactionId, KErrArgument);
	__SIP_ASSERT_LEAVE (aDialogId, KErrSIPInvalidDialogState);
	__SIP_ASSERT_LEAVE (aResponse, KErrArgument);
	__SIP_ASSERT_LEAVE (aResponse->ResponseCode() > K100Response, KErrArgument);

	__SIP_MESSAGE_LOG("Dialogs", *aResponse)

	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
	__ASSERT_ALWAYS(dialog, User::Leave(KErrSIPInvalidDialogState));
    CTransactionItemBase* transactionItem = 
        dialog->TransactionItems().FindItem(aTransactionId);
    __ASSERT_ALWAYS(transactionItem,
                    User::Leave(KErrSIPInvalidTransactionState));

	MTransactionHeaders* trHeaders = iTU.TransactionHeadersL(aTransactionId);
	__ASSERT_ALWAYS(trHeaders, User::Leave(KErrSIPInvalidTransactionState));
    TCleanupItem cleanupItem(DeleteTransactionHeaders,trHeaders);
	CleanupStack::PushL (cleanupItem);
 
    // To
	CSIPToHeader* to = trHeaders->ToHeader();
	CleanupStack::PushL(to);
	if (!to->HasParam(iTagParamName))
		{
		to->SetParamL(iTagParamName,dialog->LocalTag());
		}
	aResponse->AddHeaderL(to);
	CleanupStack::Pop(to);
    
    // Add Record-Route only to the responses to the first transaction
    if (aTransactionId == dialog->FirstTransactionId())
        {
	    dialog->AddRecordRouteToMessageL(*aResponse);
        }

    // Remote and local target
	if (!aResponse->IsErrorResponse())
	    {    
        AddContactHeaderL(*dialog,*aResponse,aResponse->To(),aTargetRefresh);
	    }
	if (aTargetRefresh && aResponse->Type() == CSIPResponse::E2XX)
		{		
		CSIPContactHeader* remoteContact = 
		    ExtractContactL(aTransactionId,*trHeaders,
                            dialog->TransportParams(KEmptyRegistrationId),
                            EFalse);
		if (remoteContact) 
		    {
		    CleanupStack::PushL(remoteContact);
            CURIContainer& remoteTarget = remoteContact->SIPAddress()->URI();
    	    dialog->SetRemoteTargetL(remoteTarget);
    		CleanupStack::PopAndDestroy(remoteContact);		    
		    }    
		dialog->UpdateLocalTargetL(*aResponse);
		}
    CleanupStack::PopAndDestroy(); // trHeaders
	dialog->CurrentState().SendL(*transactionItem, aResponse);	
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::TerminateDialog
// -----------------------------------------------------------------------------
//
TInt CSipDialogMgr::TerminateDialog (const TDialogId& aDialogId,
                                     const MDialogOwner* aOwner)
	{
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
	if(!dialog) 
        {
		return KErrNotFound;
        }
	return iDialogsStore->Remove(dialog);
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::TerminateDialogsFor
// -----------------------------------------------------------------------------
//
TInt CSipDialogMgr::TerminateDialogsFor (const MDialogOwner* aOwner)
    {
    return iDialogsStore->RemoveAllWith(aOwner);
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::TerminateDialogRefresh
// -----------------------------------------------------------------------------
//
TInt CSipDialogMgr::TerminateDialogRefresh (const TDialogId& aDialogId,
											const TRefreshId& aRefreshId,
											const MDialogOwner* aOwner)
    {
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
    if (!dialog)
        {
        return KErrNotFound;
        }
    CTransactionItemBase* refreshItem = 
        dialog->TransactionItems().FindRefreshItem(aRefreshId);
    if (!refreshItem)
        {
        return KErrNotFound;
        }
    return dialog->TransactionItems().RemoveItem(refreshItem->Id());
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::GetStateL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::GetStateL (const TDialogId& aDialogId, 
                               CSIPInternalStates::TState& aState)
    {
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId);
    if (!dialog)
        {
        User::Leave(KErrNotFound);
        }
	dialog->CurrentState().GetState(aState);
    }
    
// -----------------------------------------------------------------------------
// CSipDialogMgr::CallIDL
// -----------------------------------------------------------------------------
//    
const CSIPCallIDHeader& CSipDialogMgr::CallIDL (const TDialogId& aDialogId,
                                                const MDialogOwner* aOwner)
    {
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
    if (!dialog)
        {
	    User::Leave(KErrNotFound);
        }
    if (!dialog->CallId())
        {
	    User::Leave(KErrNotFound);
        }         
    return *(dialog->CallId());
    }
    
// -----------------------------------------------------------------------------
// CSipDialogMgr::LocalTagL
// -----------------------------------------------------------------------------
//     
const TDesC8& CSipDialogMgr::LocalTagL(const TDialogId& aDialogId,
	                                   const MDialogOwner* aOwner)
    {
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
    if (!dialog)
        {
	    User::Leave(KErrNotFound);
        }
    return dialog->LocalTag().DesC();
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::ResetStateL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::ResetStateL(const TDialogId& aDialogId,
                                const MDialogOwner* aOwner)
    {
	CSipDialogItem* dialog = iDialogsStore->FindDialogItem(aDialogId,aOwner);
    if (!dialog)
        {
	    User::Leave(KErrNotFound);
        }
    dialog->ResetStateL();
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::SIPSecUserForTransaction
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSipDialogMgr::SIPSecUserForTransaction(
    const TTransactionId& aTransactionId)
    {
    return iDialogsStore->FindDialogItemByTransaction(aTransactionId);
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::TransactionOwnerL
// -----------------------------------------------------------------------------
//
MTransactionOwner* CSipDialogMgr::TransactionOwnerL (CSIPRequest& aRequest,
													 CSIPResponse** aResponse)
	{
	if (aRequest.To() && !(aRequest.To()->HasParam(iTagParamName)))
        {
		return 0;
        }
	CSipDialogItem* dialog = iDialogsStore->DialogItemForRequest(&aRequest);
	if (!dialog)
		{
		*aResponse = 
		    CSIPResponse::NewL(K481Response,iPhraseCallTransactionDoesNotExist);
		}
	return dialog;
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::ConnectionStateChangedL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::ConnectionStateChangedL (TUint32 aIapId,
											 CSIPConnection::TState aState)
	{
	if (aState == CSIPConnection::EInactive ||
        aState == CSIPConnection::EUnavailable)
        {
		iDialogsStore->DeleteItemsWithThisIapId(aIapId);
        }
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::ExtractContactL
// -----------------------------------------------------------------------------
//
CSIPContactHeader* CSipDialogMgr::ExtractContactL (
    TTransactionId aTransactionId,
    MTransactionHeaders& aHeaders,
    const TSIPTransportParams& aParams,
    TBool aContactMandatory)
    {
    RPointerArray<CSIPContactHeader>* contacts = aHeaders.Contact();
    TCleanupItem contactsCleanupItem(DeleteContacts,contacts);
	CleanupStack::PushL(contactsCleanupItem);    
    CSIPContactHeader* contact = NULL; 
    if (contacts && contacts->Count() == 1)
        {
        contact = (*contacts)[0];
        contacts->Remove(0);
        CleanupStack::PushL(contact);
        if (!CSipDialogItem::ContactOk(*contact))
            {
            SendErrorResponseAndLeaveL(aTransactionId,
                                       KErrSIPInvalidURIType,
                                       aParams);
            }
        CleanupStack::Pop(contact);      
        }
    if (!contact && aContactMandatory)
        {
        SendErrorResponseAndLeaveL(aTransactionId,
                                   KErrSIPInvalidDialogRequest,
                                   aParams);
        }
    CleanupStack::PopAndDestroy(1); // contactsCleanupItem
    return contact;
    }
    
// -----------------------------------------------------------------------------
// CSipDialogMgr::CheckRecordRouteL
// -----------------------------------------------------------------------------
//    
void CSipDialogMgr::CheckRecordRouteL (
    TTransactionId aTransactionId,
    RPointerArray<CSIPRouteHeaderBase>& aRecordRoute,
    const TSIPTransportParams& aParams)
    {
    // For an incoming request creating a dialog it must be checked 
    // that the first Record-Route-header (the next hop) has a SIP-URI.
    TInt count = aRecordRoute.Count();
    if (count > 0)
        {
        CSIPRouteHeaderBase* nextHop = aRecordRoute[0];
        if (!nextHop->SIPAddress().URI().IsSIPURI())
            {
            SendErrorResponseAndLeaveL(aTransactionId,KErrSIPInvalidURIType,
                                       aParams);      
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::AddContactHeaderL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::AddContactHeaderL (TUint32 aIapId,
                                       TRegistrationId aRegistrationId,
									   CSIPRequest& aRequest,
									   const CSIPFromHeader* aFrom)
	{
	if (!aRequest.HasHeader(iContactHeaderName))
		{
		CreateAndAddContactL(aIapId,aRegistrationId,aRequest,aFrom);
		}
	else
		{
		FillHostInContactL(aIapId,aRegistrationId,aRequest,aFrom);
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::AddContactHeaderL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::AddContactHeaderL (TUint32 aIapId,
									   CSIPResponse& aResponse,
									   const CSIPToHeader* aTo,
									   const CURIContainer& aRequestUri)
	{
	if (!aResponse.HasHeader(iContactHeaderName))
		{
		CreateAndAddContactL(aRequestUri,aResponse);
		}
	else
		{
		FillHostInContactL(aIapId,KEmptyRegistrationId,aResponse,aTo);
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::AddContactHeaderL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::AddContactHeaderL (CSipDialogItem& aDialog, 
                                       CSIPMessage& aMessage,
									   const CSIPFromToHeaderBase* aFromTo,
                                       TBool aTargetRefresh)
	{
	if (!aMessage.HasHeader(iContactHeaderName))
		{
		if (aTargetRefresh)
            {
 	        CSIPContactHeader* contact = 
 	            static_cast<CSIPContactHeader*>(
 	                aDialog.LocalTarget()->CloneL());
	        CleanupStack::PushL(contact);
	        aMessage.AddHeaderL(contact);
	        CleanupStack::Pop(contact);
            }
		}
	else
		{
		FillHostInContactL(aDialog.IapId(),aDialog.RegistrationId(),
		                   aMessage,aFromTo);
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::CreateAndAddContactL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::CreateAndAddContactL (TUint32 aIapId,
                                          TRegistrationId aRegistrationId,
                                          CSIPMessage& aMessage,
                                          const CSIPFromToHeaderBase* aFromTo)
	{
	// Create and and Contact with empty host
	CURIContainer* uri = CURIContainer::DecodeL(KLocalHostSIPURI);
	CleanupStack::PushL(uri);
	CreateAndAddContactL(*uri,aMessage);
	CleanupStack::PopAndDestroy(uri);
	// Fill host
	FillHostInContactL(aIapId,aRegistrationId,aMessage,aFromTo);
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::FillHostInContactL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::FillHostInContactL (TUint32 aIapId,
                                        TRegistrationId aRegistrationId,
										CSIPMessage& aMessage,
										const CSIPFromToHeaderBase* aFromTo)
	{
	__SIP_ASSERT_LEAVE (aFromTo != 0, KErrArgument);	
	
	CSIPContactHeader* contact = 
        static_cast<CSIPContactHeader*>(
            aMessage.Header(iContactHeaderName,0));

	__SIP_ASSERT_LEAVE (contact != 0, KErrArgument);
	__SIP_ASSERT_LEAVE (contact->SIPAddress() != 0, KErrArgument);

    TPtrC8 localHost(SIPStrings::StringF(SipStrConsts::ELocalHost).DesC());

    CSIPURI* contactUri = contact->SIPAddress()->URI().SIPURI();
    if (contactUri && contactUri->HostPort().Host().CompareF(localHost) == 0)
		{ 
		// if contact header does not have host --> add it
		iRegistrationContact.ContactL(aIapId,aRegistrationId,aFromTo,*contact);
		}
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::CreateContactL
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::CreateAndAddContactL (const CURIContainer& aUri,
										  CSIPMessage& aMessage)
	{
	CURIContainer* uri = CURIContainer::NewLC(aUri);
	CSIPAddress* sipAddress = SIPCodecUtils::CreateSIPAddressLC(uri);
	CleanupStack::Pop(sipAddress);
	CleanupStack::Pop(uri);
	CleanupStack::PushL(sipAddress);
	CSIPContactHeader* contact = CSIPContactHeader::NewL(sipAddress);
	CleanupStack::Pop(sipAddress);
	CleanupStack::PushL(contact);
	aMessage.AddHeaderL(contact);
	CleanupStack::Pop(contact);
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::SendErrorResponseAndLeaveL
// -----------------------------------------------------------------------------
//	
void CSipDialogMgr::SendErrorResponseAndLeaveL (
    TTransactionId aTransactionId,
    TInt aLeaveCode,
    const TSIPTransportParams& aParams)
    {
    CSIPResponse* response = 
        CSIPResponse::NewLC(K400Response,iPhraseBadRequest);
    iTU.SendL(aTransactionId, response, aParams);
    CleanupStack::Pop(response); // ownership given to TU 
    iTU.ClearTransactionOwner(aTransactionId);
    User::Leave(aLeaveCode);    
    }

// -----------------------------------------------------------------------------
// CSipDialogMgr::DeleteTransactionHeaders
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::DeleteTransactionHeaders (TAny* aTransactionHeaders)
	{
	MTransactionHeaders* transactionHeaders = 
        reinterpret_cast<MTransactionHeaders*>(aTransactionHeaders);
    delete transactionHeaders;
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::DeleteRouteSet
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::DeleteRouteSet (TAny* aRouteSet)
	{
    RPointerArray<CSIPRouteHeaderBase>* routeSet =
        reinterpret_cast<RPointerArray<CSIPRouteHeaderBase>*>(aRouteSet);
    if (routeSet)
        {
        routeSet->ResetAndDestroy();
        }
	delete routeSet;
	}

// -----------------------------------------------------------------------------
// CSipDialogMgr::DeleteContacts
// -----------------------------------------------------------------------------
//
void CSipDialogMgr::DeleteContacts (TAny* aContacts)
	{
    RPointerArray<CSIPContactHeader>* contacts =
        reinterpret_cast<RPointerArray<CSIPContactHeader>*>(aContacts);
    if (contacts)
        {
        contacts->ResetAndDestroy();
        }
	delete contacts;
	}
