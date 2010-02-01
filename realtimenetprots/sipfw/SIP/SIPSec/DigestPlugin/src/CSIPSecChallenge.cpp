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
// Name          : CSIPSecChallenge.cpp
// Part of       : SIPDigestPlugin
// Version       : SIP/6.0
//



#include "CSIPSecChallenge.h"
#include "sipproxyauthenticateheader.h"
#include "sipwwwauthenticateheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipresponse.h"
#include "SipAssert.h"
#include "MSIPSecUser.h"
#include "sipsecdigestcontext.h"
#include "sipsecdigestcache.h"

// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CSIPSecChallenge::CSIPSecChallenge
// -----------------------------------------------------------------------------
//
CSIPSecChallenge::CSIPSecChallenge( CSIPSecDigest::TChallengeType aType,
									CSIPSecRequestData::TQop aQop ) :
    iType( aType ),
    iQop( aQop ),
    iOwner( NULL )
    {
    __SIP_ASSERT_RETURN( iQop != CSIPSecRequestData::EUnknown, KErrArgument );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::~CSIPSecChallenge
// -----------------------------------------------------------------------------
//
CSIPSecChallenge::~CSIPSecChallenge()
    {
    delete iAuthenticateHeader;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::Type
// -----------------------------------------------------------------------------
//
CSIPSecDigest::TChallengeType CSIPSecChallenge::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::Realm
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPSecChallenge::Realm() const
    {
	return TSIPSecDigestContext::GetDesParam( Content(), SipStrConsts::ERealm );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::Content
// -----------------------------------------------------------------------------
//
CSIPAuthenticateHeaderBase& CSIPSecChallenge::Content()
    {
    return *iAuthenticateHeader;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::Content
// -----------------------------------------------------------------------------
//
const CSIPAuthenticateHeaderBase& CSIPSecChallenge::Content() const
    {
    return *iAuthenticateHeader;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::Qop
// -----------------------------------------------------------------------------
//
CSIPSecRequestData::TQop CSIPSecChallenge::Qop() const
	{
	return iQop;	
	}

// -----------------------------------------------------------------------------
// CSIPSecChallenge::QopDescriptor
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPSecChallenge::QopDescriptor() const
	{
	switch ( iQop )
		{
    case CSIPSecRequestData::EAuth:
    	return KSIPSecAuth;
    
    case CSIPSecRequestData::EAuthInt:
    	return KSIPSecAuthInt;

    // Can't be CSIPSecRequestData::EUnknown, it is checked in constructor    	
    case CSIPSecRequestData::EDoesNotExist:
    default:
    	return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CSIPSecChallenge::HasQop
// -----------------------------------------------------------------------------
//
TBool CSIPSecChallenge::HasQop() const
	{
	return iQop != CSIPSecRequestData::EDoesNotExist;
	}

// -----------------------------------------------------------------------------
// CSIPSecChallenge::Owner
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSIPSecChallenge::Owner() const
    {
    return iOwner;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::SetOwner
// -----------------------------------------------------------------------------
//
void CSIPSecChallenge::SetOwner( const MSIPSecUser* aOwner )
    {
    iOwner = aOwner;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::ConstructL
// -----------------------------------------------------------------------------
//
void
CSIPSecChallenge::ConstructL( CSIPAuthenticateHeaderBase& aAuthenticateHeader )
    {
    __ASSERT_DEBUG( aAuthenticateHeader.HasParam(
    	SIPStrings::StringF( SipStrConsts::ERealm ) ) &&
       	aAuthenticateHeader.HasParam(
       		SIPStrings::StringF( SipStrConsts::ENonce ) ),
    	User::Leave( KErrArgument ) );
	__ASSERT_ALWAYS( iType == CSIPSecDigest::EProxy ||
    				 iType == CSIPSecDigest::EEndPoint,
    				 User::Leave( KErrArgument ) );

    iAuthenticateHeader = dynamic_cast<CSIPAuthenticateHeaderBase*>
    	( aAuthenticateHeader.CloneL() );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::PopulateCredentialsL
// Copy parameters from challenge to CSIPSecCredentials' authorization header.
// This function is used when handling a SIP response, so transaction id always
// exists.
// -----------------------------------------------------------------------------
//
void
CSIPSecChallenge::PopulateCredentialsL( TSIPSecDigestCtxSetup& aContext ) const
    {
    __ASSERT_DEBUG( aContext.TransactionId() != KEmptyTransactionId,
    				User::Panic( _L( "SIPSecChall:PopCreds" ), KErrArgument ) );

	FillCredentialParamL( aContext, SipStrConsts::ERealm );
	FillCredentialParamL( aContext, SipStrConsts::ENonce );
	FillCredentialParamL( aContext, SipStrConsts::EOpaque );
	aContext.SetParamValueL( SipStrConsts::EAlgorithm,
		Content().ParamValue(
			SIPStrings::StringF( SipStrConsts::EAlgorithm ) ).DesC() );


	TSIPSecPluginCtxResponse& parent =
    	static_cast< TSIPSecPluginCtxResponse& >( aContext.Parent() );

    const TDesC8& realm = Realm();
    __ASSERT_ALWAYS( realm.Length() > 0, User::Leave( KErrGeneral ) );
    MSIPSecUser& user = parent.SIPSecUser();

	CSIPSecUserRecord* record = parent.Cache().SearchRecordForResponse(
		realm, user, parent.RegistrationId() );
    if ( record )
        {
        // Found from cache, use it
	// This user will be attached to the record.
		VerifyTypeL( record->Type() );
		if  ( (aContext.GetParam(SipStrConsts::EAlgorithm) == 
        SIPStrings::StringF( SipStrConsts::EAKAv1MD5 ).DesC()) &&
        record->Type() == CSIPSecUserRecord::EIMS)
			{
			record->Invalidate( CSIPSecUserRecord::ENoState );
			record->ClearUsernameAndPassword();	
			}
        }
    else
        {
        // Not found from the cache, create a new one.
        record = CreateUserRecordL( realm,
        							user,
        							parent.RegistrationId(),
        							aContext.TransactionId(),
									parent.SIPResponse().ResponseCode() );
        parent.Cache().AddUserCredentials( record );
        }

    aContext.SetUserData( *record );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallenge::FillCredentialParamL
// -----------------------------------------------------------------------------
//
void CSIPSecChallenge::FillCredentialParamL( TSIPSecDigestCtxSetup& aContext,
											 TInt aParam ) const
    {
    aContext.SetDesParamValueL( aParam,
		Content().DesParamValue( SIPStrings::StringF( aParam ) ) );
    }
