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
// Name          : sipdialogstate.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include <uri8.h>
#include "siperr.h"
#include "sipinternalerr.h"
#include "SipAssert.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipdialogstate.h"
#include "SipConnectionImplementation.h"
#include "sipclientconnection.h"
#include "SipDialogImplementation.h"
#include "SipDialogAssocImplementation.h"
#include "siprefresh.h"
#include "sipservertransaction.h"
#include "sipinviteclienttransaction.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "sipcseqheader.h"
#include "csipdialogresponsesender.h"
#include "sipconnectioncallback.h"
#include "sipinvitedialogassoc.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif

// -----------------------------------------------------------------------------
// CDialogState::~CDialogState
// -----------------------------------------------------------------------------
//
CDialogState::~CDialogState()
	{
	}

// -----------------------------------------------------------------------------
// CDialogState::SendNonTargetRefreshRequestL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CDialogState::SendNonTargetRefreshRequestL(
								const CSIPDialogImplementation& /*aDialog*/,
                                CSIPDialogAssocImplementation& /*aAssoc*/,
                                RStringF /*aMethod*/,
                                CSIPMessageElements* /*aElements*/) const
    {
  	return InvalidStateL();
    }

// -----------------------------------------------------------------------------
// CDialogState::SendInviteL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendInviteL(const CSIPDialogImplementation& /*aDialog*/,
                          CSIPInviteDialogAssoc& /*aAssoc*/,
                          CSIPMessageElements* /*aElements*/) const
	{
	return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendPrackL
// Application might try to send PRACK at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendPrackL(CSIPInviteDialogAssoc& /*aAssoc*/,
                         CSIPMessageElements* /*aElements*/) const
    {    
    return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendUpdateL
// Application might try to send UPDATE at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendUpdateL(CSIPInviteDialogAssoc& /*aAssoc*/,
                          CSIPMessageElements* /*aElements*/) const
    {
    return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendAckL
// -----------------------------------------------------------------------------
//
void CDialogState::SendAckL(CSIPInviteDialogAssoc& /*aAssoc*/,
                            const CSIPClientTransaction& /*aTransaction*/,
	                        CSIPMessageElements* /*aElements*/) const
    {
    InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendByeL
// Application might try to send BYE at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendByeL(CSIPInviteDialogAssoc& /*aAssoc*/,
                       CSIPMessageElements* /*aElements*/) const
    {
	return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendNotifyL
// Application might try to send NOTIFY at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendNotifyL(CSIPNotifyDialogAssoc& /*aAssoc*/,
                    	  CSIPMessageElements* /*aElements*/) const
	{
	return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendReferL
// Application might try to send REFER at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendReferL(const CSIPDialogImplementation& /*aDialog*/,
						 CSIPReferDialogAssoc& /*aAssoc*/,
                    	 CSIPMessageElements* /*aElements*/) const
	{
	return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendResponseL
// Application might try to send response at any time
// -----------------------------------------------------------------------------
//
void CDialogState::SendResponseL(CSIPDialogImplementation& /*aDialog*/,
                                 const CSIPResponseElements& /*aElements*/,
						         TUint32 /*aRequestId*/,
                                 TBool /*aAffectsDialogState*/,
                                 TBool /*aTargetRefresh*/) const
    {    
    InvalidStateL();
    }

// -----------------------------------------------------------------------------
// CDialogState::SendSubscribeL
// Application might try to send SUBSCRIBE at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendSubscribeL(const CSIPDialogImplementation& /*aDialog*/,
                             CSIPSubscribeDialogAssoc& /*aAssoc*/,
                             CSIPMessageElements* /*aElements*/,
                             CSIPRefresh* /*aRefresh*/) const
    {
	return InvalidStateL();
    }

// -----------------------------------------------------------------------------
// CDialogState::UpdateL
// Application might try to update SUBSCRIBE at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::UpdateL(const CSIPDialogImplementation& /*aDialog*/,
                	  CSIPSubscribeDialogAssoc& /*aAssoc*/,
                	  CSIPMessageElements* /*aElements*/) const
	{
    return InvalidStateL();
	}

