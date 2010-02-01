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
// Name          : CSIPSecCredentials.cpp
// Part of       : SIPDigestPlugin
// Version       : SIP/6.0
//



#include "CSIPSecCredentials.h"
#include "CSIPSecChallenge.h"
#include "MSIPSecAlgorithm.h"
#include "sipsecdigestcontext.h"
#include "CSIPSecUserRecord.h"
#include "MSIPSecUser.h"
#include "SipLogs.h"
#include "siprequest.h"
#include "uricontainer.h"
#include "sipauthorizationheader.h"
#include "sipproxyauthorizationheader.h"
#include "sipauthenticateheaderbase.h"
#include "sipauthenticationinfoheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSIPSecCredentials::NewL
// -----------------------------------------------------------------------------
//
CSIPSecCredentials*
CSIPSecCredentials::NewL( CSIPSecChallenge* aChallenge, 
                          CSIPSecDigest& aMechanism,
			              TSIPSecPluginCtxResponse& aContext )
    {
	__ASSERT_ALWAYS( aChallenge, User::Leave( KErrArgument ) );

	const MSIPSecUser* sipSecUser = aChallenge->Owner();
	if ( !sipSecUser )
        {
        sipSecUser = &aContext.SIPSecUser();
        }

    CSIPSecCredentials* self = new ( ELeave ) CSIPSecCredentials( aMechanism,											 					  
																  *sipSecUser );
	CleanupStack::PushL( self );
	self->ConstructL( aChallenge, aContext );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecCredentials::CSIPSecCredentials
// -----------------------------------------------------------------------------
//
CSIPSecCredentials::CSIPSecCredentials( CSIPSecDigest& aMechanism,
										const MSIPSecUser& aUser ) :
    CSIPSecDigestCacheEntry( aUser ),
    iMechanism( aMechanism )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::~CSIPSecCredentials
// -----------------------------------------------------------------------------
//
CSIPSecCredentials::~CSIPSecCredentials()
    {
    delete iChallenge;
    delete iAuthorizationHeader;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::ConstructL( CSIPSecChallenge* aChallenge,
									 TSIPSecPluginCtxResponse& aContext )
    {
    __ASSERT_ALWAYS( aChallenge, User::Leave( KErrArgument ) );

    CSIPSecDigestCacheEntry::ConstructL( ChallengeType( *aChallenge,
    													&aContext ),
										 aContext.RemoteTarget() );
    CreateContentL( *aChallenge );
	TSIPSecDigestCtxSetup ctx( *this, aContext.TransactionId(), &aContext );
    aChallenge->PopulateCredentialsL( ctx );

	// Take ownership of aChallenge when leave can't occur any more
	iChallenge = aChallenge;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::UpdateL
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::UpdateL( CSIPSecUserRecord& aUserCredentials )
    {
    // If user credentials update is for this entry
    if ( HoldsUserData( aUserCredentials ) && !UserData().IsUpdating() )
        {
        TSIPSecDigestCtxSetup ctx( *this, aUserCredentials.TransactionId() );
		TBool askCredentials( EFalse );

        // As credentials were obtained, DigestUpdateL must not indicate "wait".        
        __ASSERT_ALWAYS( !DigestUpdateL( ctx, askCredentials ),
        				 User::Leave( KErrGeneral ) );
		__ASSERT_ALWAYS( !askCredentials, User::Leave( KErrGeneral ) );
        }
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::UpdateL
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::UpdateL( TSIPSecPluginCtxRequest& aContext )
    {
	__SIP_LOG( "SIPSecCred:UpdateL(ctx)" )

	if ( UserData().IsValid() )
		{
		__SIP_LOG( "user data is valid" )

		TSIPSecDigestCtxProcess ctx( *this, &aContext );
	    UpdateContentL( ctx );

	    iChallenge->Algorithm().ProcessRequestL( ctx );

	    // Add a (Proxy-)Authorization header
		aContext.SIPRequest().AddHeaderL( Content() );
		}

	__SIP_LOG( "SIPSecCred:UpdateL(ctx) end" )
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::HandleL
// Called for each matching cache entry. They store the new nonce.
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::HandleL( TSIPSecPluginCtxInfo& aAuthenticationInfo )
    {
    RStringF cnonce = SIPStrings::StringF( SipStrConsts::ECNonce );
    RStringF nc 	= SIPStrings::StringF( SipStrConsts::ENonceCount );
    RStringF qop 	= SIPStrings::StringF( SipStrConsts::EQop );
    CSIPAuthenticationInfoHeader& authInfo =
    	aAuthenticationInfo.AuthenticationInfoHeader();

    if ( Content().ParamValue( qop ) 	   == authInfo.Value( qop ) &&
    	 Content().DesParamValue( cnonce ) == authInfo.DesValue( cnonce ) &&
    	 Content().ParamValue( nc )        == authInfo.Value( nc ) )
        {
		const TDesC8& responseAuth = authInfo.DesValue( SIPStrings::StringF(
			SipStrConsts::EResponseAuth ) );
        if ( responseAuth.Length() > 0 )
            {
            TBuf8< KSIPSecDigestHashHexSize > response;
            TSIPSecDigestAuthInfoContext ctx( *this,
            								  response,
            								  &aAuthenticationInfo );
            // Process the response auth
            iChallenge->Algorithm().ProcessRequestL( ctx );

            if ( responseAuth.Compare( response ) != 0 )
                {
                User::Leave( KErrPermissionDenied );
                }
            }

        const TDesC8& nextNonce = authInfo.DesValue(
        	SIPStrings::StringF( SipStrConsts::ENextNonce ) );
        if ( nextNonce.Length() > 0 )
            {
		    RStringF nonce = SIPStrings::StringF( SipStrConsts::ENonce );			              	    
            Content().SetDesParamL( nonce, nextNonce );
            iChallenge->Content().SetDesParamL( nonce, nextNonce );
            SetNonceCountL( 0 );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::HandleL
// Process Security-Verify header
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::HandleL( TSIPSecPluginCtxVerify& aSecurityVerifyCtx )
    {
    HBufC8* requestURI =
    	aSecurityVerifyCtx.SIPRequest().RequestURI()->ToTextL();
    CleanupStack::PushL( requestURI );

	TSIPSecDigestVerifyContext ctx( *this, *requestURI, &aSecurityVerifyCtx );
    iChallenge->Algorithm().ProcessRequestL( ctx );

    CleanupStack::PopAndDestroy( requestURI );
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::DigestUpdateL
// Generate CNonce if it doesn't exist.
// -----------------------------------------------------------------------------
//
TBool CSIPSecCredentials::DigestUpdateL( TSIPSecDigestCtxSetup& aContext,
										 TBool& aAskCredentials )
    {
    UpdateContentL( aContext );

	if ( iChallenge->HasQop() &&
    	 !Content().HasParam( SIPStrings::StringF( SipStrConsts::ECNonce ) ) )
        {
        iChallenge->Algorithm().GenerateCNonceL( aContext );
        }
    return iChallenge->Algorithm().ProcessResponseL( aContext,
    												 aAskCredentials );
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::ChallengeType
// -----------------------------------------------------------------------------
//
CSIPSecDigest::TChallengeType
CSIPSecCredentials::ChallengeType( const CSIPSecChallenge& aChallenge,
    							   const TSIPSecPluginContext* aContext ) const
    {
    CSIPSecDigest::TChallengeType type = aChallenge.Type();
    TBool ignoreOutboundProxy( aContext &&
                               aContext->OutboundProxy().Length() == 0 );

    if ( type == CSIPSecDigest::EProxy &&
         !ignoreOutboundProxy &&
    	 iUserCredentials &&
    	 iUserCredentials->OutboundProxy().Length() > 0 )
        {
        type = CSIPSecDigest::EOutboundProxy;
        }

    return type;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::Type
// -----------------------------------------------------------------------------
//
CSIPSecDigest::TChallengeType CSIPSecCredentials::Type() const
    {    
    return ChallengeType( *iChallenge );
    }
    
// -----------------------------------------------------------------------------
// CSIPSecCredentials::Type
// -----------------------------------------------------------------------------
//
CSIPSecDigest::TChallengeType CSIPSecCredentials::Type(
    const TSIPSecPluginContext& aContext ) const
    {    
    return ChallengeType( *iChallenge, &aContext );
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::DoesMatch
// aChallenge::Owner() is always NULL at this phase.
// -----------------------------------------------------------------------------
//
TBool CSIPSecCredentials::DoesMatch( const CSIPSecChallenge& aChallenge,
									 const MSIPSecUser& aUser,
									 TRegistrationId aRegistrationId ) const
	{
	return aChallenge.Type() == iChallenge->Type() &&
		   ( aChallenge.Realm().Compare( iChallenge->Realm() ) == 0 ) &&
		   iUserCredentials &&
		   iUserCredentials->CompareUser( aUser, aRegistrationId );
	}

// -----------------------------------------------------------------------------
// CSIPSecCredentials::ChallengeReceived
// If "stale=true", retry with new nonce. Username and password are valid.
// Otherwise ask username and password. PopulateCredentialsL later copies nonce
// to CSIPSecCredentials::iAuthorizationHeader.
// -----------------------------------------------------------------------------
//
TBool CSIPSecCredentials::ChallengeReceived( CSIPSecChallenge& aNewChallenge )
    {
	RStringF stale = SIPStrings::StringF( SipStrConsts::EStale );	

    TBool hasStale = aNewChallenge.Content().HasParam( stale );    
	_LIT8( KSIPSecStaleTrue, "true" );
    TBool isStale = hasStale &&
		aNewChallenge.Content().ParamValue( stale ).DesC().CompareF(
			KSIPSecStaleTrue ) == 0;
    TBool hasAuts =
    	Content().HasParam( SIPStrings::StringF( SipStrConsts::EAuts ) );
	TBool isAKA = aNewChallenge.Algorithm().AlgorithmName() ==
		 				SIPStrings::StringF( SipStrConsts::EAKAv1MD5 );

    if ( !isStale || hasAuts || isAKA )
        {
        // Credentials are wrong
        UserData().Invalidate( CSIPSecUserRecord::ENoState );

		RStringF nonce = SIPStrings::StringF( SipStrConsts::ENonce );
        if ( aNewChallenge.Content().DesParamValue( nonce ) !=
        	 Content().DesParamValue( nonce ) )
        	{
        	// Server gave a new nonce, clear old username and password so even
        	// if application gives same username and password, the request is
        	// sent. As nonce is different, the result will be different.
        	UserData().ClearUsernameAndPassword();
        	}
        }

    aNewChallenge.SetOwner( &iUser );

	// Remove unless SIPSec user asked the response and waits credentials
	TBool remove = iUser.PassOnlyRealmsToUser() || !UserData().IsUpdating();

	__SIP_INT_LOG1( "SIPSecCred:ChallRecv remove", remove )
	return remove;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::Mechanism
// -----------------------------------------------------------------------------
//
CSIPSecDigest& CSIPSecCredentials::Mechanism() const
    {
    return iMechanism;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::Content
// -----------------------------------------------------------------------------
//
CSIPAuthorizationHeaderBase& CSIPSecCredentials::Content()
    {
    return *iAuthorizationHeader;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::Challenge
// -----------------------------------------------------------------------------
//
CSIPSecChallenge& CSIPSecCredentials::Challenge()
    {
    return *iChallenge;
    }
    
// -----------------------------------------------------------------------------
// CSIPSecCredentials::CreateContentL
// Delete old AuthorizationHeader when can't leave, so it retains its old value
// in case of leave. Otherwise crashes e.g. in CSIPSecCredentials::Content that
// expects iAuthorizationHeader to exist.
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::CreateContentL( CSIPSecChallenge& aChallenge )
    {
	TBool hasType = aChallenge.Type() == CSIPSecDigest::EEndPoint || 
                    aChallenge.Type() == CSIPSecDigest::EProxy;
    __ASSERT_ALWAYS( hasType, User::Leave( KErrArgument ) );

    // Create a new authorization header
    CSIPAuthorizationHeaderBase* authorization( NULL );
	RStringF digest = SIPStrings::Pool().OpenFStringL( KSIPSecDigestScheme );
	CleanupClosePushL( digest );

    if ( aChallenge.Type() == CSIPSecDigest::EEndPoint )
        {
        authorization = CSIPAuthorizationHeader::NewL( digest );
        }
    else
        {
        authorization = CSIPProxyAuthorizationHeader::NewL( digest );
        }

    CleanupStack::PopAndDestroy(); // digest
    CleanupStack::PushL( authorization );

	if ( aChallenge.HasQop() )
		{
		RStringF qop =
			SIPStrings::Pool().OpenFStringL( aChallenge.QopDescriptor() );
		CleanupClosePushL( qop );
		authorization->SetParamL( SIPStrings::StringF( SipStrConsts::EQop ),
													   qop );
        CleanupStack::PopAndDestroy(); // qop
		}

	CleanupStack::Pop( authorization );
	delete iAuthorizationHeader;
    iAuthorizationHeader = authorization;
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::UpdateContentL
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::UpdateContentL( TSIPSecDigestCtxSetup& aContext )
    {
    aContext.SetDesParamValueL( SipStrConsts::EUserName,
    						 	aContext.UserData().UserName() );
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::SetNonceCountL
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::SetNonceCountL( TUint aNonceCount )
    {
    iNonceCount = aNonceCount;

    const TInt KNonceCountLength = 8;
	TBuf8< KNonceCountLength > ncDescr;
	ncDescr.NumFixedWidth( iNonceCount, EHex, KNonceCountLength );

    TSIPSecDigestContext::SetParamValueL( Content(),
    									  SipStrConsts::ENonceCount,
    									  ncDescr );
    }

// -----------------------------------------------------------------------------
// CSIPSecCredentials::UpdateContentL
// Increment nonce count and set Request-URI.
// -----------------------------------------------------------------------------
//
void CSIPSecCredentials::UpdateContentL( TSIPSecDigestCtxProcess& aContext )
    {
    if ( iChallenge->HasQop() )
        {
        SetNonceCountL( iNonceCount + 1 );
        }

    TSIPSecPluginCtxRequest& parent =
    	static_cast<TSIPSecPluginCtxRequest&>( aContext.Parent() );
    HBufC8* requestURI = parent.SIPRequest().RequestURI()->ToTextL();
    CleanupStack::PushL( requestURI );

    aContext.SetParamValueL( SipStrConsts::EUri, *requestURI );
    CleanupStack::PopAndDestroy( requestURI );
    }
