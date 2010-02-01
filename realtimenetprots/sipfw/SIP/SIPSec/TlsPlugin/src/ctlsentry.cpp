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
// Name        : ctlsentry.cpp
// Part of     : SIPSec TLS Plugin
// Version     : %version: 2.1.1 %
//



#include "ctlsentry.h"

// -----------------------------------------------------------------------------
// CTLSEntry::NewLC
// -----------------------------------------------------------------------------
//
CTLSEntry* CTLSEntry::NewLC( const TInetAddr& aNextHop,
    					 	 TUint32 aTransportId,
    					 	 const MSIPSecUser& aSIPSecUser )
	{	
	CTLSEntry* self =
		new ( ELeave ) CTLSEntry( aNextHop, aTransportId, aSIPSecUser );
	CleanupStack::PushL( self );	
	return self;
    }

// -----------------------------------------------------------------------------
// CTLSEntry::CTLSEntry
// -----------------------------------------------------------------------------
//
CTLSEntry::CTLSEntry( const TInetAddr& aNextHop,
					  TUint32 aTransportId,
					  const MSIPSecUser& aSIPSecUser ) :
    iNextHop( aNextHop ),
    iTransportId( aTransportId ),
    iSIPSecUser( aSIPSecUser )
    {
    }

// -----------------------------------------------------------------------------
// CTLSEntry::~CTLSEntry
// -----------------------------------------------------------------------------
//
CTLSEntry::~CTLSEntry()
    {
    }

// -----------------------------------------------------------------------------
// CTLSEntry::CompareNextHop
// -----------------------------------------------------------------------------
//
TBool CTLSEntry::CompareNextHop( const TInetAddr& aNextHop ) const
	{
	return iNextHop.CmpAddr( aNextHop );
	}

// -----------------------------------------------------------------------------
// CTLSEntry::CompareSIPSecUser
// -----------------------------------------------------------------------------
//
TBool CTLSEntry::CompareSIPSecUser( const MSIPSecUser& aSIPSecUser ) const
	{
	return &iSIPSecUser == &aSIPSecUser;
	}

// -----------------------------------------------------------------------------
// CTLSEntry::TransportId
// -----------------------------------------------------------------------------
//
TUint32 CTLSEntry::TransportId() const
	{
	return iTransportId;	
	}
