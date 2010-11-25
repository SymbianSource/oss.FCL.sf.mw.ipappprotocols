/*
* =============================================================================
*  Name          : CMsrpStatusHeader.cpp
*  Part of       : MSRP API
*  Version       : %version: 1 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2007 Nokia. All rights reserved.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* =============================================================================
*/

//CLASS HEADER
#include "CMsrpStatusHeader.h"

// EXTERNAL INCLUDES
#include "msrpstrings.h"
#include <msrpstrconsts.h>

// EXTERNAL INCLUDES
#include "MsrpCommon.h"
#include "TMSRPHeaderUtil.h"
#include "CMSRPResponse.h"

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPStatusHeader* CMSRPStatusHeader::NewLC( TInt aStatusCode )
	{
	CMSRPStatusHeader* self = new (ELeave) CMSRPStatusHeader( aStatusCode );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPStatusHeader* CMSRPStatusHeader::NewL( TInt aStatusCode )
	{
	CMSRPStatusHeader* self = CMSRPStatusHeader::NewLC( aStatusCode );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPStatusHeader* CMSRPStatusHeader::NewL( )
	{
	CMSRPStatusHeader* self = new (ELeave) CMSRPStatusHeader( );
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::CMSRPStatusHeader
// -----------------------------------------------------------------------------
//
CMSRPStatusHeader::CMSRPStatusHeader( TInt aStatusCode )
	: iStatusCode( aStatusCode )
    {
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::CMSRPStatusHeader
// -----------------------------------------------------------------------------
//
CMSRPStatusHeader::CMSRPStatusHeader( )
    {
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPStatusHeader::ConstructL( ) const
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::~CMSRPStatusHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPStatusHeader::~CMSRPStatusHeader()
	{
	MSRPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPStatusHeader* CMSRPStatusHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPStatusHeader* self = CMSRPStatusHeader::NewL( );
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::SetStartPosition
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPStatusHeader::SetStatusCode( TInt aStatusCode )
	{
	iStatusCode = aStatusCode;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::StatusCode
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPStatusHeader::StatusCode( ) const
	{
	return iStatusCode;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPStatusHeader::DoInternalizeValueL( RReadStream& aReadStream )
	{
	TUint32 valueLength = aReadStream.ReadUint32L();
    if( valueLength > 0 )
        {
		HBufC8* tempString = HBufC8::NewLC( valueLength );
	    TPtr8 tempValue( tempString->Des() );
	    aReadStream.ReadL( tempValue, valueLength );
		iStatusCode = TMSRPHeaderUtil::ConvertToNumber( tempString->Des() );
		CleanupStack::PopAndDestroy( tempString );
		}
	else
		{
		User::Leave( KErrArgument );
		}
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CMSRPStatusHeader::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	TBuf8< KMaxLengthOfStatusCode > value;
	// status code
	value.AppendNum( iStatusCode, EDecimal );
	aWriteStream.WriteInt32L( value.Length() );
	aWriteStream.WriteL( value );
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPStatusHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::EStatus );
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPStatusHeader::ToTextValueL() const
	{
	TBuf8< KMaxLengthOfStatusCodeValue > statusCodeLine;
	TMSRPHeaderUtil::AppendStringL(
		MSRPStrings::StringF( MSRPStrConsts::EStatusNameSpace ).DesC(), statusCodeLine );
	TMSRPHeaderUtil::AppendStringL( KSpaceChar, statusCodeLine );

	statusCodeLine.AppendNum( iStatusCode, EDecimal );
	TMSRPHeaderUtil::AppendStringL( KSpaceChar, statusCodeLine );

	// then is the reason string
	// possible codes are defined here
	switch( iStatusCode )
		{
		case EAllOk:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EAllOk ).DesC(), statusCodeLine );
			break;
			}
		case EUnintelligibleRequest:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EUnintelligibleRequest ).DesC(), statusCodeLine );
			break;
			}
		case EActionNotAllowed:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EActionNotAllowed ).DesC(), statusCodeLine );
			break;
			}
		case ETimeout:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::ETimeout ).DesC(), statusCodeLine );
			break;
			}
		case EStopSending:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EStopSending ).DesC(), statusCodeLine );
			break;
			}
		case EMimeNotUnderstood:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EMimeNotUnderstood ).DesC(), statusCodeLine );
			break;
			}
		case EParameterOutOfBounds:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EParameterOutOfBounds ).DesC(), statusCodeLine );
			break;
			}
		case ESessionDoesNotExist:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::ESessionDoesNotExist ).DesC(), statusCodeLine );
			break;
			}
		case EUnknownRequestMethod:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::EUnknownRequestMethod ).DesC(), statusCodeLine );
			break;
			}
		case ESessionAlreadyBound:
			{
			TMSRPHeaderUtil::AppendStringL(
				MSRPStrings::StringF(
				MSRPStrConsts::ESessionAlreadyBound ).DesC(), statusCodeLine );
			break;
			}

		default:
			{
			// not a valid code
			User::Leave( KErrArgument );
			break;
			}
		}


	HBufC8* tempString = HBufC8::NewLC( statusCodeLine.Length() );
	*tempString = statusCodeLine;
	CleanupStack::Pop( tempString );

	return tempString;
	}

// -----------------------------------------------------------------------------
// CMSRPStatusHeader::__DbgTestInvariant
// -----------------------------------------------------------------------------
//
void CMSRPStatusHeader::__DbgTestInvariant() const
	{
#if defined(_DEBUG)
	// to be defined
#endif
	}

// End of File
