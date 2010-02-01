/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsec.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSEC_H
#define CSIPSEC_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <ecom/ecom.h>
#include <in_sock.h>
#include "Lwtimer.h"
#include "sipsecsecuritymechanism.h"
#include "TSIPSecMechanismIter.h"
#include "responsehandlerowner.h"
#include "mechanismstore.h"
#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class CUri8;
class CSIPRequest;
class CSIPResponse;
class MSIPSecUser;
class MSIPSecObserver;
class CSIPSecAgreementContainer;
class CSIPSecEngineContext;
class MSIPTransportMgr;
class TSIPTransportParams;

// CLASS DECLARATION
/**
* Class provides functions for adding security information in SIP requests 
* using different security mechanisms. SIP stack will always
* request to add the security parameters before sending 
* a SIP request to a remote UA.
*
*  @lib SipServer.exe
*/
class CSIPSec : public CBase,
  				public MResponseHandlerOwner,
  				public MSIPSecMechanismStore
	{
	public: // Constructors and destructor

		/**
		* Constructs an object; leaves on failure.
		* @param aTimerMgr an interface for requesting timer events
		* @param aTransportMgr an interface for allocating and de-allocating 
		*        transports resources for IPSec.
		* @param aT1 Duration of T1
		* @return new instance
		*/
		static CSIPSec* NewL( MTimerManager& aTimerMgr,
		                      MSIPTransportMgr& aTransportMgr,
		                      TUint aT1 );

		/**
		* Constructs an object and adds the pointer to the cleanup stack;
		* leaves on failure.
		* @param aTimerMgr an interface for requesting timer events
		* @param aTransportMgr an interface for allocating and de-allocating 
		*        transports resources for IPSec.
		* @param aT1 Duration of T1
		* @return new instance
		*/
		static CSIPSec* NewLC( MTimerManager& aTimerMgr,
		                       MSIPTransportMgr& aTransportMgr,
		                       TUint aT1 );

		/**
		* Destructor
		*/
		~CSIPSec();

	public: // From MResponseHandlerOwner
	
		void ResponseFirstPhaseDoneL( CSIPResponse& aResponse,
									  TInetAddr& aNextHopAddr,					    
							   		  const TDesC8& aNextHop,
							   		  MSIPSecUser& aUser,
							   		  TBool aUseSecurityServerHeaders );

		void Remove( CSIPSecResponseHandler& aResponseHandler );
		
	public: // From MSIPSecMechanismStore

		TSIPSecMechanismIter& MechanismIter();

		MSIPSecSecurityMechanism*
			MechanismByName( const TDesC8& aMechanismName );

	public: // New functions

		/**
		* Gets all supported security mechanisms
		* @return supported SIP security mechanisms names (e.g. digest);
        *         the ownership is transferred
		*/
		CDesC8Array* SupportedSecurityMechanismsL();

		/**
		* Adds the cached security parameters to the SIP request.
		* If no cache entries are found returns without any error.
		* @param aTransportParams params to be used when creating IPSec SAs. 
		*        If an SA was created on return contains also
		*        the updated transport identifier. 
		* @param aRequest SIP request where the security parameters will be
		*		 added
		* @param aRegistrationId Registration ID
		* @param aTransactionId transaction ID
		* @param aNextHopAddr the next hop IP address of the request
		* @param aNextHop the next hop of the request
		*        IP address or a domain name
		* @param aRemoteTarget Remote target of the request		
		* @param aOutboundProxy the outbound proxy of the request
		*        IP address or a domain name. Zero-length if not present.
		* @param aUser the user for the possible cache entries
		*/
		void AddSecurityParamsL( TSIPTransportParams& aTransportParams,
		                         CSIPRequest& aRequest,
		                         TRegistrationId aRegistrationId,
		                         TTransactionId aTransactionId,
			                     TInetAddr& aNextHopAddr,
			                     const TDesC8& aNextHop,
			                     const CUri8& aRemoteTarget,
			                     const TDesC8& aOutboundProxy,
							     MSIPSecUser& aUser );

		/**
		* Updates the SIPSec cache if necessary.
		* If nothing was updated returns without any error.
		* May lead to asynchronous processing. If so, returns ETrue
		* and the user must wait for the cache to be updated before
		* sending the SIP request again.
		* @param aTransportParams params to be used when creating IPSec SAs. 
		*        If an SA was updated on return contains also
		*        the updated transport identifier.		
		* @param aResponse SIP response to be processed to update the cache
		* @param aRequest IN: SIP request related to the aResponse
		* @param aRegistrationId registration ID
		* @param aTransactionId transaction ID
		* @param aNextHopAddr the next hop of the original request
		* @param aNextHop the next hop of the request
		*        IP address or a domain name
		* @param aRemoteTarget Remote target of the original request
		* @param aOutboundProxy the outbound proxy of the original request
		*        IP address or a domain name. Zero-length if not present.
		* @param aUser the user for the possible cache entries,
		*        Can be used to getting the credentials for the cache.
		*        The ownership is NOT transferred.
		* @param aObserver observer that will be notified when
		*        after an asynchronous cache update has been completed.		
		* @return ETrue if the user needs to wait asynchronously for the
		*         cache to be updated. Otherwise EFalse.
		*/
		TBool ResponseReceivedL( TSIPTransportParams& aTransportParams,
		                         CSIPResponse& aResponse,
		                         CSIPRequest& aRequest,
		                         TRegistrationId aRegistrationId,
		                         TTransactionId aTransactionId,
			                     TInetAddr& aNextHopAddr,
			                     const TDesC8& aNextHop,
			                     const CUri8& aRemoteTarget,
			                     const TDesC8& aOutboundProxy,
							     MSIPSecUser& aUser,
							     MSIPSecObserver& aObserver );
		
		/**
		* Cancels all the pending asynchronous operations 
		* for the given observer.
		* @param aObserver observer waiting for an asynchronous cache update		
		*/
		void CancelPendingOperations( MSIPSecObserver& aObserver );

		/**
		* Clears all the cached information for the given user.
		* @param aUser the user for the cache entries
		*        The ownership is NOT transferred.
		*/
		void ClearCache( MSIPSecUser* aUser );

        /**
		* Removes all set credentials for the realm
		* @param aRealm servers's realm
		* @return KErrNone if no error; KErrNoMemory if out of memory;
		*		  KErrNotFound if the given realm was not found
        */
		TInt RemoveCredentials( const TDesC8& aRealm );

		/**
		* Will be called as a result to asynchronous call 
		* to MSIPSecUser::RequestCredentialsL.
		*
		* @param aTransactionId transaction ID if available, 
		*        otherwise KEmptyTransactionId
		* @param aRealm the realm for the credentials
		* @param aOutboundProxy Outbound proxy
		* @param aUserName the username
		* @param aPassword the password
		*/
		void SetCredentialsL( TTransactionId aTransactionId,
		                      const TDesC8& aRealm,
			                  const TDesC8& aOutboundProxy, 
						      const TDesC8& aUserName,
						      const TDesC8& aPassword );

		/**
		* Will be called as a result to asynchronous call 
		* to MSIPSecUser::RequestCredentialsL.
		*
		* @param aUser SIPSec user
		* @param aRealm the realm for the credentials
		* @param aOutboundProxy Outbound proxy
		* @param aUserName the username
		* @param aPassword the password
		*/
		void SetCredentialsL( const MSIPSecUser& aUser,
		                      const TDesC8& aRealm,
			                  const TDesC8& aOutboundProxy, 
						      const TDesC8& aUserName,
						      const TDesC8& aPassword );

		/**
		* Ignores the challenge for the realm. 
        * As a result the error will be generated to the original request in
        * case.
		* @param aTransactionId transaction ID if available, 
		*        otherwise KEmptyTransactionId        
		* @param aRealm a realm for which the challenge was ignored
		* @param aTrustedUser the trusted user for the ignored challenge
		* @return KErrNone if no error;
		*		  KErrNotFound if the given realm was not found
		*/
		TInt IgnoreChallenge( TTransactionId aTransactionId,
		                      const TDesC8& aRealm,
		                      const MSIPSecUser* aTrustedUser );

		/**
		* Gets the negotiated security mechanism with the next hop
		* on the signaling path.
		* @param aHop the next hop's FQDN or numeric IP address
		* @return negotiated security mechanism or zero-pointer in case no
		*		  security mechanism was negotiated with the next hop;
		*		  the ownership is transferred
		*/
		HBufC8* NegotiatedSecurityMechanismL( const TDesC8& aHop );

	private: // Constructors

		CSIPSec( MTimerManager& aTimerMgr,
				 MSIPTransportMgr& aTransportMgr,
				 TUint aT1 );
		
		void ConstructL();
		
	private: // New functions

		void ListMechanismImplementationsL();

		static void ArrayCleanup( TAny* aArray );
		
	private: // Data

		// Available plugins, owned
		RPointerArray<MSIPSecSecurityMechanism> iMechanisms;

		// Owned
		RPointerArray<CSIPSecResponseHandler> iResponseHandlers;

		MTimerManager& iTimerMgr;
		MSIPTransportMgr& iTransportMgr;
		TSIPSecMechanismIter iMechanismIter;

		// Owned
		CSIPSecAgreementContainer* iAgreementContainer;

		// Owned
		CSIPSecEngineContext* iEngineContext;
		
		// Duration of T1
		TUint iT1;
	private: //For testing purposes
#ifdef CPPUNIT_TEST
		friend class CSipSecEngineTest;		
#endif
	};

#endif // CSIPSEC_H

// End of File
