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
// Name          : sipdialogtrying.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "sip.h"
#include "siperr.h"
#include "siptoheader.h"
#include "SipConnectionImplementation.h"
#include "sipclientconnection.h"
#include "sipdialogtrying.h"
#include "SipDialogImplementation.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "sipmessageelements.h"
#include "sipresponseelements.h"
#include "sipinvitedialogassoc.h"
#include "sipsubscribedialogassoc.h"
#include "sipreferdialogassoc.h"
#include "sipconnectioncallback.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CDialogTrying::NewL
// -----------------------------------------------------------------------------
//
CDialogTrying* CDialogTrying::NewL()
    {
    return new (ELeave) CDialogTrying();
    }

// -----------------------------------------------------------------------------
// CDialogTrying::CDialogTrying
// -----------------------------------------------------------------------------
//
CDialogTrying::CDialogTrying()
	{
	}

// -----------------------------------------------------------------------------
// CDialogTrying::~CDialogTrying
// -----------------------------------------------------------------------------
//
CDialogTrying::~CDialogTrying()
	{
	}

// -----------------------------------------------------------------------------
// CDialogTrying::State
// -----------------------------------------------------------------------------
//
CSIPDialog::TState CDialogTrying::State() const
    {
    return CSIPDialog::EInit;
    }

// -----------------------------------------------------------------------------
// CDialogTrying::SetNeighbourStates
// -----------------------------------------------------------------------------
//
void CDialogTrying::SetNeighbourStates(CDialogState& aEarly,
                                       CDialogState& aConfirmed,
                                       CDialogState& aTerminated)
    {
    iEarly = &aEarly;
    iConfirmed = &aConfirmed;
    iTerminated = &aTerminated;
    }

// -----------------------------------------------------------------------------
// CDialogTrying::SendInviteL
// Only the initial request creating the dialog, can be sent in Trying state.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogTrying::SendInviteL(const CSIPDialogImplementation& aDialog,                           
                           CSIPInviteDialogAssoc& aAssoc,
                           CSIPMessageElements* aElements) const
    {    
    aDialog.CheckNoTransactionExistsL();
    return aAssoc.DoSendInviteL(aElements);
	}

// -----------------------------------------------------------------------------
// CDialogTrying::SendSubscribeL
// Only the initial request creating the dialog, can be sent in Trying state.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogTrying::SendSubscribeL(const CSIPDialogImplementation& aDialog,
                              CSIPSubscribeDialogAssoc& aAssoc,
                              CSIPMessageElements* aElements,
                              CSIPRefresh* aRefresh) const
    {    
    aDialog.CheckNoTransactionExistsL();
    return aAssoc.DoSendSubscribeL(aElements, aRefresh, EFalse);    
    }

// -----------------------------------------------------------------------------
// CDialogTrying::SendReferL
// Only the initial request creating the dialog, can be sent in Trying state.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogTrying::SendReferL(const CSIPDialogImplementation& aDialog,
						  CSIPReferDialogAssoc& aAssoc,
                   		  CSIPMessageElements* aElements) const
  	{
  	aDialog.CheckNoTransactionExistsL();
	return aAssoc.DoSendReferL(aElements, EFalse);
  	}
              
// -----------------------------------------------------------------------------
// CDialogTrying::SendResponseL
// This is the first response sent and it can't be a 100 response. A 101-299
// response creates a dialog, a 300-699 response does not.
// -----------------------------------------------------------------------------
//
void CDialogTrying::SendResponseL(CSIPDialogImplementation& aDialog,
                                  const CSIPResponseElements& aElements,
						          TUint32 aRequestId,
                                  TBool aAffectsDialogState,
                                  TBool /*aTargetRefresh*/) const
    {
    __SIP_ASSERT_LEAVE(aAffectsDialogState, KErrArgument);
    
    CSIPConnection* connection = aDialog.Connection();
    __ASSERT_ALWAYS(connection, User::Leave(KErrSIPResourceNotAvailable));

	CSIPClientConnection& client =
		connection->Implementation().ClientConnectionL();
    if (aElements.StatusCode() >= 300)
        {        
        client.SendResponseL(aElements, aRequestId);
        aDialog.ChangeState(iTerminated);
        }
    else
        {        
        TUint32 dialogId(0);
        client.SendResponseAndCreateDialogL(aElements, aRequestId, 
                                            dialogId, aDialog.RegistrationId());       
        aDialog.SetDialogId(dialogId);
        aDialog.StoreCallIdL();                
		aDialog.FillLocalTagL(EFalse);

        //Enter next state when the response has been successfully sent
        if (aElements.StatusCode() >= 200)
            {
            aDialog.ChangeState(iConfirmed);
            }
        else
            {                
            //TransactionUser will add to-tag into other responses than 100
            aDialog.ChangeState(iEarly);
            }
        }    
    }

// -----------------------------------------------------------------------------
// CDialogTrying::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool
CDialogTrying::IncomingResponseL(CSIPDialogImplementation& aDialog,
                                 CSIPResponseElements* aElements,
		                         TUint32 aRequestId,
                                 TUint32 /*aDialogId*/,
                                 CConnectionCallback& aCallback) const
    {    
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);    

    CSIPTransactionBase* ta = NULL;
    CSIPDialogAssocBase* assoc = NULL;    
    if (aDialog.FindTransactionAndAssoc(aRequestId, &ta, &assoc))
        {
        __SIP_ASSERT_LEAVE(ta->IsSIPClientTransaction(), KErrGeneral);
        aDialog.UpdateRemoteTargetL(ta->Type(), aElements->MessageElements());

		__SIP_ASSERT_LEAVE(aElements->ToHeader() != NULL, KErrArgument);        
        aDialog.FillRemoteTagL(*aElements->ToHeader());

        CSIPClientTransaction* clientTa =
            static_cast<CSIPClientTransaction*>(ta);
        clientTa->SetResponseElements(aElements);

        aDialog.UpdateState(*clientTa,
                            *iEarly,
                            *iConfirmed,
                            *iTerminated);
        aCallback.Set(CConnectionCallback::EIncomingResponse,
                      clientTa,
                      NULL,
                      NULL,
                      assoc);
        return ETrue;
        }
    
    //Transaction not found, ignore response
    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogTrying::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CDialogTrying::IncomingResponseL(CSIPDialogImplementation& aDialog,
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
                              *iEarly,
                              *iConfirmed,
                              *iTerminated);
    }

// -----------------------------------------------------------------------------
// CDialogTrying::IncomingRequestL
// -----------------------------------------------------------------------------
//
TBool CDialogTrying::IncomingRequestL(CSIPDialogImplementation& aDialog,
                                      CSIPServerTransaction* aTransaction,
                                      CConnectionCallback& aCallback) const
    {
    __SIP_ASSERT_LEAVE(aTransaction, KErrArgument);

    if (aTransaction->Type() == SIPStrings::StringF(SipStrConsts::ENotify) &&
    	aDialog.DoesNotifyConfirmDialog())
        {
        aDialog.ChangeRefreshesToActive();
        aDialog.ChangeState(iConfirmed);
        }

    return RequestReceivedL(aDialog, aTransaction, aCallback);
    }

// -----------------------------------------------------------------------------
// CDialogTrying::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogTrying::ErrorOccured(CSIPDialogImplementation& aDialog,
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
// CDialogTrying::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogTrying::ErrorOccured(CSIPDialogImplementation& aDialog,
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
// CDialogTrying::ConnectionLost
// -----------------------------------------------------------------------------
//
void CDialogTrying::ConnectionLost(CSIPDialogImplementation& aDialog) const
    {
    aDialog.ChangeState(iTerminated);
    }
