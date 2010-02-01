/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : ctlscache.h
* Part of     : SIPSec TLS Plugin
* Interface   : 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef C_TLSCACHE_H
#define C_TLSCACHE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TInetAddr;
class MSIPSecUser;
class MSIPTransportMgr;
class CTLSEntry;

// CLASS DECLARATION

/**
 * @brief This class implements TLS mechanism, and is an ECOM plugin
 */
class CTLSCache : public CBase 
    {
public: // Constructors and destructor

    /**
    * Constructs the object.	
    * @return Return New object, ownership is transferred.    
    */
    static CTLSCache* NewL();
    
    /**
    * Destructor
    */
    ~CTLSCache();

public: // New functions

	/**
     * Add a new entry to the cache.
     *
     * @param aNextHop Next hop address
     * @param aTransportId Transport identifier
     * @param aSIPSecUser SIPSec user     
     */
	void AddEntryL( const TInetAddr& aNextHop,
    				TUint32 aTransportId,
    				const MSIPSecUser& aSIPSecUser );

    /**
     * Searches the cache for an entry with a matching next hop address.
     *
     * @param aNextHop Next hop address
     * @return CTLSEntry* Matching entry, or NULL if not found. Ownership is not
     *		   transferred.
     */
	CTLSEntry* SearchByNextHop( const TInetAddr& aNextHop ) const;

	/**
     * Remove the SIPSec user from all cache entries.
     *
     * @param aTransportMgr TransportMgr for removing the used TLS transport.
     * @param aSIPSecUser SIPSec user to be removed.
     */
	void RemoveSIPSecUser( MSIPTransportMgr& aTransportMgr,
						   const MSIPSecUser& aSIPSecUser );

private: // New functions, for internal use

	/*
     * Constructor     
     */
    CTLSCache();

private: // Data

	//There can be only one entry for each next hop address.
    //Elements of the array are owned.
    RPointerArray<CTLSEntry> iEntries;


// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CTLSCacheTest;
	friend class CSIPSecTlsPluginTest;
#endif
    };

#endif //end of C_TLSCACHE_H

// End of File
