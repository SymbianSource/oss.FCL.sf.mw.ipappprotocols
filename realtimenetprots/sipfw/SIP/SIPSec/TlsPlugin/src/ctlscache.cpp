// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ctlscache.cpp
// Part of     : SIPSec TLS Plugin
// Version     : %version: 2.1.1 %
//



#include "MSIPTransportMgr.h"
#include "ctlscache.h"
#include "ctlsentry.h"

// -----------------------------------------------------------------------------
// CTLSCache::NewL
// -----------------------------------------------------------------------------
//
CTLSCache* CTLSCache::NewL()
	{
	return new ( ELeave ) CTLSCache();	
    }

// -----------------------------------------------------------------------------
// CTLSCache::CTLSCache
// -----------------------------------------------------------------------------
//
CTLSCache::CTLSCache()
#ifdef CPPUNIT_TEST
	: iEntries( 1 )    
#endif
    {
    }

// -----------------------------------------------------------------------------
// CTLSCache::~CTLSCache
// -----------------------------------------------------------------------------
//
CTLSCache::~CTLSCache()
    {
    iEntries.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CTLSCache::AddEntryL
// -----------------------------------------------------------------------------
//
void CTLSCache::AddEntryL( const TInetAddr& aNextHop,
    					   TUint32 aTransportId,
    					   const MSIPSecUser& aSIPSecUser )
	{
	__ASSERT_DEBUG( !SearchByNextHop( aNextHop ),
					User::Leave( KErrAlreadyExists ) );

	CTLSEntry* entry = CTLSEntry::NewLC( aNextHop, aTransportId, aSIPSecUser );
	iEntries.AppendL( entry );
	CleanupStack::Pop( entry );
	}

// -----------------------------------------------------------------------------
// CTLSCache::SearchByNextHop
// -----------------------------------------------------------------------------
//
CTLSEntry* CTLSCache::SearchByNextHop( const TInetAddr& aNextHop ) const
	{
	for ( TInt i = 0; i < iEntries.Count(); ++i )
		{
		if ( iEntries[ i ]->CompareNextHop( aNextHop ) )
			{
			return iEntries[ i ];
			}
		}

	return NULL;
	}

// -----------------------------------------------------------------------------
// CTLSCache::RemoveSIPSecUser
// -----------------------------------------------------------------------------
//
void CTLSCache::RemoveSIPSecUser( MSIPTransportMgr& aTransportMgr,
								  const MSIPSecUser& aSIPSecUser )
	{	
	CTLSEntry* entry = NULL;
	for ( TInt i = iEntries.Count(); i > 0 ; --i )
		{
		entry = iEntries[ i - 1 ];
		if ( entry->CompareSIPSecUser( aSIPSecUser ) )
			{
			aTransportMgr.RemoveTransport( entry->TransportId() );
			iEntries.Remove( i - 1 );
			delete entry;
			iEntries.Compress();
			}
		}
	}
