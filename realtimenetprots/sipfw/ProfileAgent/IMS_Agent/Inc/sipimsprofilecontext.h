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
* Name        : sipimsprofilecontext.h
* Part of     : sip ims agent
* Interface   : Internal IF
* Version     : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPIMSPRROFILECONTEXT_H
#define CSIPIMSPRROFILECONTEXT_H

//INCLUDES 
#include <e32base.h>
#include <badesca.h>
#include <stringpool.h>
#include <uri8.h>
#include "sipprofilecontextbase.h"
#include "sipprofileagent.h"
#include "sipimsconnectioncontext.h"
#include "sipregeventobserver.h"
#include "sipprofileagentobserver.h"
#include "sipdialogassocbase.h"
#include <sipprofileagentextensionparams.h>

// FORWARD DECLARATIONS
class CSIPHeaderBase;
class CSIPSecurityClientHeader;
class CSIPRegEventHandler;
class CDeltaTimer;
class CSIPNotifyXmlBodyParser;
class CSIPProfileSIMRecord;

// CLASS DECLARATION
/**
*  Class for maintaining the information related
*  to a particular IMS type profile. Class stores
*  related registration, sip connection and transactions.
*
*  @lib n/a.
*/
class CSIPIMSProfileContext : public CSIPProfileContextBase,
				              public MSIPRegEventObserver,
				              public MSIPProfileAgentObserver
	{
	public:
		   
		enum TConfiguredType
			{
			/** IMS Release x */
			EIMSReleaseType=0,
			/** Early IMS*/
			EEarlyIMSType,
			/** IAPClient configured type */
			EClientConfiguredType
			};
		   
		   
        /**
        * Two-phased constructor.
        * @param aXMLParser a XML parser
	    * @param aSIP a handle to SIP server
		* @param aConnection a SIP connection
		* @param aInitState a init state
		* @param aProfile a profile
        */
		static CSIPIMSProfileContext* NewL(
		    CSIPNotifyXmlBodyParser& aXMLParser,
			CSIP& aSIP,
			CSIPIMSConnectionContext& aConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer,
			CSIPProfileSIMRecord& aSIMRecord,
			CSipProfileAgentConfigExtension& aConfigExtension);
        /**
        * Two-phased constructor.
        * @param aXMLParser a XML parser
	    * @param aSIP a handle to SIP server
		* @param aConnection a SIP connection
		* @param aInitState a init state
		* @param aProfile a profile
        */
		static CSIPIMSProfileContext* NewLC(
		    CSIPNotifyXmlBodyParser& aXMLParser,
			CSIP& aSIP,
			CSIPIMSConnectionContext& aConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer,
			CSIPProfileSIMRecord& aSIMRecord,
			CSipProfileAgentConfigExtension& aConfigExtension);

        /**
        * Destructor.
        */
		~CSIPIMSProfileContext();


    public: // From MSIPProfileContext

        MSIPProfileAgentObserver& AgentObserver() const;

		void DestroyRegistration();

        void CreateRegistrationL();

		CSIPMessageElements* CreateMsgElementsLC();
		
        CSIPMessageElements* CreateDeRegisterElementsL();
		
		void UpdateContactHeaderParamsL(CSIPConcreteProfile& aNewProfile);
				
		TBool RetryTimerInUse();

		TUint RetryAfterTime();

		TBool RetryAfterTimer();
		
		void ResolveProxyL();
				
		void CancelProxyResolving();
				
		TBool ProxiesAlreadyResolved();
		
	public: // From MSIPRegEventObserver
	
	    void RegEventSubscriptionActive();
	    
        void ExpirationTimeUpdatedL(TInt aExpirationTime);
        
        void ReRegister();
    
        void RegistrationDeactivated();
		
        void RegistrationTerminated();
     
        void RegEventSubscriptionFailedL();		
		
		
	public: // From MSIPProfileAgentObserver
	
		void SIPProfileStatusEvent(
		    CSIPConcreteProfile& aProfile,
		    TUint32 aContextId);

		void SIPProfileErrorEvent(
		    CSIPConcreteProfile& aProfile,
		    TInt aError);

		TBool ProceedRegistration(CSIPConcreteProfile& aProfile, TInt aError);		
		
		void GetFailedProfilesL(
		    const TSIPProfileTypeInfo& aType,
		    RPointerArray<CSIPConcreteProfile>& aFailedProfiles) const;
		
	public: // New functions

	    /**
	    * Deregisters SIP profile according to the profile type.
	    * @param aSIPConcreteProfile sip profile to deregister
	    */
	    void DeregisterL();

 		/**
		* Updates SIP profile. This can lead to profile de-registration
		* and registration. Function leaves with KErrArgument if profiles 
		* are the same.
		* @param aNewProfile sip profile to update
		*/
		void UpdateL(CSIPConcreteProfile& aNewProfile);

		/**
		* A SIP response creating a registration binding or an error response 
		* that is related to an refreshed registration binding  
		* has been received from the network.
		* @param aTransaction contains response elements.
		* The ownership is transferred.
		* @param aRegistration associated registration
		* @param aHandled, returned ETrue if response handled
		*/
		void IncomingResponse(CSIPClientTransaction& aTransaction,
		                      CSIPRegistrationBinding& aRegistration,
		                      TBool& aHandled);

		/**
		* A SIP request within a dialog has been received from the network.
		* The client must resolve the actual dialog association to which
		* this request belongs.
		* @param aTransaction SIP server transaction
		* The ownership is transferred.
		* @param aDialog the dialog  that
		*        this transaction belongs to.
		* @param aHandled, returned ETrue if request handled
 		*/
		void IncomingRequest(CSIPServerTransaction* aTransaction,
		                     CSIPDialog& aDialog,
		                     TBool& aHandled);

		/**
		* A SIP response received from the network.
		* @param aTransaction contains response elements.
		* The ownership is transferred.
		* @param aHandled, returned ETrue if response handled
		*/
		void IncomingResponse(CSIPClientTransaction& aTransaction,
							  TBool& aHandled);
							  
		/**
		* A SIP response that is within a dialog association or creates
		* a dialog association.
		* @param aTransaction contains response elements.
		* The ownership is transferred.
		* @param aDialogAssoc a dialog association
		* @param aHandled, returned ETrue if response handled
		*/
		void IncomingResponse(CSIPClientTransaction& aTransaction,
		                      CSIPDialogAssocBase& aDialogAssoc,
		                      TBool& aHandled);

		/**
		* An asynchronous error has occured related to a periodical refresh 
		* that belongs to SIP dialog association.
		* @param aDialogAssoc SIP dialog association
		* @param aError error code
		* @param aHandled, returned ETrue if error handled
		*/
		void ErrorOccured(CSIPDialogAssocBase& aDialogAssoc,
						  TInt aError,
						  TBool& aHandled);

		/**
		* An asynchronous error has occured related to a request within
		* an existing dialog.
		* @param aTransaction the failed transaction
		* @param aDialogAssoc the failed dialog associoation
		* @param aError error code
		* @param aHandled, returned ETrue if error handled
		*/
		void ErrorOccured(CSIPTransactionBase& aTransaction,
		                  CSIPDialogAssocBase& aDialogAssoc,
		                  TInt aError,
		                  TBool& aHandled);			                      
		/*
		* Checks whether HTTP Digest Settings are configured for the profile.
		* @param aProfile profile to be checked
		* @return ETrue if HTTP Digest settings are configured,
		*         otherwise return EFalse.
		*/
		static TBool IsHttpDigestSettingsConfigured(
		    const CSIPConcreteProfile& aProfile);

		static TBool IsSupportedSecurityMechanismL(CSIP& aSIP,
		                                           const TDesC8& aValue);

		void SetConfiguredType(TConfiguredType);
		
		CSIPIMSProfileContext::TConfiguredType ConfiguredType();
		
	private: // Second phase constructors

		CSIPIMSProfileContext(
		    CSIPNotifyXmlBodyParser& aXMLParser,
			CSIP& aSIP,
			CSIPIMSConnectionContext& iConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer,
			CSIPProfileSIMRecord& aSIMRecord,
			CSipProfileAgentConfigExtension& aConfigExtension);

        void ConstructL();


	private: // From CSIPProfileContextBase

		void ConnectionStateChangedImpl(CSIPConnection::TState aState);
		
		void SetRegisteredAORsL();
		
		TBool RetryRegister(CSIPClientTransaction* aTransaction,
							TInt aError);
							  
		TBool ShouldRetryRegistration(TInt aError);
		
		void InitializeRetryTimerValue();
		
        TBool HasTransaction(const CSIPClientTransaction& aTransaction) const;
    
        TBool HasRefresh(const CSIPRefresh& aRefresh) const;		

        const TDesC8& RegistrarUsername() const;
		
    private: // New functions

        void RemoveRegEventHandler();

		void StoreMsgElementsL();
		
		TBool IsUnsupportedHeaderValue(RStringF aValue);
				
		void AddSecurityClientHeaderL(RPointerArray<CSIPHeaderBase>& aHeaders,
		                              const CSIPConcreteProfile& aProfile);
		
		void HandleProfileRegisteredEventL();

        CSIPMessageElements* CreateMsgElementsForUpdateLC(
            CSIPConcreteProfile& aProfile);
		
		RPointerArray<CSIPHeaderBase> CreateSIPHeadersL(
		    const CSIPConcreteProfile& aProfile);
		    
		void CheckRetryAfter( CSIPClientTransaction* aTransaction );
		
		RStringF TransportProtocol(const CUri8& aUri) const;

	private: // Data
	    
	    CSIPNotifyXmlBodyParser&    iXMLParser;
		CSIPIMSConnectionContext&	iConnectionContext;
		CUri8*						iPAURI;
		CSIPRegEventHandler*		iRegEventHandler;
		RStringF                    iPathStr;
		TBool                       iRegEventSubscriptionFailed;
		TConfiguredType 			iConfiguredType;
		CSIPProfileSIMRecord&		iSIMRecord;
		TBool                       iConnectionDropped;
		TBool                       iDeactivatedByNetwork;
		TBool						iRetriedRegister;
		TBool						iRetryTimerUse;
		TUint						iRetryAfterTime;
		TUint						iRetryCounterSum;
		TBool						iUseDynamicProxyForTry;
		CSipProfileAgentConfigExtension& iConfigExtension;
	
	private: // For testing purposes
#ifdef CPPUNIT_TEST		
		friend class CSIPIMSProfileContextTest;
		friend class CSIPIMSConnectionContextTest;
		friend class CSIPIMSProfileAgentTest;
#endif
	};

#endif // CSIPIMSPRROFILECONTEXT_H
