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
#include "CMsrpMessageIdHeader.h"

// INTERNAL INCLUDES
#include "msrpstrings.h"
#include "msrpstrconsts.h"

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessageIdHeader* CMSRPMessageIdHeader::NewLC(
	const TDesC8& aMessageId )
	{
	CMSRPMessageIdHeader* self = new (ELeave) CMSRPMessageIdHeader();
    CleanupStack::PushL( self );
    self->ConstructL( aMessageId );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessageIdHeader* CMSRPMessageIdHeader::NewL(
	const TDesC8& aMessageId )
	{
	CMSRPMessageIdHeader* self = CMSRPMessageIdHeader::NewLC( aMessageId );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPMessageIdHeader* CMSRPMessageIdHeader::NewL( )
	{
	CMSRPMessageIdHeader* self = new (ELeave) CMSRPMessageIdHeader();
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::CMSRPMessageIdHeader
// -----------------------------------------------------------------------------
//
CMSRPMessageIdHeader::CMSRPMessageIdHeader()
    {
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPMessageIdHeader::ConstructL(
	const TDesC8& aMessageId )
	{
	MSRPStrings::OpenL();
	// set the ID
    iIdValue = HBufC8::NewL( aMessageId.Length() );
	*iIdValue = aMessageId;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPMessageIdHeader::ConstructL( )
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::~CMSRPMessageIdHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessageIdHeader::~CMSRPMessageIdHeader()
	{
	MSRPStrings::Close();
	delete iIdValue;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessageIdHeader* CMSRPMessageIdHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPMessageIdHeader* self = CMSRPMessageIdHeader::NewL( );
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPMessageIdHeader::DoInternalizeValueL( RReadStream& aReadStream )
	{
	// value must be in stringpool
	TUint32 valueLength = aReadStream.ReadUint32L();
    if( valueLength > 0 )
        {
		delete iIdValue;
		iIdValue = NULL;
	    iIdValue = HBufC8::NewL( valueLength );
	    TPtr8 tempValue( iIdValue->Des() );
	    aReadStream.ReadL( tempValue, valueLength );
		}
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessageIdHeader::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteInt32L( iIdValue->Length() );
	aWriteStream.WriteL( *iIdValue );
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPMessageIdHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::EMessageId );
	}

// -----------------------------------------------------------------------------
// CMSRPMessageIdHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPMessageIdHeader::ToTextValueL() const
	{
    if( iIdValue )
        {
        return iIdValue->Des().AllocL();
        }
	return NULL;
	}

// End of File
