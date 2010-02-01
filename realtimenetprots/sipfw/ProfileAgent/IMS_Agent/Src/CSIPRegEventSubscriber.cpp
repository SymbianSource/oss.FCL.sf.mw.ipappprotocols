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
// Name        : CSIPRegEventSubscriber.cpp
// Part of     : SIP Profile Agent
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include "CSIPRegEventSubscriber.h"
#include "CSIPNotifyXmlBodyParser.h"
#include "sipreginfoelement.h"
#include "TSIPRegEventStateNotSubscribed.h"
#include "TSIPRegEventStateSubscribing.h"
#include "TSIPRegEventStateSubscribed.h"
#include "TSIPRegEventStateReSubscribing.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipeventheader.h"
#include "siprefresh.h"
#include "sipmessageelements.h"
#include "sipexpiresheader.h"
#include "sipacceptheader.h"
#include "sipcontenttypeheader.h"
#include "sipsubscriptionstateheader.h"
#include "sipretryafterheader.h"
#include "sipresponseelements.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include <sipprofileagentextensionparams.h>


_LIT8(KRegEventName, "reg");
_LIT8(KAcceptType, "application");
_LIT8(KAcceptSubtype, "reginfo+xml");
_LIT8(KStateActive, "active");
_LIT8(KStatePending, "pending");
_LIT8(KStateTerminated, "terminated");
_LIT8(KReasonProbation, "probation");
_LIT8(KReasonRejected, "rejected");
_LIT8(KReasonGiveup, "giveup");
_LIT8(KReasonNoresource, "noresource");


// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::NewL
// -----------------------------------------------------------------------------
//
CSIPRegEventSubscriber* CSIPRegEventSubscriber::NewL(
    CSIPNotifyXmlBodyParser& aXmlParser,
    CSIPConnection& aConnection,
    MSIPRegistrationContext& aContext,
    CUri8& aUserIdentity,
    MSIPRegEventSubscriberObserver& aObserver,
    CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPRegEventSubscriber* self = 
	    NewLC(aXmlParser,aConnection,aContext,aUserIdentity,aObserver,aConfigExtension);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegEventSubscriber* CSIPRegEventSubscriber::NewLC(
    CSIPNotifyXmlBodyParser& aXmlParser,
    CSIPConnection& aConnection,
    MSIPRegistrationContext& aContext,
    CUri8& aUserIdentity,
    MSIPRegEventSubscriberObserver& aObserver,
    CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPRegEventSubscriber* self = 
	    new(ELeave)CSIPRegEventSubscriber(
	        aXmlParser,aConnection,aContext,aUserIdentity,aObserver,aConfigExtension);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::CSIPRegEventSubscriber
// -----------------------------------------------------------------------------
//
CSIPRegEventSubscriber::CSIPRegEventSubscriber(
    CSIPNotifyXmlBodyParser& aXmlParser,
    CSIPConnection& aConnection,
    MSIPRegistrationContext& aContext,
    CUri8& aUserIdentity,
    MSIPRegEventSubscriberObserver& aObserver,
    CSipProfileAgentConfigExtension& aConfigExtension)
 : iStates(MSIPRegEventContext::EMaxStates),
   iCurrentState(MSIPRegEventContext::ENotSubscribed),
   iXmlParser(aXmlParser),
   iConnection(aConnection),
   iRegistrationContext(aContext),
   iUserIdentity(aUserIdentity),
   iObserver(aObserver),
   iConfigExtension(aConfigExtension)
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegEventSubscriber::ConstructL()
	{
	InitializeStatesL();
    iReasonProbation = SIPStrings::Pool().OpenFStringL(KReasonProbation);
    iReasonRejected = SIPStrings::Pool().OpenFStringL(KReasonRejected);
    iReasonGiveup = SIPStrings::Pool().OpenFStringL(KReasonGiveup);
    iReasonNoresource = SIPStrings::Pool().OpenFStringL(KReasonNoresource);
    iSubscribeAssoc = CreateSubscribeAssocL();
	}

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::InitializeStatesL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventSubscriber::InitializeStatesL()
    {
	iStates.AppendL(TSIPRegEventStateNotSubscribed(*this),
					sizeof(TSIPRegEventStateNotSubscribed));
					
	iStates.AppendL(TSIPRegEventStateSubscribing(*this),
					sizeof(TSIPRegEventStateSubscribing));					
					
	iStates.AppendL(TSIPRegEventStateSubscribed(*this),
					sizeof(TSIPRegEventStateSubscribed));
					
	iStates.AppendL(TSIPRegEventStateReSubscribing(*this),
					sizeof(TSIPRegEventStateReSubscribing));
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::~CSIPRegEventSubscriber
// -----------------------------------------------------------------------------
//
CSIPRegEventSubscriber::~CSIPRegEventSubscriber()
	{
	delete iSubscribeAssoc;
	delete iClientTransaction;
    iReasonProbation.Close();
    iReasonRejected.Close();
    iReasonGiveup.Close();
    iReasonNoresource.Close();
    delete iCurrentRegInfoElement;
	}
	
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::SubscribeL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventSubscriber::SubscribeL()
    {
    CurrentState().SubscribeL();
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::DoSubscribeL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventSubscriber::DoSubscribeL()
    {
    CSIPRefresh* refresh = CSIPRefresh::NewLC();
    CSIPMessageElements* message = CreateMessageElementsLC(
            iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EIms,
            CSipProfileAgentConfigExtension::EProfileSubscriptionValue));
    
    CSIPClientTransaction* transaction = 
        iSubscribeAssoc->SendSubscribeL(message,refresh);
    
    CleanupStack::Pop(message);
    CleanupStack::Pop(refresh);
    delete iClientTransaction;
    iClientTransaction = transaction;
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ReSubscribeL
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::ReSubscribeL()
    {
    CSIPSubscribeDialogAssoc* subscribeAssoc = CreateSubscribeAssocL();
    CleanupStack::PushL(subscribeAssoc);
    
    CSIPRefresh* refresh = CSIPRefresh::NewLC();
    CSIPMessageElements* message = CreateMessageElementsLC(
            iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EIms,
            CSipProfileAgentConfigExtension::EProfileSubscriptionValue));
    
    CSIPClientTransaction* transaction = 
        subscribeAssoc->SendSubscribeL(message,refresh);    

    CleanupStack::Pop(message);
    CleanupStack::Pop(refresh);
    delete iClientTransaction;
    iClientTransaction = transaction;
    
    CleanupStack::Pop(subscribeAssoc);
    delete iSubscribeAssoc;
    iSubscribeAssoc = subscribeAssoc;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::RefreshL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventSubscriber::RefreshL()
    {
    CurrentState().RefreshL();
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::DoRefreshL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventSubscriber::DoRefreshL()
    {
    CSIPMessageElements* message = CreateMessageElementsLC(
            iConfigExtension.ExpiryValueL(TSIPProfileTypeInfo::EIms,
            CSipProfileAgentConfigExtension::EProfileSubscriptionValue));
    
    CSIPClientTransaction* transaction = iSubscribeAssoc->UpdateL(message);
    
    CleanupStack::Pop(message);
    delete iClientTransaction;
    iClientTransaction = transaction;
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ReSubscribeAfterL
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::ReSubscribeAfterL(TUint aRetryAfter)
    {
    iObserver.SubscriptionFailedL(aRetryAfter);
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::SubscriptionFailedL
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::SubscriptionFailedL()
    {
    iObserver.SubscriptionFailedL();
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::RegEventNotSupportedByNetworkL
// -----------------------------------------------------------------------------
// 
void CSIPRegEventSubscriber::RegEventNotSupportedByNetworkL()
    {
    iObserver.RegEventNotSupportedByNetworkL();
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ReRegister
// -----------------------------------------------------------------------------
//   
void CSIPRegEventSubscriber::ReRegister()
    {
    iObserver.ReRegister();
    }
      
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ChangeState
// -----------------------------------------------------------------------------
//      
void CSIPRegEventSubscriber::ChangeState(TState aNewState)
    {
    iCurrentState = aNewState;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HasTransaction
// -----------------------------------------------------------------------------
// 	    
TBool CSIPRegEventSubscriber::HasTransaction(
    const CSIPTransactionBase& aTransaction) const
    {
    return (iClientTransaction && aTransaction == *iClientTransaction);
    }    

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HasRefresh
// -----------------------------------------------------------------------------
// 	    
TBool CSIPRegEventSubscriber::HasRefresh(
    const CSIPRefresh& aRefresh) const
    {
    return (*iSubscribeAssoc->SIPRefresh() == aRefresh);
    } 
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HasDialog
// -----------------------------------------------------------------------------
//    
TBool CSIPRegEventSubscriber::HasDialog(const CSIPDialog& aDialog) const
    {
    return (aDialog == iSubscribeAssoc->Dialog());
    }
    	
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::RequestReceivedL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventSubscriber::RequestReceivedL(
    CSIPServerTransaction* aTransaction,
    CSIPDialog& aDialog)
    {
    if (HasDialog(aDialog))
        {
        __ASSERT_ALWAYS(aTransaction, User::Leave(KErrCorrupt));
        __ASSERT_ALWAYS(aTransaction->RequestElements() != NULL, 
                        User::Leave(KErrCorrupt));
        const CSIPRequestElements& request = *(aTransaction->RequestElements());  
        CSIPResponseElements* response = HandleReceivedRequestLC(request);
        if (response->StatusCode() == 200)
            {
            TRAPD(err, ParseXmlL(request));
            if (err == KErrNone)
                {
                TBool terminated = EFalse;
                TInt retryAfter = 0;
                HandleSubscriptionStateL(request,terminated,retryAfter); 
                aTransaction->SendResponseL(response);
                CleanupStack::Pop(response);
                if (terminated)
                    {
                    iObserver.TerminatedL(*iCurrentRegInfoElement,retryAfter);
                    }
                else
                    {
                    iObserver.NotifyReceivedL(*iCurrentRegInfoElement);
                    } 
                }
            else if (err == KErrNoMemory)
                {
                User::Leave(err);
                }
            else
                {
                response->SetStatusCodeL(400);
                response->SetReasonPhraseL(
                    SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
                aTransaction->SendResponseL(response);
                CleanupStack::Pop(response);               
                } 
            }
        else
            {
            aTransaction->SendResponseL(response);
            CleanupStack::Pop(response);
            }
        delete aTransaction;
        }
    }
	
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ResponseReceivedL
// -----------------------------------------------------------------------------
//					           
void CSIPRegEventSubscriber::ResponseReceivedL(
    CSIPClientTransaction& aTransaction)
    {
    if (HasTransaction(aTransaction))
        {
        const CSIPResponseElements* response = aTransaction.ResponseElements();
        __ASSERT_ALWAYS(response != NULL, User::Leave(KErrCorrupt));        
        HandleReceivedResponseL(*response);
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ErrorOccured
// -----------------------------------------------------------------------------
//
void CSIPRegEventSubscriber::ErrorOccured(
    TInt /*aError*/,
    CSIPTransactionBase& aTransaction)
    {
    if (HasTransaction(aTransaction))
        {
        TRAP_IGNORE(CurrentState().ErrorOccuredL())
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ErrorOccured
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::ErrorOccured(TInt aError,
                                          CSIPDialog& aDialog)
    {
    if (HasDialog(aDialog))
        {
        const CSIPClientTransaction* transaction = 
            iSubscribeAssoc->SIPRefresh()->SIPTransaction();
        if (transaction && aError == KErrSIPTerminatedWithResponse)
            {
            TUint statusCode = transaction->ResponseElements()->StatusCode();
            TUint after = RetryAfterValue(*transaction->ResponseElements());
            TRAP_IGNORE(CurrentState().ResponseReceivedL(statusCode,after))
            }
        else
            {
            TRAP_IGNORE(CurrentState().ErrorOccuredL())
            }
        }
    }    
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HandleReceivedRequestLC
// -----------------------------------------------------------------------------
//
CSIPResponseElements* CSIPRegEventSubscriber::HandleReceivedRequestLC(
    const CSIPRequestElements& aRequest)
    {
    if (!MethodOK(aRequest) || 
        !ContentTypeOK(aRequest) ||
        !SubscriptionStateOK(aRequest))
        {
        return CSIPResponseElements::NewLC(400,
            SIPStrings::StringF(SipStrConsts::EPhraseBadRequest));
        }
        
    if (!EventOK(aRequest))
        {
        return CSIPResponseElements::NewLC(489,
            SIPStrings::StringF(SipStrConsts::EPhraseBadEvent));
        }

    return CSIPResponseElements::NewLC(200,
        SIPStrings::StringF(SipStrConsts::EPhraseOk));    
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HandleReceivedResponseL
// -----------------------------------------------------------------------------
//					           
void CSIPRegEventSubscriber::HandleReceivedResponseL(
    const CSIPResponseElements& aResponse)
    {
    TUint statusCode = aResponse.StatusCode();
    if (statusCode >= 200)
        {
        TUint retryAfter = RetryAfterValue(aResponse);
        delete iClientTransaction;
        iClientTransaction = NULL;
        CurrentState().ResponseReceivedL(statusCode,retryAfter);
        }    
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::RetryAfterValue
// -----------------------------------------------------------------------------
//    
TUint CSIPRegEventSubscriber::RetryAfterValue(
    const CSIPResponseElements& aResponse) const
    {
    TUint retryAfter = 0;
    CSIPRetryAfterHeader* retryAfterHeader = 
        static_cast<CSIPRetryAfterHeader*>(
            FindHeader(aResponse.MessageElements(),
                SIPStrings::StringF(SipStrConsts::ERetryAfterHeader)));
    if (retryAfterHeader)
        {
        retryAfter = retryAfterHeader->RetryAfter();
        }
    return retryAfter;
    }    
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ParseXmlL
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::ParseXmlL(const CSIPRequestElements& aRequest)
    {
    CSIPRegInfoElement* regInfoElement = CSIPRegInfoElement::NewLC();
    iXmlParser.ParseL(regInfoElement,aRequest.MessageElements().Content());
    CleanupStack::Pop(regInfoElement);
    delete iCurrentRegInfoElement;
    iCurrentRegInfoElement = regInfoElement;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HandleSubscriptionStateL
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::HandleSubscriptionStateL(
    const CSIPRequestElements& aRequest,
    TBool& aTerminated,
    TInt& aRetryAfter)
    {
    TBool terminated = EFalse;
    
    CSIPHeaderBase* header = 
        FindHeader(aRequest.MessageElements(),
            SIPStrings::StringF(SipStrConsts::ESubscriptionStateHeader));
            
    __ASSERT_ALWAYS(header != NULL, User::Leave(KErrCorrupt));        
            
    CSIPSubscriptionStateHeader* ssHeader = 
        static_cast<CSIPSubscriptionStateHeader*>(header);

    CSIPRefresh* refresh = 
        const_cast<CSIPRefresh*>(iSubscribeAssoc->SIPRefresh());
        
    TPtrC8 state(ssHeader->SubStateValue());
    
    if (state.CompareF(KStateActive) == 0 || 
        state.CompareF(KStatePending) == 0)
        {
        TInt expires = ssHeader->ExpiresParameter();
        if (expires > 0)
            {
            refresh->SetIntervalL(static_cast<TUint>(expires));   
            }
        }
    else if (state.CompareF(KStateTerminated) == 0)
        {
        HandleTerminatedState(*ssHeader, aRetryAfter);
        terminated = ETrue;
        }
    else // Unknown state
        {
        // Just refresh after one minute
        refresh->SetIntervalL(60);
        }
        
    aTerminated = terminated;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::HandleTerminatedState
// -----------------------------------------------------------------------------
//    
void CSIPRegEventSubscriber::HandleTerminatedState(
    CSIPSubscriptionStateHeader& aState,
    TInt& aRetryAfter)
    {
    TInt retryAfter = aState.RetryAfterParameter();
    
    RStringF reason = 
        aState.ParamValue(SIPStrings::StringF(SipStrConsts::EReason));
        
    if (reason == iReasonProbation ||
        reason == iReasonGiveup)
        {
        if (retryAfter < 0)
            {
            // re-SUBSCRIBE after one hour
            retryAfter = 3600;
            }
        }
    else if (reason == iReasonRejected ||
             reason == iReasonNoresource)
        {
        // do NOT re-SUBSRIBE
        retryAfter = -1;
        }
    else
        {
        //  deactivated, timeout or anything else: re-SUBSCRIBE immediately
        retryAfter = 0;
        }
        
    aRetryAfter = retryAfter;
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::CreateSubscribeAssocL
// -----------------------------------------------------------------------------
//   
CSIPSubscribeDialogAssoc* CSIPRegEventSubscriber::CreateSubscribeAssocL()
    {
	TPtrC8 uriDes(iUserIdentity.Uri().UriDes());
	CUri8* remoteUri = CUri8::NewLC(iUserIdentity.Uri());
	CSIPFromHeader* from = CSIPFromHeader::DecodeL(uriDes);
	CleanupStack::PushL(from);
	CSIPEventHeader* event = CSIPEventHeader::NewLC(KRegEventName);
    
    CSIPSubscribeDialogAssoc* subscribeAssoc = 
        CSIPSubscribeDialogAssoc::NewL(iConnection,remoteUri,
                                       iRegistrationContext,event,from);
        
    CleanupStack::Pop(event);
    CleanupStack::Pop(from);
    CleanupStack::Pop(remoteUri);
    
    return subscribeAssoc;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::CreateMessageElementsLC
// -----------------------------------------------------------------------------
//    
CSIPMessageElements* 
CSIPRegEventSubscriber::CreateMessageElementsLC(TUint aExpiresValue)
    {
    CSIPMessageElements* message = CSIPMessageElements::NewLC();
    RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
    // Expires
    CSIPExpiresHeader* expires = new(ELeave)CSIPExpiresHeader(aExpiresValue);
    CleanupStack::PushL(expires);
    headers.AppendL(expires);
    CleanupStack::Pop(expires);
    // Accept
    CSIPAcceptHeader* accept = 
        CSIPAcceptHeader::NewLC(KAcceptType,KAcceptSubtype);
    headers.AppendL(accept);
    CleanupStack::Pop(accept); 
    message->SetUserHeadersL(headers);
    CleanupStack::Pop(1); // headers
    headers.Close();
    return message;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::MethodOK
// -----------------------------------------------------------------------------
//    
TBool CSIPRegEventSubscriber::MethodOK(const CSIPRequestElements& aRequest)
    {
    return (aRequest.Method() == SIPStrings::StringF(SipStrConsts::ENotify));  
    }

// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::ContentTypeOK
// -----------------------------------------------------------------------------
//    
TBool CSIPRegEventSubscriber::ContentTypeOK(const CSIPRequestElements& aRequest)
    {
    const CSIPContentTypeHeader* contentType = 
        aRequest.MessageElements().ContentType();
    if (!contentType)
        {
        return EFalse;
        }
    if (contentType->MediaType().CompareF(KAcceptType) != 0)
        {
        return EFalse;
        }
    return (contentType->MediaSubtype().CompareF(KAcceptSubtype) == 0);
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::SubscriptionStateOK
// -----------------------------------------------------------------------------
//    
TBool CSIPRegEventSubscriber::SubscriptionStateOK(
    const CSIPRequestElements& aRequest)
    {
    CSIPHeaderBase* header = 
        FindHeader(aRequest.MessageElements(),
            SIPStrings::StringF(SipStrConsts::ESubscriptionStateHeader));
    return (header != NULL);
    }    
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::EventOK
// -----------------------------------------------------------------------------
//    
TBool CSIPRegEventSubscriber::EventOK(const CSIPRequestElements& aRequest)
    {
    CSIPHeaderBase* header = 
        FindHeader(aRequest.MessageElements(),
            SIPStrings::StringF(SipStrConsts::EEventHeader));
    if (!header)
        {
        return EFalse;
        }
    CSIPEventHeader* event = static_cast<CSIPEventHeader*>(header);
    return (event->EventPackage().CompareF(KRegEventName) == 0);
    }    
    
// -----------------------------------------------------------------------------
// CSIPRegEventSubscriber::FindHeader
// -----------------------------------------------------------------------------
// 
CSIPHeaderBase* CSIPRegEventSubscriber::FindHeader(
    const CSIPMessageElements& aMessage,
    RStringF aHeaderName) const
    {
    TBool found = EFalse;
    CSIPHeaderBase* header = NULL;
    const RPointerArray<CSIPHeaderBase>& headers = aMessage.UserHeaders();
    for (TInt i=0; i < headers.Count() && !found; i++)
        {
        header = headers[i];
        if (header->Name() == aHeaderName)
            {
            found = ETrue;
            }
        else
            {
            header = NULL;
            }
        }
    return header;
    }
    
// ----------------------------------------------------------------------------
// CSIPRegEventSubscriber::CurrentState
// ----------------------------------------------------------------------------
//
TSIPRegEventSubscriptionStateBase& CSIPRegEventSubscriber::CurrentState()
	{
	return iStates.At(iCurrentState);
	}