// -----------------------------------------------------------------------------
// CDialogState::SendUnsubscribeL
// Application might try to send (un)SUBSCRIBE at any time
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CDialogState::SendUnsubscribeL(CSIPSubscribeDialogAssoc& /*aAssoc*/,
                               CSIPMessageElements* /*aElements*/) const
    {    
    return InvalidStateL();
    }

// -----------------------------------------------------------------------------
// CDialogState::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CDialogState::IncomingResponseL(CSIPDialogImplementation& /*aDialog*/,
                                      CSIPResponseElements* /*aElements*/,
		                              TUint32 /*aRequestId*/,
                                      TUint32 /*aDialogId*/,
                                      CConnectionCallback& /*aCallback*/) const
    {
	__SIP_ASSERT_LEAVE(EFalse, KErrSIPInvalidDialogState);
    return EFalse;
	}

// -----------------------------------------------------------------------------
// CDialogState::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool CDialogState::IncomingResponseL(CSIPDialogImplementation& /*aDialog*/,
                                      CSIPResponseElements* /*aElements*/,
		                              TUint32 /*aRequestId*/,
                                      TUint32 /*aRefreshId*/,
                                      TUint32 /*aDialogId*/,
                                      CConnectionCallback& /*aCallback*/) const
    {
    __SIP_ASSERT_LEAVE(EFalse, KErrSIPInvalidDialogState);
    return EFalse;
	}

// -----------------------------------------------------------------------------
// CDialogState::IncomingRequestL
// If the received request is not handled by current state, ignore it.
// -----------------------------------------------------------------------------
//
TBool
CDialogState::IncomingRequestL(CSIPDialogImplementation& /*aDialog*/,
                               CSIPServerTransaction* aTransaction,
                               CConnectionCallback& /*aCallback*/) const                               
    {
    delete aTransaction;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogState::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogState::ErrorOccured(CSIPDialogImplementation& /*aDialog*/,
                                 TInt /*aError*/,
                                 TUint32 /*aRequestId*/,
                                 CConnectionCallback& /*aCallback*/) const
    {
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CDialogState::ErrorOccured() no event handler"),
					KErrSIPInvalidDialogState));

    return EFalse;
	}

