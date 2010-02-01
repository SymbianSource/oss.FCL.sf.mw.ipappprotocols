/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsecresponsehandler.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECRESPONSEHANDLER_H
#define CSIPSECRESPONSEHANDLER_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "MSIPSecSecurityMechanismObserver.h"
#include "sipsecobserver.h"
#include "responsehandlerowner.h"
#include "tsipseccontext.h"
#include "_sipcodecdefs.h"
#include "SipStackServerDefs.h"

// FORWARD DECLARATIONS
class RStringF;
class CSIPRequest;
class CUri8;
class TSIPTransportParams;
class MSIPSecUser;
class MSIPSecMechanismStore;


// CLASS DECLARATION
class CSIPSecResponseHandler : public CBase,
  							   public MSIPSecSecurityMechanismObserver
	{
public: // Constructors and destructor

	static CSIPSecResponseHandler*
		NewLC( MSIPSecObserver& aObserver,
			   MResponseHandlerOwner& aOwner,
			   MSIPSecMechanismStore& aMechanismStore,
			   TSIPTransportParams& aTransportParams,
			   TInetAddr& aNextHopAddr,
			   const TDesC8& aNextHop,
			   const CUri8& aRemoteTarget,
			   const TDesC8& aOutboundProxy,
			   MSIPSecUser& aUser,
			   CSIPResponse& aResponse,
			   CSIPRequest& aRequest,
			   TRegistrationId aRegistrationId,
			   TTransactionId aTransactionId );

	~CSIPSecResponseHandler();	
	
public: // From MSIPSecSecurityMechanismObserver	

	void CacheUpdated( TBool aSuccess, TBool aUseSecurityServerHeaders );

	void SAReady( TBool aSuccess );
	
public: // New functions

	void CancelPendingOperations( MSIPSecObserver& aObserver );
	
	/**
	 * Process the SIP response. Processing may happen synchronously or it
	 * may go asynchronous if a plugin needs to do time consuming processing.
	 * @return TBool ETrue if the response is processed asynchronously.
	 *				 In this case, the client will be notified with
	 *				 MSIPSecObserver::SIPSecCacheUpdated when the processing
	 *				 has completed.
	 *				 EFalse if response processing has been done synchronously.
	 */
	TBool StartResponseProcessingL();
	
	void Completed( TBool aSuccess );

private: // Constructors

	CSIPSecResponseHandler( MSIPSecObserver& aObserver,
						    MResponseHandlerOwner& aOwner,
						    MSIPSecMechanismStore& aMechanismStore,
						    TSIPTransportParams& aTransportParams,
						    TInetAddr& aNextHopAddr,
						    const CUri8& aRemoteTarget,
						    const TDesC8& aOutboundProxy,
						    MSIPSecUser& aUser,
						    CSIPResponse& aResponse,
						    CSIPRequest& aRequest,
						    TRegistrationId aRegistrationId,
						    TTransactionId aTransactionId );

	void ConstructL(const TDesC8& aNextHop);

private: // New functions, for internal use

	void CheckResponseL( CSIPResponse& aResponse ) const;

	TBool CheckAuthenticateHeaders( CSIPResponse& aResponse,
									RStringF aHeaderName ) const;

	void FirstAsyncPhaseL();

	void SecondAsyncPhaseL();

	/**
	 * Cancel all such pending asynchronous operations in the plugins, that
	 * were initiated by this CSIPSecResponseHandler instance.	 
	 */	
	void CancelPendingPluginOperations();	

private: // Data

	// TransactionUser, will be notified when asynchronous operations have all
	// been done.
  	MSIPSecObserver& iObserver;

  	MResponseHandlerOwner& iOwner;

  	// Available mechanisms
  	MSIPSecMechanismStore& iMechanismStore;

  	TSIPTransportParams& iTransportParams;
	TSIPSecResponseContext iResponseContext;

  	// Amount of pending asynchronous requests
  	TInt iPendingAsyncRequests;

  	// Next-hop, can be IP-address or FQDN. Owned.
  	HBufC8* iNextHop;

  	// SIP request associated with the SIP response. Not owned.
  	CSIPRequest& iRequest;

  	// Registration ID
  	TRegistrationId iRegistrationId;
	
  	// Transaction ID
  	TTransactionId iTransactionId;

	// If response handling is asynchronous. This is the final status, that
	// will be passed to MSIPSecObserver::SIPSecCacheUpdated.
  	TBool iSuccess;

	// Information from all plugins is collected here. If even one plugin tells
	// that Security-Server headers must not be used, they are not used.
	TBool iUseSecurityServerHeaders;

  	UNIT_TEST( CSIPSecResponseHandlerTest )
	};
	
#endif // CSIPSECRESPONSEHANDLER_H
