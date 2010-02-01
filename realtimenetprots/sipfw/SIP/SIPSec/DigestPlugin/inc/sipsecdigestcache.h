/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : sipsecdigestcache.h
* Part of       : SIPSec
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_DIGESTCACHE_H__
#define __SIPSEC_DIGESTCACHE_H__

// INCLUDES
#include "CSIPSecDigestCacheEntry.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPMessage;
class CSIPResponse;
class CSIPRequest;
class CSIPAuthHeaderBase;
class MSIPSecSecurityMechanismObserver;
class CSIPSecDigestObserver;
class MTimerManager;
class TSIPSecPluginContext;
class CSIPSecUserRecord;


// CLASS DECLARATION
/**
 * @brief Class is cache entry iterator
 * 
 * @class TSIPSecDigestCacheEntryIterator sipsecdigestcache.h
 *		  "sipsecdigestcache.h" 
 */
class TSIPSecDigestCacheEntryIterator :
	public TSglQueIter<CSIPSecDigestCacheEntry>
    {
public:

    /**
    * Constructor
    * @param aList The list to iterate
    */
    TSIPSecDigestCacheEntryIterator(
    	TSglQue< CSIPSecDigestCacheEntry >& aList );

    /**
    * Returns next matching entry for the matching context
    * @param aContext a context
    * @return next matching entry or NULL
    */	
    CSIPSecDigestCacheEntry* Next( TSIPSecPluginContext& aContext );

    /**
    * Returns next matching entry regardless of context    
    * @return next matching entry or NULL
    */	
    CSIPSecDigestCacheEntry* Next();

public: // Data

    static const TInt iSIPSecOffset;

    TSglQueLink iLink; 
    };




/**
 * @brief Class is cache iterator
 * 
 * @class TSIPSecDigestCacheIterator sipsecdigestcache.h "sipsecdigestcache.h"
 *
 */
class TSIPSecDigestCacheIterator 
    {
public:

    /**
    * Constructor
    */
    TSIPSecDigestCacheIterator();

    /**
    * Constructor
    * @param aContext the context
    */	
    TSIPSecDigestCacheIterator( TSIPSecPluginContext& aContext );

    /**
    * Returns next matching entry
    * @return Next matching entry or NULL
    */	
    CSIPSecDigestCacheEntry* Next();

public: // Data

    // Queue
    TSglQue<TSIPSecDigestCacheEntryIterator> iList;

    TSglQueIter<TSIPSecDigestCacheEntryIterator> iListIterator;

	// Not owned
    TSIPSecPluginContext* iContext;
    };



/**
 * @brief Class represents cache for credentials
 * 
 * @class CSIPSecDigestCache sipsecdigestcache.h "sipsecdigestcache.h"
 *
 * Class provides caching (adding, removing and seraching) of
 * - credentials, searced based on request context
 * - user credentials, searched based on realm
 * Class also holds array of SIPSec observers
 *
 *
 * Cache can have many CSIPSecUserRecords with a same realm, if MSIPSecUser
 * differs.
 *
 * For one MSIPSecUser, cache can have one CSIPSecUserRecord per realm.
 * Further challenges with the same MSIPSecUser and realm use the same record.
 *
 * For one transaction id, cache can have many CSIPSecUserRecords if the realms
 * differ (SIP response had challenges with different realms).
 * This does not apply to a case when the transaction id is empty.
 */
