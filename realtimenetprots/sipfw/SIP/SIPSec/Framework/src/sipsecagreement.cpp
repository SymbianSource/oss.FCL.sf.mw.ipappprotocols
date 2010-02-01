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
// Name          : sipsecagreement.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "sipsecagreement.h"
#include "siperr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "SipSecUtils.h"
#include "sipsecurityclientheader.h"
#include "sipsecurityserverheader.h"
#include "sipsecurityverifyheader.h"
#include "sipstrings.h"
#include "TSIPSecMechanismIter.h"
#include "MSIPSecSecurityMechanism.h"
#include "mechanismstore.h"
#include "ClientMechanisms.h"

const TInt CSIPSecAgreement::iOffset = _FOFF( CSIPSecAgreement, iLink );

// ----------------------------------------------------------------------------
// CSIPSecAgreement::NewLC
// Server initiated security agreement
// ----------------------------------------------------------------------------
//
CSIPSecAgreement*
CSIPSecAgreement::NewLC( const TInetAddr& aNextHopAddr,
						 const TDesC8& aNextHop,
						 const MSIPSecUser& aUser,
						 MSIPSecMechanismStore& aMechanismStore )
	{	
	CSIPSecAgreement* self = new ( ELeave ) CSIPSecAgreement( aNextHopAddr,														   
											  			      aMechanismStore );
	CleanupStack::PushL( self );
	self->ConstructL( aNextHop, aUser );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::CSIPSecAgreement
// ----------------------------------------------------------------------------
//	
CSIPSecAgreement::CSIPSecAgreement( const TInetAddr& aNextHopAddr,								   
								    MSIPSecMechanismStore& aMechanismStore ) :
	iNextHopAddr( aNextHopAddr ),
	iMechanism( NULL ),
  	iMechanismStore( aMechanismStore )
#ifdef CPPUNIT_TEST
	// Use array granularity 1 to get every append operation to allocate memory
	, iUsers( 1 ),
  	iSecurityServer( 1 ),
  	iSecurityVerify( 1 )
#endif
	{
	}

// -----------------------------------------------------------------------------
// CSIPSecAgreement::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecAgreement::ConstructL( const TDesC8& aNextHop,
								   const MSIPSecUser& aUser )
	{
	iNextHop = aNextHop.AllocL();
	iClientMechanisms = CSIPSecClientMechanisms::NewL();
	iUsers.AppendL( &aUser );
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::~CSIPSecAgreement
// ----------------------------------------------------------------------------
//	
CSIPSecAgreement::~CSIPSecAgreement()
	{
	delete iNextHop;	
	delete iClientMechanisms;
	iUsers.Reset();
	iSecurityServer.ResetAndDestroy();
	iSecurityVerify.ResetAndDestroy();
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::NextHop
// Port should not be compared.
// ----------------------------------------------------------------------------
//	
TBool CSIPSecAgreement::NextHop( const TInetAddr& aNextHopAddr ) const
	{
	return iNextHopAddr.Match( aNextHopAddr );
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::NextHop
// ----------------------------------------------------------------------------
//	
TBool CSIPSecAgreement::NextHop( const TDesC8& aNextHop ) const
	{
	return aNextHop.CompareF( *iNextHop ) == 0;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::MechanismNameL
// ----------------------------------------------------------------------------
//	
HBufC8* CSIPSecAgreement::MechanismNameL() const
	{
	if ( iMechanism )
		{
		return iMechanism->Name().AllocL();
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPSecAgreement::ProcessSecurityClientL
// -----------------------------------------------------------------------------
//
void CSIPSecAgreement::ProcessSecurityClientL( CSIPRequest& aRequest ) const
	{
	__TEST_INVARIANT;

	RStringF secClientStr =
		SIPStrings::StringF( SipStrConsts::ESecurityClientHeader );
	__ASSERT_ALWAYS( aRequest.HasHeader( secClientStr ),
					User::Leave( KErrNotFound ) );
	iClientMechanisms->RemoveAll();

	TSglQueIter<CSIPHeaderBase> iter = aRequest.Headers( secClientStr );
	CSIPHeaderBase* header = NULL;
	CSIPSecurityClientHeader* secClient = NULL;

	// Let the plugin listed in Security-Client fill its parameters to the
	// Security-Client header.
	while ( ( header = iter++ ) != NULL )
		{
		secClient = static_cast<CSIPSecurityClientHeader*>( header );

		// Security-Client must not have preference parameter
		secClient->DeleteParam( SIPStrings::StringF( SipStrConsts::EQ ) );

		const TDesC8& mechanismName = secClient->MechanismName();
		MSIPSecSecurityMechanism* mechanism =
			iMechanismStore.MechanismByName( mechanismName );		
		// Application must not indicate an unsupported mechanism
		__ASSERT_ALWAYS( mechanism,	User::Leave( KErrNotFound ) );

		// Do this before InitializeSecurityClientL, which sets the
		// mechanism name to the header.
		iClientMechanisms->AddL( mechanismName );
		mechanism->InitializeSecurityClientL( *secClient );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::CreateSecurityVerifyL
// ----------------------------------------------------------------------------
//
void CSIPSecAgreement::CreateSecurityVerifyL()
	{
	__TEST_INVARIANT;
	// Error if no Security-Server headers were received in the response
	__ASSERT_ALWAYS( iSecurityServer.Count() > 0, User::Leave( KErrNotFound ) );
	// Remove the possibly existing old headers
	iSecurityVerify.ResetAndDestroy();

	for ( TInt i = 0; i < iSecurityServer.Count(); ++i )
		{
		CSIPSecurityVerifyHeader* secVerify =
			CSIPSecurityVerifyHeader::NewLC( *iSecurityServer[ i ] );
		iSecurityVerify.AppendL( secVerify );
		CleanupStack::Pop( secVerify );
		}

	__TEST_INVARIANT;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::CopySecurityVerifyToRequestL
// A refreshed request may already have Security-Verify. Remove them as the
// current Security-Verify headers may be different due to a re-negotiation.
// ----------------------------------------------------------------------------
//
void
CSIPSecAgreement::CopySecurityVerifyToRequestL( CSIPRequest& aRequest ) const
	{
	__TEST_INVARIANT;

	if ( !IsExceptionRequest( aRequest ) )
		{
		aRequest.DeleteHeaders(
			SIPStrings::StringF( SipStrConsts::ESecurityVerifyHeader ) );

		for ( TInt i = 0; i < iSecurityVerify.Count(); ++i )
			{
			CSIPSecurityVerifyHeader* secVerify =
				static_cast<CSIPSecurityVerifyHeader*>
					( iSecurityVerify[i]->CloneL() );
			CleanupStack::PushL( secVerify );
			aRequest.AddHeaderL( secVerify );
			CleanupStack::Pop( secVerify );
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::CopySecurityServerFromResponseL
// ----------------------------------------------------------------------------
//
void
CSIPSecAgreement::CopySecurityServerFromResponseL( CSIPResponse& aResponse )
	{
	__TEST_INVARIANT;	

	RStringF secServer =
		SIPStrings::StringF( SipStrConsts::ESecurityServerHeader );
	if ( !aResponse.HasHeader( secServer ) )
		{
		User::Leave( KErrNotFound );
		}

	// Remove the possibly existing old headers
	iSecurityServer.ResetAndDestroy();

	TSglQueIter<CSIPHeaderBase> iter = aResponse.Headers( secServer );
	for ( CSIPHeaderBase* header = iter++; header; header = iter++ )
		{
		CSIPSecurityServerHeader* secServerHeader =
			static_cast<CSIPSecurityServerHeader*>( header->CloneL() );
		CleanupStack::PushL( secServerHeader );
		iSecurityServer.AppendL( secServerHeader );
		CleanupStack::Pop( secServerHeader );
		}

	__TEST_INVARIANT;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::HandleRequestL
// ----------------------------------------------------------------------------
//
void CSIPSecAgreement::HandleRequestL( TSIPTransportParams& aTransportParams,
									   CSIPRequest& aRequest,
									   const CUri8& aRemoteTarget,
									   const TDesC8& aOutboundProxy,
									   MSIPSecUser& aUser,
									   TRegistrationId aRegistrationId )
	{
	__TEST_INVARIANT;

	AddUserIfNotStoredL( aUser );

	RStringF secClient =
		SIPStrings::StringF( SipStrConsts::ESecurityClientHeader );
	
	switch ( CurrentState() )
		{
		case EWaitResponse:
			if ( aRequest.HasHeader( secClient ) )
				{
				ProcessSecurityClientL( aRequest );
				}
			break;

		case EMechanismSelected:
			CreateSecurityVerifyL();
			//lint -fallthrough

		case EVerified:			
		default:	
			if ( !SipSecUtils::Match( SipStrConsts::ERegister,
									  aRequest.Method() ) )
				{
				aRequest.DeleteHeaders( secClient );
				}			
			iMechanism->ProcessSecurityVerifyL( aTransportParams,
											    aRequest,
											    iNextHopAddr, 
											    aRemoteTarget,
											    aOutboundProxy,
											    &aUser,
											    aRegistrationId,
											    iSecurityServer,
											    iSecurityVerify );
			CopySecurityVerifyToRequestL( aRequest );
		}

	__TEST_INVARIANT;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::ResponseReceivedL
// If a security-agreement related response containing Security-Server is
// received, (re)select the mechanism.
// Response is not received in EMechanismSelected state, as when the updated
// request is sent, agreement enters EVerified state.
// ----------------------------------------------------------------------------
//
void CSIPSecAgreement::ResponseReceivedL( CSIPResponse& aResponse,
										  const MSIPSecUser& aUser,
										  TBool aUseSecurityServerHeaders )
	{
	__TEST_INVARIANT;

	AddUserIfNotStoredL( aUser );

	TBool hasSecurityServer = aResponse.HasHeader(
		SIPStrings::StringF( SipStrConsts::ESecurityServerHeader ) );
	if ( hasSecurityServer &&
		 IsSecAgreeResponse( aResponse.ResponseCode() ) &&
		 aUseSecurityServerHeaders )
		{
		// Clear old Security-Verify and mechanism, causing agreement enter
		// EWaitResponse state.
		iSecurityVerify.ResetAndDestroy();
		iMechanism = NULL;
		}

	if ( CurrentState() == EWaitResponse )
		{
		// Don't accept a direct 2xx to the initial request which had
		// Security-Client and thus expects a challenge.
		__ASSERT_ALWAYS( aResponse.Type() != CSIPResponse::E2XX,
						 User::Leave( KErrNotFound ) );		
		if ( IsSecAgreeResponse( aResponse.ResponseCode() ) )
			{			
			// Security-Server required, as request had Security-Client.
			__ASSERT_ALWAYS( hasSecurityServer, User::Leave( KErrNotFound ) );

			if ( aUseSecurityServerHeaders )
				{
				ChooseMechanismL( aResponse );

				// Check if server initiated sec-agree is allowed
				if ( iClientMechanisms->IsEmpty() &&
					 !iMechanism->IsServerInitiatedSecAgreeAllowed() )
					{
					User::Leave( KErrNotSupported );
					}
				}
			}
		}

	__TEST_INVARIANT;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::ChooseMechanismL
// Select a common mechanism with the highest preference.
// If application set Security-Client, look only from those mechanisms.
// Otherwise look from all mechanisms.
// ----------------------------------------------------------------------------
//
void CSIPSecAgreement::ChooseMechanismL( CSIPResponse& aResponse )
	{
	__TEST_INVARIANT;

	CopySecurityServerFromResponseL( aResponse );

	// Highest preference of a common mechanism found so far
	TReal highestPreference = -1.0;

	for ( TInt i = 0; i < iSecurityServer.Count(); ++i )
		{
		if ( iSecurityServer[i]->PreferenceParam() > highestPreference )
			{
			const TDesC8& name = iSecurityServer[i]->MechanismName();
			if ( iClientMechanisms->IsEmpty() ||
				 iClientMechanisms->IsPresent( name ) )
				{
				MSIPSecSecurityMechanism* mechanism =
					iMechanismStore.MechanismByName( name );
				if ( mechanism )
					{
					iMechanism = mechanism;
					highestPreference = iSecurityServer[ i ]->PreferenceParam();
					}
				}
			}
		}

	__ASSERT_ALWAYS( iMechanism, User::Leave( KErrNotFound ) );

	__TEST_INVARIANT;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::AddUserIfNotStoredL
// ----------------------------------------------------------------------------
//
void CSIPSecAgreement::AddUserIfNotStoredL( const MSIPSecUser& aUser )
	{
	if ( iUsers.Find( &aUser ) == KErrNotFound )
		{
		iUsers.AppendL( &aUser );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::RemoveUser
// ----------------------------------------------------------------------------
//
TBool CSIPSecAgreement::RemoveUser( const MSIPSecUser& aUser )
	{
	TInt pos = iUsers.Find( &aUser );
	if ( pos >= 0 )
		{
		iUsers.Remove( pos );
		iUsers.Compress();
		}

	return iUsers.Count() == 0;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::IsExceptionRequestL
// ----------------------------------------------------------------------------
//
TBool CSIPSecAgreement::IsExceptionRequest( const CSIPRequest& aRequest ) const
	{
	return ( SipSecUtils::Match( SipStrConsts::EAck, aRequest.Method() ) ||
			 SipSecUtils::Match( SipStrConsts::EPrack, aRequest.Method() ) ||
			 SipSecUtils::Match( SipStrConsts::ECancel, aRequest.Method() ) );
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::CurrentState
// ----------------------------------------------------------------------------
//
CSIPSecAgreement::TState CSIPSecAgreement::CurrentState() const
	{
	__TEST_INVARIANT;

	if ( iSecurityVerify.Count() > 0 )
		{		
		return EVerified;
		}

	if ( iMechanism != NULL )
		{
		return EMechanismSelected;
		}

	return EWaitResponse;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreement::IsSecAgreeResponse
// ----------------------------------------------------------------------------
//
TBool CSIPSecAgreement::IsSecAgreeResponse( TUint aResponseCode )
	{
	switch ( aResponseCode )
		{
	case 401:
	case 407:
	case 421:
	case 494:
		return ETrue;
	default:
		return EFalse;
		}
	}

// -----------------------------------------------------------------------------
// CSIPSecAgreement::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPSecAgreement::__DbgTestInvariant() const
	{
	// Mechanism must be selected before Security-Verify headers can be created
    if ( iSecurityVerify.Count() > 0 && !iMechanism )
        {
		User::Invariant();
		}
	}

