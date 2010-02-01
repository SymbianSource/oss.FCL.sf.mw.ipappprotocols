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
* Name        : CSIPRegEventHandler.h
* Part of     : SIP Profile Agent
* Interface   : 
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef	CSIPREGEVENTHANDLER_H
#define CSIPREGEVENTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <uri8.h>
#include <stringpool.h>
#include "sipregeventsubscriberobserver.h"
#include "sipregeventobserver.h"
#include "sipdialog.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "_sipcodecdefs.h"
#include <sipprofileagentextensionparams.h>
// FORWARD DECLARATIONS
class CSIPConnection;
class MSIPRegistrationContext;
class CSIPNotifyXmlBodyParser;
class CSIPRegEventSubscriber;
class CSIPRegistrationElement;
class CSIPContactElement;

// CLASS DECLARATION
//
class CSIPRegEventHandler : public CBase, 
                            public MSIPRegEventSubscriberObserver
	{
    public: // Constructors and destructor
	
		/**
		* Creates a new instance of CSIPRegEventHandler.
		*
		* @pre aConnection::State() == CSIPConnection::EActive
		* @pre aContext.IsContextActive() == ETrue
		*
		* @param aXMLParser a XML parser for NOTIFY bodies
		* @param aLocalIP the local IP address of the related CSIPConnection
		* @param aDeltaTimer an object used for ordering timer events
		* @param aConnection an active SIP connection
		* @param aContext an active context
		* @param aUserIdentity a URI received in the topmost 
		*        P-Associated-URI-header in the 200 OK for the registration.
		* @param a observer for the network initiated re-/de-registration.
		* @return a new instance of CSIPRegEventHandler. 
		*         The ownership is transferred.
		*/	
	    static CSIPRegEventHandler* NewL(CSIPNotifyXmlBodyParser& aXMLParser,
	                                     const TDesC8& aLocalIP,
	                                     CDeltaTimer& aDeltaTimer,
	                                     CSIPConnection& aConnection,
	                                     MSIPRegistrationContext& aContext,
	                                     CUri8& aUserIdentity,
	                                     MSIPRegEventObserver& aObserver,
	                                     CSipProfileAgentConfigExtension& aConfigExtension);

		/**
		* Creates a new instance of CSIPRegEventHandler and 
		* puts it to CleanupStack.
		*
		* @pre aConnection::State() == CSIPConnection::EActive
		* @pre aContext.IsContextActive() == ETrue
		*
		* @param aXMLParser a XML parser for NOTIFY bodies
		* @param aLocalIP the local IP address of the related CSIPConnection
		* @param aDeltaTimer an object used for ordering timer events
		* @param aConnection an active SIP connection
		* @param aContext an active context
		* @param aUserIdentity a URI received in the topmost 
		*        P-Associated-URI-header in the 200 OK for the registration.
		* @param a observer for the network initiated re-/de-registration.
		* @return a new instance of CSIPRegEventHandler. 
		*         The ownership is transferred.
		*/	                                    
	    static CSIPRegEventHandler* NewLC(CSIPNotifyXmlBodyParser& aXMLParser,
	                                      const TDesC8& aLocalIP,
	                                      CDeltaTimer& aDeltaTimer,
	                                      CSIPConnection& aConnection,
	                                      MSIPRegistrationContext& aContext,
	                                      CUri8& aUserIdentity,
	                                      MSIPRegEventObserver& aObserver,
	                                      CSipProfileAgentConfigExtension& aConfigExtension);
	                                      
		/**
		* Destructor.   
		*/
	    ~CSIPRegEventHandler();	
	
	public: // New functions
	
		/**
		* Sends the SUBCRIBE for the registration state event to the network.   
		*/	
	    void SubscribeL();
	   
		/**
		* Refreshes the subscription for the registration state event.
		* In other words send an immediate updated SUBSRIBE to the network.   
		*/	    
	    void RefreshL();	
	
		/**
		* Checks whether this handler owns the transaction given. 
		*
		* @param aTransaction the transaction to be checked
		* @return ETrue if this handler owns the transaction. Otherwise EFalse.
		*/	    
	    TBool HasTransaction(const CSIPTransactionBase& aTransaction) const;	

		/**
		* Checks whether this handler owns the refresh given. 
		*
		* @param aRefresh the refresh to be checked
		* @return ETrue if this handler owns the refresh. Otherwise EFalse.
		*/	    
	    TBool HasRefresh(const CSIPRefresh& aRefresh) const;
	
		/**
		* Checks whether this handler has created the dialog given. 
		*
		* @param aDialog the dialog to be checked
		* @return ETrue if this handler owns the dialog. Otherwise EFalse. 
		*/
	    TBool HasDialog(const CSIPDialog& aDialog) const;
	
		/**
		* A response has been received from the network.
		* In practice this should be a response for 
		* the SUBSCRIBE that was sent for the registration state event.
		*
		* @pre HasTransaction(aTransaction) == ETrue
		*		
		* @param aTransaction an existing transaction
		*/					           
        void ResponseReceivedL(CSIPClientTransaction& aTransaction);	
	
		/**
		* A request within a dialog has been received from the network.
		* In practice this should be a NOTIFY for the registration state event.
		*
		* @pre HasDialog(aDialog) == ETrue
		*		
		* @param aTransaction a new transaction. The ownership is transferred. 
		* @param aDialog an existing dialog related to the transaction 
		*/	
	    void RequestReceivedL(CSIPServerTransaction* aTransaction,
					          CSIPDialog& aDialog);
	
		/**
		* An error has occured related to an existing transaction.
		* This can be an error related to a SUBSCRIBE send to network or
		* a response sent for a NOTIFY.
		*
		* @pre HasTransaction(aTransaction) == ETrue
		*		
		* @param aError the error
		* @param aTransaction an existing transaction
		*/			            
        void ErrorOccured(TInt aError,
				          CSIPTransactionBase& aTransaction);

		/**
		* An error has occured related to an existing transaction.
		* In practice this error is related 
		* to a refreshed SUBSCRIBE send to network.
		*
		* @pre HasDialog(aDialog) == ETrue
		*		
		* @param aError the error
		* @param aDialog an existing dialog
		*/				          
        void ErrorOccured(TInt aError,
				          CSIPDialog& aDialog);				          
				          
		
    public: // From MSIPRegEventSubscriberObserver

        void SubscriptionFailedL();
        
        void SubscriptionFailedL(TInt aRetryAfter);
		
        void RegEventNotSupportedByNetworkL();
        
        void ReRegister();
    
        void NotifyReceivedL(CSIPRegInfoElement& aNotifyData);
        
        void TerminatedL(CSIPRegInfoElement& aNotifyData,TInt aRetryAfter);		
				         
	public: // A callback for CDeltaTimer
	
	    static TInt ReSubscribeTimerExpired(TAny* aPtr);

    private: // New functions

        void ReSubscribeAfterL(TInt aRetryAfter);        

        void ReSubscribeL();
        
        void HandleRegInfoL(CSIPRegInfoElement& aElement,
                            TBool& aTerminated);
        
        void HandleRegistrationElementL(CSIPRegistrationElement& aElement,
                                        TBool& aTerminated,
                                        TBool& aLocalIPFound);
        
        TBool AllLocalContactsTerminated(CSIPRegInfoElement& aNotifyData) const;
        
        TBool HasLocalIP(const CSIPContactElement& aContact) const;

    private: // Second phase constructors

	    void ConstructL(const TDesC8& aLocalIP,
	                    CUri8& aUserIdentity,
	                    CSipProfileAgentConfigExtension& aConfigExtension);
	
	    CSIPRegEventHandler(CSIPNotifyXmlBodyParser& aXMLParser,
	                        CDeltaTimer& aDeltaTimer,
	                        CSIPConnection& aConnection,
	                        MSIPRegistrationContext& aContext,
	                        MSIPRegEventObserver& aObserver);

    private: // Data
	
	    CSIPNotifyXmlBodyParser& iXMLParser;
	    CDeltaTimer& iDeltaTimer;
	    CSIPConnection& iConnection;
	    MSIPRegistrationContext& iRegistrationContext;
	    MSIPRegEventObserver& iObserver;
	    HBufC8* iLocalIP;
	    CUri8* iUserId;
	    CSIPRegEventSubscriber* iSubscriber;
	    TCallBack iDeltaTimerCallBack;
	    TDeltaTimerEntry iDeltaTimerEntry;
	    TUint iRegEventVersion;
	    TBool iFirstNotifyProcessed;
	    // String constants
	    RString iStrFull;
	    RString iStrActive;
	    RString iStrShortened;
	    RString iStrRejected;
	    RString iStrTerminated;
	    RString iStrDeactivated;
	    RString iStrUnregistered;
	    RString iStrProbation;
	    RString iStrExpired;
	    
	private: // Unit test
	
	    UNIT_TEST(CSIPRegEventHandlerTest)
	    UNIT_TEST(CSIPIMSProfileContextTest)
	};

#endif // CSIPREGEVENTHANDLER_H
