/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : CSIPRegEventSubscriber.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef	CSIPREGEVENTSUBSCRIBER_H
#define CSIPREGEVENTSUBSCRIBER_H

//  INCLUDES
#include <e32base.h>
#include <uri8.h>
#include <stringpool.h>
#include "sipregeventcontext.h"
#include "TSIPRegEventSubscriptionStateBase.h"
#include "sipregeventsubscriberobserver.h"
#include "sipconnection.h"
#include "sipdialog.h"
#include "sipsubscribedialogassoc.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "sipmessageelements.h"
#include "siprequestelements.h"
#include "sipheaderbase.h"
#include "sipreginfoelement.h"
#include "_sipcodecdefs.h"
#include <sipprofileagentextensionparams.h>

// FORWARD DECLARATIONS
class CSIPSubscriptionStateHeader;
class CSIPNotifyXmlBodyParser;

// CLASS DECLARATION
//
class CSIPRegEventSubscriber : public CBase, public MSIPRegEventContext
	{
    public: // Constructors and destructor
	
	    static CSIPRegEventSubscriber* NewL(CSIPNotifyXmlBodyParser& aXmlParser,
	                                        CSIPConnection& aConnection,
	                                        MSIPRegistrationContext& aContext,
	                                        CUri8& aUserIdentity,
	                                        MSIPRegEventSubscriberObserver& aObserver,
	                                        CSipProfileAgentConfigExtension& aConfigExtension);
	                                    
	    static CSIPRegEventSubscriber* NewLC(CSIPNotifyXmlBodyParser& aXmlParser,
	                                         CSIPConnection& aConnection,
	                                         MSIPRegistrationContext& aContext,
	                                         CUri8& aUserIdentity,
	                                         MSIPRegEventSubscriberObserver& aObserver,
	                                         CSipProfileAgentConfigExtension& aConfigExtension);
	    ~CSIPRegEventSubscriber();		
	
	public: // New functions
	
	    void SubscribeL();
	    
	    void ReSubscribeL();
	    
	    void RefreshL();	

        TBool HasTransaction(const CSIPTransactionBase& aTransaction) const;

        TBool HasRefresh(const CSIPRefresh& aRefresh) const;

	    TBool HasDialog(const CSIPDialog& aDialog) const;
	
	    void RequestReceivedL(CSIPServerTransaction* aTransaction,
					          CSIPDialog& aDialog);
					           
        void ResponseReceivedL(CSIPClientTransaction& aTransaction);
					            
        void ErrorOccured(TInt aError,
				          CSIPTransactionBase& aTransaction);
				          
        void ErrorOccured(TInt aError,
				          CSIPDialog& aDialog);
				          			          

	public: // From MSIPRegEventContext
	
        void DoSubscribeL();
        
        void DoRefreshL();
        
        void ReSubscribeAfterL(TUint aRetryAfter);
        
        void SubscriptionFailedL();
        
        void RegEventNotSupportedByNetworkL();
        
        void ReRegister();
        
        void ChangeState(TState aNewState);


    private: // New functions
    
	    CSIPResponseElements* 
	        HandleReceivedRequestLC(const CSIPRequestElements& aRequest);
					           
        void HandleReceivedResponseL(const CSIPResponseElements& aResponse);
        
        TUint RetryAfterValue(const CSIPResponseElements& aResponse) const; 
    
        void ParseXmlL(const CSIPRequestElements& aRequest);
    
        void HandleSubscriptionStateL(const CSIPRequestElements& aRequest,
                                      TBool& aTerminated,
                                      TInt& aRetryAfter);
        
        void HandleTerminatedState(CSIPSubscriptionStateHeader& aState,
                                   TInt& aRetryAfter);
        
        CSIPSubscribeDialogAssoc* CreateSubscribeAssocL();
        
        CSIPMessageElements* CreateMessageElementsLC(TUint aExpiresValue);
    
        TBool MethodOK(const CSIPRequestElements& aRequest);
        
        TBool ContentTypeOK(const CSIPRequestElements& aRequest);
        
        TBool SubscriptionStateOK(const CSIPRequestElements& aRequest);
        
        TBool EventOK(const CSIPRequestElements& aRequest);
        
        CSIPHeaderBase* FindHeader(const CSIPMessageElements& aMessage,
                                   RStringF aHeaderName) const;
                                   
        inline TSIPRegEventSubscriptionStateBase& CurrentState();

    private: // Second phase constructors

	    void ConstructL();
	                    
	    void InitializeStatesL();
	
	    CSIPRegEventSubscriber(CSIPNotifyXmlBodyParser& aXmlParser,
	                           CSIPConnection& aConnection,
	                           MSIPRegistrationContext& aContext,
	                           CUri8& aUserIdentity,
	                           MSIPRegEventSubscriberObserver& aObserver,
	                           CSipProfileAgentConfigExtension& aConfigExtension);

    private: // Data
	
	    CArrayVarFlat<TSIPRegEventSubscriptionStateBase> iStates;
	    MSIPRegEventContext::TState iCurrentState;
	    CSIPNotifyXmlBodyParser& iXmlParser;
	    CSIPConnection& iConnection;
	    MSIPRegistrationContext& iRegistrationContext;
	    CUri8& iUserIdentity;
	    MSIPRegEventSubscriberObserver& iObserver;
	    CSIPSubscribeDialogAssoc* iSubscribeAssoc;
	    CSIPClientTransaction* iClientTransaction;
	    RStringF iReasonProbation;
	    RStringF iReasonRejected;
	    RStringF iReasonGiveup;
	    RStringF iReasonNoresource;
	    CSIPRegInfoElement* iCurrentRegInfoElement;
	    CSipProfileAgentConfigExtension& iConfigExtension;
	    
	private: // Unit test
	
	    UNIT_TEST(CSIPRegEventSubscriberTest)
	    UNIT_TEST(CSIPRegEventHandlerTest)
	    UNIT_TEST(CSIPIMSProfileContextTest)
	};

#endif // CSIPREGEVENTSUBSCRIBER_H
