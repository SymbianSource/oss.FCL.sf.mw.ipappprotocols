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
#include "CMsrpSuccessReportHeader.h"

// INTERNAL INCLUDES
#include "msrpstrings.h"
#include "msrpstrconsts.h"

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPSuccessReportHeader* CMSRPSuccessReportHeader::NewLC(
	const RStringF& aType )
	{
	CMSRPSuccessReportHeader* self = new (ELeave) CMSRPSuccessReportHeader();
    CleanupStack::PushL( self );
    self->ConstructL( aType );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPSuccessReportHeader* CMSRPSuccessReportHeader::NewL(
	const RStringF& aType )
	{
	CMSRPSuccessReportHeader* self = CMSRPSuccessReportHeader::NewLC( aType );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPSuccessReportHeader* CMSRPSuccessReportHeader::NewL( )
	{
	CMSRPSuccessReportHeader* self = new (ELeave) CMSRPSuccessReportHeader();
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::CMSRPSuccessReportHeader
// -----------------------------------------------------------------------------
//
CMSRPSuccessReportHeader::CMSRPSuccessReportHeader()
    {
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPSuccessReportHeader::ConstructL(
	RStringF aType )
	{
	MSRPStrings::OpenL();
	iReportType = aType.Copy();
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPSuccessReportHeader::ConstructL( )
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::~CMSRPSuccessReportHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPSuccessReportHeader::~CMSRPSuccessReportHeader()
	{
	MSRPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPSuccessReportHeader* CMSRPSuccessReportHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPSuccessReportHeader* self = CMSRPSuccessReportHeader::NewL( );
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPSuccessReportHeader::DoInternalizeValueL( RReadStream& aReadStream )
	{
	// value must be in stringpool
	TUint32 valueLength = aReadStream.ReadUint32L();
    if( valueLength > 0 )
        {
	    HBufC8* tempBuffer = HBufC8::NewLC( valueLength );
	    TPtr8 tempValue( tempBuffer->Des() );
	    aReadStream.ReadL( tempValue, valueLength );

		// let's check if the value matches the supported content-types
		RStringF tempString = MSRPStrings::StringF( MSRPStrConsts::EYes );
		if( tempString.DesC() == tempValue )
			{
			// OK
			iReportType = tempString.Copy();
			}
		else
			{
			RStringF tempStr = MSRPStrings::StringF( MSRPStrConsts::ENo );
			if( tempStr.DesC() == tempValue )
				{
				// OK
				iReportType = tempStr.Copy();
				}
			else
				{
				// not supported
		        CleanupStack::PopAndDestroy( tempBuffer );
				User::Leave( KErrNotSupported );
				}
			}
        CleanupStack::PopAndDestroy( tempBuffer );
		}
	}

// -----------------------------------------------------------------------------
// CMSRPSuccessReportHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CMSRPSuccessReportHeader::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteInt32L( iReportType.DesC().Length() );
	aWriteStream.WriteL( iReportType.DesC() );
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPSuccessReportHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::ESuccessReport );
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPSuccessReportHeader::ToTextValueL() const
	{
    if( iReportType.DesC().Length() )
        {
        return iReportType.DesC().AllocL();
        }
    return NULL;
	}

// End of File
