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
// Name        : CSIPRegEventHandler.cpp
// Part of     : SIP Profile Agent
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include "CSIPRegEventHandler.h"
#include "CSIPRegEventSubscriber.h"
#include "CSIPNotifyXmlBodyParser.h"
#include "sipcontactelement.h"
#include "sipregistrationelement.h"

const TInt KMicroSecsInSec = 1000000;


// -----------------------------------------------------------------------------
// CSIPRegEventHandler::NewL
// -----------------------------------------------------------------------------
//
CSIPRegEventHandler* CSIPRegEventHandler::NewL(
    CSIPNotifyXmlBodyParser& aXMLParser,
    const TDesC8& aLocalIP,
    CDeltaTimer& aDeltaTimer,
    CSIPConnection& aConnection,
    MSIPRegistrationContext& aContext,
    CUri8& aUserIdentity,
    MSIPRegEventObserver& aObserver,
    CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPRegEventHandler* self = 
	    CSIPRegEventHandler::NewLC(aXMLParser,aLocalIP,aDeltaTimer,
	        aConnection,aContext,aUserIdentity,aObserver,aConfigExtension);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegEventHandler* CSIPRegEventHandler::NewLC(
    CSIPNotifyXmlBodyParser& aXMLParser,
    const TDesC8& aLocalIP,
    CDeltaTimer& aDeltaTimer,
    CSIPConnection& aConnection,
    MSIPRegistrationContext& aContext,
    CUri8& aUserIdentity,
    MSIPRegEventObserver& aObserver,
    CSipProfileAgentConfigExtension& aConfigExtension)
	{
	CSIPRegEventHandler* self = 
	    new(ELeave)CSIPRegEventHandler(aXMLParser,aDeltaTimer,aConnection,
	        aContext,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aLocalIP,aUserIdentity,aConfigExtension);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::CSIPRegEventHandler
// -----------------------------------------------------------------------------
//
CSIPRegEventHandler::CSIPRegEventHandler(
    CSIPNotifyXmlBodyParser& aXMLParser,
    CDeltaTimer& aDeltaTimer,
    CSIPConnection& aConnection,
    MSIPRegistrationContext& aContext,
    MSIPRegEventObserver& aObserver)
 : iXMLParser(aXMLParser),
   iDeltaTimer(aDeltaTimer),
   iConnection(aConnection),
   iRegistrationContext(aContext),
   iObserver(aObserver),
   iDeltaTimerCallBack(ReSubscribeTimerExpired,this),
   iFirstNotifyProcessed(EFalse)
	{
	iDeltaTimerEntry.Set(iDeltaTimerCallBack);
	}

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::ConstructL(const TDesC8& aLocalIP,
                                     CUri8& aUserIdentity,
                                     CSipProfileAgentConfigExtension& aConfigExtension)
	{
    iLocalIP = aLocalIP.AllocL();
	iUserId = CUri8::NewL(aUserIdentity.Uri());
    // String constants for XML
    RStringPool& stringPool = iXMLParser.StringPool();
    iStrFull = stringPool.OpenStringL(_L8("full"));
	iStrActive = stringPool.OpenStringL(_L8("active"));
    iStrTerminated = stringPool.OpenStringL(_L8("terminated"));
    iStrShortened = stringPool.OpenStringL(_L8("shortened"));
    iStrRejected = stringPool.OpenStringL(_L8("rejected"));
    iStrDeactivated = stringPool.OpenStringL(_L8("deactivated"));
    iStrUnregistered = stringPool.OpenStringL(_L8("unregistered"));
    iStrProbation = stringPool.OpenStringL(_L8("probation"));
    iStrExpired = stringPool.OpenStringL(_L8("expired"));
	// Subscriber
	iSubscriber = CSIPRegEventSubscriber::NewL(iXMLParser,
	                                           iConnection,
	                                           iRegistrationContext,
	                                           *iUserId,
	                                           *this,aConfigExtension);
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::~CSIPRegEventHandler
// -----------------------------------------------------------------------------
//
CSIPRegEventHandler::~CSIPRegEventHandler()
	{
	iDeltaTimer.Remove(iDeltaTimerEntry);
	delete iSubscriber;
	iStrFull.Close();
	iStrActive.Close();
	iStrTerminated.Close();
    iStrShortened.Close();
    iStrRejected.Close();
    iStrDeactivated.Close();
    iStrUnregistered.Close();
    iStrProbation.Close();
    iStrExpired.Close();
	delete iUserId;
	delete iLocalIP;
	}

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::SubscriptionFailedL
// From MSIPRegEventSubscriberObserver
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::SubscriptionFailedL()
    {
    iObserver.RegEventSubscriptionFailedL();
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::SubscriptionFailedL
// From MSIPRegEventSubscriberObserver
// -----------------------------------------------------------------------------
//    
void CSIPRegEventHandler::SubscriptionFailedL(TInt aRetryAfter)
    {
    ReSubscribeAfterL(aRetryAfter);
    iObserver.RegEventSubscriptionActive();
    }
		
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::RegEventNotSupportedByNetworkL
// From MSIPRegEventSubscriberObserver
// -----------------------------------------------------------------------------
//		
void CSIPRegEventHandler::RegEventNotSupportedByNetworkL()
    {
    delete iSubscriber;
    iSubscriber = NULL;
    // The observer does not have to know that subscription failed,
    // since in this case we are dealing with 
    // IMS implentation that does not support reg-event and 
    // the observer can consider the profile registered.
    iObserver.RegEventSubscriptionActive();  
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ReRegister
// From MSIPRegEventSubscriberObserver
// -----------------------------------------------------------------------------
//   
void CSIPRegEventHandler::ReRegister()
    {
    iObserver.ReRegister();
    }    
   
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::NotifyReceived
// From MSIPRegEventSubscriberObserver
// -----------------------------------------------------------------------------
//    
void CSIPRegEventHandler::NotifyReceivedL(CSIPRegInfoElement& aNotifyData)
    {
    TBool terminated = EFalse;
    if (iFirstNotifyProcessed)
        {
        if (aNotifyData.Version() == iRegEventVersion+1)
            {
            // process data
            HandleRegInfoL(aNotifyData,terminated);
            if (!terminated)
                {
                iRegEventVersion = aNotifyData.Version();
                }
            else
                {
                iRegEventVersion = 0;
                }
            }
        else if (aNotifyData.Version() > iRegEventVersion+1)
            {
            // discard data and refresh subscription
            iSubscriber->RefreshL();
            iRegEventVersion = 0;
            iFirstNotifyProcessed = EFalse;
            }            
        else  
            {
            // discard data: aNotifyData.Version() <= iRegEventVersion
            }
        }
    else
        {
        if (aNotifyData.State() != iStrFull)
            {
            iSubscriber->RefreshL();
            }
        else
            {
            HandleRegInfoL(aNotifyData,terminated);
            if (!terminated)
                {
                iRegEventVersion = aNotifyData.Version();
                iFirstNotifyProcessed = ETrue;
                iObserver.RegEventSubscriptionActive();              
                }
            }
        }
    }
        
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::TerminatedL
// From MSIPRegEventSubscriberObserver
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::TerminatedL(CSIPRegInfoElement& aNotifyData,
                                      TInt aRetryAfter)
    {
    if (aRetryAfter < 0)
        {
        delete iSubscriber;
        iSubscriber = NULL;  
        iObserver.RegistrationTerminated();
        }
    else
        {
        if (AllLocalContactsTerminated(aNotifyData))
            {
            delete iSubscriber;
            iSubscriber = NULL;  
            iObserver.RegistrationTerminated();            
            }
        else
            {
            ReSubscribeAfterL(aRetryAfter);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::SubscribeL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventHandler::SubscribeL()
    {
    if (iSubscriber)
        {
        iSubscriber->SubscribeL();
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::RefreshL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventHandler::RefreshL()
    {
    if (iSubscriber)
        {
        iSubscriber->RefreshL();
        }
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::HasTransaction
// -----------------------------------------------------------------------------
// 	    
TBool CSIPRegEventHandler::HasTransaction(
    const CSIPTransactionBase& aTransaction) const
    {
    if (iSubscriber)
        {    
        return iSubscriber->HasTransaction(aTransaction);
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::HasTransaction
// -----------------------------------------------------------------------------
//
TBool CSIPRegEventHandler::HasRefresh(const CSIPRefresh& aRefresh) const
    {
    if (iSubscriber)
        {    
        return iSubscriber->HasRefresh(aRefresh);
        }
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::HasDialog
// -----------------------------------------------------------------------------
//    
TBool CSIPRegEventHandler::HasDialog(const CSIPDialog& aDialog) const
    {
    if (iSubscriber)
        {
        return iSubscriber->HasDialog(aDialog);
        }
    return EFalse;
    }
    	
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::RequestReceivedL
// -----------------------------------------------------------------------------
//	
void CSIPRegEventHandler::RequestReceivedL(
    CSIPServerTransaction* aTransaction,
    CSIPDialog& aDialog)
    {
    if (iSubscriber)
        {    
        iSubscriber->RequestReceivedL(aTransaction,aDialog);
        }
    }
	
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ResponseReceivedL
// -----------------------------------------------------------------------------
//					           
void CSIPRegEventHandler::ResponseReceivedL(
    CSIPClientTransaction& aTransaction)
    {
    if (iSubscriber)
        {    
        iSubscriber->ResponseReceivedL(aTransaction);
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ErrorOccured
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::ErrorOccured(
    TInt aError,
    CSIPTransactionBase& aTransaction)
    {
    if (iSubscriber)
        {    
        iSubscriber->ErrorOccured(aError,aTransaction);
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ErrorOccured
// -----------------------------------------------------------------------------
//    
void CSIPRegEventHandler::ErrorOccured(TInt aError,
                                       CSIPDialog& aDialog)
    {
    if (iSubscriber)
        {    
        iSubscriber->ErrorOccured(aError,aDialog);
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ReSubscribeTimerExpired
// A callback for CDeltaTimer
// -----------------------------------------------------------------------------
//    
TInt CSIPRegEventHandler::ReSubscribeTimerExpired(TAny* aPtr)
    {
    CSIPRegEventHandler* self = reinterpret_cast<CSIPRegEventHandler*>(aPtr);
    TRAP_IGNORE(self->ReSubscribeL())
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ReSubscribeAfterL
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::ReSubscribeAfterL(TInt aRetryAfter)
    {
    if (aRetryAfter <= 0)
        {
        ReSubscribeL();
        }
    else
        {
        iDeltaTimer.Remove(iDeltaTimerEntry);
        TTimeIntervalMicroSeconds32 interval(aRetryAfter*KMicroSecsInSec);
        iDeltaTimer.Queue(interval,iDeltaTimerEntry);    
        }
    }
 
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::ReSubscribeL
// -----------------------------------------------------------------------------
//    
void CSIPRegEventHandler::ReSubscribeL()
    {
    if (iSubscriber)
        {
        iSubscriber->ReSubscribeL();
        iRegEventVersion = 0;
        iFirstNotifyProcessed = EFalse;        
        }                                    
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::HandleRegInfoL
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::HandleRegInfoL(
    CSIPRegInfoElement& aElement,
    TBool& aTerminated)
    {
    aTerminated = EFalse;
    TBool localIPFound = EFalse;
    RPointerArray<CSIPRegistrationElement>& regElements =
        aElement.RegistrationElements();
        
    for (TInt i=0; !aTerminated && i < regElements.Count(); i++)
        {
        CSIPRegistrationElement* element = regElements[i];
        if (element)
            {
            HandleRegistrationElementL(*element,aTerminated,localIPFound);
            }
        }
    if (!aTerminated && !localIPFound && aElement.State() == iStrFull)
        {
        aTerminated = ETrue;
        if (iFirstNotifyProcessed)
            {
            iObserver.ReRegister();
            }
        else
            {
            iObserver.RegEventSubscriptionFailedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPRegEventHandler::HandleRegistrationElementL
// -----------------------------------------------------------------------------
//
void CSIPRegEventHandler::HandleRegistrationElementL(
    CSIPRegistrationElement& aElement,
    TBool& aTerminated,
    TBool& aLocalIPFound)
    {
    aTerminated = EFalse;
    TUint expirationTime = KMaxTUint;
    TBool updateExpirationTime = EFalse;
    RString registrationState = aElement.State();
    RPointerArray<CSIPContactElement>& contacts = aElement.ContactElements();
    for (TInt i=0; !aTerminated && i < contacts.Count(); i++)
        {
        CSIPContactElement* contact = contacts[i];
        if (HasLocalIP(*contact))
            {
            aLocalIPFound = ETrue;
            RString event = contact->Event();
             // Network initiated RE-registration
            if (registrationState == iStrActive &&
                contact->State() == iStrActive &&
                event == iStrShortened &&
                contact->HasExpires())
                {
                updateExpirationTime = ETrue;
                if (expirationTime > contact->Expires())
                    {
                    expirationTime = contact->Expires();
                    }
                }
            // Network initiated DE-registration
            TInt compareErr = aElement.AOR().Uri().Equivalent(iUserId->Uri());
            if (compareErr == KErrNoMemory)
                {
                User::Leave(KErrNoMemory);
                }
            if (compareErr == KErrNone && contact->State() == iStrTerminated)
                {
                if (event == iStrRejected || 
                    event == iStrTerminated ||
                    event == iStrUnregistered ||
                    event == iStrProbation)
                    {
                    iObserver.RegistrationTerminated();
                    aTerminated = ETrue;
                    }           
                if (!aTerminated && 
                    (event == iStrDeactivated || event == iStrExpired))
                    {
                    iObserver.RegistrationDeactivated();
                    aTerminated = ETrue;
                    }
                }
            }
        }
    if (!aTerminated && updateExpirationTime)
        {
        iObserver.ExpirationTimeUpdatedL(expirationTime);
        }
    }
  
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::AllLocalContactsTerminated
// -----------------------------------------------------------------------------
//   
TBool CSIPRegEventHandler::AllLocalContactsTerminated(
    CSIPRegInfoElement& aNotifyData) const
    {
    TInt terminatedLocalContactCount = 0;
    TInt localContactCount = 0;
    
    RPointerArray<CSIPRegistrationElement>& regElements =
        aNotifyData.RegistrationElements();
        
    for (TInt i=0; i < regElements.Count(); i++)
        {
        RPointerArray<CSIPContactElement>& contacts = 
            regElements[i]->ContactElements();
        for (TInt j=0; j < contacts.Count(); j++)
            {
            CSIPContactElement* contact = contacts[j];
            if (contact && HasLocalIP(*contact))
                {
                localContactCount++;
                if (contact->State() == iStrTerminated)
                    {
                    terminatedLocalContactCount++;
                    }
                }
            }
        }
        
    if (localContactCount > 0 && 
        localContactCount == terminatedLocalContactCount)
        {
        return ETrue;
        }
        
    return EFalse;
    }
  
// -----------------------------------------------------------------------------
// CSIPRegEventHandler::HasLocalIP
// -----------------------------------------------------------------------------
//  
TBool CSIPRegEventHandler::HasLocalIP(const CSIPContactElement& aContact) const
    {
    TPtrC8 host(aContact.Uri8().Uri().Extract(EUriHost));
    return (host.CompareF(*iLocalIP) == 0);
    }
