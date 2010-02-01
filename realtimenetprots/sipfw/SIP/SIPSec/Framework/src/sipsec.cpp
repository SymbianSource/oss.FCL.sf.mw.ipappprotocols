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
// Name          : sipsec.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "SipAssert.h"
#include "sipsec.h"
#include "sipstrings.h"
#include "MSIPSecSecurityMechanism.h"
#include "tsipsecmechanisminitparams.h"
#include "sipsecresponsehandler.h"
#include "sipsecagreementcontainer.h"
#include "TSIPSecResponsehandlerIter.h"
#include "CSIPSecEngineContext.h"
#include "tcleanresponsehandler.h"

// ----------------------------------------------------------------------------
// CSIPSec::NewL
// ----------------------------------------------------------------------------
//
CSIPSec* CSIPSec::NewL( MTimerManager& aTimerMgr,
					    MSIPTransportMgr& aTransportMgr,
					    TUint aT1 )
	{
	CSIPSec* self = CSIPSec::NewLC( aTimerMgr, aTransportMgr, aT1 );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSec::NewLC
// ----------------------------------------------------------------------------
//
CSIPSec* CSIPSec::NewLC( MTimerManager& aTimerMgr,
						 MSIPTransportMgr& aTransportMgr,
						 TUint aT1 )
	{
	CSIPSec* self = new ( ELeave ) CSIPSec( aTimerMgr, aTransportMgr, aT1 );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSec::CSIPSec
// ----------------------------------------------------------------------------
//
CSIPSec::CSIPSec( MTimerManager& aTimerMgr,
				  MSIPTransportMgr& aTransportMgr,
				  TUint aT1 ) :
#ifdef CPPUNIT_TEST
	//In unit tests, use array granularity 1 to make every append operation to
	//allocate memory.
  	iMechanisms( 1 ),
  	iResponseHandlers( 1 ),
#endif
	iTimerMgr( aTimerMgr ),
	iTransportMgr( aTransportMgr ),
  	iMechanismIter( iMechanisms ),
	iT1( aT1 )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSec::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSec::ConstructL()
	{
	iEngineContext = CSIPSecEngineContext::NewL();
	ListMechanismImplementationsL();
	iAgreementContainer = CSIPSecAgreementContainer::NewL( *this );
	}

// ----------------------------------------------------------------------------
// CSIPSec::~CSIPSec
// ----------------------------------------------------------------------------
//
CSIPSec::~CSIPSec()
	{
	//This can lead to using iAgreementContainer, so iMechanisms must be freed
	//before iAgreementContainer.
	iMechanisms.ResetAndDestroy();	
	iResponseHandlers.ResetAndDestroy();
	delete iEngineContext;
	delete iAgreementContainer;
	}

// ----------------------------------------------------------------------------
// CSIPSec::AddSecurityParamsL
// ----------------------------------------------------------------------------
//
void CSIPSec::AddSecurityParamsL( TSIPTransportParams& aTransportParams,
								  CSIPRequest& aRequest,
								  TRegistrationId aRegistrationId,
								  TTransactionId aTransactionId,			                    
			                      TInetAddr& aNextHopAddr,
			                      const TDesC8& aNextHop,
			                      const CUri8& aRemoteTarget,
			                      const TDesC8& aOutboundProxy,
							      MSIPSecUser& aUser )
	{
	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();
	while ( mechanism != NULL )
		{
		mechanism->AddSecurityParamsL( aTransportParams,
									   aRequest,
									   aRegistrationId,
									   aTransactionId, 
									   aNextHopAddr, 
									   aRemoteTarget, 
									   aOutboundProxy, 
									   &aUser );
		mechanism = mechanismIter.Next();
		}
	iAgreementContainer->AddSecurityParamsL( aTransportParams,
											 aRequest,
											 aNextHopAddr,
											 aNextHop,
											 aRemoteTarget,
											 aOutboundProxy,
											 aUser,
											 aRegistrationId );
	}

// ----------------------------------------------------------------------------
// CSIPSec::ResponseReceivedL
// ----------------------------------------------------------------------------
//
TBool CSIPSec::ResponseReceivedL( TSIPTransportParams& aTransportParams,
								  CSIPResponse& aResponse,
								  CSIPRequest& aRequest,
								  TRegistrationId aRegistrationId,
								  TTransactionId aTransactionId,
			                      TInetAddr& aNextHopAddr,
			                      const TDesC8& aNextHop,
			                      const CUri8& aRemoteTarget,
			                      const TDesC8& aOutboundProxy,
							      MSIPSecUser& aUser,
							      MSIPSecObserver& aObserver )
	{
	CSIPSecResponseHandler* handler = 
		CSIPSecResponseHandler::NewLC( aObserver,
									   *this,
									   *this,
									   aTransportParams,
									   aNextHopAddr,
									   aNextHop,
									   aRemoteTarget,
									   aOutboundProxy,
									   aUser,
									   aResponse,
									   aRequest,
									   aRegistrationId,
									   aTransactionId );
	iResponseHandlers.AppendL( handler );
	CleanupStack::Pop( handler );

	TCleanResponseHandler cleanitem( this, handler );
    CleanupStack::PushL( cleanitem.CleanupItem());
	TBool mustWait = handler->StartResponseProcessingL();
	CleanupStack::Pop(); // cleanupItem
	if ( !mustWait )
		{
		//Response processed, handler no longer needed
		Remove( *handler );
		delete handler;
		}

	return mustWait;
	}

// ----------------------------------------------------------------------------
// CSIPSec::CancelPendingOperations
// ----------------------------------------------------------------------------
//		
void CSIPSec::CancelPendingOperations( MSIPSecObserver& aObserver )
	{
	TSIPSecResponsehandlerIter responsehandlers( iResponseHandlers );
	CSIPSecResponseHandler* handler = responsehandlers.First();
	while ( handler != NULL )
		{
		handler->CancelPendingOperations( aObserver );
		handler = responsehandlers.Next();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSec::ClearCache
// ----------------------------------------------------------------------------
//
void CSIPSec::ClearCache( MSIPSecUser* aUser )
	{
	__SIP_ASSERT_RETURN( aUser, KErrArgument );

	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();
	while ( mechanism != NULL )
		{
		mechanism->ClearCache( aUser );
		mechanism = mechanismIter.Next();
		}

	iAgreementContainer->ClearCache( *aUser );
	}

// ----------------------------------------------------------------------------
// CSIPSec::SupportedSecurityMechanismsL
// ----------------------------------------------------------------------------
//
CDesC8Array* CSIPSec::SupportedSecurityMechanismsL()
	{
	CDesC8ArrayFlat* securityMechanism = new (ELeave) CDesC8ArrayFlat( 1 );
	CleanupStack::PushL( securityMechanism );
	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();

	while ( mechanism != NULL )
		{
		securityMechanism->AppendL( mechanism->Name() );
		mechanism = mechanismIter.Next();
		}

	CleanupStack::Pop( securityMechanism );
	return securityMechanism;
	}

// ----------------------------------------------------------------------------
// CSIPSec::RemoveCredentials
// If at least one plugin succeeded, return KErrNone. Since e.g. IpSec plugin
// won't set credentials in SetCredentialsL.
// ----------------------------------------------------------------------------
//
TInt CSIPSec::RemoveCredentials( const TDesC8& aRealm )
	{
	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();	

	TInt result = KErrNotFound;
	while ( mechanism != NULL )
		{
		if ( mechanism->RemoveCredentials( aRealm ) == KErrNone )
			{
			result = KErrNone;
			}
		mechanism = mechanismIter.Next();
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSIPSec::SetCredentialsL
// ----------------------------------------------------------------------------
//
void CSIPSec::SetCredentialsL( TTransactionId aTransactionId,
                               const TDesC8& aRealm,
			                   const TDesC8& aOutboundProxy, 
						       const TDesC8& aUserName,
						       const TDesC8& aPassword )
	{
	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();
	while ( mechanism != NULL )
		{
		mechanism->SetCredentialsL( aTransactionId,
		                            aRealm,
								    aOutboundProxy, 
								    aUserName,
								    aPassword );
		mechanism = mechanismIter.Next();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSec::SetCredentialsL
// ----------------------------------------------------------------------------
//
void CSIPSec::SetCredentialsL( const MSIPSecUser& aUser,
		                       const TDesC8& aRealm,
			                   const TDesC8& aOutboundProxy, 
						       const TDesC8& aUserName,
						       const TDesC8& aPassword )
	{
	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();
	while ( mechanism != NULL )
		{
		mechanism->SetCredentialsL( aUser,
		                            aRealm,
								    aOutboundProxy, 
								    aUserName,
								    aPassword );
		mechanism = mechanismIter.Next();
		}
	}

// ----------------------------------------------------------------------------
// CSIPSec::IgnoreChallenge
// If at least one plugin succeeded, return KErrNone.
// ----------------------------------------------------------------------------
//	
TInt CSIPSec::IgnoreChallenge( TTransactionId aTransactionId,
                               const TDesC8& aRealm,
                               const MSIPSecUser* aTrustedUser )
	{
	TSIPSecMechanismIter mechanismIter( iMechanisms );
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();	

	TInt result = KErrNotFound;
	while ( mechanism != NULL )
		{
		TInt err = 
		    mechanism->IgnoreChallenge( aTransactionId, aRealm, aTrustedUser );
		if ( err == KErrNone )
			{
			result = KErrNone;
			}
		mechanism = mechanismIter.Next();
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSIPSec::NegotiatedSecurityMechanismL
// ----------------------------------------------------------------------------
//
HBufC8* CSIPSec::NegotiatedSecurityMechanismL( const TDesC8& aHop )
	{
	return iAgreementContainer->NegotiatedSecurityMechanismL( aHop );
	}

// ----------------------------------------------------------------------------
// CSIPSec::ListMechanismImplementationsL
// ----------------------------------------------------------------------------
//
void CSIPSec::ListMechanismImplementationsL()
	{
	RImplInfoPtrArray infoArray;		
	CleanupStack::PushL( TCleanupItem( ArrayCleanup, &infoArray ) );	
	REComSession::ListImplementationsL( KSIPSecMechanismInterfaceUid,
										infoArray );

	for ( TInt i = 0; i < infoArray.Count(); i++ )
		{
		TSIPSecMechanismInitParams initParam( iTimerMgr,
											  *iEngineContext,
											  iTransportMgr,
											  iT1 );
		CSIPSecSecurityMechanism* data =
			CSIPSecSecurityMechanism::NewL( infoArray[ i ]->DataType(),
											initParam );
		CleanupStack::PushL( data );
		iMechanisms.AppendL( data );
		CleanupStack::Pop( data );
		}
	CleanupStack::PopAndDestroy(); // TCleanupItem
	}

// ----------------------------------------------------------------------------
// CSIPSec::ResponseFirstPhaseDoneL
// Run security agreement part
// ----------------------------------------------------------------------------
//
void CSIPSec::ResponseFirstPhaseDoneL( CSIPResponse& aResponse,
								 	   TInetAddr& aNextHopAddr,
								       const TDesC8& aNextHop,
								       MSIPSecUser& aUser,
								       TBool aUseSecurityServerHeaders )
	{
	iAgreementContainer->ResponseReceivedL( aResponse,
										    aNextHopAddr,
										    aNextHop,
										    aUser,
										    aUseSecurityServerHeaders );
	}
	
// ----------------------------------------------------------------------------
// CSIPSec::Remove
// ----------------------------------------------------------------------------
//
void CSIPSec::Remove( CSIPSecResponseHandler& aResponseHandler )
	{
	TInt index = 0;
	index = iResponseHandlers.Find( &aResponseHandler);
	if ( index != KErrNotFound )
		{
		iResponseHandlers.Remove( index );
		iResponseHandlers.Compress();		
		}
	}

// ----------------------------------------------------------------------------
// CSIPSec::MechanismIter
// ----------------------------------------------------------------------------
//
TSIPSecMechanismIter& CSIPSec::MechanismIter()
	{
	return iMechanismIter;
	}

// ----------------------------------------------------------------------------
// CSIPSec::MechanismByName
// ----------------------------------------------------------------------------
//
MSIPSecSecurityMechanism*
CSIPSec::MechanismByName( const TDesC8& aMechanismName )
	{	
	TSIPSecMechanismIter& mechanismIter = MechanismIter();
	MSIPSecSecurityMechanism* mechanism = mechanismIter.First();

	while ( mechanism != NULL )
		{
		if ( aMechanismName.CompareF( mechanism->Name() ) == 0 )
			{
			return mechanism;
			}
		mechanism = mechanismIter.Next();
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPSec::ArrayCleanup
// -----------------------------------------------------------------------------
//
void CSIPSec::ArrayCleanup( TAny* aArray )
	{    
    if ( aArray )
        {
        reinterpret_cast<RImplInfoPtrArray*>( aArray )->ResetAndDestroy();
        }
	}
