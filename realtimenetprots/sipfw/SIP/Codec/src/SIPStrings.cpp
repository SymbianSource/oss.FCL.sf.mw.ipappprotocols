// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SIPStrings.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "CSIPCodecTlsPtrs.h"
#include "CSIPStrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// SIPStrings::OpenL
// Opens stringpool owner, leaves if one already exists in TLS
// -----------------------------------------------------------------------------
//
EXPORT_C void SIPStrings::OpenL()
	{
	CSIPCodecTlsPtrs* tlsPtrs = static_cast<CSIPCodecTlsPtrs*>( Dll::Tls() );
	if( !tlsPtrs )
		{
		CSIPStrings* strings = CSIPStrings::NewL();
        CleanupStack::PushL( strings );
		tlsPtrs = new(ELeave)CSIPCodecTlsPtrs( strings );
		CleanupStack::Pop( strings );
		CleanupStack::PushL( tlsPtrs );
		User::LeaveIfError( Dll::SetTls( tlsPtrs ) );
		CleanupStack::Pop( tlsPtrs );
		}
	else
		{
		CSIPStrings* strings = tlsPtrs->iStrings;
		// SIPHeaderLookup cannot have created the tls, 
		// since SIPHeaderLookup is depended on SIPStrings		
		__ASSERT_ALWAYS( strings, 
		                User::Panic( KNullDesC, KErrAlreadyExists ));
		strings->IncrementUsageCount();
		}
	}

// -----------------------------------------------------------------------------
// SIPStrings::Close
// Closes & deletes string pool
// -----------------------------------------------------------------------------
//
EXPORT_C void SIPStrings::Close()
	{
	CSIPCodecTlsPtrs* tlsPtrs = static_cast<CSIPCodecTlsPtrs*>( Dll::Tls() );
	if ( !tlsPtrs )
        {
        return;
        }
    CSIPStrings* strings = tlsPtrs->iStrings;
    // SIPStrings must be always Closed after SIPHeaderLookup,
    // since SIPHeaderLookup depends on SIPStrings
 	__ASSERT_ALWAYS( strings, User::Panic( KNullDesC, KErrNotFound ));
    strings->DecrementUsageCount();
    if ( strings->UsageCount() == 0 )
    	{ 
		if ( !tlsPtrs->iHeaderLookup )
			{
			delete tlsPtrs;
			Dll::FreeTls();
			}
		else
			{
			tlsPtrs->DestroyStrings();
			}
    	}
	}
	
// -----------------------------------------------------------------------------
// SIPStrings::StringF
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF SIPStrings::StringF( TInt aIndex )
	{
	CSIPStrings* strings = Strings();	              	
	return strings->Pool().StringF( aIndex, strings->Table() );
	}

// -----------------------------------------------------------------------------
// SIPStrings::StringPool
// Returns string pool
// -----------------------------------------------------------------------------
//
EXPORT_C RStringPool SIPStrings::Pool()
    {
	return Strings()->Pool();
    }

// -----------------------------------------------------------------------------
// SIPStrings::StringTableL
// Returns string table
// -----------------------------------------------------------------------------
//
EXPORT_C const TStringTable& SIPStrings::Table()
    {
	return Strings()->Table();
    }
    
// -----------------------------------------------------------------------------
// SIPStrings::Strings
// -----------------------------------------------------------------------------
//    
CSIPStrings* SIPStrings::Strings()
	{
	CSIPCodecTlsPtrs* tlsPtrs = static_cast<CSIPCodecTlsPtrs*>( Dll::Tls() );	
    __ASSERT_ALWAYS( tlsPtrs, User::Panic( KNullDesC, KErrNotReady ) );	
	
    CSIPStrings* strings = tlsPtrs->iStrings;
    __ASSERT_ALWAYS( strings, User::Panic( KNullDesC, KErrNotReady ) );
    
    return strings;
	}
