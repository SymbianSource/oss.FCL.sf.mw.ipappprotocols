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
// Name          : SIPHeaderLookup.cpp
// Part of       : Codec
// Version       : SIP/4.0 
//




#include "SIPHeaderLookup.h"
#include "CSIPCodecTlsPtrs.h"
#include "CSIPHeaderLookupTable.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// SIPHeaderLookup::OpenL
// -----------------------------------------------------------------------------
//
EXPORT_C void SIPHeaderLookup::OpenL()
	{
	CSIPCodecTlsPtrs* tlsPtrs = static_cast<CSIPCodecTlsPtrs*>( Dll::Tls() );
	// If SIPStrings has not created the tls-pointer,
	// the string pool has not been opened and header lookup cannot be used.
	__ASSERT_ALWAYS( tlsPtrs, User::Leave ( KErrNotReady ) );
	
	CSIPHeaderLookupTable* lookup = tlsPtrs->iHeaderLookup;
	if( lookup )
		{
		lookup->IncrementUsageCount();
		}
	else
		{
		tlsPtrs->iHeaderLookup = CSIPHeaderLookupTable::NewL();
		}
	}

// -----------------------------------------------------------------------------
// SIPHeaderLookup::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void SIPHeaderLookup::Close()
	{	
	CSIPCodecTlsPtrs* tlsPtrs = static_cast<CSIPCodecTlsPtrs*>( Dll::Tls() );
	if ( !tlsPtrs || !tlsPtrs->iHeaderLookup )
        {
        return;
        }
        
	CSIPHeaderLookupTable* lookup = tlsPtrs->iHeaderLookup;
	lookup->DecrementUsageCount();        
	if ( !tlsPtrs->iStrings )
		{
		if( lookup->UsageCount() == 0 )
			{
			delete tlsPtrs;
			Dll::FreeTls();			
			}
		}
	else
		{
		if( lookup->UsageCount() == 0 )
			{
			tlsPtrs->DestroyHeaderLookup();
			}
		}		
	}

// -----------------------------------------------------------------------------
// SIPHeaderLookup::IsSupportedL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool SIPHeaderLookup::IsSupported(RStringF aHeaderName)
	{
	return LookupTable()->IsSupported(aHeaderName);
	}

// -----------------------------------------------------------------------------
// SIPHeaderLookup::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
SIPHeaderLookup::InternalizeL(RStringF aName,RReadStream& aReadStream)
	{
	return LookupTable()->InternalizeL(aName,aReadStream);
	}

// -----------------------------------------------------------------------------
// SIPHeaderLookup::IsAPIHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool SIPHeaderLookup::IsAPIHeader(RStringF aName)
    {
    // The headers that support serialization and 
    // are supported as non-extension headers are API headers.
    return LookupTable()->InternalizeSupported(aName);
    }

// -----------------------------------------------------------------------------
// SIPHeaderLookup::CreateHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPHeaderBase> 
SIPHeaderLookup::CreateHeaderL(RStringF aName,const TDesC8& aValue)
	{
	return LookupTable()->CreateHeaderL(aName,aValue);
	}

// -----------------------------------------------------------------------------
// SIPHeaderLookup::LookupTable
// -----------------------------------------------------------------------------
//
CSIPHeaderLookupTable* SIPHeaderLookup::LookupTable()
	{
	CSIPCodecTlsPtrs* tlsPtrs = static_cast<CSIPCodecTlsPtrs*>( Dll::Tls() );	
    __ASSERT_ALWAYS( tlsPtrs, User::Panic( KNullDesC, KErrNotReady ) );	
	
	CSIPHeaderLookupTable* lookup = tlsPtrs->iHeaderLookup;	
    __ASSERT_ALWAYS( lookup, User::Panic( KNullDesC, KErrNotReady ) );	
    
	return lookup;
	}

// -----------------------------------------------------------------------------
// SIPHeaderLookup::SetConvertToSIPURI
// -----------------------------------------------------------------------------
//
EXPORT_C void SIPHeaderLookup::SetConvertToSIPURI(TBool aValue)
    {
    LookupTable()->SetConvertToSIPURI(aValue);
    }

// -----------------------------------------------------------------------------
// SIPHeaderLookup::ConvertToSIPURI
// -----------------------------------------------------------------------------
//
TBool SIPHeaderLookup::ConvertToSIPURI()
    {
    return LookupTable()->ConvertToSIPURI();
    }
