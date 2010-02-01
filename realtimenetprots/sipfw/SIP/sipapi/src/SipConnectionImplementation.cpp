// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SipConnectionImplementation.cpp
// Part of       : SIPAPI
// Version       : SIP/5.0
//



#include "SipAssert.h"
#include "SipConnectionImplementation.h"
#include "sipconnectionobserver.h"
#include "sipclientconnection.h"
#include "sip.h"
#include "SipImplementation.h"
#include "siperr.h"
#include "sipinternalerr.h"
#include "sipmessageelements.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "RegBindingImplementation.h"
#include "SipDialogImplementation.h"
#include "siprefresh.h"
#include "sipconnectioncallback.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::NewL
// -----------------------------------------------------------------------------
//
CSIPConnectionImplementation*
CSIPConnectionImplementation::NewL(CSIP& aSIP,
								   CSIPConnection& aConnection,
					 			   TUint32 aIapId,
					 			   MSIPConnectionObserver& aObserver)
	{
    CSIPConnectionImplementation* self =
    	new (ELeave) CSIPConnectionImplementation(aSIP,
    											  aConnection,
    											  aIapId,
    											  aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::CSIPConnectionImplementation
// -----------------------------------------------------------------------------
//
CSIPConnectionImplementation::CSIPConnectionImplementation(CSIP& aSIP,
							   CSIPConnection& aConnection,
                               TUint32 aIapId,
                               MSIPConnectionObserver& aObserver) :
    iSIP(&aSIP),
    iConnection(aConnection),
    iIapId(aIapId),
    iObserver(aObserver)
#ifdef CPPUNIT_TEST
    , iRegistrations(1),
    iDialogs(1),
    iTransactions(1),
    iRefreshes(1)
#endif
    {
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::ConstructL
// Pass "*this" to AddConnectionL which asks for CSIPConnection. Can't pass
// CSIPConnection, as AddConnectionL asks for CSIPConnection::iImplementation
// which is NULL, as execution is inside CSIPConnectionImplementation::NewL.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::ConstructL()
	{
	CheckCSipL();

    iCallbackInfo = CConnectionCallback::NewL();
    iClientConnection =
    	CSIPClientConnection::NewL(iSIP->Implementation().SIPClient(),
                                   *this,
                                   iIapId);	
	iSIP->Implementation().AddConnectionL(*this, iIapId);
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::~CSIPConnectionImplementation
// -----------------------------------------------------------------------------
//
CSIPConnectionImplementation::~CSIPConnectionImplementation()
	{	
    if (iSIP)
        {        
        iSIP->Implementation().RemoveConnection(iConnection);
        }    

    TInt i = 0;
    for (i = 0; i < iRegistrations.Count(); i++)
        {
        iRegistrations[i]->ConnectionDeleted();
        }
    iRegistrations.Reset();
    
    for (i = 0; i < iDialogs.Count(); i++)
        {
        iDialogs[i]->ConnectionDeleted();
        }
    iDialogs.Reset();

    for (i = 0; i < iTransactions.Count(); i++)
        {
        iTransactions[i]->Detach(*this);
        }
    iTransactions.Reset();

    for (i = 0; i < iRefreshes.Count(); i++)
        {
        iRefreshes[i]->RemoveRefreshOwner(*this);
        }    
    iRefreshes.Reset();

    delete iClientConnection;    
    delete iCallbackInfo;
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::State
// -----------------------------------------------------------------------------
//
CSIPConnection::TState CSIPConnectionImplementation::State() const
    {
    __TEST_INVARIANT;
    return iClientConnection->State();
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::SendRequestL
// No need to check connection state. It is checked in SIPClient.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPConnectionImplementation::SendRequestL(CSIPRequestElements* aElements,
			                               TUint32* aRegistrationId,
										   CSIPRefresh* aRefresh)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aElements &&
    	(aRegistrationId || (aElements->FromHeader() != NULL)),
    	User::Leave(KErrArgument));
	RStringF method = aElements->Method();
	__ASSERT_ALWAYS(method != SIPStrings::StringF(SipStrConsts::ERegister) &&
					method != SIPStrings::StringF(SipStrConsts::EInvite) &&
					method != SIPStrings::StringF(SipStrConsts::EPrack) &&
					method != SIPStrings::StringF(SipStrConsts::EAck) &&
					method != SIPStrings::StringF(SipStrConsts::ECancel) &&
					method != SIPStrings::StringF(SipStrConsts::EUpdate) &&
					method != SIPStrings::StringF(SipStrConsts::EBye) &&
					method != SIPStrings::StringF(SipStrConsts::ESubscribe) &&
					method != SIPStrings::StringF(SipStrConsts::ERefer) &&
					method != SIPStrings::StringF(SipStrConsts::ENotify),
					User::Leave(KErrArgument));
    CSIPClientTransaction* ta =
    	CSIPClientTransaction::NewLC(method, *this, aRefresh);
    TUint32 requestId(0);
    iClientConnection->SendRequestL(*aElements,
                                    requestId,
                                    aRegistrationId ? *aRegistrationId : 0,
                                    (aRefresh != NULL));
    if (aRefresh)
        {
        iRefreshes.AppendL(aRefresh);
        aRefresh->SetRequestType(method);
        aRefresh->SetRefreshOwner(*this);
        }
    CleanupStack::Pop(ta);
    ta->SetRequestId(requestId);
    delete aElements;

    __TEST_INVARIANT;
    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FetchRegistrationsL
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPConnectionImplementation::FetchRegistrationsL(
											CSIPRequestElements* aElements)
    {
    __TEST_INVARIANT;

	CSIPClientTransaction* ta = CSIPClientTransaction::NewL(
    							SIPStrings::StringF(SipStrConsts::ERegister),
                                *this);
	CleanupStack::PushL(ta);
	TUint32 requestId(0);
    iClientConnection->FetchRegistrationsL(*aElements, requestId);
    CleanupStack::Pop(ta);
	ta->SetRequestId(requestId);
    delete aElements;

    __TEST_INVARIANT;
    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingRequestL
// Invariant mustn't be used after callback, as observer may delete this object
// -----------------------------------------------------------------------------
//
void
CSIPConnectionImplementation::IncomingRequestL(CSIPRequestElements* aElements,
	                                           TUint32 aRequestId)									
    {
    __TEST_INVARIANT;
     __SIP_ASSERT_LEAVE(aElements, KErrArgument);

    iObserver.IncomingRequest(
    	CSIPServerTransaction::NewL(aRequestId, *this, aElements));
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingDialogRequestL
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingDialogRequestL(
											CSIPRequestElements* aElements,
		                                    TUint32 aRequestId,
								            TUint32 aDialogId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);

	CSIPDialogImplementation* dialog = FindDialog(aDialogId);
    if (dialog)
        {
	    CSIPServerTransaction* ta =
    		CSIPServerTransaction::NewLC(aRequestId, *this, aElements);
    	CleanupStack::PushL(TCleanupItem(
        	CSIPServerTransaction::DetachRequestElements, ta));

        TBool useCallback = dialog->IncomingRequestL(ta, *iCallbackInfo);
		CleanupStack::Pop(2); //TCleanupItem, ta

        if (useCallback)
            {         
            iCallbackInfo->ExecuteCallback(iObserver);            
            }
        }
    else
        {
        delete aElements;
        }    
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingResponseL
// An error response to REGISTER comes without registerId.
// This response is not related to any existing refresh or dialog.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingResponseL(
											CSIPResponseElements* aElements,
                                       		TUint32 aRequestId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    CSIPTransactionBase* ta = FindTransaction(aRequestId);
    if (ta)
        {
        __SIP_ASSERT_LEAVE(ta->IsSIPClientTransaction(), KErrArgument);

        CSIPClientTransaction* clientTa =
            static_cast<CSIPClientTransaction*>(ta);        
        clientTa->SetResponseElements(aElements);        

        iObserver.IncomingResponse(*clientTa);
        }
    else
        {        
        CRegBindingImplementation* reg = FindRegistration(aRequestId);
        if (reg)
            {
            if (reg->IncomingResponseL(aElements,
		                               aRequestId,
                                       0 /* empty registration id */,
                                       0 /* empty refresh id */,
					                   *iCallbackInfo))
                {
                iCallbackInfo->ExecuteCallback(iObserver);
                }
            }
        else
            {
            delete aElements;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingRegistrationResponseL
// Response to an existing refreshed registration, or a response creating a
// refreshed registration.
// If the response to the initial REGISTER is an error response, it comes in
// IncomingResponseL or IncomingRefreshResponseL.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingRegistrationResponseL(
											CSIPResponseElements* aElements,
											TUint32 aRequestId,
		                                    TUint32 aRefreshId,
								            TUint32 aRegistrationId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

	CRegBindingImplementation* reg = FindRegistration(aRegistrationId,
													  aRequestId);
    if (reg)
        {
        if (reg->IncomingResponseL(aElements,
		                           aRequestId,
                                   aRegistrationId,
                                   aRefreshId,
                                   *iCallbackInfo))
            {
            iCallbackInfo->ExecuteCallback(iObserver);            
            }
        }
    else
        {
        delete aElements;
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingRefreshResponseL
// Response creating a refresh or an error response to an existing refresh.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingRefreshResponseL(
											CSIPResponseElements* aElements,
		                               		TUint32 aRequestId,
							           		TUint32 aRefreshId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    CSIPRefresh* refresh = NULL;
    CSIPClientTransaction* ta = NULL;
    if (FindRefresh(aRequestId, aRefreshId, &refresh, &ta))
        {
		refresh->SetRefreshIdIfEmpty(aRefreshId);
	    refresh->UpdateRefreshState(aElements->StatusCode());

		TUint statusCode = aElements->StatusCode();
		if (ta)
			{
	       	ta->SetResponseElements(aElements);
	       	aElements = NULL; // Ownership of aElements was passed
			}
		if (statusCode >= 300)
			{
        	iObserver.ErrorOccured(KErrSIPTerminatedWithResponse, *refresh);
			}
		else
			{
			// Ignore the response if application has deleted the transaction
	        if (ta)
	            {
	        	iObserver.IncomingResponse(*ta);
	            }
			}
        }
    else
    	{
    	CRegBindingImplementation* reg = FindRegistration(aRequestId);
        if (reg)
            {
            if (reg->IncomingResponseL(aElements,
		                               aRequestId,
                                       0 /* empty registration id */,
                                       aRefreshId,
					                   *iCallbackInfo))
                {
                iCallbackInfo->ExecuteCallback(iObserver);
                }
            aElements = NULL; // Ownership of aElements was passed
            }
    	}

    delete aElements;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::ErrorOccured
// If the initial REGISTER failed, the error is related to a requestId of a
// registration transaction. RegisterId is received together with 2xx.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::ErrorOccured(TInt aError, TUint32 aRequestId)
    {
    __TEST_INVARIANT;

	CRegBindingImplementation* reg = FindRegistration(aRequestId);
    if (reg)
        {
        if (reg->ErrorOccured(aError, aRequestId, *iCallbackInfo))
            {
            iCallbackInfo->ExecuteCallback(iObserver);            
            }
        }
    else
        {
        CSIPTransactionBase* ta = FindTransaction(aRequestId);
        if (ta)
            {
            ta->ChangeState(CSIPTransactionBase::ETerminated);

            if (aError == KInviteTransactionCancelled)
            	{
            	__SIP_ASSERT_RETURN(!ta->IsSIPClientTransaction(),
            						KErrArgument);
            	iObserver.InviteCanceled(
            		static_cast<CSIPServerTransaction&>(*ta));
            	}
            else
            	{
            	iObserver.ErrorOccured(aError, *ta);
            	}
            }
        else
            {
            CSIPDialogImplementation* dialog = 
                FindDialogByRequestId(aRequestId);
            if (dialog &&
                dialog->ErrorOccured(aError, aRequestId, *iCallbackInfo))
                {
                iCallbackInfo->ExecuteCallback(iObserver);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::RefreshErrorOccured
// Error to a standalone refresh request, not to registration or dialog refresh.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::RefreshErrorOccured(TInt aError,
													   TUint32 aRefreshId)
    {
    __TEST_INVARIANT;

    TUint32 dummyRequestId(0);
    CSIPRefresh* refresh = NULL;
    CSIPClientTransaction* ta = NULL;
    if (FindRefresh(dummyRequestId, aRefreshId, &refresh, &ta))
        {
        if (ta)
            {
            ta->ChangeState(CSIPTransactionBase::ETerminated);
            }
        refresh->ChangeState(CSIPRefresh::ETerminated);

        iObserver.ErrorOccured(aError, *refresh);
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::RegistrationErrorOccured
// -----------------------------------------------------------------------------
//
void
CSIPConnectionImplementation::RegistrationErrorOccured(TInt aError,
                                  			  		   TUint32 aRegistrationId,
                                  			  		   TUint32 aRequestId)
    {
    __TEST_INVARIANT;

 	CRegBindingImplementation* reg = FindRegistration(aRegistrationId,
 													  aRequestId);
    if (reg && reg->ErrorOccured(aError, aRequestId, *iCallbackInfo))
        {
        iCallbackInfo->ExecuteCallback(iObserver);
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::DialogErrorOccured
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::DialogErrorOccured(TInt aError,
                                  					  TUint32 aDialogId,
                                  					  TUint32 aRequestId)
    {
    __TEST_INVARIANT;

    CSIPDialogImplementation* dialog = FindDialog(aDialogId);            
    if (dialog && dialog->ErrorOccured(aError, aRequestId, *iCallbackInfo))
        {
        iCallbackInfo->ExecuteCallback(iObserver);
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::DialogErrorOccured
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::DialogErrorOccured(TInt aError,
                                  	 				  TUint32 aDialogId,
                                  					  TUint32 aRefreshId,
                                  					  TUint32 aRequestId)
    {
    __TEST_INVARIANT;

    CSIPDialogImplementation* dialog = FindDialog(aDialogId);    
    if (dialog &&        
        dialog->ErrorOccured(aError, aRefreshId, aRequestId, *iCallbackInfo))
        {
        iCallbackInfo->ExecuteCallback(iObserver);
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::ConnectionStateChanged
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::ConnectionStateChanged(
											CSIPConnection::TState aState)
    {
    __TEST_INVARIANT;

    if (aState == CSIPConnection::EInactive ||
    	aState == CSIPConnection::EUnavailable)
        {
        ConnectionLost();
        }

    iObserver.ConnectionStateChanged(aState);
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::ConnectionLost
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::ConnectionLost() const
    {
    __TEST_INVARIANT;

    TInt i = 0;
    for (i = 0; i < iRegistrations.Count(); i++)
        {
        iRegistrations[i]->ConnectionLost();
        }

    for (i = 0; i < iDialogs.Count(); i++)
        {
        iDialogs[i]->ConnectionLost();
        }

    for (i = 0; i < iRefreshes.Count(); i++)
        {
        iRefreshes[i]->ChangeState(CSIPRefresh::ETerminated);
        }

    for (i = 0; i < iTransactions.Count(); i++)
        {        
        iTransactions[i]->ChangeState(CSIPTransactionBase::ETerminated);
        }    
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::InviteCompleted
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::InviteCompleted(TUint32 aRequestId)
    {
    __TEST_INVARIANT;

    CSIPDialogImplementation* dialog = FindDialogByRequestId(aRequestId);
    if (dialog)
        {
        CSIPClientTransaction* ta = static_cast<CSIPClientTransaction*>
            (dialog->FindTransaction(aRequestId));
        __SIP_ASSERT_RETURN(ta, KErrNotFound);

        ta->ChangeState(CSIPTransactionBase::ETerminated);
        iObserver.InviteCompleted(*ta);
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IapId
// -----------------------------------------------------------------------------
//
TUint32 CSIPConnectionImplementation::IapId() const
    {
    __TEST_INVARIANT;
    return iIapId;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::SIP
// -----------------------------------------------------------------------------
//
CSIP* CSIPConnectionImplementation::SIP()
	{
	__TEST_INVARIANT;
	return iSIP;
	}
 
// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::SIP
// -----------------------------------------------------------------------------
//      
const CSIP* CSIPConnectionImplementation::SIP() const
	{
	__TEST_INVARIANT;
	return iSIP;
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::SetOptL
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::SetOptL(TUint aOptionName,
				                      	   TUint aOptionLevel,
				                      	   const TDesC8& aOption)
	{
	__TEST_INVARIANT;
	return iClientConnection->SetOptL(aOptionName, aOptionLevel, aOption);
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::SetOptL
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::SetOptL(TUint aOptionName,
			                          	   TUint aOptionLevel,
			                          	   TInt aOption)
	{
	__TEST_INVARIANT;
	return iClientConnection->SetOptL(aOptionName, aOptionLevel, aOption);
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::AddRegistrationL
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::AddRegistrationL(
									CRegBindingImplementation& aRegistration)									
    {
    __TEST_INVARIANT;
    iRegistrations.AppendL(&aRegistration);
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::RemoveRegistration
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::RemoveRegistration(
							const CRegBindingImplementation& aRegistration)						
    {
    __TEST_INVARIANT;

    TInt pos = iRegistrations.Find(&aRegistration);
    if (pos != KErrNotFound)
        {
        iRegistrations.Remove(pos);
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::AddDialog
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::AddDialogL(CSIPDialogImplementation& aDialog)
    {
    __TEST_INVARIANT;
    iDialogs.AppendL(&aDialog);
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::RemoveDialog
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::RemoveDialog(
									const CSIPDialogImplementation& aDialog)
    {
    __TEST_INVARIANT;

	iClientConnection->TerminateDialog(aDialog.DialogId());

    TInt pos = iDialogs.Find(&aDialog);
    if (pos != KErrNotFound)
        {
        iDialogs.Remove(pos);
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindTransaction
// -----------------------------------------------------------------------------
//
CSIPTransactionBase* CSIPConnectionImplementation::FindTransaction(
    TUint32 aRequestId) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        if (iTransactions[i]->RequestId() == aRequestId)
            {
            return iTransactions[i];
            }   
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindClientTransaction
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPConnectionImplementation::FindClientTransaction(
    TUint32 aRequestId) const
    {
    __TEST_INVARIANT;
    
    CSIPTransactionBase* ta = FindTransaction(aRequestId);
    if (ta && ta->IsSIPClientTransaction())
        {
        return static_cast<CSIPClientTransaction*>(ta);
        }
    
    CRegBindingImplementation* registration = FindRegistration(aRequestId);
    if (registration)
        {
        return registration->FindTransaction(aRequestId);
        }
    
    CSIPDialogImplementation* dialog = FindDialogByRequestId(aRequestId);
    if (dialog)
        {
        ta = dialog->FindTransaction(aRequestId);
        if (ta && ta->IsSIPClientTransaction())
            {
            return static_cast<CSIPClientTransaction*>(ta);
            }
        }
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindRefresh
// -----------------------------------------------------------------------------
//
CSIPRefresh* CSIPConnectionImplementation::FindRefresh(
    TUint32 aRequestId,
    TUint32 aRefreshId) const
    {
    CSIPRefresh* refresh = NULL;
    
    CSIPClientTransaction* dummyTa = NULL;
    if (FindRefresh(aRequestId,aRefreshId,&refresh,&dummyTa))
        {
        return refresh;
        }
        
    for (TInt i = 0; i < iDialogs.Count(); i++)
        {
        refresh = iDialogs[i]->FindRefresh(aRequestId,aRefreshId);
        }
 
    for (TInt i = 0; !refresh && i < iRegistrations.Count(); i++)
        {
        CSIPRefresh* regRefresh = iRegistrations[i]->SIPRefresh();
        if (regRefresh)
           {
            if((regRefresh->RefreshId() == aRefreshId) ||
               (regRefresh->SIPTransaction() && 
               regRefresh->SIPTransaction()->RequestId() == aRequestId))
              {
               refresh = regRefresh;
              }
           }
        }
        
    return refresh;
    }    

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindRefresh
// -----------------------------------------------------------------------------
//
TBool CSIPConnectionImplementation::FindRefresh(TUint32 aRequestId,
	TUint32 aRefreshId,
	CSIPRefresh** aRefresh,
	CSIPClientTransaction** aTransaction) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iRefreshes.Count(); i++)
        {
        if (iRefreshes[i]->DoesMatch(aRefreshId))
            {
            *aRefresh = iRefreshes[i];
            //Transaction can be NULL
            *aTransaction = iRefreshes[i]->Transaction();
            return ETrue;
            }
        }

    CSIPTransactionBase* ta = FindTransaction(aRequestId);
    if (ta && ta->IsSIPClientTransaction())
        {
        CSIPClientTransaction* clientTa =
            static_cast<CSIPClientTransaction*>(ta);

        //Application may've deleted the refresh, even if transaction is found
        if (clientTa->Refresh())
            {
            *aTransaction = clientTa;
            *aRefresh = clientTa->Refresh();
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindRegistration
// First search by aRegistrationId. If not found, the RegistrationId hasn't yet
// been received from SIP client. Search again with aRequestId.
// -----------------------------------------------------------------------------
//
CRegBindingImplementation*
CSIPConnectionImplementation::FindRegistration(TUint32 aRegistrationId,
                                 			   TUint32 aRequestId) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iRegistrations.Count(); i++)
        {
        if (iRegistrations[i]->ContextId() == aRegistrationId)
            {        
            return iRegistrations[i];
            }
        }

    return FindRegistration(aRequestId);
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindRegistration
// -----------------------------------------------------------------------------
//
CRegBindingImplementation*
CSIPConnectionImplementation::FindRegistration(TUint32 aRequestId) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iRegistrations.Count(); i++)
        {
        if (iRegistrations[i]->FindTransaction(aRequestId))
            {
            return iRegistrations[i];
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindDialog
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation*
CSIPConnectionImplementation::FindDialog(TUint32 aDialogId) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iDialogs.Count(); i++)
        {
        if (iDialogs[i]->DialogId() == aDialogId)
            {        
            return iDialogs[i];
            }   
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::FindDialogByRequestId
// -----------------------------------------------------------------------------
//
CSIPDialogImplementation*
CSIPConnectionImplementation::FindDialogByRequestId(TUint32 aRequestId) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iDialogs.Count(); i++)
        {
        if (iDialogs[i]->FindTransaction(aRequestId))
            {        
            return iDialogs[i];
            }   
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::InitialRegisterStateL
// -----------------------------------------------------------------------------
//
CRegistrationState* CSIPConnectionImplementation::InitialRegisterStateL() const
    {
    __TEST_INVARIANT;
    CheckCSipL();

    return iSIP->Implementation().InitialRegisterState();
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::InitialDialogStateL
// -----------------------------------------------------------------------------
//
CDialogState* CSIPConnectionImplementation::InitialDialogStateL() const
    {
    __TEST_INVARIANT;
    CheckCSipL();

    return iSIP->Implementation().InitialDialogState();
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::CSIPDeleted
// Inform registrations and dialogs, as CSIP deletes their state handlers.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::CSIPDeleted()
    {
    __TEST_INVARIANT;

    iSIP = NULL;
     
    TInt i = 0;
    for (i = 0; i < iRegistrations.Count(); i++)
        {
        iRegistrations[i]->ChangeState(NULL);
        }
    
    for (i = 0; i < iDialogs.Count(); i++)
        {
        iDialogs[i]->ChangeState(NULL);        
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::CheckCSipL
// Don't call invariant as CheckCSipL is also used by ConstructL.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::CheckCSipL() const
    {
    __ASSERT_ALWAYS(iSIP, User::Leave(KErrSIPResourceNotAvailable));
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::ClientConnectionL
// -----------------------------------------------------------------------------
//
CSIPClientConnection& CSIPConnectionImplementation::ClientConnectionL()
    {
    __TEST_INVARIANT;
    return *iClientConnection;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::ClientConnection
// -----------------------------------------------------------------------------
//
CSIPClientConnection* CSIPConnectionImplementation::ClientConnection()
	{
	__TEST_INVARIANT;
	return iClientConnection;
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::SIPConnectionL
// -----------------------------------------------------------------------------
//
CSIPConnection& CSIPConnectionImplementation::SIPConnectionL()
    {
    __TEST_INVARIANT;    
    return iConnection;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::UpdateRefreshL
// No need to check connection state. It is checked in SIPClient.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPConnectionImplementation::UpdateRefreshL(CSIPRefresh& aRefresh,
                               				 CSIPMessageElements* aElements,
                               				 TBool aTerminate)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(iRefreshes.Find(&aRefresh) != KErrNotFound,
                    User::Leave(KErrNotFound));   

	CSIPClientTransaction* ta =
        CSIPClientTransaction::NewLC(aRefresh.RequestType(),                                    
                                     *this,
                                     &aRefresh);
    TUint32 requestId(0);
    if (aTerminate)
    	{
    	iClientConnection->SendRequestAndTerminateRefreshL(aRefresh.RefreshId(),
	                                                       requestId,
	                                                       aElements);
    	}
    else
    	{
    	iClientConnection->SendRequestAndUpdateRefreshL(aRefresh.RefreshId(),
                                                    	requestId,
                                                    	aElements);
    	}
	CleanupStack::Pop(ta);
	ta->SetRequestId(requestId);
    delete aElements;

    __TEST_INVARIANT;
    return ta;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::DeletingRefresh
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::DeletingRefresh(CSIPRefresh& aRefresh,
												   TUint32 aRefreshId)
    {
    __TEST_INVARIANT;

    TInt pos = iRefreshes.Find(&aRefresh);
    if (pos != KErrNotFound)
        {
        iRefreshes.Remove(pos);
        }

    iClientConnection->TerminateRefresh(aRefreshId);

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::TransactionAssociation
// -----------------------------------------------------------------------------
//
MTransactionAssociation& CSIPConnectionImplementation::TransactionAssociation()
	{
	__TEST_INVARIANT;
	return *this;
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::CheckIfStandAlone
// -----------------------------------------------------------------------------
//
TInt CSIPConnectionImplementation::CheckIfStandAlone()
	{
	__TEST_INVARIANT;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::AddTransactionL
// -----------------------------------------------------------------------------
//
void
CSIPConnectionImplementation::AddTransactionL(CSIPTransactionBase& aTransaction)
    {
    __TEST_INVARIANT;

    iTransactions.AppendL(&aTransaction);

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::RemoveTransaction
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::RemoveTransaction(
									const CSIPTransactionBase& aTransaction)
    {
    __TEST_INVARIANT;

    TInt pos = iTransactions.Find(&aTransaction);
    if (pos != KErrNotFound)
        {
        iTransactions.Remove(pos);
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingDialogResponseL
// Response in a dialog or a one that creates a dialog has been received.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingDialogResponseL(
											CSIPResponseElements* aElements,
		                                    TUint32 aRequestId,
								            TUint32 aDialogId)
    {
    __TEST_INVARIANT;

    IncomingResponseToDialogL(aElements, aRequestId, aDialogId, NULL); 
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingDialogResponseL
// Refresh related response in a dialog or a one that creates a dialog.
// If error response to an existing refresh, aRequestId is the transactionId of
// a transaction initiated by Refresh subsystem, and not recognized by SIP API.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingDialogResponseL(
											CSIPResponseElements* aElements,
		                                    TUint32 aRequestId,
                                            TUint32 aRefreshId,
								            TUint32 aDialogId)
    {
    __TEST_INVARIANT;

    IncomingResponseToDialogL(aElements, aRequestId, aDialogId, &aRefreshId);
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::IncomingResponseToDialogL
// If forked response, aDialogId won't find the CSIPDialogImplementation, so
// aRequestId is also used for searching.
// -----------------------------------------------------------------------------
//
void CSIPConnectionImplementation::IncomingResponseToDialogL(
											CSIPResponseElements* aElements,
		                                    TUint32 aRequestId,
                                            TUint32 aDialogId,
                                            TUint32* aRefreshId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements != NULL, KErrArgument);

    CSIPDialogImplementation* dialog = FindDialog(aDialogId);
    if (!dialog)
    	{
    	dialog = FindDialogByRequestId(aRequestId);
    	}

    if (dialog)
        {
        TBool useCallback = EFalse;
        if (aRefreshId)
            {
            useCallback = dialog->IncomingResponseL(aElements,
		                                            aRequestId,
                                                    *aRefreshId,
                                                    aDialogId,
                                                    *iCallbackInfo);
            }
        else
            {
            useCallback = dialog->IncomingResponseL(aElements,
		                                            aRequestId,
                                                    aDialogId,
                                                    *iCallbackInfo);
            }
        if (useCallback)
            {
            iCallbackInfo->ExecuteCallback(iObserver);
            }
        }
    else
        {
        delete aElements;
        }
    }

// -----------------------------------------------------------------------------
// CSIPConnectionImplementation::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPConnectionImplementation::__DbgTestInvariant() const
	{
    if (!iClientConnection)
		{	
		User::Invariant();
		}
	}
