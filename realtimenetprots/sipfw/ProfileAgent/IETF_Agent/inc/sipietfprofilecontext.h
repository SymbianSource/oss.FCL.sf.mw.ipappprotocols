/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipietfprofilecontext.h
* Part of     : sip ietf agent
* Interface   : Internal IF
* Version     : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPIETFPRROFILECONTEXT_H
#define SIPIETFPRROFILECONTEXT_H

//INCLUDES 
#include "sipprofilecontextbase.h"
#include "sipietfconnectioncontext.h"
#include <sipprofileagentextensionparams.h>

// FORWARD DECLARATIONS
class CSIPConcreteProfile;


// CLASS DECLARATION
/**
*  Class for maintaining the information related
*  to a particular IETF type profile. Class stores
*  related registration, sip connection and transactions.
*
*  @lib n/a.
*/
class CSIPIetfProfileContext : public CSIPProfileContextBase
	{
	public:
		   
        /**
        * Two-phased constructor.
	    * @param aSIP a handle to SIP server
		* @param aConnection a SIP connection
		* @param aInitState a init state
		* @param aProfile a profile
        */
		static CSIPIetfProfileContext* NewL(
			CSIP& aSIP,
			CSIPIetfConnectionContext& aConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer,
			CSipProfileAgentConfigExtension& aConfigExtension);
        /**
        * Two-phased constructor.
	    * @param aSIP a handle to SIP server
		* @param aConnection a SIP connection
		* @param aInitState a init state
		* @param aProfile a profile
        */
		static CSIPIetfProfileContext* NewLC(
			CSIP& aSIP,
			CSIPIetfConnectionContext& aConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer,
			CSipProfileAgentConfigExtension& aConfigExtension);
        /**
        * Destructor.
        */
		~CSIPIetfProfileContext();


    public: // From MSIPProfileContext

		void DestroyRegistration();

        void CreateRegistrationL();

		CSIPMessageElements* CreateMsgElementsLC();
		
        CSIPMessageElements* CreateDeRegisterElementsL();
		
		void UpdateContactHeaderParamsL(CSIPConcreteProfile& aNewProfile);
		
        void SetRegisteredAORsL();	
        
        void ResolveProxyL();
        
        void CancelProxyResolving();	
		
		
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
		                      
		TBool RetryTimerInUse();                      
		
		TUint DelayTime();
		TBool RetryAfterTimer();
		
	private: // Constructor
	
		CSIPIetfProfileContext(
			CSIP& aSIP,
			CSIPIetfConnectionContext& iConnection,
			MSIPProfileAgentObserver& aObserver,
			CSIPPrflStateBase& aInitState,
			CSIPConcreteProfile& aProfile,
			CDeltaTimer& aDeltaTimer,
			CSipProfileAgentConfigExtension& aConfigExtension);
			
	    void ConstructL();


	private: // From CSIPProfileContextBase

		void ConnectionStateChangedImpl(CSIPConnection::TState aState);
		
		TBool RetryRegister(CSIPClientTransaction* aTransaction, 
                            TInt aError);
							 
		TBool ShouldRetryRegistration(TInt aError);					 

		void InitializeRetryTimerValue();


    private: // New functions
    
        CSIPMessageElements* CreateMsgElementsForUpdateLC(
            CSIPConcreteProfile& aProfile);
		TUint RandomPercent();
		TBool IsAddressSIPSURIL(const TDesC8& aValue) const;
		HBufC8* SetAddressToSIPSURIL(const TDesC8& aValue);
		TBool SetTlsToSecurityClientL();
		TBool IsDynamicNumericIPAddress( const TDesC8& aValue );
		TBool IsConfiguredOutboundProxySIPURIL( const TDesC8& aValue );
        RStringF TransportProtocol(const CUri8& aUri) const;

	private: // Data
	
		CSIPIetfConnectionContext&	iConnectionContext;
		RStringF iStrNATTraversalRequired;
		TInt64 						iRandomSeed;
		TUint  						iRetryTime;
		TUint						iRetryCounter;
		TUint						iRetryCounterSum;
		TBool						iRetryTimerUse;
		CSipProfileAgentConfigExtension& iConfigExtension;

    private: // For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSIPIetfProfileContextTest;
		friend class CSIPIetfConnectionContextTest;
		friend class CSIPIetfProfileAgentTest;
#endif
	};

#endif
