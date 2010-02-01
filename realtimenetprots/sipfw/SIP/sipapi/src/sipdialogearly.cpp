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
// Name          : sipdialogearly.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sip.h"
#include "siperr.h"
#include "SipAssert.h"
#include "sipdialogearly.h"
#include "SipDialogImplementation.h"
#include "SipDialogAssocImplementation.h"
#include "sipinvitedialogassoc.h"
#include "sipsubscribedialogassoc.h"
#include "sipservertransaction.h"
#include "sipmessageelements.h"
#include "sipresponseelements.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CDialogEarly::NewL
// -----------------------------------------------------------------------------
//
CDialogEarly* CDialogEarly::NewL()
    {
    return new (ELeave) CDialogEarly();
    }

// -----------------------------------------------------------------------------
// CDialogEarly::CDialogEarly
// -----------------------------------------------------------------------------
//
CDialogEarly::CDialogEarly()
	{
	}

// -----------------------------------------------------------------------------
// CDialogEarly::~CDialogEarly
// -----------------------------------------------------------------------------
//
CDialogEarly::~CDialogEarly()
	{
	}

// -----------------------------------------------------------------------------
// CDialogEarly::State
// -----------------------------------------------------------------------------
//
CSIPDialog::TState CDialogEarly::State() const
    {
    return CSIPDialog::EEarly;
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CDialogEarly::SetNeighbourStates(CDialogState& aConfirmed,
                                      CDialogState& aTerminated)
    {    
    iConfirmed = &aConfirmed;
    iTerminated = &aTerminated;
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SendSubscribeL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* 
CDialogEarly::SendSubscribeL(const CSIPDialogImplementation& /*aDialog*/,
                             CSIPSubscribeDialogAssoc& aAssoc,
                             CSIPMessageElements* aElements,
                             CSIPRefresh* aRefresh) const
    {    
    return aAssoc.DoSendSubscribeL(aElements, aRefresh, ETrue);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SendNonTargetRefreshRequestL
// Dialog has to be at least in "early" state, before a non target refresh
// request can be sent.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CDialogEarly::SendNonTargetRefreshRequestL(
									const CSIPDialogImplementation& aDialog,
                                    CSIPDialogAssocImplementation& aAssoc,
                                    RStringF aMethod,
                                    CSIPMessageElements* aElements) const
    {
    return aDialog.SendRequestInDialogL(aAssoc, aMethod, aElements);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CDialogEarly::IncomingResponseL(CSIPDialogImplementation& aDialog,
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
                                         *iConfirmed,
                                         *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CDialogEarly::IncomingResponseL(CSIPDialogImplementation& aDialog,
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
                              *iConfirmed,
                              *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::IncomingRequestL
// Enter next state before passing request to application, because application
// may delete the last dialog association, causing CSIPDialog to be deleted and
// CSIPDialog shouldn't be accessed after the callback.
// -----------------------------------------------------------------------------
//
TBool CDialogEarly::IncomingRequestL(CSIPDialogImplementation& aDialog,
                                     CSIPServerTransaction* aTransaction,
                                     CConnectionCallback& aCallback) const
    {
    __SIP_ASSERT_LEAVE(aTransaction, KErrArgument);

    if (aTransaction->Type() == SIPStrings::StringF(SipStrConsts::ENotify) &&
        aDialog.DoesNotifyConfirmDialog())
        {
        aDialog.ChangeState(iConfirmed);
        }

    return RequestReceivedL(aDialog, aTransaction, aCallback);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SendResponseL
// Since at least one response has already been sent (it caused the dialog to
// enter Early state), this response doesn't create dialog anymore.
// -----------------------------------------------------------------------------
//
void CDialogEarly::SendResponseL(CSIPDialogImplementation& aDialog,
                                 const CSIPResponseElements& aElements,
						         TUint32 aRequestId,
                                 TBool aAffectsDialogState,
                                 TBool aTargetRefresh) const
    {
    SendResponseWithinDialogL(aDialog, aElements, aRequestId, aTargetRefresh);
    
    if (aAffectsDialogState)
        {        
        if (aElements.StatusCode() >= 300)
            {
            aDialog.ChangeState(iTerminated);
            }
        else
            {
            if (aElements.StatusCode() >= 200)
                {
                aDialog.ChangeState(iConfirmed);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SendUpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogEarly::SendPrackL(CSIPInviteDialogAssoc& aAssoc,
                         CSIPMessageElements* aElements) const
    {    
    return aAssoc.DoSendRequestWithinDialogL(
    							SIPStrings::StringF(SipStrConsts::EPrack),
								aElements);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SendUpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogEarly::SendUpdateL(CSIPInviteDialogAssoc& aAssoc,
                          CSIPMessageElements* aElements) const
    {
    return aAssoc.DoSendRequestWithinDialogL(
    							SIPStrings::StringF(SipStrConsts::EUpdate),
								aElements);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::SendByeL
// Caller can send BYE in Early state, but callee can't.
// If aAssoc has an INVITE client transaction, it means our endpoint has sent
// an INVITE since dialog state is Early.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogEarly::SendByeL(CSIPInviteDialogAssoc& aAssoc,
                       CSIPMessageElements* aElements) const
    {
    if (!aAssoc.Implementation().HasTransaction(aAssoc.Type(), ETrue))
    	{
        User::Leave(KErrSIPInvalidDialogState);
        }

    return aAssoc.DoSendRequestWithinDialogL(
	    							SIPStrings::StringF(SipStrConsts::EBye),
									aElements);
    }

// -----------------------------------------------------------------------------
// CDialogEarly::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogEarly::ErrorOccured(CSIPDialogImplementation& aDialog,
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
// CDialogEarly::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogEarly::ErrorOccured(CSIPDialogImplementation& aDialog,
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
// CDialogEarly::ConnectionLost
// -----------------------------------------------------------------------------
//
void CDialogEarly::ConnectionLost(CSIPDialogImplementation& aDialog) const
    {
    aDialog.ChangeState(iTerminated);
    }
