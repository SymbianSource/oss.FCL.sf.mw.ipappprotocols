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
// Name          : sipsecdigestcache.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "SipLogs.h"
#include "siperr.h"
#include "SipAssert.h"
#include "sipsecdigestcache.h"
#include "CSIPSecDigestCacheEntry.h"
#include "CSIPSecDigestObserver.h"
#include "CSIPSecUserRecord.h"
#include "CSIPSecSIMRecord.h"
#include "CSIPSecCredentials.h"
#include "CSIPSecChallengeAKA.h"
#include "RSIPSecChallengeResolver.h"
#include "MSIPSecUser.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipsecdigestcontext.h"
#include "sipauthenticateheaderbase.h"


const TInt TSIPSecDigestCacheEntryIterator::iSIPSecOffset =
	_FOFF( TSIPSecDigestCacheEntryIterator, iLink );

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPSecDigestCache::NewL
// ----------------------------------------------------------------------------
//
CSIPSecDigestCache* CSIPSecDigestCache::NewL( MTimerManager& aTimerManager )
	{
	CSIPSecDigestCache* self = NewLC( aTimerManager );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::NewLC
// ----------------------------------------------------------------------------
//		
CSIPSecDigestCache* CSIPSecDigestCache::NewLC( MTimerManager& aTimerManager )
	{
	CSIPSecDigestCache* self =
		new ( ELeave ) CSIPSecDigestCache( aTimerManager );
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache
// ----------------------------------------------------------------------------
//	
CSIPSecDigestCache::CSIPSecDigestCache( MTimerManager& aTimerManager ) :
	iProxyList( CSIPSecDigestCacheEntry::iSIPSecOffset ),
	iProxyListIter( iProxyList ),
	iEndpointList( CSIPSecDigestCacheEntry::iSIPSecOffset ),
	iEndpointListIter( iEndpointList ),
	iCredentialsList( CSIPSecUserRecord::iSIPSecOffset ),
	iCredentialsListIter( iCredentialsList ),
#ifdef CPPUNIT_TEST
	// Use granularity 1 to make each append operation to allocate memory.
  	iDigestObservers( 1 ),
#endif
	iTimerManager( aTimerManager )
	{
	}	

// ----------------------------------------------------------------------------
// ~CSIPSecDigestCache
// ----------------------------------------------------------------------------
//	
CSIPSecDigestCache::~CSIPSecDigestCache()
	{
	RemoveAllEntries();
	RemoveAllCredentials();
	iDigestObservers.ResetAndDestroy();
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::InitializeIterator
// ----------------------------------------------------------------------------
//
void
CSIPSecDigestCache::InitializeIterator( TSIPSecDigestCacheIterator& aIterator )
    {
    iProxyListIter.SetToFirst();
    iEndpointListIter.SetToFirst();

    aIterator.iList.Reset();
    aIterator.iList.AddLast( iProxyListIter );
    aIterator.iList.AddLast( iEndpointListIter );
    aIterator.iListIterator.SetToFirst();
    }
	
// ----------------------------------------------------------------------------
// CSIPSecDigestCache::AddEntry
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::AddEntry( CSIPSecDigestCacheEntry* aCacheEntry )
	{
	__SIP_ASSERT_RETURN( aCacheEntry, KErrArgument );
	__SIP_ASSERT_RETURN( !IsEntryInCache( *aCacheEntry ), KErrAlreadyExists );

	SelectCache( *aCacheEntry ).AddLast( *aCacheEntry );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveEntry
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::RemoveEntry( CSIPSecDigestCacheEntry& aCacheEntry )
	{
	__SIP_ASSERT_RETURN( IsEntryInCache( aCacheEntry ), KErrNotFound );

	aCacheEntry.UserData().CredentialsDetach();
    SelectCache( aCacheEntry ).Remove( aCacheEntry );
	delete &aCacheEntry;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveCacheEntry
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::RemoveCacheEntry( CSIPSecDigestCacheEntry& aEntry,
											const MSIPSecUser& aUser )
	{
	CSIPSecUserRecord& record = aEntry.UserData();
    RemoveEntry( aEntry );
	if ( record.CredentialsAttached() )
		{
		ChangeRecordUserIfNeeded( record, aUser );
		return EFalse;
		}

	__SIP_LOG( "SIPSecDigestCache:RemoveCacheEntry record not used" )
	__SIP_ASSERT_RETURN_VALUE( IsCredentialsInList( record ), KErrNotFound );
	iCredentialsList.Remove( record );
	delete &record;
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::IsEntryInCache
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::IsEntryInCache( CSIPSecDigestCacheEntry& aCacheEntry )
	{
    TSIPSecDigestCacheEntryIterator entryIterator =
		TSIPSecDigestCacheEntryIterator( SelectCache( aCacheEntry ) );

	CSIPSecDigestCacheEntry* entry( NULL );
	TBool found( EFalse );
	while ( !found && ( entry = entryIterator.Next() ) != NULL )
		{
		found = ( entry == &aCacheEntry );
		}
	return found;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::SelectCache
// ----------------------------------------------------------------------------
//
TSglQue< CSIPSecDigestCacheEntry >&
CSIPSecDigestCache::SelectCache( const CSIPSecDigestCacheEntry& aCacheEntry )
	{
	if ( aCacheEntry.Type() == CSIPSecDigest::EEndPoint )
		{
		return iEndpointList;
		}
	return iProxyList;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::AddUserCredentials
// ----------------------------------------------------------------------------
//		
void CSIPSecDigestCache::AddUserCredentials( CSIPSecUserRecord* aCredentials )
	{
	__SIP_ASSERT_RETURN( aCredentials, KErrArgument );
	__SIP_ASSERT_RETURN( !IsCredentialsInList( *aCredentials ),
						 KErrAlreadyExists );
    iCredentialsList.AddLast( *aCredentials );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::SearchRecordForResponse
// Cache can have only one entry with matching MSIPSecUser and realm.
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord*
CSIPSecDigestCache::SearchRecordForResponse( const TDesC8& aRealm,
			  					  			 const MSIPSecUser& aUser,
			  					  			 TRegistrationId aRegistrationId )
	{
	__ASSERT_ALWAYS( aRealm.Length() > 0,
		User::Panic( _L( "DigestCache:SearchRecForResp" ), KErrArgument ) );

	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( record->IsSameRealm( aRealm ) &&
			 record->CompareUser( aUser, aRegistrationId ) )
			{
			return record;
			}
		}

	return NULL;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::SearchRecord
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord*
CSIPSecDigestCache::SearchRecord( const TDesC8& aRealm,
							      TTransactionId aTransactionId )
	{
	__ASSERT_ALWAYS( aRealm.Length() > 0,
		User::Panic( _L( "DigestCache:SearchRec(id)" ), KErrArgument ) );

	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( record->IsSameRealm( aRealm ) &&
			 record->TransactionIdPassedToClient() == aTransactionId )
			{
			return record;
			}
		}

	return NULL;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::SearchRecord
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord* CSIPSecDigestCache::SearchRecord( const TDesC8& aRealm,
			  					  					 const MSIPSecUser& aUser )
	{
	__ASSERT_ALWAYS( aRealm.Length() > 0,
		User::Panic( _L( "DigestCache:SearchRec(user)" ), KErrArgument ) );

	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( record->IsSameRealm( aRealm ) && &aUser == &record->SIPSecUser() )
			{
			return record;
			}
		}

	return NULL;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveAllEntries
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::RemoveAllEntries()
	{
	RemoveAllEntriesFromList( iProxyList );
	RemoveAllEntriesFromList( iEndpointList );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveAllEntriesFromList
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::RemoveAllEntriesFromList(
	TSglQue<CSIPSecDigestCacheEntry>& aList )
	{
	if ( !aList.IsEmpty() )
		{
		CSIPSecDigestCacheEntry* entry( NULL );
    	TSIPSecDigestCacheEntryIterator entryIterator =
    		TSIPSecDigestCacheEntryIterator( aList );

		while ( ( entry = entryIterator.Next() ) != NULL )
			{
			aList.Remove( *entry );
			delete entry;
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveAllCredentials
// ----------------------------------------------------------------------------
//		
void CSIPSecDigestCache::RemoveAllCredentials()
	{
	if ( !iCredentialsList.IsEmpty() )
		{
		CSIPSecUserRecord* record( NULL );
		iCredentialsListIter.SetToFirst();
		while ( ( record = iCredentialsListIter++ ) != NULL )
			{
			iCredentialsList.Remove( *record );
			delete record;
			}
		}		
	}
	
// ----------------------------------------------------------------------------
// CSIPSecDigestCache::IsCredentialsInList
// ----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::IsCredentialsInList(
	const CSIPSecUserRecord& aCredentials )
	{
	if ( !iCredentialsList.IsEmpty() )
		{
		CSIPSecUserRecord* record( NULL );
		iCredentialsListIter.SetToFirst();
		while ( ( record = iCredentialsListIter++ ) != NULL )
			{
			if ( record == &aCredentials )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::ClearCache
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::ClearCache( const MSIPSecUser& aUser )
	{
	RemoveUsedEntry( iProxyList, aUser );
	RemoveUsedEntry( iEndpointList, aUser );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveUsedEntry
// ----------------------------------------------------------------------------
//
void
CSIPSecDigestCache::RemoveUsedEntry( TSglQue< CSIPSecDigestCacheEntry >& aList,
								     const MSIPSecUser& aUser )
	{
	if ( !aList.IsEmpty() )
        {
        CSIPSecDigestCacheEntry* entry( NULL );
    	TSIPSecDigestCacheEntryIterator entryIter =
    		TSIPSecDigestCacheEntryIterator( aList );

        while ( ( entry = entryIter.Next() ) != NULL )
            {
            // When removing cache items, do NOT compare trusted user
			if ( &entry->SIPSecUser() == &aUser )
		        {
		        RemoveCacheEntry( *entry, aUser );
		        }
            }
        }
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::ClearCache
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::ClearCache( CSIPSecUserRecord& aCredentials,
									 TBool aAKAOnly )
	{
	RemoveMatchingEntry( iProxyList, aCredentials, aAKAOnly );
	RemoveMatchingEntry( iEndpointList, aCredentials, aAKAOnly );
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RemoveMatchingEntry
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::RemoveMatchingEntry(
	TSglQue< CSIPSecDigestCacheEntry >& aList,
	CSIPSecUserRecord& aCredentials,
	TBool aAKAOnly )
	{
	if ( !aList.IsEmpty() )
        {
        CSIPSecDigestCacheEntry* entry( NULL );
    	TSIPSecDigestCacheEntryIterator entryIter =
    		TSIPSecDigestCacheEntryIterator( aList );

		while ( ( entry = entryIter.Next() ) != NULL )
			{
			if ( entry->HoldsUserData( aCredentials ) &&
				 ( !aAKAOnly ||
				   ( static_cast< CSIPSecCredentials* >( entry )->Challenge().
				       Algorithm().AlgorithmName() ==
				           CSIPSecChallengeAKA::SupportedAlgorithm() ) ) )
			    {
				if ( RemoveCacheEntry( *entry, entry->SIPSecUser() ) )
					{
					// Removed aCredentials, nothing refers to it, so exit loop
					return;
					}
			    }
			}
        }
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::ClearAKAEntriesWithOldRealm
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::ClearAKAEntriesWithOldRealm( const TDesC8& aRealm )
	{
	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( !record->IsSameRealm( aRealm ) )
			{
			ClearCache( *record, ETrue );
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::RegisterObserverL
// Check if aObserver has a matching CSIPSecDigestObserver. If not, create
// CSIPSecDigestObserver for it. Each SIP response has its own aObserver.
// ----------------------------------------------------------------------------
//
CSIPSecDigestObserver* CSIPSecDigestCache::RegisterObserverL(
	MSIPSecSecurityMechanismObserver& aObserver,
	CSIPSecUserRecord& aUserData )
	{
    CSIPSecDigestObserver* observer( NULL );
    TInt i( 0 );
    while ( !observer && i < iDigestObservers.Count() )
    	{
    	observer = iDigestObservers[ i++ ];
    	observer = observer->HasObserver( aObserver ) ? observer : NULL;
    	}

    if ( !observer )
        {
        observer = aUserData.CreateObserverL( aObserver );
        CleanupStack::PushL( observer );
    	iDigestObservers.AppendL( observer );
    	CleanupStack::Pop( observer );
        }

	return observer;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::CancelPendingOperations
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::CancelPendingOperations(
	MSIPSecSecurityMechanismObserver* aObserver )
	{
	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		record->CancelPendingOperations( aObserver );
		}

	CleanObservers();
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::Cancel
// ----------------------------------------------------------------------------
//
TInt CSIPSecDigestCache::Cancel( TTransactionId aTransactionId,
                                 const TDesC8& aRealm,
                                 const MSIPSecUser* aTrustedUser )
	{
	if ( aRealm.Length() == 0 )
    	{
    	return KErrArgument;
    	}

	TInt status( KErrNotFound );
	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( record->IsSameRealm( aRealm ) &&
			 ( record->TransactionIdPassedToClient() == aTransactionId || 
			   &record->SIPSecUser() == aTrustedUser ) )
			{
			status = KErrNone; // At least one found
            record->CancelPendingOperations( NULL /* cancel all */ );
			}
		}

	CleanObservers();
	return status;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::Remove
// ----------------------------------------------------------------------------
//
TInt CSIPSecDigestCache::Remove( const TDesC8& aRealm )
	{
	if ( aRealm.Length() == 0 )
    	{
    	return KErrArgument;
    	}

	TInt status( KErrNotFound );
	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( record->IsSameRealm( aRealm ) )
			{
			status = KErrNone; // At least one found
	        // Remove entries pointing to record, then remove record.
			ClearCache( *record, EFalse );
			}
		}

	CleanObservers();
	return status;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::CountResponses
// There can be just one record per realm/transaction id pair, as many SIPSec
// users can't have the same transaction id. Cache has just one IMS record.
// Compare the real transaction ids, regardless of the value returned by
// MSIPSecUser::PassOnlyRealmsToUser().
// ----------------------------------------------------------------------------
//
TInt CSIPSecDigestCache::CountResponses( CSIPResponse& aResponse,
									     CSIPRequest& aRequest,
									     TTransactionId aTransactionId,
									     RStringF aDefaultAlgorithm )
	{
	TInt status( KErrNone );

	RStringF alg;
	RStringF dummyQop;
	RSIPSecChallengeResolver::GetDigestParamsFromSecurityServer( aResponse,
									   			     			 alg,
									   				 			 dummyQop );
	CSIPSecUserRecord* record( NULL );
	iCredentialsListIter.SetToFirst();
	while ( ( record = iCredentialsListIter++ ) != NULL )
		{
		if ( record->Type() == CSIPSecUserRecord::EIMS &&
			 record->TransactionId() == aTransactionId )
			{			
			TBool match( EFalse );
	    	if ( !HandleChallenges( aResponse,
	    						    alg,
	    						    aDefaultAlgorithm,
	    						    record->Realm(),
	    						    match ) )
				{
				__SIP_ASSERT_RETURN_VALUE( !match, KErrAlreadyExists );
				match = HandleAuthorizations( aRequest,
											  alg,
											  aDefaultAlgorithm,
											  record->Realm() );
				}
			if ( match &&
				 static_cast< CSIPSecSIMRecord* >( record )->
				 	UpdateMessageCounter( aResponse.ResponseCode() ) )
				{
				status = KErrSIPForbidden;
				}
			}
		}
	return status;
	}

// ----------------------------------------------------------------------------
// CSIPSecDigestCache::CleanObservers
// ----------------------------------------------------------------------------
//
void CSIPSecDigestCache::CleanObservers()
	{
    TInt i( 0 );
    while ( i < iDigestObservers.Count() )
    	{
    	CSIPSecDigestObserver* observer = iDigestObservers[ i ];
    	if ( observer->IsCompleted() )
    	    {
    	    iDigestObservers.Remove( i );
    	    delete observer;
    	    }
        else
            {
            ++i;
            }
    	}
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestCache::HandleChallenges
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::HandleChallenges( CSIPResponse& aResponse,
										    RStringF aAlgorithm,	
										    RStringF aDefaultAlgorithm,									  
											const TDesC8& aRealm,
											TBool& aMatch ) const
	{
	TBool proxyChallenges( EFalse );
	TBool endpointChallenges( EFalse );

	aMatch = HandleAuthHeaders( aResponse,
				aAlgorithm,
				aDefaultAlgorithm,
				SIPStrings::StringF( SipStrConsts::EProxyAuthenticateHeader ),
				aRealm,
				ETrue,
				proxyChallenges ) ||
			 HandleAuthHeaders( aResponse,
				aAlgorithm,
				aDefaultAlgorithm,
				SIPStrings::StringF( SipStrConsts::EWWWAuthenticateHeader ),
				aRealm,
				ETrue,
				endpointChallenges );
	return proxyChallenges || endpointChallenges;
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestCache::HandleAuthorizations
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::HandleAuthorizations( CSIPRequest& aRequest,
										  	 	RStringF aAlgorithm,
										  	 	RStringF aDefaultAlgorithm,
										  		const TDesC8& aRealm ) const
	{
	TBool dummy( EFalse );
	return HandleAuthHeaders( aRequest,
				aAlgorithm,
				aDefaultAlgorithm,
				SIPStrings::StringF( SipStrConsts::EAuthorizationHeader ),
				aRealm,
				EFalse,
				dummy ) ||
		   HandleAuthHeaders( aRequest,
				aAlgorithm,
				aDefaultAlgorithm,
				SIPStrings::StringF( SipStrConsts::EProxyAuthorizationHeader ),
				aRealm,
				EFalse,
				dummy );
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestCache::HandleAuthHeaders
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::HandleAuthHeaders( CSIPMessage& aMessage,
											 RStringF aAlgorithm,
											 RStringF aDefaultAlgorithm,
											 RStringF aHeaderName,
											 const TDesC8& aRealm,
											 TBool aCheckChallenges,
											 TBool& aChallengeFound ) const
	{
	aChallengeFound = EFalse;
	TBool match( EFalse );

	if ( aMessage.HeaderCount( aHeaderName ) > 0 )
		{
		TSglQueIter< CSIPHeaderBase > headers = aMessage.Headers( aHeaderName );
    	while ( headers && !match )
    		{
	        CSIPAuthHeaderBase* authHeader =
	        	static_cast< CSIPAuthHeaderBase* >( headers++ );
			if ( !aCheckChallenges ||
				 RSIPSecChallengeResolver::IsValidDigestChallenge(
				 	*authHeader ) )
				{
				aChallengeFound = ETrue;				
				match =	CompareAKARealm( *authHeader,
										 aRealm,
										 aAlgorithm,
										 aDefaultAlgorithm );
				}
	    	}
		}
	return match;
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestCache::CompareAKARealm
// aAlgorithm may change in this function, but caller's algorithm must not.
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigestCache::CompareAKARealm( CSIPAuthHeaderBase& aHeader,
										   const TDesC8& aCachedRealm,
										   RStringF aAlgorithm,
										   RStringF aDefaultAlgorithm ) const
	{
	RSIPSecChallengeResolver::SelectAlgorithm( aHeader,
											   aDefaultAlgorithm,
											   aAlgorithm );					   	   
	return ( TSIPSecDigestContext::GetDesParam( aHeader,
				SipStrConsts::ERealm ).Compare( aCachedRealm ) == 0 ) &&
		   ( aAlgorithm == SIPStrings::StringF( SipStrConsts::EAKAv1MD5 ) );
	}

// -----------------------------------------------------------------------------
// CSIPSecDigestCache::ChangeRecordUserIfNeeded
// If none of the entries in iProxyList or iEndpointList that point to aRecord,
// have the same SIPSec user as the aRecord, change aRecord's SIPSec user to be
// any of the referring entries' SIPSec user.
// -----------------------------------------------------------------------------
//
void CSIPSecDigestCache::ChangeRecordUserIfNeeded( CSIPSecUserRecord& aRecord,
												   const MSIPSecUser& aUser )
	{
	// Do not compare trusted user
	if ( &aRecord.User() == &aUser )
		{
		// Loop both proxy and endpoint caches.
		// iProxyListIter and iEndpointListIter are already used by the code
		// that calls ChangeRecordUserIfNeeded(), so use new iterators.
    	TSIPSecDigestCacheEntryIterator proxyListIter( iProxyList );
    	TSIPSecDigestCacheEntryIterator endpointListIter( iEndpointList );
		TSIPSecDigestCacheIterator cacheIterator;
    	// Initialize iterators
    	proxyListIter.SetToFirst();
    	endpointListIter.SetToFirst();
    	cacheIterator.iList.Reset();
    	cacheIterator.iList.AddLast( proxyListIter );
    	cacheIterator.iList.AddLast( endpointListIter );
    	cacheIterator.iListIterator.SetToFirst();

		const MSIPSecUser* sipSecUser( NULL );
		CSIPSecDigestCacheEntry* entry( NULL );
		while ( ( entry = cacheIterator.Next() ) != NULL )
			{
			if ( entry->HoldsUserData( aRecord ) )
				{
				if ( &entry->SIPSecUser() == &aUser )
					{
					return; // Same SIPSec user, no action needed
					}
				else
					{
					sipSecUser = &entry->SIPSecUser();
					}
				}
			}

		// Original SIPSec user no longer uses aRecord.
		// Change SIPSec user to one of those still using it.
		__ASSERT_DEBUG( sipSecUser != NULL,
						User::Panic( _L( "DigestCache:ChangeRecUser" ),
									 KErrNotFound ) );
		if ( sipSecUser )
			{
			aRecord.SetUser( *sipSecUser );
			}
		}
	}

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// TSIPSecDigestCacheEntryIterator::TSIPSecDigestCacheEntryIterator
// ----------------------------------------------------------------------------
//
TSIPSecDigestCacheEntryIterator::TSIPSecDigestCacheEntryIterator(
	TSglQue<CSIPSecDigestCacheEntry>& aList ) :
    TSglQueIter<CSIPSecDigestCacheEntry>( aList ),
    iLink()
    {
    SetToFirst();
    }

// ----------------------------------------------------------------------------
// TSIPSecDigestCacheEntryIterator::Next
// Return the next matching entry in the cache.
// ----------------------------------------------------------------------------
//
CSIPSecDigestCacheEntry*
TSIPSecDigestCacheEntryIterator::Next( TSIPSecPluginContext& aContext )
	{
	CSIPSecDigestCacheEntry* entry( NULL );

	while ( ( entry = Next() ) != NULL )
 		{
 		if ( entry->Match( aContext ) )
 			{
 			return entry;
 			}
  		}

 	return NULL;
	}

// ----------------------------------------------------------------------------
// TSIPSecDigestCacheEntryIterator::Next
// ----------------------------------------------------------------------------
//
CSIPSecDigestCacheEntry* TSIPSecDigestCacheEntryIterator::Next()
	{
    return ( *this )++;
	}



// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// TSIPSecDigestCacheIterator::TSIPSecDigestCacheIterator
// ----------------------------------------------------------------------------
//
TSIPSecDigestCacheIterator::TSIPSecDigestCacheIterator() :
    iList( TSIPSecDigestCacheEntryIterator::iSIPSecOffset ),
    iListIterator( TSglQueIter<TSIPSecDigestCacheEntryIterator>( iList ) ),
    iContext( NULL )
    {
    }

// ----------------------------------------------------------------------------
// TSIPSecDigestCacheIterator::TSIPSecDigestCacheIterator
// ----------------------------------------------------------------------------
//
TSIPSecDigestCacheIterator::TSIPSecDigestCacheIterator(
	TSIPSecPluginContext& aContext ) :
	iList( TSIPSecDigestCacheEntryIterator::iSIPSecOffset ),
    iListIterator( TSglQueIter<TSIPSecDigestCacheEntryIterator>( iList ) ),
    iContext( &aContext )
    {    
    }

// ----------------------------------------------------------------------------
// TSIPSecDigestCacheIterator::TSIPSecDigestCacheIterator
// ----------------------------------------------------------------------------
//
CSIPSecDigestCacheEntry* TSIPSecDigestCacheIterator::Next()
    {
    TSIPSecDigestCacheEntryIterator* iter( NULL );
    CSIPSecDigestCacheEntry* entry( NULL );

	// Loop until an entry is found or both lists have been traversed
    while ( !entry && ( iter = iListIterator ) != NULL )
        {
        // Get next entry from current list
        entry = iContext ? iter->Next( *iContext ) : iter->Next();

        // If the current list has been exhausted, move to next list
        if ( !entry )
        	{
        	iter = ( iListIterator )++;
        	}
        }

    return entry;
	}
