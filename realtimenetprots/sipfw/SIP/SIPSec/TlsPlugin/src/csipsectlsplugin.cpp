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
// Name        : csipsectlsplugin.cpp
// Part of     : SIPSec TLS Plugin
// Version     : %version: 2.1.1 %
//



#include "TSIPTransportParams.h"
#include "MSIPTransportMgr.h"
#include "csipsectlsplugin.h"
#include "tsipsecmechanisminitparams.h"
#include "sipsecurityclientheader.h"
#include "sipsecurityserverheader.h"
#include "sipsecurityverifyheader.h"
#include "ctlscache.h"
#include "ctlsentry.h"

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::NewL
// -----------------------------------------------------------------------------
//
CSIPSecTlsPlugin* CSIPSecTlsPlugin::NewL( TAny* aInitParams )
	{
	__ASSERT_ALWAYS( aInitParams, User::Leave( KErrArgument ) );

	CSIPSecTlsPlugin* self = new ( ELeave ) CSIPSecTlsPlugin(
		static_cast< TSIPSecMechanismInitParams* >
			( aInitParams )->iTransportMgr );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::CSIPSecTlsPlugin
// -----------------------------------------------------------------------------
//
CSIPSecTlsPlugin::CSIPSecTlsPlugin( MSIPTransportMgr& aTransportMgr ) :
  	iTransportMgr( aTransportMgr )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::ConstructL()
    {
	iCache = CTLSCache::NewL();
    }

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::~CSIPSecTlsPlugin
// -----------------------------------------------------------------------------
//
CSIPSecTlsPlugin::~CSIPSecTlsPlugin()
    {
    delete iCache;
    }

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::Name
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPSecTlsPlugin::Name() const
	{
	_LIT8( KSIPSecTLSName, "tls");
	return KSIPSecTLSName;
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::InitializeSecurityClientL
// Making the offer.
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::InitializeSecurityClientL(
	CSIPSecurityClientHeader& aSecurityClient )
	{
    aSecurityClient.SetMechanismNameL( Name() );
    }

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::ProcessSecurityVerifyL
// After the framework has chosen security mechanism. it calls this function of
// the selected mechanism for all outgoing requests using the security
// agreement.
// Check that both Security-Server and Security-Verify header exist for this
// mechanism.
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::ProcessSecurityVerifyL(
	TSIPTransportParams& aTransportParams,
	CSIPRequest& /*aRequest*/,
	TInetAddr& aNextHop,
	const CUri8& /*aRemoteTarget*/,
	const TDesC8& /*aOutboundProxy*/,
	MSIPSecUser* aUser,
	TRegistrationId /*aRegistrationId*/,
	RPointerArray<CSIPSecurityServerHeader>& aSecurityServer,
	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify )
	{
	__ASSERT_ALWAYS( aUser, User::Leave( KErrArgument ) );

	TInt i = 0;
	CSIPSecurityServerHeader* secServer = NULL;
	for ( i = 0; i < aSecurityServer.Count() && !secServer; ++i )
		{
		if ( aSecurityServer[i]->MechanismName().CompareF( Name() ) == 0 )
            {
            secServer = aSecurityServer[i];
            }
		}
	__ASSERT_ALWAYS( secServer, User::Leave( KErrArgument ) );

	CSIPSecurityVerifyHeader* secVerify = NULL;
	for ( i = 0; i < aSecurityVerify.Count() && !secVerify; ++i )
		{
		if ( aSecurityVerify[i]->MechanismName().CompareF( Name() ) == 0 )
            {
            secVerify = aSecurityVerify[i];
            }
		}
	__ASSERT_ALWAYS( secVerify, User::Leave( KErrArgument ) );

	ProcessRequestL( aTransportParams, aNextHop, *aUser );
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::ProcessRequestL
// If no TLS connection to the next hop, create it.
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::ProcessRequestL( TSIPTransportParams& aTransportParams,
										const TInetAddr& aNextHop,	
										const MSIPSecUser& aSIPSecUser )
	{
	CTLSEntry* entry = iCache->SearchByNextHop( aNextHop );
	if ( !entry )
		{
		iTransportMgr.CreateTLSTransportL( aTransportParams );

		iCache->AddEntryL( aNextHop,
						   aTransportParams.TransportId(),
						   aSIPSecUser );
		}
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::AddSecurityParamsL
// Framework calls this function for all SIPSec plugins.
// -----------------------------------------------------------------------------
//
void
CSIPSecTlsPlugin::AddSecurityParamsL( TSIPTransportParams& aTransportParams,
								      CSIPRequest& /*aRequest*/,
								      TRegistrationId /*aRegistrationId*/,
								      TTransactionId /*aTransactionId*/,
					                  TInetAddr& aNextHop,
					                  const CUri8& /*aRemoteTarget*/,
					                  const TDesC8& /*aOutboundProxy*/,
									  MSIPSecUser* aUser )
	{
	__ASSERT_ALWAYS( aUser, User::Leave( KErrArgument ) );

	CTLSEntry* entry = iCache->SearchByNextHop( aNextHop );
	if ( entry )
		{
		aTransportParams.SetTransportId( entry->TransportId() );
		}	
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::ResponseReceivedL
// Framework calls this function for all SIPSec plugins.
// -----------------------------------------------------------------------------
//
TBool CSIPSecTlsPlugin::ResponseReceivedL(
	TSIPTransportParams& /*aTransportParams*/,
	CSIPResponse& /*aResponse*/,
	CSIPRequest& /*aRequest*/,
	TRegistrationId /*aRegistrationId*/,
	TTransactionId /*aTransactionId*/,
	TInetAddr& /*aNextHop*/,
	const CUri8& /*aRemoteTarget*/,
	const TDesC8& /*aOutboundProxy*/,
	MSIPSecUser* /*aUser*/,
	MSIPSecSecurityMechanismObserver& /*aObserver*/ )
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::IsServerInitiatedSecAgreeAllowed
// Framework calls this function if it has selected TLS as the security
// mechanism to use.
// Server initiated TLS must not be supported.
// -----------------------------------------------------------------------------
//
TBool CSIPSecTlsPlugin::IsServerInitiatedSecAgreeAllowed() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::ParametersUpdatedL
// -----------------------------------------------------------------------------
//
TBool CSIPSecTlsPlugin::ParametersUpdatedL( MSIPSecUser* /*aUser*/ )
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::CancelPendingOperations
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::CancelPendingOperations(
	MSIPSecSecurityMechanismObserver* /*aObserver*/ )
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::ClearCache
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::ClearCache( MSIPSecUser* aUser )
	{
	if ( aUser )
		{
		iCache->RemoveSIPSecUser( iTransportMgr, *aUser );
		}
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::SetCredentialsL
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::SetCredentialsL( TTransactionId /*aTransactionId*/,
                                        const TDesC8& /*aRealm*/,
						                const TDesC8& /*aOutboundProxy*/, 
									    const TDesC8& /*aUserName*/,
									    const TDesC8& /*aPassword*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::SetCredentialsL
// -----------------------------------------------------------------------------
//
void CSIPSecTlsPlugin::SetCredentialsL( const MSIPSecUser& /*aUser*/,
                                        const TDesC8& /*aRealm*/,
						                const TDesC8& /*aOutboundProxy*/, 
									    const TDesC8& /*aUserName*/,
									    const TDesC8& /*aPassword*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::IgnoreChallenge
// -----------------------------------------------------------------------------
//
TInt CSIPSecTlsPlugin::IgnoreChallenge( TTransactionId /*aTransactionId*/, 
                                        const TDesC8& /*aRealm*/,
                                        const MSIPSecUser* /*aTrustedUser*/ )
	{
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPSecTlsPlugin::RemoveCredentials
// -----------------------------------------------------------------------------
//	
TInt CSIPSecTlsPlugin::RemoveCredentials( const TDesC8& /*aRealm*/ )
	{
	return KErrNotFound;
 	}
