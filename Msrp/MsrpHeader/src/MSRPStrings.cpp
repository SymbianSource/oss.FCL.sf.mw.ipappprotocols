/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// CLASS HEADER
#include "MSRPStrings.h"

// INTERNAL INCLUDES
#include "CMSRPTlsPtrs.h"
#include "CMSRPStrings.h"

// -----------------------------------------------------------------------------
// MSRPStrings::OpenL
// -----------------------------------------------------------------------------
//
EXPORT_C void MSRPStrings::OpenL( RStringPool& aStringPool )
	{
	OpenL();
	aStringPool.OpenL( Table() );
	}

// -----------------------------------------------------------------------------
// MSRPStrings::OpenL
// -----------------------------------------------------------------------------
//
EXPORT_C void MSRPStrings::OpenL()
	{
	CMSRPTlsPtrs* tlsPtrs = static_cast<CMSRPTlsPtrs*>( Dll::Tls() );
	if( !tlsPtrs )
		{
		CMSRPStrings* strings = CMSRPStrings::NewL();
        CleanupStack::PushL( strings );
		tlsPtrs = new (ELeave) CMSRPTlsPtrs( strings );
		CleanupStack::Pop( strings );
		CleanupStack::PushL( tlsPtrs );
		User::LeaveIfError( Dll::SetTls( tlsPtrs ) );
		CleanupStack::Pop( tlsPtrs );
		}
	else
		{
		CMSRPStrings* strings = tlsPtrs->GetMsrpString();
		__ASSERT_ALWAYS( strings,
		                User::Panic( KNullDesC, KErrAlreadyExists ));
		strings->IncrementUsageCount();
		}
	}

// -----------------------------------------------------------------------------
// MSRPStrings::Close
// Closes & deletes string pool
// -----------------------------------------------------------------------------
//
EXPORT_C void MSRPStrings::Close()
	{
	CMSRPTlsPtrs* tlsPtrs = static_cast<CMSRPTlsPtrs*>( Dll::Tls() );
	if ( !tlsPtrs )
        {
        return;
        }
    CMSRPStrings* strings = tlsPtrs->GetMsrpString();

 	__ASSERT_ALWAYS( strings, User::Panic( KNullDesC, KErrNotFound ));
    strings->DecrementUsageCount();
    if ( strings->UsageCount() == 0 )
    	{
		delete tlsPtrs;
		Dll::FreeTls();
    	}
	}

// -----------------------------------------------------------------------------
// MSRPStrings::StringF
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF MSRPStrings::StringF( TInt aIndex )
	{
	CMSRPStrings* strings = Strings();
	return strings->Pool().StringF( aIndex, strings->Table() );
	}

// -----------------------------------------------------------------------------
// MSRPStrings::StringPool
// Returns string pool
// -----------------------------------------------------------------------------
//
EXPORT_C RStringPool MSRPStrings::Pool()
    {
	return Strings()->Pool();
    }

// -----------------------------------------------------------------------------
// MSRPStrings::StringTableL
// Returns string table
// -----------------------------------------------------------------------------
//
EXPORT_C const TStringTable& MSRPStrings::Table()
    {
	return Strings()->Table();
    }

// -----------------------------------------------------------------------------
// MSRPStrings::Strings
// -----------------------------------------------------------------------------
//
CMSRPStrings* MSRPStrings::Strings()
	{
	CMSRPTlsPtrs* tlsPtrs = static_cast<CMSRPTlsPtrs*>( Dll::Tls() );
    __ASSERT_ALWAYS( tlsPtrs, User::Panic( KNullDesC, KErrNotReady ) );

    CMSRPStrings* strings = tlsPtrs->GetMsrpString();
    __ASSERT_ALWAYS( strings, User::Panic( KNullDesC, KErrNotReady ) );

    return strings;
	}
