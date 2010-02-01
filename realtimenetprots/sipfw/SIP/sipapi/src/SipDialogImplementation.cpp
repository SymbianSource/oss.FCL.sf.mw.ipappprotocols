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
// Name          : SipDialogImplementation.cpp
// Part of       : SIPAPI
// Version       : SIP/6.0
//



#include <uri8.h>
#include "siperr.h"
#include "SipAssert.h"
#include "SipDialogImplementation.h"
#include "SipDialogAssocImplementation.h"
#include "sipinvitedialogassoc.h"
#include "sipsubscribedialogassoc.h"
#include "sipdialogstate.h"
#include "sipmessageelements.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "SipConnectionImplementation.h"
#include "sipclientconnection.h"
#include "sipinviteclienttransaction.h"
#include "sipservertransaction.h"
#include "sipregistrationcontext.h"
#include "siprefresh.h"
#include "sipaddress.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipcontactheader.h"
#include "sipcallidheader.h"
#include "sip.h"
#include "SipImplementation.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CSIPDialogImplementation::NewLC
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation* CSIPDialogImplementation::NewLC(
						CSIPConnectionImplementation& aConnImplementation,
						const MSIPRegistrationContext* aContext)
	{
	CSIPDialog* dialog = aContext ?
		CSIPDialog::NewL(aConnImplementation, *aContext) :
		CSIPDialog::NewL(aConnImplementation);	

    CSIPDialogImplementation* self = &dialog->Implementation();
    CleanupStack::PushL(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::NewL
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation* CSIPDialogImplementation::NewL(
						CSIPDialog* aDialog,
						CSIPConnectionImplementation& aConnImplementation)
    {
    CSIPDialogImplementation* self =
    	new (ELeave) CSIPDialogImplementation(aConnImplementation);
    CleanupStack::PushL(self);
    self->ConstructL(aDialog);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::NewL
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation* CSIPDialogImplementation::NewL(
							CSIPDialog* aDialog,
							CSIPConnectionImplementation& aConnImplementation,
                            const MSIPRegistrationContext& aContext)
    {
    CSIPDialogImplementation* self =
    	new (ELeave) CSIPDialogImplementation(aConnImplementation, aContext);
    CleanupStack::PushL(self);
    self->ConstructL(aDialog);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CSIPDialogImplementation
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation::CSIPDialogImplementation(
						CSIPConnectionImplementation& aConnImplementation) :
    iConnection(&aConnImplementation)
#ifdef CPPUNIT_TEST
    , iDialogAssocs(1)
#endif	
    {
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CSIPDialogImplementation
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation::CSIPDialogImplementation(
						CSIPConnectionImplementation& aConnImplementation,
                       	const MSIPRegistrationContext& aContext) :
    iConnection(&aConnImplementation),
    iRegistration(&aContext)
#ifdef CPPUNIT_TEST
    , iDialogAssocs(1)
#endif
	, iStringPoolOpened(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::ConstructL(CSIPDialog* aDialog)
    {
    CheckConnectionL();
    iConnection->AddDialogL(*this);
    iState = iConnection->InitialDialogStateL();
    iDialog = aDialog;

    SIPStrings::OpenL();
    iStringPoolOpened = ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::~CSIPDialogImplementation
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation::~CSIPDialogImplementation()
    {
    if (iConnection)
        {        
        iConnection->RemoveDialog(*this);
        }

    iDialogAssocs.Reset();

	delete iDialog;
    delete iFrom;
    delete iTo;
    delete iRemoteUri;
    delete iContact;
    delete iCallID;

	if (iStringPoolOpened)
		{
    	SIPStrings::Close();
		}
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::State
// -----------------------------------------------------------------------------
//
CSIPDialog::TState CSIPDialogImplementation::State() const
    {
    __TEST_INVARIANT;
    return iState ? iState->State() : CSIPDialog::ETerminated;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ChangeState
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::ChangeState(const CDialogState* aNewState)
	{
    __TEST_INVARIANT;

	iState = aNewState;

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ConnectionLost
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::ConnectionLost()
    {
    __TEST_INVARIANT;

    if (iState)
        {        
        iState->ConnectionLost(*this);
        }
    
    for (TInt i = 0; i < iDialogAssocs.Count(); ++i)
        {
        iDialogAssocs[i]->Implementation().ConnectionLost();
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SIPDialogAssociations
// -----------------------------------------------------------------------------
//
const RPointerArray<CSIPDialogAssocBase>&
CSIPDialogImplementation::SIPDialogAssociations() const
    {
    __TEST_INVARIANT;
    return iDialogAssocs;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::RegistrationContext
// -----------------------------------------------------------------------------
//
const MSIPRegistrationContext*
CSIPDialogImplementation::RegistrationContext() const
    {
    __TEST_INVARIANT;
    return iRegistration;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::IsAssociated
// -----------------------------------------------------------------------------
//
TBool
CSIPDialogImplementation::IsAssociated(const CSIPDialogAssocBase& aAssoc) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iDialogAssocs.Count(); ++i)
        {
        if (iDialogAssocs[i] == &aAssoc)
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::Connection
// -----------------------------------------------------------------------------
//
CSIPConnection* CSIPDialogImplementation::Connection()
    {    
    return GetConnection();
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::Connection
// -----------------------------------------------------------------------------
//
const CSIPConnection* CSIPDialogImplementation::Connection() const
    {
    return GetConnection();
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::GetConnection
// -----------------------------------------------------------------------------
//
CSIPConnection* CSIPDialogImplementation::GetConnection() const
	{
	__TEST_INVARIANT;

	CSIPConnection* conn = NULL;
    if (iConnection)
    	{
    	TRAP_IGNORE((conn = &iConnection->SIPConnectionL()))
    	}
    return conn;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FromHeader
// -----------------------------------------------------------------------------
//
const CSIPFromHeader& CSIPDialogImplementation::FromHeader() const
    {
    __TEST_INVARIANT;
    return *iFrom;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ToHeader
// -----------------------------------------------------------------------------
//
const CSIPToHeader& CSIPDialogImplementation::ToHeader() const
    {
    __TEST_INVARIANT;
    return *iTo;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::RemoteURI
// -----------------------------------------------------------------------------
//
const CUri8& CSIPDialogImplementation::RemoteURI() const
    {
    __ASSERT_DEBUG(iRemoteUri,
    	User::Panic(_L("CSIPDlgImp:RemoteURI"), KErrNotFound));
    return *iRemoteUri;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CallIdL
// If Call-ID doesn't exist yet, application has asked it too early.
// -----------------------------------------------------------------------------
//
const CSIPCallIDHeader& CSIPDialogImplementation::CallIdL() const
	{
	__ASSERT_ALWAYS(iCallID, User::Leave(KErrSIPInvalidDialogState));
	return *iCallID;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::StoreCallIdL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::StoreCallIdL()
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iDialogId != 0, KErrNotFound);	
	CheckConnectionL();

    CSIPCallIDHeader* tmp = iConnection->ClientConnectionL().CallIDL(iDialogId);
	delete iCallID;
	iCallID = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CopyCallIdL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::CopyCallIdL(
    const CSIPDialogImplementation& aDialog)
	{
	__TEST_INVARIANT;	

	CSIPCallIDHeader* tmp = 
	    static_cast<CSIPCallIDHeader*>(aDialog.CallIdL().CloneL());
	delete iCallID;
	iCallID = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::operator==(
						const CSIPDialogImplementation& aDialog) const
    {
    __TEST_INVARIANT;
    return this == &aDialog ||
    	   (iDialogId && aDialog.iDialogId && (iDialogId == aDialog.iDialogId));
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ReuseInitialRequestData
// -----------------------------------------------------------------------------
//
TInt CSIPDialogImplementation::ReuseInitialRequestData()
    {
    TInt err = KErrSIPInvalidDialogState;
    
    if (iState && iState->State() == CSIPDialog::ETerminated &&
        iConnection && iConnection->SIP() && iConnection->ClientConnection())
        {
        TUint32 originalRequestId = iInitialRequestId;
        iInitialRequestId = 0;
        const CDialogState* originalState = iState;
        iState = iConnection->SIP()->Implementation().InitialDialogState();
        err = iConnection->ClientConnection()->ResetDialogState(iDialogId);
        if (err != KErrNone)
            {
            // Rollback to the original state
            iInitialRequestId = originalRequestId;
            iState = originalState;
            }
        else
            {
            iTo->DeleteParam(SIPStrings::StringF(SipStrConsts::ETag));
            }
        }       
    
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::Dialog
// Don't use invariant here.
// -----------------------------------------------------------------------------
//
CSIPDialog& CSIPDialogImplementation::Dialog()
	{
	__ASSERT_DEBUG(iDialog, User::Panic(_L("CSIPDlgImp:Dialog"), KErrNotFound));
	return *iDialog;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool
CSIPDialogImplementation::IncomingResponseL(CSIPResponseElements* aElements,
				                            TUint32 aRequestId,
		                                    TUint32 aDialogId,
		                                    CConnectionCallback& aCallback)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    if (iState)
        {
        return iState->IncomingResponseL(*this,
                                         aElements,
		                                 aRequestId,
                                         aDialogId,
                                         aCallback);
        }
    delete aElements;
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::IncomingResponseL
// -----------------------------------------------------------------------------
//
TBool
CSIPDialogImplementation::IncomingResponseL(CSIPResponseElements* aElements,
				                            TUint32 aRequestId,
		                                    TUint32 aRefreshId,
							                TUint32 aDialogId,
		                                    CConnectionCallback& aCallback)                                   
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    if (iState)
        {
        return iState->IncomingResponseL(*this,
                                         aElements,
		                                 aRequestId,
                                         aRefreshId,
                                         aDialogId,
                                         aCallback);
        }
    delete aElements;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::IncomingRequest
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::IncomingRequestL(
									CSIPServerTransaction* aTransaction,
                                   	CConnectionCallback& aCallback)                                  
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aTransaction != NULL, KErrArgument);

    if (iState)
        {
        return iState->IncomingRequestL(*this, aTransaction, aCallback);
        }
    delete aTransaction;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::ErrorOccured(TInt aError,
				                             TUint32 aRequestId,
				                             CConnectionCallback& aCallback)
    {
    __TEST_INVARIANT;
    return iState &&
    	   iState->ErrorOccured(*this, aError, aRequestId, aCallback);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ErrorOccured
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::ErrorOccured(TInt aError,             
				                             TUint32 aRefreshId,
				                             TUint32 aRequestId,
				                             CConnectionCallback& aCallback)
    {
    __TEST_INVARIANT;
    return iState &&
    	iState->ErrorOccured(*this, aError, aRefreshId, aRequestId, aCallback);        
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::DialogId
// Don't use invariant here.
// -----------------------------------------------------------------------------
//
TUint32 CSIPDialogImplementation::DialogId() const
    {    
    return iDialogId;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SetDialogId
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::SetDialogId(TUint32 aDialogId)
    {
    __TEST_INVARIANT;

    iDialogId = aDialogId;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FindTransaction
// -----------------------------------------------------------------------------
//
CSIPTransactionBase*
CSIPDialogImplementation::FindTransaction(TUint32 aRequestId) const
    {
    __TEST_INVARIANT;

    CSIPTransactionBase* ta = NULL;
    for (TInt i = 0; i < iDialogAssocs.Count() && !ta; ++i)
        {
        ta = iDialogAssocs[i]->Implementation().FindTransaction(aRequestId);
        }

    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FindTransactionAndAssoc
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::FindTransactionAndAssoc(TUint32 aRequestId,
                                    CSIPTransactionBase** aTransaction,
                                    CSIPDialogAssocBase** aAssoc) const
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN_VALUE(aTransaction && aAssoc, EFalse);

    CSIPTransactionBase* ta = NULL;
    for (TInt i = 0; i < iDialogAssocs.Count(); ++i)
        {
        ta = iDialogAssocs[i]->Implementation().FindTransaction(aRequestId);
        if (ta)
            {
            *aTransaction = ta;
            *aAssoc = iDialogAssocs[i];
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FindAssocAndRefreshL
// Search with aRequestId. If not found, search with aRefreshId.
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::FindAssocAndRefreshL(TUint32 aRequestId,
                                  TUint32 aRefreshId,
                                  CSIPDialogAssocBase** aAssoc,
                                  CSIPRefresh** aRefresh,
                                  CSIPTransactionBase** aTransaction) const
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN_VALUE(aAssoc && aRefresh && aTransaction, EFalse);

    if (FindTransactionAndAssoc(aRequestId, aTransaction, aAssoc))
        {
        *aRefresh =
            static_cast<CSIPClientTransaction*>(*aTransaction)->Refresh();

        //Refresh must exist now
        __ASSERT_DEBUG(*aRefresh,
		    User::Panic(_L("CSIPDlgImp:FindAssocAndRfr"), KErrNotFound));
        if (*aRefresh)
            {
            return ETrue;
            }
        }

	if (FindAssocAndRefresh(aRefreshId, aAssoc, aRefresh))
		{
		CSIPTransactionBase* ta = (*aRefresh)->Transaction();
		if (ta)
			{
			CSIPTransactionBase::TState state = ta->StateL();
			//If transaction has received a final response, it is the initial
			//transaction creating the refresh. Ignore EConfirmed state, as only
			//INVITE _server_ transaction has it.
			if (state != CSIPTransactionBase::ECompleted &&
				state != CSIPTransactionBase::ETerminated)
				{
				*aTransaction = ta;
				}
			}
		return ETrue;
		}
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FindAssocAndRefresh
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::FindAssocAndRefresh(TUint32 aRefreshId,
                                      CSIPDialogAssocBase** aAssoc,
                                      CSIPRefresh** aRefresh) const
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN_VALUE(aAssoc && aRefresh, EFalse);
	
    CSIPRefresh* refresh = NULL;
    for (TInt i = 0; i < iDialogAssocs.Count(); ++i)
        {
        refresh = iDialogAssocs[i]->FindRefresh(aRefreshId);
        if (refresh)
            {
            *aAssoc = iDialogAssocs[i];
            *aRefresh = refresh;
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FindRefresh
// -----------------------------------------------------------------------------
//  
CSIPRefresh* CSIPDialogImplementation::FindRefresh(
    TUint32 aRequestId,
    TUint32 aRefreshId) const
    {
    CSIPDialogAssocBase* dummyAssoc = NULL;
    CSIPRefresh* refresh = NULL;
    CSIPTransactionBase* dummyTransaction;
    if (FindTransactionAndAssoc(aRequestId, &dummyTransaction, &dummyAssoc))
        {
        return static_cast<CSIPClientTransaction*>(dummyTransaction)->Refresh();
        }
    FindAssocAndRefresh(aRefreshId,&dummyAssoc,&refresh);
    return refresh;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendNonTargetRefreshRequestL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPDialogImplementation::SendNonTargetRefreshRequestL(
										CSIPDialogAssocImplementation& aAssoc,
                                        RStringF aMethod,
                                        CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;
    return StateL().SendNonTargetRefreshRequestL(*this,
                                                 aAssoc,
                                                 aMethod,
                                                 aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendInviteL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendInviteL(CSIPInviteDialogAssoc& aAssoc,
                        			  CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;    
    return StateL().SendInviteL(*this, aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendPrackL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendPrackL(CSIPInviteDialogAssoc& aAssoc,
                       				 CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;    
    return StateL().SendPrackL(aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendUpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendUpdateL(CSIPInviteDialogAssoc& aAssoc,
                        			  CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;
    return StateL().SendUpdateL(aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendAckL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::SendAckL(CSIPInviteDialogAssoc& aAssoc,
			                       const CSIPClientTransaction& aTransaction,
			                       CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;

    StateL().SendAckL(aAssoc, aTransaction, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendSubscribeL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendSubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
			                             CSIPMessageElements* aElements,
			                             CSIPRefresh* aRefresh) const
    {
    __TEST_INVARIANT;
    return StateL().SendSubscribeL(*this, aAssoc, aElements, aRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::UpdateL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::UpdateL(CSIPSubscribeDialogAssoc& aAssoc,
			                      CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;
    return StateL().UpdateL(*this, aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendUnsubscribeL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendUnsubscribeL(CSIPSubscribeDialogAssoc& aAssoc,
                             			   CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;
    return StateL().SendUnsubscribeL(aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendByeL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendByeL(CSIPInviteDialogAssoc& aAssoc,
                     			   CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;    
    return StateL().SendByeL(aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendCancelL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendCancelL(TUint32 aRequestId) const
    {
    __TEST_INVARIANT;

    CSIPTransactionBase* dummyTa = NULL;
    CSIPDialogAssocBase* assoc = NULL;
	__ASSERT_ALWAYS(FindTransactionAndAssoc(aRequestId, &dummyTa, &assoc),
					User::Leave(KErrSIPInvalidTransactionState));
	__ASSERT_ALWAYS(assoc->Type() == SIPStrings::StringF(SipStrConsts::EInvite),
					User::Leave(KErrSIPInvalidTransactionState));
    return static_cast<CSIPInviteDialogAssoc*>
    	(assoc)->DoSendCancelL(aRequestId);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendNotifyL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendNotifyL(CSIPNotifyDialogAssoc& aAssoc,
                        			  CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;    
    return StateL().SendNotifyL(aAssoc, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendReferL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendReferL(CSIPReferDialogAssoc& aAssoc,
                       				 CSIPMessageElements* aElements) const
   	{
   	__TEST_INVARIANT;    
    return StateL().SendReferL(*this, aAssoc, aElements);
   	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendResponseL
// -----------------------------------------------------------------------------
//
void
CSIPDialogImplementation::SendResponseL(const CSIPResponseElements& aElements,
							            TUint32 aRequestId,
			                            TBool aAffectsDialogState,
			                            TBool aTargetRefresh)
    {
    __TEST_INVARIANT;

    StateL().SendResponseL(*this,
                           aElements,
                           aRequestId,
                           aAffectsDialogState,
                           aTargetRefresh);
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendRequestInDialogL
// CSIPClientConnection checks that connection is active.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPDialogImplementation::SendRequestInDialogL(
                                        CSIPDialogAssocImplementation& aAssoc,
                                        RStringF aMethod,
                                        CSIPMessageElements* aElements) const
    {
    __TEST_INVARIANT;
    CheckConnectionL();

	CSIPClientTransaction* ta = CreateClientTransactionL(aMethod, aAssoc, NULL);
    CleanupStack::PushL(ta);

    TUint32 requestId(0);
    TUint32 dummyRefreshId(0); // Not used for non-refreshed requests
    iConnection->ClientConnectionL().SendRequestWithinDialogL(iDialogId,
							  requestId,
							  dummyRefreshId,
                              aMethod,
							  aElements,
                              CSIPTransactionBase::IsTargetRefresh(aMethod));
	CleanupStack::Pop(ta);
    ta->SetRequestId(requestId);

    delete aElements;
    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CreateClientTransactionL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::CreateClientTransactionL(RStringF aType,                                     
                                     CSIPDialogAssocImplementation& aAssoc,                                     
                                     CSIPRefresh* aRefresh) const
    {
    __TEST_INVARIANT;

    if (aType == SIPStrings::StringF(SipStrConsts::EInvite))
        {
        __SIP_ASSERT_LEAVE(!aRefresh, KErrArgument);
        return CSIPInviteClientTransaction::NewL(aAssoc);
        }

    return CSIPClientTransaction::NewL(aType, aAssoc, aRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::AddAssocL
// Needs aType as aAssoc.Type can't be used as aAssoc::iImplementation is NULL
// (CSIPDialogAssocImplementation::NewL hasn't returned yet). No invariant here.
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::AddAssocL(CSIPDialogAssocBase& aAssoc,
										 RStringF aType)
    {
    __ASSERT_ALWAYS(iDialogAssocs.Find(&aAssoc) == KErrNotFound &&
        			(aType != SIPStrings::StringF(SipStrConsts::EInvite) ||
         			!HasInviteAssoc()), User::Leave(KErrAlreadyExists));    
    iDialogAssocs.AppendL(&aAssoc);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::RemoveAssoc
// CDeleteMgr isn't used, as it'd be in CSIP, which is not always accessible as
// CSIPConnection and/or CSIP may've been deleted. Don't use invariant here.
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::RemoveAssoc(const CSIPDialogAssocBase& aAssoc)
    {
    //Returns KErrNotFound e.g. if CSIPDialogAssocImplementation::ConstructL
    //runs out of memory.
    TInt pos = iDialogAssocs.Find(&aAssoc);
    if (pos != KErrNotFound)
        {
	    iDialogAssocs.Remove(pos);

	    if (iDialogAssocs.Count() == 0)
	        {
	        delete this;
	        }
        }
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::HasInviteAssoc
// Don't use invariant here.
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::HasInviteAssoc() const
    {
    for (TInt i = 0; i < iDialogAssocs.Count(); ++i)
        {
        if (iDialogAssocs[i]->Type() ==
        	SIPStrings::StringF(SipStrConsts::EInvite))
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ConnectionDeleted
// As CSIPClientConnection is deleted, SIP client terminates all dialogs. So
// TerminateDialog is not called here. Clear iState, as CSIPDialogImplementation
// won't get noticed, when the CSIP owning the dialog states, is deleted.
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::ConnectionDeleted()
    {
    __TEST_INVARIANT;

    iState = NULL;
    iConnection = NULL;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CheckConnectionL
// Don't use invariant here.
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::CheckConnectionL() const
    {
    __ASSERT_ALWAYS(iConnection, User::Leave(KErrSIPResourceNotAvailable));
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::StateL
// -----------------------------------------------------------------------------
//
const CDialogState& CSIPDialogImplementation::StateL() const
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(iState != NULL, User::Leave(KErrSIPResourceNotAvailable));
    return *iState;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::UpdateState
// -----------------------------------------------------------------------------
//
void
CSIPDialogImplementation::UpdateState(const CSIPClientTransaction& aTransaction,
			                          const CDialogState& aEarly,
			                          const CDialogState& aConfirmed,
			                          const CDialogState& aTerminated)
    {
    __TEST_INVARIANT;

    const CSIPResponseElements* elem = aTransaction.ResponseElements();
    __SIP_ASSERT_RETURN(elem != NULL, KErrArgument);

	TUint status = elem->StatusCode();
    if (aTransaction.AffectsDialogState())
        {
        if (status >= 300)
            {
            ChangeState(&aTerminated);
            }
        else if (status >= 200)
            {
            ChangeState(&aConfirmed);
            }
        else
            {
            if (status > 100 && State() == CSIPDialog::EInit)
                {
                ChangeState(&aEarly);
                }
            }
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::CheckNoTransactionExistsL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::CheckNoTransactionExistsL() const
    {
    __TEST_INVARIANT;    
    __ASSERT_ALWAYS(!iInitialRequestId, User::Leave(KErrSIPInvalidDialogState));
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::InitialTransactionStarted
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::InitialTransactionStarted(TUint32 aRequestId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_RETURN(aRequestId, KErrArgument);
    __SIP_ASSERT_RETURN(!iInitialRequestId, KErrAlreadyExists);

	iInitialRequestId = aRequestId;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::InitialTransactionStarted
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::InitialTransactionStarted(
											CSIPTransactionBase& aTransaction)
    {
    __TEST_INVARIANT;

    InitialTransactionStarted(aTransaction.RequestId());
    aTransaction.SetAffectsDialogState();

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::DoesNotifyConfirmDialog
// -----------------------------------------------------------------------------
//
TBool CSIPDialogImplementation::DoesNotifyConfirmDialog() const
	{
	__TEST_INVARIANT;

	if (iInitialRequestId)
		{
		CSIPTransactionBase* ta = FindTransaction(iInitialRequestId);
		return (ta &&
				(ta->Type() == SIPStrings::StringF(SipStrConsts::ESubscribe) ||
				 ta->Type() == SIPStrings::StringF(SipStrConsts::ERefer)) &&
				ta->IsSIPClientTransaction());
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ChangeRefreshesToActive
// Adding a virtual function CSIPDialogAssocBase::ChangeRefreshToActive breaks
// BC, so an ugly way like this is used.
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::ChangeRefreshesToActive() const
	{
	__TEST_INVARIANT;

    for (TInt i = 0; i < iDialogAssocs.Count(); ++i)
        {
        if (iDialogAssocs[i]->Type() ==
        	SIPStrings::StringF(SipStrConsts::ESubscribe))
        	{
        	const CSIPRefresh* rfr = static_cast<CSIPSubscribeDialogAssoc*>
        		(iDialogAssocs[i])->SIPRefresh();
			if (rfr && rfr->State() == CSIPRefresh::EInactive)
				{
				//Get a modifiable pointer to the refresh
				CSIPRefresh* refresh =
					iDialogAssocs[i]->FindRefresh(rfr->RefreshId());
				__SIP_ASSERT_RETURN(refresh != NULL, KErrNotFound);
				refresh->ChangeState(CSIPRefresh::EActive);
				}
        	}
        }
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::UpdateRemoteTargetL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::UpdateRemoteTargetL(RStringF aMethod,
									 const CSIPMessageElements& aElements)
	{
	__TEST_INVARIANT;

	if (CSIPTransactionBase::IsTargetRefresh(aMethod))
		{
		CUri8* uri = GetUriFromContactL(aElements);
		if (uri)
			{
			delete iRemoteUri;
			iRemoteUri = uri;
			}
		}
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SetHeadersL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::SetHeadersL(CSIPFromHeader* aFrom,
			                               CSIPToHeader* aTo,
			                               CUri8* aRemoteUri,
			                               CSIPContactHeader* aContact)
    {
    __SIP_ASSERT_LEAVE(aRemoteUri, KErrArgument);
    __SIP_ASSERT_LEAVE(!iFrom && !iTo && !iRemoteUri && !iContact,
                       KErrAlreadyExists);
   	if (!aFrom)
        {
        __SIP_ASSERT_LEAVE(iRegistration != NULL, KErrNotFound);
        iFrom =
            iConnection->ClientConnectionL().AorL(iRegistration->ContextId());
        iFrom->DeleteParam(SIPStrings::StringF(SipStrConsts::ETag));
        }

	if (aTo)
		{
		iTo = aTo;
		}
	else
		{
		iTo = CSIPToHeader::DecodeL(aRemoteUri->Uri().UriDes());
		}

    if (aFrom)
        {
        iFrom = aFrom;
        }

	iRemoteUri = aRemoteUri;
    iContact = aContact;

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::InitialTransactionReceivedL
// Request must have exactly one Contact header. Dialog subsystem hasn't checked
// that as request came outside dialog. iRemoteUri is set with the Contact URI.
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::InitialTransactionReceivedL(
										CSIPServerTransaction& aTransaction)
    {
    __SIP_ASSERT_LEAVE(!iFrom && !iTo && !iRemoteUri, KErrAlreadyExists);

	const CSIPRequestElements* elem = aTransaction.RequestElements();
    __SIP_ASSERT_LEAVE(elem != NULL, KErrArgument);

	//From includes the remote tag, if remote filled it
	iFrom = CSIPFromHeader::NewL(*elem->FromHeader());
    iTo = CSIPToHeader::NewL(*elem->ToHeader());
	iRemoteUri = GetUriFromContactL(elem->MessageElements());
	InitialTransactionStarted(aTransaction);

	if (!iRemoteUri)
		{
		CSIPResponseElements* resp = CSIPResponseElements::NewLC(400,
        	SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
        aTransaction.SendResponseL(resp);
        CleanupStack::Pop(resp);
        User::Leave(KErrSIPMalformedMessage);
		}

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::SendDialogCreatingRequestL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPDialogImplementation::SendDialogCreatingRequestL(
									   CSIPDialogAssocImplementation& aAssoc,
									   CSIPMessageElements* aElements,
									   CSIPRefresh* aRefresh)
	{
	__TEST_INVARIANT;
	CheckConnectionL();

	CSIPClientTransaction* ta =
		CreateClientTransactionL(aAssoc.Type(), aAssoc, aRefresh);
    CleanupStack::PushL(ta);

    TUint32 requestId(0);
	TUint32 refreshId(0);
	iConnection->ClientConnectionL().SendRequestAndCreateDialogL(requestId,
                         refreshId,
					     iDialogId,
						 aAssoc.Type(),
						 *iRemoteUri,
						 iFrom,
						 aElements,
                     	 iTo,
                     	 RegistrationId(),
                     	 (aRefresh != NULL));
	ta->SetRequestId(requestId);
	InitialTransactionStarted(*ta);
	StoreCallIdL();
	FillLocalTagL(ETrue);
	CleanupStack::Pop(ta);

	if (aRefresh)
        {
        aRefresh->SetRefreshIdIfEmpty(refreshId);
        }
	delete aElements;

    //Contact can now be freed
    delete iContact;
    iContact = NULL;

	return ta;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::ContactHeader
// -----------------------------------------------------------------------------
//
const CSIPContactHeader* CSIPDialogImplementation::ContactHeader() const
	{
	__TEST_INVARIANT;
	return iContact;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::GetUriFromContactL
// Don't use invariant here.
// -----------------------------------------------------------------------------
//
CUri8* CSIPDialogImplementation::GetUriFromContactL(
								const CSIPMessageElements& aElements) const
	{
	CUri8* uri = NULL;
	
	RStringF contact = SIPStrings::StringF(SipStrConsts::EContactHeader);
	if (aElements.UserHeaderCount(contact) == 1)
		{
		RPointerArray<CSIPHeaderBase> headers = aElements.UserHeadersL(contact);
		CleanupClosePushL(headers);

		CSIPContactHeader* contactHeader =
			static_cast<CSIPContactHeader*>(headers[0]);

		const CSIPAddress* addr = contactHeader->SIPAddress();
		if (addr)
			{
			uri = CUri8::NewL(addr->Uri8().Uri());
			}
	    CleanupStack::PopAndDestroy(); //headers
		}
	return uri;
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FillLocalTagL
// -----------------------------------------------------------------------------
//
void CSIPDialogImplementation::FillLocalTagL(TBool aClientInitiatedDialog) const
	{
	__TEST_INVARIANT;
	__SIP_ASSERT_LEAVE(iDialogId != 0, KErrNotFound);

	RStringF tag = SIPStrings::StringF(SipStrConsts::ETag);
	CSIPFromToHeaderBase* header = iFrom;
	if (!aClientInitiatedDialog)
		{		
		header = iTo;
		}

	if (!header->HasParam(tag))
		{
		CheckConnectionL();
		RStringF localTag =
			iConnection->ClientConnectionL().LocalTagL(iDialogId);
		CleanupClosePushL(localTag);
		header->SetParamL(tag, localTag);
		CleanupStack::PopAndDestroy(); //localTag
		}	
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::FillRemoteTagL
// -----------------------------------------------------------------------------
//
void
CSIPDialogImplementation::FillRemoteTagL(const CSIPToHeader& aToHeader) const
	{
	__TEST_INVARIANT;

	RStringF tag = SIPStrings::StringF(SipStrConsts::ETag);
	if (aToHeader.HasParam(tag))
		{
		iTo->SetParamL(tag, aToHeader.ParamValue(tag));
		}
	}

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::RegistrationId
// -----------------------------------------------------------------------------
//
TUint32 CSIPDialogImplementation::RegistrationId() const
    {
    return (iRegistration ? iRegistration->ContextId() : 0);
    }

// -----------------------------------------------------------------------------
// CSIPDialogImplementation::__DbgTestInvariant
// iState is NULL if CSIP has been deleted.
// Don't check iRemoteUri, as 400 response is sent when missing iRemoteUri from
// an incoming request that creates a dialog.
// -----------------------------------------------------------------------------
//

void CSIPDialogImplementation::__DbgTestInvariant() const
	{
	if (!iDialog || !iFrom || !iTo)
		{
		User::Invariant();
		}
	}