// -----------------------------------------------------------------------------
// CDialogState::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CDialogState::ErrorOccured(CSIPDialogImplementation& /*aDialog*/,
                                 TInt /*aError*/,
                                 TUint32 /*aRefreshId*/,
                                 TUint32 /*aRequestId*/,
                                 CConnectionCallback& /*aCallback*/) const
    {
    __ASSERT_DEBUG(EFalse,
		User::Panic(_L("CDialogState::ErrorOccured(refresh) no event handler"),
					KErrSIPInvalidDialogState));
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogState::HandleErrorOccurred
// If the transaction and association are not found, error is ignored.
// If ACK wasn't received, the application must still be able to send a BYE to
// terminate the session, so the dialog must not be terminated.
// If dialog is confirmed, error won't cause it go into terminated state.
// -----------------------------------------------------------------------------
//
TBool CDialogState::HandleErrorOccurred(CSIPDialogImplementation& aDialog,
                                        TInt aError,
                                        TUint32 aRequestId,
                                        CConnectionCallback& aCallback,
                                        CDialogState& aTerminated) const
    {
    __SIP_ASSERT_RETURN_VALUE(aError != KErrNone, EFalse);

    CSIPTransactionBase* ta = NULL;
    CSIPDialogAssocBase* assoc = NULL;

	CConnectionCallback::TCallbackMethod callback =
		CConnectionCallback::EErrorOccuredDialogTransaction;
	if (!aDialog.FindTransactionAndAssoc(aRequestId, &ta, &assoc))	
		{
		CSIPConnection* conn = aDialog.Connection();
		if (conn)
			{
			ta = conn->Implementation().FindTransaction(aRequestId);
			if (ta)			
				{		
				callback = CConnectionCallback::EErrorOccuredTransaction;
				}
			}
		}

	if (ta)
		{
        if (ta->AffectsDialogState() && aError != KErrSIPNoAckReceived)
            {
			aDialog.ChangeState(&aTerminated);
            }
        ta->ChangeState(CSIPTransactionBase::ETerminated);

    	if (aError == KInviteTransactionCancelled)
        	{
        	__SIP_ASSERT_RETURN_VALUE(!ta->IsSIPClientTransaction(), EFalse);
        	callback = CConnectionCallback::EInviteCanceled;
        	assoc = NULL;        	
        	}

        aCallback.Set(callback, ta, NULL, NULL, assoc, aError);
        return ETrue;
        }
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogState::HandleRefreshErrorOccurred
// INVITE can't be refreshed, so KInviteTransactionCancelled and
// KErrSIPNoAckReceived don't need to be handled.
// -----------------------------------------------------------------------------
//
TBool
CDialogState::HandleRefreshErrorOccurred(CSIPDialogImplementation& aDialog,
		                                 TInt aError,
		                                 TUint32 aRefreshId,
		                                 CConnectionCallback& aCallback,
		                                 CDialogState& aTerminated) const
    {
 	__SIP_ASSERT_RETURN_VALUE(aError != KErrNone, EFalse);

    CSIPDialogAssocBase* assoc = NULL;
    CSIPRefresh* refresh = NULL;
    if (aDialog.FindAssocAndRefresh(aRefreshId, &assoc, &refresh))
        {
        CSIPClientTransaction* ta = refresh->Transaction();
        if (ta)
            {
            if (ta->AffectsDialogState() &&
            	aDialog.State() != CSIPDialog::EConfirmed)
                {
                aDialog.ChangeState(&aTerminated);
                }
            ta->ChangeState(CSIPTransactionBase::ETerminated);
            }
        refresh->ChangeState(CSIPRefresh::ETerminated);		
        aCallback.Set(CConnectionCallback::EErrorOccuredDialog,
                      NULL,
                      NULL,
                      NULL,
                      assoc,
                      aError);
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogState::RequestReceivedL
// Application must resolve the actual dialog association to which this request
// belongs. No dialog association is created by SIP API, it is application's
// responsibility to create the association.
// Dialog-related sender is set, so that the response goes through the dialog.
// -----------------------------------------------------------------------------
//
TBool CDialogState::RequestReceivedL(CSIPDialogImplementation& aDialog,
                                     CSIPServerTransaction* aTransaction,                             
                                     CConnectionCallback& aCallback) const
    {
    __SIP_ASSERT_LEAVE(aTransaction, KErrArgument);

    aDialog.UpdateRemoteTargetL(aTransaction->Type(),
						aTransaction->RequestElements()->MessageElements());

    aTransaction->SetResponseSender(CSIPDialogResponseSender::NewL(aDialog));
    aCallback.Set(CConnectionCallback::EIncomingRequest,
                  aTransaction,
                  NULL,
                  &aDialog.Dialog());
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CDialogState::SendResponseWithinDialogL
// -----------------------------------------------------------------------------
//
void
CDialogState::SendResponseWithinDialogL(CSIPDialogImplementation& aDialog,
                                        const CSIPResponseElements& aElements,
				    		            TUint32 aRequestId,
                                        TBool aTargetRefresh) const
    {
    CSIPConnection* connection = aDialog.Connection();
    __ASSERT_ALWAYS(connection, User::Leave(KErrSIPResourceNotAvailable));    
    
    connection->Implementation().ClientConnectionL().SendResponseWithinDialogL(
    													aElements,
                                                        aRequestId,
                                                        aDialog.DialogId(),
                                                        aTargetRefresh);    
    }

// -----------------------------------------------------------------------------
// CDialogState::IncomingResponseWithinDialogL
// As response isn't refresh related, drop it if no matching transaction exists.
// -----------------------------------------------------------------------------
//
TBool CDialogState::IncomingResponseWithinDialogL(
									CSIPDialogImplementation& aDialog,
                                    CSIPResponseElements* aElements,
		                            TUint32 aRequestId,
                                    TUint32 aDialogId,
                                    CConnectionCallback& aCallback,
                                    const CDialogState& aEarly,
                                    const CDialogState& aConfirmed,
                                    const CDialogState& aTerminated) const
    {
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);

    CSIPTransactionBase* ta = NULL;
    CSIPDialogAssocBase* assoc = NULL;
    if (!aDialog.FindTransactionAndAssoc(aRequestId, &ta, &assoc))
        {
        delete aElements;
        return EFalse;
        }

    __SIP_ASSERT_LEAVE(ta->IsSIPClientTransaction(), KErrGeneral);
    CSIPClientTransaction* clientTa = static_cast<CSIPClientTransaction*>(ta);
    CSIPInviteDialogAssoc* newAssoc = CheckIfForkingL(aDialog,
                                                      aDialogId,
                                                      aElements,
                                                      *clientTa);
	CSIPDialogImplementation* dialog = &aDialog;
    CConnectionCallback::TCallbackMethod callback =
    	CConnectionCallback::EIncomingResponse;
	if (newAssoc)
		{
		assoc = newAssoc;
        dialog = &newAssoc->Dialog().Implementation();
        callback = CConnectionCallback::EIncomingResponseNewAssoc;
		}
	else
		{
		aDialog.UpdateRemoteTargetL(ta->Type(), aElements->MessageElements());
		}

    //Ownership of aElements is taken when leave cannot occur
    clientTa->SetResponseElements(aElements);

    dialog->UpdateState(*clientTa, aEarly, aConfirmed, aTerminated);
    aCallback.Set(callback, clientTa, NULL, NULL, assoc);

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CDialogState::CheckIfForkingL
// A forked 1xx or 2xx creates a new dialog and INVITE dialog association. No
// need to give contact to the new CSIPInviteDialogAssoc. Even the original
// dialog does not have iContact anymore.
// -----------------------------------------------------------------------------
//
CSIPInviteDialogAssoc*
CDialogState::CheckIfForkingL(CSIPDialogImplementation& aDialog,
                              TUint32 aDialogId,
                              CSIPResponseElements* aElements,
                              CSIPClientTransaction& aTransaction) const
    {
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    CSIPInviteDialogAssoc* assoc = NULL;

    if (aElements->StatusCode() < 300 &&
        aTransaction.Type() == SIPStrings::StringF(SipStrConsts::EInvite) &&
        aDialogId != 0 &&
        aDialogId != aDialog.DialogId())
        {
        assoc = CreateForkedInviteAssocLC(aDialog, *aElements);

        //Link transaction with the new assoc
        static_cast<CSIPInviteClientTransaction&>(aTransaction).
            AddAssociationL(assoc->Implementation());

        assoc->Dialog().Implementation().SetDialogId(aDialogId);
        CleanupStack::Pop(assoc);

        assoc->Dialog().Implementation().InitialTransactionStarted(
        	aTransaction.RequestId());
        }
    return assoc;
    }

// -----------------------------------------------------------------------------
// CDialogState::CreateForkedInviteAssocLC
// -----------------------------------------------------------------------------
//
CSIPInviteDialogAssoc*
CDialogState::CreateForkedInviteAssocLC(CSIPDialogImplementation& aDialog,
                                        CSIPResponseElements& aElements) const
    {
    __SIP_ASSERT_LEAVE(aElements.FromHeader() != NULL, KErrArgument);
    __SIP_ASSERT_LEAVE(aElements.ToHeader() != NULL, KErrArgument);
    __ASSERT_ALWAYS(aDialog.Connection(),
        			User::Leave(KErrSIPResourceNotAvailable));

    CSIPFromHeader* from = CSIPFromHeader::NewLC(*aElements.FromHeader());
    CSIPToHeader* to = CSIPToHeader::NewLC(*aElements.ToHeader());

	CUri8* remoteUri = aDialog.GetUriFromContactL(aElements.MessageElements());
    if (remoteUri)
    	{
    	CleanupStack::PushL(remoteUri);
    	}
    else
    	{
    	remoteUri = CUri8::NewLC(aDialog.RemoteURI().Uri());
    	}

    CSIPInviteDialogAssoc* assoc = NULL;
    if (aDialog.RegistrationContext())
        {
        assoc = CSIPInviteDialogAssoc::NewL(*aDialog.Connection(),
        									remoteUri,
                                            *aDialog.RegistrationContext(),
                                            from,
                                            to);
        }
    else
        {
        assoc = CSIPInviteDialogAssoc::NewL(*aDialog.Connection(),
                                            from,
                                            remoteUri,
                                            to);
        }
    CleanupStack::Pop(3); //remoteUri, to, from
    CleanupStack::PushL(assoc);        
    assoc->Dialog().Implementation().CopyCallIdL(aDialog);

    return assoc;
    }

// -----------------------------------------------------------------------------
// CDialogState::ResponseToRefreshL
// For 3xx-6xx to an existing refresh, the aRequestId is not recognized.
// If ErrorOccured is sent to application, it can get the CSIPRefresh and from
// it the client transaction and SIP response.
// -----------------------------------------------------------------------------
//
TBool CDialogState::ResponseToRefreshL(CSIPDialogImplementation& aDialog,
                                       CSIPResponseElements* aElements,
                                       TUint32 aRequestId,
                                       TUint32 aRefreshId,
                                       CConnectionCallback& aCallback,
                                       const CDialogState& aEarly,
                                       const CDialogState& aConfirmed,
                                       const CDialogState& aTerminated) const
    {
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    CSIPDialogAssocBase* assoc = NULL;
    CSIPRefresh* refresh = NULL;
    CSIPTransactionBase* ta = NULL;
    if (aDialog.FindAssocAndRefreshL(aRequestId,
                                     aRefreshId,
                                     &assoc,
                                     &refresh,
                                     &ta))
        {
        aDialog.UpdateRemoteTargetL(aElements->CSeqHeader()->Method(),
        							aElements->MessageElements());

        CSIPClientTransaction* clientTa =
            static_cast<CSIPClientTransaction*>(ta);
        CBase* objToDelete = NULL;
        if (ta)
            {
            __SIP_ASSERT_LEAVE(ta->IsSIPClientTransaction(), KErrGeneral);
            }
        else
            {
        	__SIP_ASSERT_LEAVE(aElements->CSeqHeader() != NULL, KErrArgument);
            clientTa = aDialog.CreateClientTransactionL(
					                aElements->CSeqHeader()->Method(),				             
					                assoc->Implementation(),
					                refresh);
			clientTa->SetRequestId(aRequestId);
			objToDelete = clientTa;
            }

        clientTa->SetResponseElements(aElements);
        refresh->SetRefreshIdIfEmpty(aRefreshId);
        refresh->UpdateRefreshState(aElements->StatusCode());
        aDialog.UpdateState(*clientTa, aEarly, aConfirmed, aTerminated);

        if (!ta && aElements->StatusCode() >= 300)
            {
            aCallback.Set(CConnectionCallback::EErrorOccuredDialog,
                          NULL,
                          NULL,
                          NULL,
                          assoc,
                          KErrSIPTerminatedWithResponse,
                          objToDelete);
            }
        else
            {
            aCallback.Set(CConnectionCallback::EIncomingResponse,
                          clientTa,
                          NULL,
                          NULL,
                          assoc,
                          KErrNone,
                          objToDelete);
            }
        return ETrue;
        }

    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CDialogState::InvalidStateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CDialogState::InvalidStateL() const
	{
	User::Leave(KErrSIPInvalidDialogState);
    return NULL;
	}
