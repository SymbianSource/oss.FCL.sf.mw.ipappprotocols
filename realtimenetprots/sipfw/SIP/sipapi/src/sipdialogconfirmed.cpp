// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipdialogconfirmed.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sip.h"
#include "SipAssert.h"
#include "sipdialogconfirmed.h"
#include "SipDialogImplementation.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "sipresponseelements.h"
#include "siprefresh.h"
#include "sipinvitedialogassoc.h"
#include "sipsubscribedialogassoc.h"
#include "sipnotifydialogassoc.h"
#include "sipreferdialogassoc.h"
#include "sipconnectioncallback.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CDialogConfirmed::NewL
// -----------------------------------------------------------------------------
//
CDialogConfirmed* CDialogConfirmed::NewL()
    {
    return new (ELeave) CDialogConfirmed();
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::CDialogConfirmed
// -----------------------------------------------------------------------------
//
CDialogConfirmed::CDialogConfirmed()
	{
	}

// -----------------------------------------------------------------------------
// CDialogConfirmed::~CDialogConfirmed
// -----------------------------------------------------------------------------
//
CDialogConfirmed::~CDialogConfirmed()
	{
	}

// -----------------------------------------------------------------------------
// CDialogConfirmed::State
// -----------------------------------------------------------------------------
//
CSIPDialog::TState CDialogConfirmed::State() const
    {
    return CSIPDialog::EConfirmed;
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CDialogConfirmed::SetNeighbourStates(CDialogState& aTerminated)
    {    
    iTerminated = &aTerminated;
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendInviteL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendInviteL(const CSIPDialogImplementation& /*aDialog*/,
                              CSIPInviteDialogAssoc& aAssoc,
                              CSIPMessageElements* aElements) const
    {
    return aAssoc.DoSendRequestWithinDialogL(
    							SIPStrings::StringF(SipStrConsts::EInvite),
								aElements);
	}

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendSubscribeL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* 
CDialogConfirmed::SendSubscribeL(const CSIPDialogImplementation& /*aDialog*/,
                                 CSIPSubscribeDialogAssoc& aAssoc,
                                 CSIPMessageElements* aElements,
                                 CSIPRefresh* aRefresh) const
    {
    return aAssoc.DoSendSubscribeL(aElements, aRefresh, ETrue);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::UpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::UpdateL(const CSIPDialogImplementation& /*aDialog*/,
                	  	  CSIPSubscribeDialogAssoc& aAssoc,
                	  	  CSIPMessageElements* aElements) const
	{
	return aAssoc.DoSendSubscribeL(aElements, NULL, ETrue);
	}

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendNonTargetRefreshRequestL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CDialogConfirmed::SendNonTargetRefreshRequestL(
									const CSIPDialogImplementation& aDialog,
                                    CSIPDialogAssocImplementation& aAssoc,
                                    RStringF aMethod,
                                    CSIPMessageElements* aElements) const
    {
    return aDialog.SendRequestInDialogL(aAssoc, aMethod, aElements);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::IncomingResponseL
// "this" is passed instead of the real early-state, since early state is never
// entered from confirmed state.
// -----------------------------------------------------------------------------
//
TBool
CDialogConfirmed::IncomingResponseL(CSIPDialogImplementation& aDialog,
                                    CSIPResponseElements* aElements,
		                            TUint32 aRequestId,
                                    TUint32 aDialogId,
                                    CConnectionCallback& aCallback) const
    {
    return IncomingResponseWithinDialogL(aDialog,
                                         aElements,
		                                 aRequestId,
                                         aDialogId,
                                         aCallback,
                                         *this,
                                         *this,
                                         *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::IncomingResponseL
// "this" is passed instead of the real early-state, since early state is never
// entered from confirmed state.
// -----------------------------------------------------------------------------
//
TBool CDialogConfirmed::IncomingResponseL(CSIPDialogImplementation& aDialog,
                                          CSIPResponseElements* aElements,
		                                  TUint32 aRequestId,
                                          TUint32 aRefreshId,
                                          TUint32 /*aDialogId*/,
                                          CConnectionCallback& aCallback) const
    {    
    return ResponseToRefreshL(aDialog,
                              aElements,
		                      aRequestId,
                              aRefreshId,
                              aCallback,                              
                              *this,
                              *this,                               
                              *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::IncomingRequestL
// -----------------------------------------------------------------------------
//
TBool CDialogConfirmed::IncomingRequestL(CSIPDialogImplementation& aDialog,
                                         CSIPServerTransaction* aTransaction,
                                         CConnectionCallback& aCallback) const
    {
    __SIP_ASSERT_LEAVE(aTransaction, KErrArgument);
    return RequestReceivedL(aDialog, aTransaction, aCallback);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendAckL
// -----------------------------------------------------------------------------
//
void CDialogConfirmed::SendAckL(CSIPInviteDialogAssoc& aAssoc,
                                const CSIPClientTransaction& aTransaction,
			                    CSIPMessageElements* aElements) const
    {
    aAssoc.DoSendAckL(aTransaction, aElements);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendResponseL
// Even when sending an error response to the initial request, dialog doesn't
// enter terminated state, if it currently is in confirmed state.
// -----------------------------------------------------------------------------
//
void CDialogConfirmed::SendResponseL(CSIPDialogImplementation& aDialog,
                                     const CSIPResponseElements& aElements,
						             TUint32 aRequestId,
                                     TBool /*aAffectsDialogState*/,
                                     TBool aTargetRefresh) const
    {
    SendResponseWithinDialogL(aDialog, aElements, aRequestId, aTargetRefresh);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendPrackL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendPrackL(CSIPInviteDialogAssoc& aAssoc,
                             CSIPMessageElements* aElements) const
    {
    return aAssoc.DoSendRequestWithinDialogL(
    							SIPStrings::StringF(SipStrConsts::EPrack),
								aElements);    
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendUpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendUpdateL(CSIPInviteDialogAssoc& aAssoc,
                              CSIPMessageElements* aElements) const
    {
    return aAssoc.DoSendRequestWithinDialogL(
    							SIPStrings::StringF(SipStrConsts::EUpdate),
								aElements);    
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendByeL
// Either caller or callee can send BYE in Confirmed state.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendByeL(CSIPInviteDialogAssoc& aAssoc,
                           CSIPMessageElements* aElements) const
    { 
    return aAssoc.DoSendRequestWithinDialogL(
	    							SIPStrings::StringF(SipStrConsts::EBye),
									aElements);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendUnsubscribeL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendUnsubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
                                   CSIPMessageElements* aElements) const
    {
    return aAssoc.DoSendUnsubscribeL(aElements);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendNotifyL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendNotifyL(CSIPNotifyDialogAssoc& aAssoc,
                    		  CSIPMessageElements* aElements) const
	{	
	return aAssoc.DoSendNotifyWithinDialogL(aElements);
	}

// -----------------------------------------------------------------------------
// CDialogConfirmed::SendReferL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogConfirmed::SendReferL(const CSIPDialogImplementation& /*aDialog*/,
							 CSIPReferDialogAssoc& aAssoc,
                   		     CSIPMessageElements* aElements) const
  	{
	return aAssoc.DoSendReferL(aElements, ETrue);
  	}

// -----------------------------------------------------------------------------
// CDialogConfirmed::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogConfirmed::ErrorOccured(CSIPDialogImplementation& aDialog,
                                     TInt aError,
                                     TUint32 aRequestId,
                                     CConnectionCallback& aCallback) const
    {    
    return HandleErrorOccurred(aDialog,
                               aError,
                               aRequestId,
                               aCallback,
                               *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogConfirmed::ErrorOccured(CSIPDialogImplementation& aDialog,
                                     TInt aError,
                                     TUint32 aRefreshId,
                                     TUint32 /*aRequestId*/,
                                     CConnectionCallback& aCallback) const
    {    
    return HandleRefreshErrorOccurred(aDialog,
                               		  aError,
                               		  aRefreshId,
                               		  aCallback,
                               		  *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogConfirmed::ConnectionLost
// This is the only case where dialog goes from confirmed to terminated state.
// -----------------------------------------------------------------------------
//
void CDialogConfirmed::ConnectionLost(CSIPDialogImplementation& aDialog) const
    {
    aDialog.ChangeState(iTerminated);
    }