class CSIPSecDigestCache : public CBase
	{
public:	// Constructors and destructor

    /**
    * Constructs an object; leaves on failure.
    * @param aTimerManager Timer Manager
    * @return new instance
    */
    static CSIPSecDigestCache* NewL( MTimerManager& aTimerManager );

    /**
    * Constructs an object and adds the pointer to the cleanup stack;
    * leaves on failure.
    * @param aTimerManager Timer Manager
    * @return new instance
    */
    static CSIPSecDigestCache* NewLC( MTimerManager& aTimerManager );

    /**
    * Destructor
    */
    ~CSIPSecDigestCache();

public: // New functions

    /**
    * Creates cache iterator
    *
    * @param aContext the context
    * @return iterator
    */
    void InitializeIterator( TSIPSecDigestCacheIterator& aIterator );

    /**
    * Adds an CSIPSecDigestCacheEntry object into the cache.    
    * @pre aCacheEntry != NULL
    * @pre aCacheEntry is not yet stored in the cache
    * @param aCacheEntry an cache entry. Ownership is transferred.
    */
    void AddEntry( CSIPSecDigestCacheEntry* aCacheEntry );

    /**
    * Removes the entry from the cache and deletes the entry.    
    * @pre aCacheEntry is in the cache
    * @param aCacheEntry Cache entry
    */
    void RemoveEntry( CSIPSecDigestCacheEntry& aCacheEntry );

    /**
    * Checks if the specified entry is in the cache.
    * @param aCacheEntry Cache entry
    * @return ETrue if entry is in the cache, EFalse if not.
    */
    TBool IsEntryInCache( CSIPSecDigestCacheEntry& aCacheEntry );

    /**
    * Adds an CSIPSecUserRecord object into the cache.
    * @pre aCredentials != NULL
    * @param aCredentials user credentials
    * The ownership is transferred.
    */		
    void AddUserCredentials( CSIPSecUserRecord* aCredentials );

    /**
    * Search for an user record with a matching realm and is related to the
    * specified SIPSec user (or its trusted user).
    * @pre aRealm.Length() > 0
    * @param aRealm realm
    * @param aUser SIPSec user
    * @param aRegistrationId Registration id
    * @return CSIPSecUserRecord Found record or NULL if there is no match.
    *		  The ownership is not transferred.
    */
    CSIPSecUserRecord*
    	SearchRecordForResponse( const TDesC8& aRealm,
					 			 const MSIPSecUser& aUser,
								 TRegistrationId aRegistrationId );

	/**
    * Search for an user record with a matching realm and transaction id.
    * @pre aRealm.Length() > 0
    * @param aRealm realm
    * @param aTransactionId Transaction id, can be empty.
    * @return CSIPSecUserRecord Found record or NULL if there is no match.
    *	The ownership is not transferred.
    */
	CSIPSecUserRecord* SearchRecord( const TDesC8& aRealm,
								     TTransactionId aTransactionId );

	/**
    * Search for an user record with a matching realm and SIPSec user.
    * @pre aRealm.Length() > 0
    * @param aRealm realm
    * @param aUser SIPSec user that is setting the credentials
    * @return CSIPSecUserRecord Found record or NULL if there is no match.
    *	The ownership is not transferred.
    */
	CSIPSecUserRecord* SearchRecord( const TDesC8& aRealm,
			  						 const MSIPSecUser& aUser );

    /**
    * Clears all cache entries belonging to a user
    * @param aUser SIPSec user
    */	
    void ClearCache( const MSIPSecUser& aUser );

    /**
    * Clears all cache entries belonging to realm (credentials).
    * @param aCredentials User credentials
    * @param aAKAOnly If ETrue, only AKA cache entries are cleared
    *				  If EFalse, all cache entries are cleared
    */
    void ClearCache( CSIPSecUserRecord& aCredentials, TBool aAKAOnly );

	/**
    * Clears all AKA cache entries that have a different realm than aRealm.
    * @param aRealm Realm
    */
	void ClearAKAEntriesWithOldRealm( const TDesC8& aRealm );

    /**
    * Registers mechanism observer and returns matching digest observer
    * @param aObserver Mechanism observer
    * @param aUserData User record for which the observer is for
    * @return Digest observer, ownership is not transferred.
    */	
    CSIPSecDigestObserver*
    	RegisterObserverL( MSIPSecSecurityMechanismObserver& aObserver,
    					   CSIPSecUserRecord& aUserData );
    
    /**
    * Removes observers, which do not have pending operations.
    */	
    void CleanObservers();

    /**
    * Cancels pending operations requested by SIP
    * @param aObserver observer
    */
    void CancelPendingOperations( MSIPSecSecurityMechanismObserver* aObserver );

	/**
    * Removes an entry from the cache, or cancels pending operations.
    * @param aTransactionId Transaction id of the targeted cache entry, can be
    *						empty.
    * @param aRealm Realm of the targeted cache entry
    * @param aRemove ETrue if the entry is to be removed
    *				 EFalse if the pending operations related to the cache
    *						entry will be canceled
    * @return KErrNone if successful, otherwise a system-wide error code
    */
	TInt Cancel( TTransactionId aTransactionId,
				 const TDesC8& aRealm,
                 const MSIPSecUser* aTrustedUser );

	/**
    * Removes all cache entries for the realm
    * @param aRealm Realm of the targeted cache entry
    * @return KErrNone if successful, otherwise a system-wide error code
    */
	TInt Remove( const TDesC8& aRealm );

	/*
     * Checks if too many successive 401 responses are received with the same
     * realm. This affects only IMS.
	 * @param aResponse SIP response
	 * @param aRequest SIP request
	 * @param aTransactionId Transaction id	
	 * @param aDefaultAlgorithm Default algorithm
	 * @return KErrSIPForbidden Too many successive 401 responses received
	 *		   KErrNone otherwise
     */
	TInt CountResponses( CSIPResponse& aResponse,
					     CSIPRequest& aRequest,
					     TTransactionId aTransactionId,
					     RStringF aDefaultAlgorithm );

    /**
    * Return timer services interface    
    */
    inline MTimerManager& Timer() 
        {
        return iTimerManager;
        }

private: // New functions, for internal use

    /**
    * Constructor
    * @param aTimerManager Timer Manager
    */
    CSIPSecDigestCache( MTimerManager& aTimerManager );

    /**
    * Returns the cache that is associated with aCacheEntry.
    * @param aCacheEntry Cache enty
    * @return Cache to use with aCacheEntry
    */
	TSglQue< CSIPSecDigestCacheEntry >&
		SelectCache( const CSIPSecDigestCacheEntry& aCacheEntry );

    /**
    * Removes all entries
    */
    void RemoveAllEntries();

	/**
    * Removes all entries from aList.
    */
    void RemoveAllEntriesFromList( TSglQue<CSIPSecDigestCacheEntry>& aList );

    /**
    * Removes all credentials
    */
    void RemoveAllCredentials();

    /**
    * Checks if the specified credentials are in the list.
    * @param aCredentials Credentials
    * @return ETrue  if credentials are in the list,
    *		  EFalse otherwise
    */
    TBool IsCredentialsInList( const CSIPSecUserRecord& aCredentials );

	/**
	* Clears all entries from the specified cache belonging to the given user.
    * @param aList Cache list
    * @param aUser SIPSec user
    */
	void RemoveUsedEntry( TSglQue<CSIPSecDigestCacheEntry>& aList,
						  const MSIPSecUser& aUser );

	/**
    * If aList contains aCredentials, the aCredentials and the cache entries
    * referring to it are removed from aList and deleted.
    * @param aList Queue of cache entries
    * @param aCredentials Credentials
    * @param aAKAOnly If ETrue, only AKA cache entries are cleared
    *				  If EFalse, all cache entries are cleared
    */
	void RemoveMatchingEntry( TSglQue<CSIPSecDigestCacheEntry>& aList,
							  CSIPSecUserRecord& aCredentials,
							  TBool aAKAOnly );

	/**
    * Removes entry from cache and deletes it. If the associated
    * CSIPSecUserRecord is no longer referred, it is also deleted.
    * @param aEntry Cache entry
    * @return ETrue if the associated CSIPSecUserRecord was also deleted,
    *		  EFalse otherwise.
    */
	TBool RemoveCacheEntry( CSIPSecDigestCacheEntry& aEntry,
							const MSIPSecUser& aUser );

	/*
     * Search the challenges present in aResponse, and check if they match the
     * cache entry.
	 * @param aResponse SIP response
	 * @param aAlgorithm Algorithm from Security-Server header or empty.
	 * @param aDefaultAlgorithm Default algorithm
	 * @param aRealm Realm in the cache
	 * @param aMatch OUT: Set to ETrue if a challenge matches the cache entry.
	 * @return ETrue if the SIP response had at least one challenge
     */
	TBool HandleChallenges( CSIPResponse& aResponse,
						    RStringF aAlgorithm,
						    RStringF aDefaultAlgorithm,
						    const TDesC8& aRealm,
						    TBool& aMatch ) const;

	/*
     * Check if an authorization-header in aRequest matches a cache entry.
	 * @param aRequest SIP request
	 * @param aAlgorithm Algorithm from Security-Server header or empty
	 * @param aDefaultAlgorithm Default algorithm
	 * @param aRealm Realm in the cache entry
	 * @return ETrue If a header matched the cache entry.
     */
	TBool HandleAuthorizations( CSIPRequest& aRequest,
							    RStringF aAlgorithm,
							    RStringF aDefaultAlgorithm,
							    const TDesC8& aRealm ) const;

	/*
     * Check if the authenticate- or authorization-headers present in aMessage,
     * match the cache entry.
	 * @param aMessage SIP message
	 * @param aAlgorithm Algorithm from Security-Server header or empty
	 * @param aDefaultAlgorithm Default algorithm
	 * @param aHeaderName Name of the headers to process.
	 * @param aRealm Realm in the cache entry
	 * @param aCheckChallenges ETrue if the challenges in authenticate must be
	 *		  checked that they are valid digest challenges.
	 * @param aChallengeFound ETrue if response had at least one challenge.
	 * @return ETrue If a header matched the cache entry.
     */
	TBool HandleAuthHeaders( CSIPMessage& aMessage,
							 RStringF aAlgorithm,
							 RStringF aDefaultAlgorithm,
							 RStringF aHeaderName,
							 const TDesC8& aRealm,
							 TBool aCheckChallenges,
							 TBool& aChallengeFound ) const;

	TBool CompareAKARealm( CSIPAuthHeaderBase& aHeader,
						   const TDesC8& aCachedRealm,
						   RStringF aAlgorithm,
						   RStringF aDefaultAlgorithm ) const;

	/*
	 * When proxy- or endpoint cache entry is removed from the cache, checks
	 * if the MSIPSecUser of the CSIPSecUserRecord that was pointed by the
	 * removed cache entry, needs to be changed.
     * @param 
     */
	void ChangeRecordUserIfNeeded( CSIPSecUserRecord& record,
								   const MSIPSecUser& aUser );

private: // Data
        
    // Proxy cache entries, owned.
	TSglQue< CSIPSecDigestCacheEntry > iProxyList;
    // Proxy cache iterator
    TSIPSecDigestCacheEntryIterator iProxyListIter;

    // Endpoint cache entries, owned.
	TSglQue< CSIPSecDigestCacheEntry > iEndpointList;
    // Endpoint cache iterator
    TSIPSecDigestCacheEntryIterator iEndpointListIter;

    // Credentials list, owned.
	TSglQue< CSIPSecUserRecord > iCredentialsList;    
    // Credentials list iterator
	TSglQueIter< CSIPSecUserRecord > iCredentialsListIter;

    // Requests, owned.
	RPointerArray< CSIPSecDigestObserver > iDigestObservers;
	
	// Timer services
    MTimerManager& iTimerManager;

#ifdef CPPUNIT_TEST
    friend class CSIPSecDigestTest;
	friend class CSIPSecDigestCacheTest;
#endif
	};
	
#endif // __SIPSEC_DIGESTCACHE_H__

// End of File
