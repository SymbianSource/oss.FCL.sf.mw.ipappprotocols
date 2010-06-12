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
#include "CMsrpContentTypeHeader.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "TMSRPHeaderUtil.h"
#include "msrpstrings.h"
#include "msrpstrconsts.h"

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPContentTypeHeader* CMSRPContentTypeHeader::NewLC(
	const TDesC8& aMediaType )
	{
	CMSRPContentTypeHeader* self = new (ELeave) CMSRPContentTypeHeader( );
    CleanupStack::PushL( self );
    self->ConstructL( aMediaType );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPContentTypeHeader* CMSRPContentTypeHeader::NewL(
	const TDesC8& aMediaType )
	{
	CMSRPContentTypeHeader* self = CMSRPContentTypeHeader::NewLC( aMediaType );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPContentTypeHeader* CMSRPContentTypeHeader::NewL( )
	{
	CMSRPContentTypeHeader* self = new (ELeave) CMSRPContentTypeHeader( );
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::CMSRPContentTypeHeader
// -----------------------------------------------------------------------------
//
CMSRPContentTypeHeader::CMSRPContentTypeHeader( )
    {
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPContentTypeHeader::ConstructL( const TDesC8& aMediaType )
	{
	MSRPStrings::OpenL();
	if ( !TMSRPHeaderUtil::CheckStringValidity( aMediaType ) )
		{
		User::Leave( KErrCorrupt );
		}
	iDataType = aMediaType;
	// let's check string is valid
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPContentTypeHeader::ConstructL( )
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::~CMSRPContentTypeHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPContentTypeHeader::~CMSRPContentTypeHeader()
	{
	MSRPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPContentTypeHeader* CMSRPContentTypeHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPContentTypeHeader* self = CMSRPContentTypeHeader::NewL( );
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPContentTypeHeader::DoInternalizeValueL( RReadStream& aReadStream )
	{
	TUint32 valueLength = aReadStream.ReadUint32L();
    if( valueLength > 0)
        {
	    HBufC8* tempBuffer = HBufC8::NewLC( valueLength );
	    TPtr8 tempValue( tempBuffer->Des() );
	    aReadStream.ReadL( tempValue, valueLength );
		iDataType = tempValue;
        CleanupStack::PopAndDestroy( tempBuffer );
		}
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CMSRPContentTypeHeader::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteInt32L( iDataType.Des8().Length() );
	aWriteStream.WriteL( iDataType.Des8() );
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPContentTypeHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::EContentType );
	}

// -----------------------------------------------------------------------------
// CMSRPContentTypeHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPContentTypeHeader::ToTextValueL() const
	{
    if( iDataType.Des8().Length())
        {
        HBufC8* tempBuffer = HBufC8::NewL( iDataType.Des8().Length() );
        TPtr8 tempValue( tempBuffer->Des() );
        tempValue = iDataType.Des8();
        return tempBuffer;
        }
    return NULL;
	}

