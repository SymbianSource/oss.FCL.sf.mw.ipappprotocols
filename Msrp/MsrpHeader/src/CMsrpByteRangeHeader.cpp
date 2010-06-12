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
#include "CMsrpByteRangeHeader.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "MsrpCommon.h"
#include "TMSRPHeaderUtil.h"
#include "msrpstrings.h"
#include "msrpstrconsts.h"

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPByteRangeHeader* CMSRPByteRangeHeader::NewLC(
	TInt aStart, TInt aEnd, TInt aTotal )
	{
	CMSRPByteRangeHeader* self = new (ELeave) CMSRPByteRangeHeader(
		aStart, aEnd, aTotal );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPByteRangeHeader* CMSRPByteRangeHeader::NewL(
	TInt aStart, TInt aEnd, TInt aTotal )
	{
	CMSRPByteRangeHeader* self = CMSRPByteRangeHeader::NewLC(
		aStart, aEnd, aTotal );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPByteRangeHeader* CMSRPByteRangeHeader::NewL( )
	{
	CMSRPByteRangeHeader* self = new (ELeave) CMSRPByteRangeHeader( );
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::CMSRPByteRangeHeader
// -----------------------------------------------------------------------------
//
CMSRPByteRangeHeader::CMSRPByteRangeHeader(
	TInt aStart, TInt aEnd, TInt aTotal )
	: iStartPosition( aStart ),
	iEndPosition( aEnd ),
	iTotalBytes( aTotal )
    {
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::CMSRPByteRangeHeader
// -----------------------------------------------------------------------------
//
CMSRPByteRangeHeader::CMSRPByteRangeHeader( )
    {
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPByteRangeHeader::ConstructL( ) const
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::~CMSRPByteRangeHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPByteRangeHeader::~CMSRPByteRangeHeader()
	{
	MSRPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPByteRangeHeader* CMSRPByteRangeHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPByteRangeHeader* self = CMSRPByteRangeHeader::NewL();
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::SetStartPosition
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPByteRangeHeader::SetStartPosition( TInt aStartPos )
	{
	iStartPosition = aStartPos;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::StartPosition
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPByteRangeHeader::StartPosition( ) const
	{
	return iStartPosition;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::SetEndPosition
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPByteRangeHeader::SetEndPosition( TInt aEndPos )
	{
	iEndPosition = aEndPos;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::EndPosition
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPByteRangeHeader::EndPosition( )	const
	{
	return iEndPosition;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::SetTotalLength
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPByteRangeHeader::SetTotalLength( TInt aTotal )
	{
	iTotalBytes = aTotal;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::TotalLength
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPByteRangeHeader::TotalLength( ) const
	{
	return iTotalBytes;
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPByteRangeHeader::DoInternalizeValueL( RReadStream& aReadStream )
	{
	TUint32 valueLength = aReadStream.ReadUint32L();
    if( valueLength > 0 )
        {
		HBufC8* tempString = HBufC8::NewLC( valueLength );
	    TPtr8 tempValue( tempString->Des() );
	    aReadStream.ReadL( tempValue, valueLength );
		iStartPosition = TMSRPHeaderUtil::ConvertToNumber( tempString->Des() );
		CleanupStack::PopAndDestroy( tempString );

		TUint32 valLength = aReadStream.ReadUint32L();
	    if( valLength > 0 )
		    {
			HBufC8* tempStr = HBufC8::NewLC( valLength );
			TPtr8 tempVal( tempStr->Des() );
			aReadStream.ReadL( tempVal, valLength );
			iEndPosition = TMSRPHeaderUtil::ConvertToNumber( tempStr->Des() );
			CleanupStack::PopAndDestroy( tempString );

			TUint32 valLgth = aReadStream.ReadUint32L();
			if( valLgth > 0 )
				{
				HBufC8* tmpStr = HBufC8::NewLC( valLgth );
				TPtr8 tmpVal( tmpStr->Des() );
				aReadStream.ReadL( tmpVal, valLgth );
				iTotalBytes = TMSRPHeaderUtil::ConvertToNumber( tmpStr->Des() );
				CleanupStack::PopAndDestroy( tmpStr );
				}
			else
				{
				User::Leave( KErrArgument );
				}
			}
		else
			{
			User::Leave( KErrArgument );
			}
		}
	else
		{
		User::Leave( KErrArgument );
		}
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPByteRangeHeader::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	TBuf8< KMaxLengthOfNumbers > value;
	// start pos
	value.AppendNum( iStartPosition );
	aWriteStream.WriteInt32L( value.Length() );
	aWriteStream.WriteL( value );

	// end pos
	value.Zero();
	value.AppendNum( iEndPosition );
	aWriteStream.WriteInt32L( value.Length() );
	aWriteStream.WriteL( value );

	// total Length
	value.Zero();
	value.AppendNum( iTotalBytes );
	aWriteStream.WriteInt32L( value.Length() );
	aWriteStream.WriteL( value );
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPByteRangeHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::EByteRange );
	}

// -----------------------------------------------------------------------------
// CMSRPByteRangeHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPByteRangeHeader::ToTextValueL() const
	{
	TBuf8< KMaxLengthOfNumbers > value;
	value.AppendNum( iStartPosition, EDecimal );
	TMSRPHeaderUtil::AppendStringL( KByteRangeSeparator, value );
	if ( iEndPosition == KUnknownRange )
		{
		value.Append( KAsterisk );
		}
	else
		{
		value.AppendNum( iEndPosition, EDecimal );
		}
	TMSRPHeaderUtil::AppendStringL( KByteRangeTotalSeparator, value );

	if ( iTotalBytes == KUnknownRange )
		{
		value.Append( KAsterisk );
		}
	else
		{
		value.AppendNum( iTotalBytes, EDecimal );
		}
	HBufC8* tempString = HBufC8::NewLC( value.Length() );
	*tempString = value;
	CleanupStack::Pop( tempString );

	return tempString;
	}

// End of File
