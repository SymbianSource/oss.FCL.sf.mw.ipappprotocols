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
// Name          : CSIPSecDigestPlugin.cpp
// Part of       : SIPDigestPlugin
// Version       : SIP/6.0
//



#include "SipLogs.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipauthorizationheader.h"
#include "sipauthenticationinfoheader.h"
#include "sipsecurityclientheader.h"
#include "sipsecurityserverheader.h"
#include "sipsecurityverifyheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SipSecUtils.h"
#include "tsipsecmechanisminitparams.h"
#include "CSIPSecDigestPlugin.h"
#include "sipsecdigestcontext.h"
#include "sipsecdigestcache.h"
#include "CSIPSecUserRecord.h"
#include "sipprivatecrkeys.h"
#include <centralrepository.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::NewL
// -----------------------------------------------------------------------------
//
CSIPSecDigestPlugin* CSIPSecDigestPlugin::NewL( TAny* aInitParams ) 
    {
    __ASSERT_ALWAYS( aInitParams, User::Leave( KErrArgument ) );
    				 
    TSIPSecMechanismInitParams* initParams =
    	static_cast<TSIPSecMechanismInitParams*>( aInitParams );
	CSIPSecDigestPlugin* self =
		new ( ELeave ) CSIPSecDigestPlugin( initParams->iEngineContext );
	CleanupStack::PushL( self );
	self->ConstructL( initParams->iTimer );
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::ConstructL( MTimerManager& aTimerMgr )
    {
    iDigestMechanism = CSIPSecDigest::NewL();
	iCache = CSIPSecDigestCache::NewL( aTimerMgr );

	CRepository* repository = CRepository::NewLC( KCRUidSIP );
	TInt emptyResponseAfterSqnFailure( 0 );
	if ( repository->Get(KSIPSendEmptyResponseParameterAfterSqnFailure,
		 				 emptyResponseAfterSqnFailure ) == KErrNone &&
		 emptyResponseAfterSqnFailure )
		{
		iSendEmptyResponseParameterAfterSqnFailure = ETrue;
		}
	CleanupStack::PopAndDestroy( repository );
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::CSIPSecDigestPlugin
// -----------------------------------------------------------------------------
//
CSIPSecDigestPlugin::CSIPSecDigestPlugin( MSIPSecEngineContext& aEngineContext )
  : iEngineContext( aEngineContext )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::~CSIPSecDigestPlugin
// -----------------------------------------------------------------------------
//
CSIPSecDigestPlugin::~CSIPSecDigestPlugin()
    {
    delete iDigestMechanism;
    delete iCache;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::Name
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPSecDigestPlugin::Name() const
    {
    return iDigestMechanism->Name();
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::InitializeSecurityClientL
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::InitializeSecurityClientL(
	CSIPSecurityClientHeader& aSecurityClient )
    {
    aSecurityClient.SetMechanismNameL( Name() );
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::ProcessSecurityVerifyL
// Find Security-Server and Security-Verify with a matching mechanism, and
// process them.
// A2 is computed from all Sec-Server headers, e.g.
// "Security-Server:ipsec-ike;q=0.1,tls;q=0.2". I.e. "Security-Server" followed
// by all mechanisms and parameters. Resulting digest-verify parameter is put to
// the Security-Verify header that has "digest" mechanism.
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::ProcessSecurityVerifyL(
	TSIPTransportParams& /*aTransportParams*/,
	CSIPRequest& aRequest,
	TInetAddr& /*aNextHop*/,
	const CUri8& aRemoteTarget,
	const TDesC8& aOutboundProxy,
	MSIPSecUser* aUser,
	TRegistrationId aRegistrationId,
	RPointerArray<CSIPSecurityServerHeader>& aSecurityServer,        
	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify )
    {
    __ASSERT_ALWAYS( aUser, User::Leave( KErrArgument ) );

	CSIPSecurityServerHeader* secServer = NULL;
	for ( TInt i = 0; i < aSecurityServer.Count() && !secServer; ++i )
		{
		if ( aSecurityServer[ i ]->MechanismName().CompareF( Name() ) == 0 )
            {
            secServer = aSecurityServer[ i ];
            }
		}
	__ASSERT_ALWAYS( secServer, User::Leave( KErrArgument ) );

	CSIPSecurityVerifyHeader* secVerify = NULL;
	for ( TInt j = 0; j < aSecurityVerify.Count() && !secVerify; ++j )
		{
		if ( aSecurityVerify[ j ]->MechanismName().CompareF( Name() ) == 0 )
            {
            secVerify = aSecurityVerify[ j ];
            }        
		}
	__ASSERT_ALWAYS( secVerify, User::Leave( KErrArgument ) );

    iDigestMechanism->InitializeL( *secServer );
    TSIPSecPluginCtxVerify secVerifyContext( *this, aRemoteTarget,
    										 aOutboundProxy, aRequest,
                                             *secVerify, aSecurityServer, 
                                             *aUser, aRegistrationId );
    TSIPSecDigestCacheIterator cacheIter( secVerifyContext );
    iCache->InitializeIterator( cacheIter );

    CSIPSecDigestCacheEntry* entry = NULL;
	while ( ( entry = cacheIter.Next() ) != NULL )
		{
		if ( entry->Match( secVerifyContext ) )
		    {
		    entry->HandleL( secVerifyContext );
    		return;
		    }
		}
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::AddSecurityParamsL
// Nothing is done for ACK.
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::AddSecurityParamsL(
	TSIPTransportParams& /*aTransportParams*/,
	CSIPRequest& aRequest,
	TRegistrationId aRegistrationId,
	TTransactionId aTransactionId,
	TInetAddr& /*aNextHop*/,
	const CUri8& aRemoteTarget,
	const TDesC8& aOutboundProxy,
	MSIPSecUser* aUser )
	{
	__ASSERT_ALWAYS( aUser, User::Leave( KErrArgument ) );	

	if ( SipSecUtils::Match( SipStrConsts::EAck, aRequest.Method() ) )
		{
		return;
		}

	TSIPSecPluginCtxRequest requestContext( *this,
											aRemoteTarget,
											aOutboundProxy,
											aRequest,
											*aUser,
											aRegistrationId,
											aTransactionId );
	TSIPSecDigestCacheIterator cacheIter( requestContext );
	iCache->InitializeIterator( cacheIter );

    // Cleanup
    CSIPAuthorizationHeader* nonDigestHeader = PrepareRequestL( aRequest );

	CSIPSecDigestCacheEntry* entry = NULL;
	while ( ( entry = cacheIter.Next() ) != NULL )
		{
		if ( nonDigestHeader &&
		     entry->Type( requestContext ) == CSIPSecDigest::EEndPoint )
		    {
		    User::LeaveIfError( aRequest.RemoveHeader( nonDigestHeader ) );
		    delete nonDigestHeader;
		    nonDigestHeader = NULL;
		    }

		entry->UpdateL( requestContext );
		}
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::PrepareRequestL
// Remove all Proxy-Authorization headers. Remove Authorization headers
// selectively, based on response value.
// -----------------------------------------------------------------------------
//
CSIPAuthorizationHeader*
CSIPSecDigestPlugin::PrepareRequestL( CSIPRequest& aRequest ) const
    {
    aRequest.DeleteHeaders(
    	SIPStrings::StringF( SipStrConsts::EProxyAuthorizationHeader ) );

    const RStringF auth =
    	SIPStrings::StringF( SipStrConsts::EAuthorizationHeader );

	CSIPAuthorizationHeader* nonDigestHeader = NULL;
    if ( aRequest.HeaderCount( auth ) > 0 )
        {
    	TSglQueIter<CSIPHeaderBase> authHeaders = aRequest.Headers( auth );

        CSIPAuthorizationHeader* header = NULL;
        while ( ( header =
                  static_cast<CSIPAuthorizationHeader*>( authHeaders++ ) )
                  != NULL )
            {
            if ( header->DesParamValue( SIPStrings::StringF(
            	SipStrConsts::EResponse ) ).Length() == 0 )
                {
                nonDigestHeader = header;
                }
            else
                {
                // This is created by digest plugin -> can be deleted now
    		    User::LeaveIfError( aRequest.RemoveHeader( header ) );
    		    delete header;
    		    header = NULL;
                }
            }
        }

    return nonDigestHeader;
    }    

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::ResponseReceivedL
// If 2xx, try to update with authentication info.
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestPlugin::ResponseReceivedL(
	TSIPTransportParams& /*aTransportParams*/,
	CSIPResponse& aResponse,
	CSIPRequest& aRequest,
	TRegistrationId aRegistrationId,
	TTransactionId aTransactionId,
	TInetAddr& /*aNextHop*/,
	const CUri8& aRemoteTarget,
	const TDesC8& aOutboundProxy,
	MSIPSecUser* aUser,
	MSIPSecSecurityMechanismObserver& aObserver )
    {
    __ASSERT_ALWAYS( aUser, User::Leave( KErrArgument ) );

	User::LeaveIfError( iCache->CountResponses( aResponse,
		aRequest,
		aTransactionId,
		iDigestMechanism->Algorithm() ) );

    if ( aResponse.ResponseCode() == 401 || // WWW Authorization
         aResponse.ResponseCode() == 407 || // Proxy Authorization
         aResponse.ResponseCode() == 494 )  // Security Agreement
   		{
        TSIPSecPluginCtxResponse respContext( *this,
    										  aRemoteTarget,
    										  aOutboundProxy,
    										  aResponse,
										      *iCache,
										      aObserver,
										      *aUser,
										      aRegistrationId,
										      aTransactionId );
		return iDigestMechanism->UpdateCacheL( respContext );
        }

	CSIPAuthenticationInfoHeader* authInfo( NULL );
    if ( ( aResponse.Type() == CSIPResponse::E2XX ) &&
         ( authInfo = AuthenticationInfo( aResponse ) ) != NULL )
        {
        TSIPSecPluginCtxInfo authInfoCtx( *this,
        								  aRemoteTarget,
        								  aOutboundProxy,
        								  aResponse,
        								  *authInfo,
									      *iCache,
									      aObserver,
									      *aUser,
									      aRegistrationId );
    	TSIPSecDigestCacheIterator cacheIterator( authInfoCtx );
    	iCache->InitializeIterator( cacheIterator );
		CSIPSecDigestCacheEntry* entry( NULL );

		while ( ( entry = cacheIterator.Next() ) != NULL )
			{
			// Let each matching cache entry handle the Authentication-Info
			entry->HandleL( authInfoCtx );
			}
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::IsServerInitiatedSecAgreeAllowed
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestPlugin::IsServerInitiatedSecAgreeAllowed() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::ParametersUpdatedL
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestPlugin::ParametersUpdatedL( MSIPSecUser* /*aUser*/ )
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::CancelPendingOperations
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::CancelPendingOperations(
	MSIPSecSecurityMechanismObserver* aObserver )
    {
	iCache->CancelPendingOperations( aObserver );
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::ClearCache
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::ClearCache( MSIPSecUser* aUser )
    {
    if ( aUser )
    	{
    	iCache->ClearCache( *aUser );	
    	}
	iCache->CleanObservers();
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::SetCredentialsL
// Cache can have just one record for a realm and transaction id pair.
// SIPSec users with PassOnlyRealmsToUser == ETrue set aTransactionId to value
// KEmptyTransactionId. Correct operation is not guaranteed if several of them
// use the same realm.
//
// This function will eventually be removed and clients start to use
// SetCredentialsL( const MSIPSecUser& .. ) instead.
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::SetCredentialsL( TTransactionId aTransactionId,
                                           const TDesC8& aRealm,
                                           const TDesC8& aOutboundProxy, 
                                           const TDesC8& aUserName,
                                           const TDesC8& aPassword )
	{
	__ASSERT_ALWAYS( aRealm.Length() > 0, User::Leave( KErrArgument ) );

	CSIPSecUserRecord* record = iCache->SearchRecord( aRealm, aTransactionId );
	__ASSERT_ALWAYS( record != NULL, User::Leave( KErrNotFound ) );

	SetCredentialsToRecordL( *record, aOutboundProxy, aUserName, aPassword );
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::SetCredentialsL
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::SetCredentialsL( const MSIPSecUser& aUser,
                                           const TDesC8& aRealm,
                                           const TDesC8& aOutboundProxy, 
                                           const TDesC8& aUserName,
                                           const TDesC8& aPassword )
	{
	__ASSERT_ALWAYS( aRealm.Length() > 0, User::Leave( KErrArgument ) );	

	CSIPSecUserRecord* record = iCache->SearchRecord( aRealm, aUser );
	__ASSERT_ALWAYS( record != NULL, User::Leave( KErrNotFound ) );

	SetCredentialsToRecordL( *record, aOutboundProxy, aUserName, aPassword );
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::SetCredentialsToRecordL
// -----------------------------------------------------------------------------
//
void CSIPSecDigestPlugin::SetCredentialsToRecordL( CSIPSecUserRecord& aRecord,
												   const TDesC8& aOutboundProxy,
                                           		   const TDesC8& aUserName,
                                           		   const TDesC8& aPassword )
	{
	TBool valid = aRecord.SetUserCredentialsL( aUserName, aPassword );
	if ( valid )
	    {
		aRecord.SetOutboundProxyL( aOutboundProxy );

    	TSIPSecDigestCacheIterator cacheIterator;
    	iCache->InitializeIterator( cacheIterator );

		CSIPSecDigestCacheEntry* entry( NULL );
		while ( ( entry = cacheIterator.Next() ) != NULL )
			{
			entry->UpdateL( aRecord );
			}
	    }

	aRecord.Updated();

	if ( !valid )
		{
		iCache->ClearCache( aRecord, EFalse );
		}

    iCache->CleanObservers();
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::IgnoreChallenge
// If aTransactionId is KEmptyTransactionId removes cache entries with
// PassOnlyRealmsToUser == ETrue.
// -----------------------------------------------------------------------------
//
TInt CSIPSecDigestPlugin::IgnoreChallenge( TTransactionId aTransactionId,
                                           const TDesC8& aRealm,
                                           const MSIPSecUser* aTrustedUser )
	{
	return iCache->Cancel( aTransactionId, aRealm, aTrustedUser );
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::RemoveCredentials
// Only remove cache entries for which PassOnlyRealmsToUser == ETrue.
// -----------------------------------------------------------------------------
//
TInt CSIPSecDigestPlugin::RemoveCredentials( const TDesC8& aRealm )    
    {
    return iCache->Remove( aRealm );
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::EngineContext
// -----------------------------------------------------------------------------
//
MSIPSecEngineContext& CSIPSecDigestPlugin::EngineContext()
    {
    return iEngineContext;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::EmptyResponseAfterSqnFailure
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestPlugin::EmptyResponseAfterSqnFailure() const
	{
	return iSendEmptyResponseParameterAfterSqnFailure;
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestPlugin::AuthenticationInfo
// -----------------------------------------------------------------------------
//
CSIPAuthenticationInfoHeader*
CSIPSecDigestPlugin::AuthenticationInfo( CSIPResponse& aResponse ) const
    {
    CSIPAuthenticationInfoHeader* header( NULL );

    if ( aResponse.Type() == CSIPResponse::E2XX )
        {
        const RStringF authInfo =
    		SIPStrings::StringF( SipStrConsts::EAuthenticationInfoHeader );
	    TInt count = aResponse.HeaderCount( authInfo );
	    if ( count > 0 )
	        {
	    	TSglQueIter< CSIPHeaderBase > headers =
	    		aResponse.Headers( authInfo );
    		for ( TInt i = 0; i < count; i++ )
                {
                header =
                	static_cast< CSIPAuthenticationInfoHeader* >( headers++ );
                }
	        }
        }

    return header;
    }
