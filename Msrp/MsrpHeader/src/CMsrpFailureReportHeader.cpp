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
#include "CMsrpFailureReportHeader.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "msrpstrings.h"
#include "msrpstrconsts.h"

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPFailureReportHeader* CMSRPFailureReportHeader::NewLC(
	const RStringF& aType )
	{
	CMSRPFailureReportHeader* self = new (ELeave) CMSRPFailureReportHeader();
    CleanupStack::PushL( self );
    self->ConstructL( aType );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPFailureReportHeader* CMSRPFailureReportHeader::NewL(
	const RStringF& aType )
	{
	CMSRPFailureReportHeader* self = CMSRPFailureReportHeader::NewLC( aType );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPFailureReportHeader* CMSRPFailureReportHeader::NewL( )
	{
	CMSRPFailureReportHeader* self = new (ELeave) CMSRPFailureReportHeader();
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::CMSRPFailureReportHeader
// -----------------------------------------------------------------------------
//
CMSRPFailureReportHeader::CMSRPFailureReportHeader()
    {
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPFailureReportHeader::ConstructL(
	RStringF aType )
	{
	MSRPStrings::OpenL();
	iReportType = aType.Copy();
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPFailureReportHeader::ConstructL( )
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::~CMSRPFailureReportHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPFailureReportHeader::~CMSRPFailureReportHeader()
	{
	MSRPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPFailureReportHeader* CMSRPFailureReportHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPFailureReportHeader* self = CMSRPFailureReportHeader::NewL( );
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPFailureReportHeader::DoInternalizeValueL( RReadStream& aReadStream )
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
				RStringF tmpStr = MSRPStrings::StringF( MSRPStrConsts::EPartial );
				if( tmpStr.DesC() == tempValue )
					{
					// OK
					iReportType = tmpStr.Copy();
					}
				else
					{
					// not supported
			        CleanupStack::PopAndDestroy( tempBuffer );
					User::Leave( KErrNotSupported );
					}
				}
			}
        CleanupStack::PopAndDestroy( tempBuffer );
		}
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CMSRPFailureReportHeader::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteInt32L( iReportType.DesC().Length() );
	aWriteStream.WriteL( iReportType.DesC() );
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPFailureReportHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::EFailureReport );
	}

// -----------------------------------------------------------------------------
// CMSRPFailureReportHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPFailureReportHeader::ToTextValueL() const
	{
    if( iReportType.DesC().Length() )
        {
        return iReportType.DesC().AllocL();
        }
	return NULL;
	}

// End of File
