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
// Name          : RSIPSecChallengeResolver.cpp
// Part of       : SIPDigestPlugin
// Version       : SIP/6.0
//



#include "siperr.h"
#include "SipAssert.h"
#include "sipsecurityserverheader.h"
#include "sipauthenticateheaderbase.h"
#include "CSIPSecDigest.h"
#include "RSIPSecChallengeResolver.h"
#include "sipsecdigestcache.h"
#include "CSIPSecChallengeMD5.h"
#include "CSIPSecChallengeAKA.h"
#include "CSIPSecCredentials.h"
#include "sipresponse.h"
#include "sipsecplugincontext.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// CONSTANTS
const TInt KSIPSecMaxChallengesPerResponse( 10 );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::RSIPSecChallengeResolver
// -----------------------------------------------------------------------------
//
RSIPSecChallengeResolver::RSIPSecChallengeResolver(
	TSIPSecPluginCtxResponse& aContext,
	CSIPSecDigest& aMechanism ) :
	RPointerArray< CSIPSecChallenge >(),
   	iContext( &aContext ),
   	iMechanism( aMechanism )
    {
    }

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::Close
// -----------------------------------------------------------------------------
//
void RSIPSecChallengeResolver::Close()
    {
    ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::OpenL
// -----------------------------------------------------------------------------
//
void RSIPSecChallengeResolver::OpenL( const MSIPSecUser& aUser )
    {
    CSIPResponse& response = iContext->SIPResponse();
    RStringF algorithm;
	RStringF qop;
	TBool digestInSecServer =
		GetDigestParamsFromSecurityServer( response, algorithm, qop );

    ResetAndDestroy();
    const RStringF proxyAuth =
    	SIPStrings::StringF( SipStrConsts::EProxyAuthenticateHeader );

    TBool digestInAuthHeaders( EFalse );
    
    TInt challengeCount = 0;    
    if ( response.HeaderCount( proxyAuth ) > 0 )
        {
    	TSglQueIter< CSIPHeaderBase > authHeaders =
    		response.Headers( proxyAuth );
    	FindAndCreateChallengesL( authHeaders, CSIPSecDigest::EProxy,
    	                          challengeCount, algorithm, qop, 
    	                          digestInAuthHeaders );
        }

    const RStringF wwwAuth =
    	SIPStrings::StringF( SipStrConsts::EWWWAuthenticateHeader );
    if ( response.HeaderCount( wwwAuth ) > 0 &&
    	 challengeCount < KSIPSecMaxChallengesPerResponse )
        {
    	TSglQueIter< CSIPHeaderBase > authHeaders = response.Headers( wwwAuth );
    	FindAndCreateChallengesL( authHeaders, CSIPSecDigest::EEndPoint, 
    	                          challengeCount, algorithm, qop,
    	                          digestInAuthHeaders );
        }

    PrepareCache( aUser );

	// If 401/407 has no www-authenticate or proxy-authenticate headers or all
	// challenges are ignored => error.
	if ( ( digestInSecServer || digestInAuthHeaders ) &&
		 ( response.ResponseCode() == 401 || response.ResponseCode() == 407 ) &&
		 Count() == 0 )
        {
        User::Leave( KErrSIPForbidden );
        }	
    }

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::FindAndCreateChallengesL
// -----------------------------------------------------------------------------
//
void RSIPSecChallengeResolver::FindAndCreateChallengesL(
	TSglQueIter< CSIPHeaderBase >& aAuthHeaders,
	CSIPSecDigest::TChallengeType aType,
	TInt& aCount,
	RStringF aAlgorithm,
	RStringF aQop,
	TBool& aDigestInAuthHeaders )
    {
    while ( aAuthHeaders )
        {
        CSIPAuthenticateHeaderBase* header =
            static_cast< CSIPAuthenticateHeaderBase* >( aAuthHeaders++ );
        if ( IsDigestChallenge( *header ) )
            {
            aDigestInAuthHeaders = ETrue;
        	if ( IsValidChallenge( *header ) &&
        		 aCount < KSIPSecMaxChallengesPerResponse )
	        	{
	            CreateChallengeL( *header, aType, aCount, aAlgorithm, aQop );
	        	}
            }
        }
    }

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::Pop
// -----------------------------------------------------------------------------
//
CSIPSecChallenge* RSIPSecChallengeResolver::Pop()
    {
    CSIPSecChallenge* challenge( NULL );
    if ( Count() > 0 )
        {
        challenge = ( *this )[ 0 ];
        Remove( 0 );
        }

    return challenge;
    }

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::PrepareCache
// Ignore challenges that match a cache entry, but have different algorithm.
// Then check if any matching cache entries must be removed.
// -----------------------------------------------------------------------------
//
void RSIPSecChallengeResolver::PrepareCache( const MSIPSecUser& aUser )
    {
    TRegistrationId regId = iContext->RegistrationId();
    CSIPSecDigestCacheEntry* entry( NULL );
    CSIPSecCredentials* credentials( NULL );
    TInt i( 0 );

	// Start search from the end of the array, so if a challenge is removed, it
	// won't affect the position of challenges that haven't yet been checked.
	for ( i = Count() - 1; i >= 0; i-- )
        {
        CSIPSecChallenge* challenge = ( *this )[ i ];
        TSIPSecDigestCacheIterator iterator( *iContext );
        iContext->Cache().InitializeIterator( iterator );

        while ( ( entry = iterator.Next() ) != NULL && challenge )
            {
            credentials = static_cast< CSIPSecCredentials* >( entry );
			if ( credentials->DoesMatch( *challenge, aUser, regId ) &&
				 MixedAlgorithms( *credentials, *challenge ))
				{
    			Remove( i );
    			delete challenge;
    			challenge = NULL; // exits while-loop
				}
            }
        }

    for ( i = 0; i < Count(); i++ )
        {
        CSIPSecChallenge* challenge = ( *this )[ i ];
        TSIPSecDigestCacheIterator iterator( *iContext );
        iContext->Cache().InitializeIterator( iterator );

        while ( ( entry = iterator.Next() ) != NULL )
            {
            credentials = static_cast< CSIPSecCredentials* >( entry );
			if ( credentials->Type( *iContext ) == CSIPSecDigest::EProxy )
				{
				__SIP_ASSERT_RETURN(
					credentials->IsUsedByUser(iContext->SIPSecUser(),
											  ETrue,
											  regId ),
                  	KErrGeneral );
				}

            if ( credentials->DoesMatch( *challenge, aUser, regId ) &&
        		 credentials->ChallengeReceived( *challenge ) )
                {
                iContext->Cache().RemoveEntry( *credentials );
                }
            }
        }

	RemoveObsoleteAKAEntries();
    }

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::RemoveObsoleteAKAEntries
// Don't compare SIPSec user. Cache can have only one CSIPSecSIMRecord,
// regardless of SIPSec user.
// -----------------------------------------------------------------------------
//
void RSIPSecChallengeResolver::RemoveObsoleteAKAEntries() const
	{
	for ( TInt i = 0; i < Count(); ++i )
        {
        CSIPSecChallenge* challenge = ( *this )[ i ];

    	if ( challenge->Algorithm().AlgorithmName() ==
    		 CSIPSecChallengeAKA::SupportedAlgorithm() )
    		{
    		iContext->Cache().ClearAKAEntriesWithOldRealm( challenge->Realm() );
    		}
        }
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::CreateChallengeL
// If qop exists, but has unknown value, ignore the challenge.
// -----------------------------------------------------------------------------
//
void
RSIPSecChallengeResolver::CreateChallengeL( CSIPAuthenticateHeaderBase& aHeader,
											CSIPSecDigest::TChallengeType aType,
											TInt& aCount,
											RStringF aAlgorithm,
											RStringF aQop )
	{
	CSIPSecRequestData::TQop qop = SelectQopL( aHeader, aQop );
	if ( qop != CSIPSecRequestData::EUnknown )
		{
	    SelectAlgorithm( aHeader, iMechanism.Algorithm(), aAlgorithm );
		aHeader.SetParamL( SIPStrings::StringF( SipStrConsts::EAlgorithm ),
						   aAlgorithm );

	    CSIPSecChallenge* challenge( NULL );
	    if ( aAlgorithm == CSIPSecChallengeMD5::SupportedAlgorithm() )
	        {
	        challenge = CSIPSecChallengeMD5::NewLC( aType, aHeader, qop );
	        }
	    if ( aAlgorithm == CSIPSecChallengeAKA::SupportedAlgorithm() )
	        {
	        challenge = CSIPSecChallengeAKA::NewLC( aType, aHeader, qop );
	        }

	    if ( challenge )
	        {
	        AppendL( challenge );
	        CleanupStack::Pop( challenge );
	        ++aCount;
	        }
		}
    }
		
// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::IsValidDigestChallenge
// -----------------------------------------------------------------------------
//
TBool RSIPSecChallengeResolver::IsValidDigestChallenge(
	const CSIPAuthHeaderBase& aHeader )
	{
	return IsDigestChallenge( aHeader ) && IsValidChallenge( aHeader );
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::GetDigestParamsFromSecurityServer
// -----------------------------------------------------------------------------
//
TBool RSIPSecChallengeResolver::GetDigestParamsFromSecurityServer(
	CSIPResponse& aResponse,
	RStringF& aAlgorithm,
	RStringF& aQop )
	{
	const RStringF empty = SIPStrings::StringF( SipStrConsts::EEmpty );
	aAlgorithm = empty;
	aQop = empty;

	RStringF secServer =
		SIPStrings::StringF( SipStrConsts::ESecurityServerHeader );
	if ( aResponse.HasHeader( secServer ) )
		{
		TSglQueIter< CSIPHeaderBase > iter = aResponse.Headers( secServer );
		for ( CSIPHeaderBase* header = iter++; header; header = iter++ )
			{
			CSIPSecurityServerHeader* secServerHeader =
				static_cast< CSIPSecurityServerHeader* >( header );
			if ( secServerHeader->MechanismName().CompareF(
				KSIPSecDigestScheme ) == 0 )
				{
				aAlgorithm = secServerHeader->ParamValue(
					SIPStrings::StringF( SipStrConsts::EDigestAlgorithm ) );
				aQop = secServerHeader->ParamValue(
					SIPStrings::StringF( SipStrConsts::EDigestQop ) );
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::SelectQopL
// Security-Server can have one qop value. If it has, use it. Challenge can have
// a list of qop values ("auth,auth-int"). Qop is not case-sensitive.
// -----------------------------------------------------------------------------
//
CSIPSecRequestData::TQop
RSIPSecChallengeResolver::SelectQopL( CSIPAuthenticateHeaderBase& aHeader,
									  RStringF aQopInSecurityServer ) const
	{
	if ( aQopInSecurityServer != SIPStrings::StringF( SipStrConsts::EEmpty ) )
		{
		if ( aQopInSecurityServer.DesC().CompareF( KSIPSecAuthInt ) == 0 )
			{
			return CSIPSecRequestData::EAuthInt;
			}
		if ( aQopInSecurityServer.DesC().CompareF( KSIPSecAuth ) == 0 )
			{
			return CSIPSecRequestData::EAuth;
			}
		}


	if ( !aHeader.HasParam( SIPStrings::StringF( SipStrConsts::EQop ) ) )
		{
		// No qop => accept, but "auth" is used later on.
		return CSIPSecRequestData::EDoesNotExist;
		}

	// If many values ("auth,auth-int"), use strongest ("auth-int")
	if ( aHeader.HasQopValueL( KSIPSecAuthInt ) )
		{
		return CSIPSecRequestData::EAuthInt;
		}
	if ( aHeader.HasQopValueL( KSIPSecAuth ) )
		{
		return CSIPSecRequestData::EAuth;
		}

	return CSIPSecRequestData::EUnknown;
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::SelectAlgorithm
// -----------------------------------------------------------------------------
//
void
RSIPSecChallengeResolver::SelectAlgorithm( const CSIPAuthHeaderBase& aHeader,
										   RStringF aDefaultAlgorithm,
										   RStringF& aAlgorithm )
	{
    const RStringF empty = SIPStrings::StringF( SipStrConsts::EEmpty );
    if ( aAlgorithm == empty )
		{
        // No algorithm in Security-Server, use challenge's algorithm
        aAlgorithm = aHeader.ParamValue(
        	SIPStrings::StringF( SipStrConsts::EAlgorithm ) );
		}

	if ( aAlgorithm == empty )
		{
		// Use the default algorithm. If it isn't set yet (security agreement
		// not yet done), assume MD5.
		if ( aDefaultAlgorithm == empty )
			{
			aAlgorithm = SIPStrings::StringF( SipStrConsts::EMD5 );
			}
		else
			{
        	aAlgorithm = aDefaultAlgorithm;
			}
		}
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::IsDigestChallenge
// Scheme is not case-sensitive.
// -----------------------------------------------------------------------------
//
TBool RSIPSecChallengeResolver::IsDigestChallenge(	
	const CSIPAuthHeaderBase& aHeader )
	{
	return aHeader.AuthScheme().DesC().CompareF( KSIPSecDigestScheme ) == 0;
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::IsValidChallenge
// -----------------------------------------------------------------------------
//
TBool RSIPSecChallengeResolver::IsValidChallenge(
	const CSIPAuthHeaderBase& aHeader )
	{
	return aHeader.HasParam( SIPStrings::StringF( SipStrConsts::ERealm ) ) &&
		   aHeader.HasParam( SIPStrings::StringF( SipStrConsts::ENonce ) );
	}

// -----------------------------------------------------------------------------
// RSIPSecChallengeResolver::MixedAlgorithms
// -----------------------------------------------------------------------------
//
TBool
RSIPSecChallengeResolver::MixedAlgorithms( CSIPSecCredentials& aCredentials,
										   CSIPSecChallenge& aChallenge ) const
	{
	RStringF alg = aCredentials.Challenge().Algorithm().AlgorithmName();
	RStringF challengeAlg = aChallenge.Algorithm().AlgorithmName();
	RStringF md5 = CSIPSecChallengeMD5::SupportedAlgorithm();
	RStringF aka = CSIPSecChallengeAKA::SupportedAlgorithm();

	return ( ( md5 == alg ) ^ ( md5 == challengeAlg ) ) &&
	       ( ( aka == alg ) ^ ( aka == challengeAlg ) );
	}
