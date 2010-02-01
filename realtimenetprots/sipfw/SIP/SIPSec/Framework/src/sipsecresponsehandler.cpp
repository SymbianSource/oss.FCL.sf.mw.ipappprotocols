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
// Name          : sipsecresponsehandler.cpp
// Part of       : SIPSec
// Version       : SIP/6.0 
//



#include "SipLogs.h"
#include "sipresponse.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipauthenticateheaderbase.h"
#include "sipsecresponsehandler.h"
#include "TSIPSecMechanismIter.h"
#include "MSIPSecSecurityMechanism.h"
#include "mechanismstore.h"
#include "sipsecagreement.h"

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::NewL
// ----------------------------------------------------------------------------
//
CSIPSecResponseHandler*
CSIPSecResponseHandler::NewLC( MSIPSecObserver& aObserver,
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
							   TTransactionId aTransactionId )
	{
	CSIPSecResponseHandler* self =
		new ( ELeave ) CSIPSecResponseHandler( aObserver,
											   aOwner,
   											   aMechanismStore,
											   aTransportParams,
											   aNextHopAddr,											
											   aRemoteTarget,
											   aOutboundProxy,
											   aUser,
											   aResponse,
											   aRequest,
											   aRegistrationId,
											   aTransactionId );
	CleanupStack::PushL( self );
	self->ConstructL( aNextHop );	
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::CSIPSecResponseHandler
// ----------------------------------------------------------------------------
//
CSIPSecResponseHandler::CSIPSecResponseHandler( MSIPSecObserver& aObserver,
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
  									TTransactionId aTransactionId ) :
	iObserver( aObserver ),
  	iOwner( aOwner ),
  	iMechanismStore( aMechanismStore ),
  	iTransportParams( aTransportParams ),
  	iResponseContext( aNextHopAddr,
  					  aRemoteTarget,
  					  aOutboundProxy,
  					  aUser,
  				      aObserver,
  				      aResponse ),
	iPendingAsyncRequests( 0 ),
	iRequest( aRequest ),
	iRegistrationId( aRegistrationId ),
	iTransactionId( aTransactionId ),
	iSuccess( EFalse ),
	iUseSecurityServerHeaders( ETrue )
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecResponseHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::ConstructL( const TDesC8& aNextHop )
	{
	iNextHop = aNextHop.AllocL();	
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::~CSIPSecResponseHandler
// ----------------------------------------------------------------------------
//	
CSIPSecResponseHandler::~CSIPSecResponseHandler()
	{
	iOwner.Remove( *this );
	CancelPendingPluginOperations();
	delete iNextHop;	
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::StartResponseProcessingL
// ----------------------------------------------------------------------------
//
TBool CSIPSecResponseHandler::StartResponseProcessingL()
	{
	CheckResponseL( iResponseContext.SIPResponse() );

	FirstAsyncPhaseL();
	if ( iPendingAsyncRequests > 0 )
		{
		return ETrue;
		}

	iOwner.ResponseFirstPhaseDoneL( iResponseContext.SIPResponse(),
								    iResponseContext.NextHop(),
								    *iNextHop,
								    iResponseContext.SIPSecUser(),
								    iUseSecurityServerHeaders );
	SecondAsyncPhaseL();

	return iPendingAsyncRequests > 0;
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::CheckResponseL
// SIPSec-related response (except 421) must have Security-Server or at least
// one authenticate header with a supported plugin. Otherwise the request must
// not be sent again. If Security-Server exists, SA routines check it later.
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::CheckResponseL( CSIPResponse& aResponse ) const
	{
	if ( CSIPSecAgreement::IsSecAgreeResponse( aResponse.ResponseCode() ) &&
		 ( aResponse.ResponseCode() != 421 ) &&
		 !aResponse.HasHeader(
			SIPStrings::StringF( SipStrConsts::ESecurityServerHeader) ) &&		
		 !CheckAuthenticateHeaders( aResponse,
			SIPStrings::StringF( SipStrConsts::EProxyAuthenticateHeader ) ) &&
		 !CheckAuthenticateHeaders( aResponse,
			SIPStrings::StringF( SipStrConsts::EWWWAuthenticateHeader ) ) )
		{
		User::Leave( KErrNotFound );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::CheckAuthenticateHeaders
// ----------------------------------------------------------------------------
//
TBool
CSIPSecResponseHandler::CheckAuthenticateHeaders( CSIPResponse& aResponse,
												  RStringF aHeaderName ) const
	{
    if ( aResponse.HeaderCount( aHeaderName ) > 0 )
        {
    	TSglQueIter<CSIPHeaderBase> headers = aResponse.Headers( aHeaderName );
    	while ( headers )
        	{
        	CSIPAuthenticateHeaderBase* h =
            	static_cast<CSIPAuthenticateHeaderBase*>( headers++ );
            if ( iMechanismStore.MechanismByName( h->AuthScheme().DesC() ) )
            	{
            	return ETrue;
            	}
            }
        }
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::FirstAsyncPhaseL
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::FirstAsyncPhaseL()
	{
	__ASSERT_DEBUG( iPendingAsyncRequests == 0, User::Leave( KErrNotReady ) );

	TSIPSecMechanismIter& iterator = iMechanismStore.MechanismIter();
	MSIPSecSecurityMechanism* mechanism = iterator.First();
	while ( mechanism != NULL )
		{
		if ( mechanism->ResponseReceivedL( iTransportParams,
										   iResponseContext.SIPResponse(),
									 	   iRequest,
									 	   iRegistrationId,
									 	   iTransactionId,
		 								   iResponseContext.NextHop(),
		 								   iResponseContext.RemoteTarget(),
										   iResponseContext.OutboundProxy(),
										   &iResponseContext.SIPSecUser(),
										   *this ) )
			{
			iPendingAsyncRequests++;
			}
		mechanism = iterator.Next();
		}
	}
	
// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::SecondAsyncPhaseL
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::SecondAsyncPhaseL()
	{
	//The first asynchronous phase must be completed by now
	__ASSERT_DEBUG( iPendingAsyncRequests == 0, User::Leave( KErrNotReady ) );

	TSIPSecMechanismIter& iterator = iMechanismStore.MechanismIter();
	MSIPSecSecurityMechanism* mechanism = iterator.First();	

	while ( mechanism != NULL )
		{
		if (mechanism->ParametersUpdatedL( &iResponseContext.SIPSecUser() ) )
			{
			iPendingAsyncRequests++;
			}
		mechanism = iterator.Next();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::CancelPendingOperations
// ----------------------------------------------------------------------------
//
void 
CSIPSecResponseHandler::CancelPendingOperations(MSIPSecObserver& aObserver)
	{	
	if ( &aObserver == &iObserver )
		{
		CancelPendingPluginOperations();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::CancelPendingPluginOperations
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::CancelPendingPluginOperations()
	{	
	TSIPSecMechanismIter& iterator = iMechanismStore.MechanismIter();
	MSIPSecSecurityMechanism* mechanism = iterator.First();

	while ( mechanism != NULL )
		{
		mechanism->CancelPendingOperations( this );
		mechanism = iterator.Next();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::CacheUpdated
// Destructor's call to CancelPendingPluginOperations may lead here, with
// iPendingAsyncRequests being zero. Nothing should be done in that case.
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::CacheUpdated( TBool aSuccess,
										   TBool aUseSecurityServerHeaders )
	{
	__SIP_INT_LOG2( "SIPSecRespHa:CacheUpdated success, use sec-serv",
					aSuccess,
					aUseSecurityServerHeaders )
	if ( !aUseSecurityServerHeaders )
		{
		iUseSecurityServerHeaders = EFalse;
		}
	if ( aSuccess )
		{
		// At least one plugin reports success
		iSuccess = ETrue;
		}

	if ( --iPendingAsyncRequests == 0 )
		{
		__SIP_LOG( "SIPSecRespHa proceed to second phase" )

		TInt err = KErrNone;
		if ( iSuccess )
			{
			TRAP( err, iOwner.ResponseFirstPhaseDoneL(
							iResponseContext.SIPResponse(),
				 	        iResponseContext.NextHop(),
				 	        *iNextHop,
				 	        iResponseContext.SIPSecUser(),
				 	        iUseSecurityServerHeaders ) );
			__SIP_INT_LOG1( "RespFirstPhaseDoneL leave code", err )

			// If FW's sec-agree leaves, abort response handling
			if ( err == KErrNone )
				{
				TRAP( err, SecondAsyncPhaseL() );
				__SIP_INT_LOG1( "SecondAsyncPhaseL leave code", err )

				// At least one plugin started asynchronous processing. Even if
				// leave occurred in another plugin, wait for the asynchronous
				// request to complete, because it may complete succesfully.
				if ( iPendingAsyncRequests > 0 )
					{
					__SIP_LOG( "wait for pending requests" )
					// Success depends on the second phase asyncronous result
					iSuccess = EFalse;
					return;
					}
				}
			}
		Completed( iSuccess && err == KErrNone );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::SAReady
// Plugin has completed asynchronous processing started in ParametersUpdatedL.
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::SAReady( TBool aSuccess )
	{
	__SIP_INT_LOG1( "SIPSecRespHa:SAReady success", aSuccess )

	__ASSERT_DEBUG( iPendingAsyncRequests > 0,
		User::Panic( _L( "SIPSecRespHandler::SAReady" ), KErrUnderflow ) );

	if ( aSuccess )
		{
		// At least one plugin reports success
		iSuccess = ETrue;
		}

	if ( --iPendingAsyncRequests == 0 )
		{
		Completed( iSuccess );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecResponseHandler::Completed
// ----------------------------------------------------------------------------
//
void CSIPSecResponseHandler::Completed( TBool aSuccess )
	{
    __SIP_INT_LOG1( "SIPSecRespHa:Completed success", aSuccess )
	
	iObserver.SIPSecCacheUpdated( aSuccess );
	delete this;
	}
