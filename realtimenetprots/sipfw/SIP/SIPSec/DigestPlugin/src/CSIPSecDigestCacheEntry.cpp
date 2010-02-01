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
// Name          : CSIPSecDigestCacheEntry.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "CSIPSecDigestCacheEntry.h"
#include "sipsecplugincontext.h"
#include "CSIPSecUserRecord.h"
#include "CSIPSecSIMRecord.h"
#include "MSIPSecUser.h"
#include <uri8.h>


const TInt CSIPSecDigestCacheEntry::iSIPSecOffset =
	_FOFF( CSIPSecDigestCacheEntry, iLink );


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::CSIPSecDigestCacheEntry
// ----------------------------------------------------------------------------
//
CSIPSecDigestCacheEntry::CSIPSecDigestCacheEntry( const MSIPSecUser& aUser ) :
	iUser( aUser )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCacheEntry::ConstructL( CSIPSecDigest::TChallengeType aType,
										  const CUri8& aRemoteTarget )
	{
	if ( aType == CSIPSecDigest::EEndPoint )
	    {
    	iRemoteTarget = CUri8::NewL( aRemoteTarget.Uri() );
	    }
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::~CSIPSecDigestCacheEntry
// ----------------------------------------------------------------------------
//
CSIPSecDigestCacheEntry::~CSIPSecDigestCacheEntry()
	{	
	delete iRemoteTarget;	
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::UserData
// Once CSIPSecDigestCacheEntry has been constructed, iUserCredentials always
// exist.
// -----------------------------------------------------------------------------
//
CSIPSecUserRecord& CSIPSecDigestCacheEntry::UserData()
    {
    __ASSERT_ALWAYS( iUserCredentials,
    	User::Panic( _L( "DigestCacheEntry:UserData !cred" ), KErrNotFound ) );

    return *iUserCredentials;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::HoldsUserData
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestCacheEntry::HoldsUserData(
	const CSIPSecUserRecord& aUserData ) const
    {
    return iUserCredentials == &aUserData;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::SetUserData
// -----------------------------------------------------------------------------
//
void CSIPSecDigestCacheEntry::SetUserData( CSIPSecUserRecord& aUserData )
    {    
    iUserCredentials = &aUserData;
    iUserCredentials->CredentialsAttach();
    }

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::Match
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestCacheEntry::Match( TSIPSecPluginContext& aContext ) const
    {
    __ASSERT_ALWAYS( iUserCredentials,
    	User::Panic( _L( "DigestCacheEntry:Match !cred" ), KErrNotFound ) );

	if ( IsUsedByUser( aContext.SIPSecUser(),
					   aContext.AlsoTrustedUser(),
					   aContext.RegistrationId() ) )
		{
	    switch ( Type( aContext ) )
	        {
	        case CSIPSecDigest::EOutboundProxy:
	            return iUserCredentials->OutboundProxy().CompareF(
	            	aContext.OutboundProxy() ) == 0;

	        case CSIPSecDigest::EProxy:
				return ETrue; // IsUsedByUser checked that SIPSec user is same

	        default: // CSIPSecDigest::EEndPoint:
	        	return iRemoteTarget &&
			   		   ( iRemoteTarget->Uri().Equivalent(
			   		   		aContext.RemoteTarget().Uri() ) == KErrNone );
	        }
		}

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::IsUsedByUser
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestCacheEntry::IsUsedByUser( const MSIPSecUser& aUser,
	TBool aAlsoTrustedUser,
	TRegistrationId aRegistrationId ) const
	{
	return CompareUsers( aUser, iUser, aAlsoTrustedUser, aRegistrationId );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::CompareUsers
// SIPSec user can have only one trusted user. I.e. a trusted user doesn't
// have another trusted user.
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestCacheEntry::CompareUsers( const MSIPSecUser& aUser,
						  					 const MSIPSecUser& aCachedUser,
						  					 TBool aAlsoTrustedUser,
						  					 TRegistrationId aRegistrationId )
	{
	MSIPSecUser* user = const_cast< MSIPSecUser* >( &aUser );
	return ( user == &aCachedUser ) ||
		   ( aAlsoTrustedUser &&
		   	 user->TrustedUser( aRegistrationId ) == &aCachedUser );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCacheEntry::SIPSecUser
// ----------------------------------------------------------------------------
//
const MSIPSecUser& CSIPSecDigestCacheEntry::SIPSecUser() const
	{
	return iUser;
	}
